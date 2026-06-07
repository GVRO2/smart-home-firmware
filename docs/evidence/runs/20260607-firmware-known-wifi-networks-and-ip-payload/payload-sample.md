# MQTT Payload Sample

## Payload with Valid IP Address

When the ESP32 has a valid IP address, the MQTT payload includes the `ipAddress` field:

```json
{
  "deviceId": "esp32-bedroom-01",
  "room": "bedroom",
  "temperatureCelsius": 24.8,
  "humidityPercentage": 61.2,
  "luminosityLux": 180,
  "ipAddress": "192.168.15.42",
  "measuredAt": "2026-06-07T18:30:00Z"
}
```

## Payload without IP Address (Invalid or Unavailable)

When the ESP32 does not have a valid IP address (e.g., before Wi-Fi connects, or IP is 0.0.0.0), the `ipAddress` field is omitted:

```json
{
  "deviceId": "esp32-bedroom-01",
  "room": "bedroom",
  "temperatureCelsius": 24.8,
  "humidityPercentage": 61.2,
  "luminosityLux": 180,
  "measuredAt": "2026-06-07T18:30:00Z"
}
```

## Payload with Partial Sensor Data

The IP address field works independently of sensor data availability:

```json
{
  "deviceId": "esp32-bedroom-01",
  "room": "bedroom",
  "temperatureCelsius": 24.8,
  "humidityPercentage": 61.2,
  "ipAddress": "192.168.15.42",
  "measuredAt": "2026-06-07T18:30:00Z"
}
```

## Invalid IP Addresses Not Published

The following IP addresses are considered invalid and will not be published:
- `0.0.0.0` - Uninitialized or DHCP failure
- `127.0.0.1` - Loopback address
- Empty string - No IP assigned

## Backward Compatibility

Existing MQTT consumers that do not expect the `ipAddress` field will continue to work because:
- The field is optional in the JSON schema
- Existing fields remain unchanged
- Field order does not matter in JSON
- Unknown fields are typically ignored by JSON parsers

## Backend Integration

The backend can now:
1. Extract the `ipAddress` field from MQTT payloads
2. Store the last known IP for each device
3. Use the IP to send HTTP-based audio commands to the correct device
4. Track IP changes over time for debugging

## Example Backend Processing (Pseudocode)

```javascript
function handleEnvironmentPayload(payload) {
  const { deviceId, room, ipAddress, ...sensorData } = payload;
  
  // Store sensor data
  storeSensorReading(deviceId, room, sensorData);
  
  // Update device IP if valid
  if (ipAddress && isValidIp(ipAddress)) {
    updateDeviceIp(deviceId, ipAddress);
  }
}
```
