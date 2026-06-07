# Wi-Fi Selection Design

## Architecture Overview

The Wi-Fi selection feature is implemented through three main components:

### 1. KnownWifiNetworkProvider
**Purpose**: Manages the list of known Wi-Fi networks from configuration.

**Responsibilities**:
- Load network configurations from Secrets.h
- Provide access to network count and individual networks
- Support up to 10 networks internally

**API**:
```cpp
class KnownWifiNetworkProvider {
public:
    KnownWifiNetworkProvider();
    int getNetworkCount() const;
    KnownWifiNetwork getNetwork(int index) const;
};
```

**Data Structure**:
```cpp
struct KnownWifiNetwork {
    const char* ssid;
    const char* password;
    int priority;  // Higher = preferred
};
```

### 2. WiFiConnection
**Purpose**: Manages Wi-Fi connection with automatic network selection.

**Responsibilities**:
- Scan available networks
- Match scanned networks against known networks
- Sort candidates by priority and RSSI
- Attempt connection to best candidate
- Fallback to next candidate on failure
- Throttle scans to avoid excessive scanning
- Provide local IP address

**Selection Algorithm**:
```
1. Scan available Wi-Fi networks
2. For each scanned network:
   a. Check if SSID matches known network
   b. If match, add to candidates with RSSI
3. Sort candidates by:
   a. Priority (descending)
   b. RSSI (descending) as tiebreaker
4. For each candidate in sorted order:
   a. Attempt connection
   b. If success, return
   c. If failure, continue to next
5. If all candidates fail, return failure
```

**Scan Throttling**:
- Scans are throttled to 30-second intervals
- Prevents excessive scanning during connection issues
- Allows network conditions to stabilize between attempts

**Logging**:
- `WIFI_SCAN_STARTED` - Scan initiated
- `WIFI_KNOWN_NETWORK_FOUND` - Known network detected with priority and RSSI
- `WIFI_CONNECTING` - Connection attempt with SSID, priority, and RSSI
- `WIFI_CONNECTED` - Connection successful with SSID, IP, and RSSI
- `WIFI_CONNECTION_FAILED` - All connection attempts failed
- `WIFI_NO_KNOWN_NETWORK_AVAILABLE` - No known networks in scan results

### 3. NetworkAddressProvider
**Purpose**: Validates and provides local IP address.

**Responsibilities**:
- Retrieve local IP from Wi-Fi stack
- Validate IP address (filter invalid values)
- Provide safe IP address for MQTT payload

**Validation Rules**:
- IP must not be empty
- IP must not be "0.0.0.0"
- IP must not be "127.0.0.1"
- Wi-Fi must be connected

**API**:
```cpp
class NetworkAddressProvider {
public:
    String getLocalIpAddress();
    bool hasValidLocalIpAddress();
};
```

## Configuration

### Secrets.h Structure
```cpp
#define KNOWN_WIFI_NETWORKS_COUNT 3

#define KNOWN_WIFI_NETWORK_0_SSID "SSID_CASA"
#define KNOWN_WIFI_NETWORK_0_PASSWORD "SENHA_CASA"
#define KNOWN_WIFI_NETWORK_0_PRIORITY 100

#define KNOWN_WIFI_NETWORK_1_SSID "SSID_CELULAR"
#define KNOWN_WIFI_NETWORK_1_PASSWORD "SENHA_CELULAR"
#define KNOWN_WIFI_NETWORK_1_PRIORITY 80

#define KNOWN_WIFI_NETWORK_2_SSID "SSID_ESCRITORIO"
#define KNOWN_WIFI_NETWORK_2_PASSWORD "SENHA_ESCRITORIO"
#define KNOWN_WIFI_NETWORK_2_PRIORITY 60
```

### Priority Guidelines
- **100-80**: Primary home/office networks (most reliable)
- **79-60**: Secondary networks (hotspots, backup)
- **59-40**: Tertiary networks (guest networks, less reliable)
- **Below 40**: Emergency/fallback networks

## Integration with Main Loop

### Initialization
```cpp
KnownWifiNetworkProvider knownWifiNetworkProvider;
WiFiConnection wifiConnection(knownWifiNetworkProvider);
NetworkAddressProvider networkAddressProvider;
```

### Loop Processing
```cpp
void loop() {
    // Ensure Wi-Fi connection (with automatic network selection)
    wifiConnection.ensureConnected();
    
    // Update IP on connection
    if (wifiConnected && !wifiWasConnected) {
        String ipAddress = wifiConnection.getLocalIpAddress();
        mqttPublisher.setLocalIpAddress(std::string(ipAddress.c_str()));
    }
    
    // Continue with MQTT and sensor processing...
}
```

## Error Handling

### No Known Networks Available
- Log: `WIFI_NO_KNOWN_NETWORK_AVAILABLE`
- Action: Retry after scan interval
- No connection attempt made

### All Candidates Failed
- Log: `WIFI_CONNECTION_FAILED`
- Action: Retry after retry interval (10 seconds)
- Re-scan on next attempt

### Invalid IP Address
- Log: `MQTT_PAYLOAD_IP_OMITTED reason=invalid_ip`
- Action: Publish payload without IP field
- Continue normal operation

## Design Decisions

### Why Priority + RSSI?
- **Priority**: Allows user preference (e.g., home network over hotspot)
- **RSSI**: Ensures best signal among equal-priority networks
- **Combination**: Balances user preference with signal quality

### Why Scan Throttling?
- Prevents battery drain from continuous scanning
- Allows network conditions to stabilize
- Reduces interference with connection attempts
- ESP32 scan operation is resource-intensive

### Why Omit Invalid IP Instead of Null?
- JSON null is valid but may cause parsing issues in some systems
- Omitting field is cleaner for optional data
- Backward compatible with existing consumers
- Follows JSON best practices for optional fields

### Why Separate NetworkAddressProvider?
- Single responsibility principle
- Easy to test independently
- Can be extended for future features (e.g., IPv6)
- Reusable across different contexts

## Future Enhancements

### Potential Improvements
1. **Persistent Network Ranking**: Learn which networks work best over time
2. **Signal Quality Thresholds**: Skip networks with RSSI below threshold
3. **Connection History**: Track success rate per network
4. **Dynamic Priority Adjustment**: Adjust priority based on success rate
5. **IPv6 Support**: Add IPv6 address validation and publishing
6. **Network Health Monitoring**: Detect and avoid problematic networks

### Out of Scope (Per Task Requirements)
- mDNS discovery
- Complex active scan with persistent ranking
- Configuration via captive portal
- Wi-Fi provisioning via BLE
- Dynamic credential storage in NVS
