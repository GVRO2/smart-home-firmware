# Risk Analysis

## Implementation Risks

### 1. Wi-Fi Scan Performance
**Risk**: Frequent Wi-Fi scanning may impact battery life and system responsiveness.

**Mitigation**:
- Scan throttling implemented (30-second minimum interval)
- Scan only performed when connection is lost
- Maximum 10 networks scanned per attempt
- Scan deleted immediately after use

**Residual Risk**: Low - Throttling significantly reduces impact

### 2. Network Selection Complexity
**Risk**: Complex selection logic may have edge cases or bugs.

**Mitigation**:
- Simple, well-documented algorithm
- Priority-based sorting is straightforward
- Fallback to next candidate on failure
- Comprehensive logging for debugging
- Code structure allows for easy testing

**Residual Risk**: Low - Algorithm is simple and well-structured

### 3. IP Address Validation
**Risk**: Invalid IP addresses might be published or valid IPs might be incorrectly filtered.

**Mitigation**:
- Clear validation rules (not 0.0.0.0, not 127.0.0.1, not empty)
- Wi-Fi connection status checked before IP retrieval
- Unit tests for valid and invalid IP cases
- Logging for IP inclusion/omission events

**Residual Risk**: Low - Validation logic is simple and tested

### 4. Backward Compatibility
**Risk**: Changes to MQTT payload might break existing consumers.

**Mitigation**:
- IP field is optional in payload
- Default parameter ensures backward compatibility
- All existing tests pass without modification
- Field omission when IP is invalid
- JSON parsers typically ignore unknown fields

**Residual Risk**: Very Low - Optional field design ensures compatibility

### 5. Secrets Management
**Risk**: Real Wi-Fi passwords might be accidentally committed.

**Mitigation**:
- Only Secrets.h.example was modified
- Secrets.h remains gitignored
- Example uses placeholder values
- No real credentials in codebase

**Residual Risk**: None - Proper secrets management maintained

## Operational Risks

### 1. Network Flapping
**Risk**: Device might frequently switch between networks if signal quality fluctuates.

**Mitigation**:
- Scan throttling prevents rapid re-scanning
- Retry interval (10 seconds) prevents immediate reconnection attempts
- Priority-based selection prefers stable networks
- Current implementation doesn't actively disconnect from working network

**Residual Risk**: Low - Throttling prevents rapid switching

### 2. No Known Networks Available
**Risk**: Device might be unable to connect if no known networks are available.

**Mitigation**:
- Logs clear error message
- Continues to retry at intervals
- Does not crash or hang
- Other functionality (sensors) continues to work

**Residual Risk**: Medium - Device will be offline until known network appears

### 3. IP Address Changes
**Risk**: Backend might send commands to old IP address after device reconnects with new IP.

**Mitigation**:
- IP is published with each environmental reading
- Backend should use latest IP from most recent payload
- IP update happens on every Wi-Fi connection event
- Backend can track IP changes over time

**Residual Risk**: Low - Frequent IP updates minimize window of stale data

### 4. DHCP Issues
**Risk**: Device might get 0.0.0.0 or invalid IP from DHCP.

**Mitigation**:
- IP validation filters 0.0.0.0
- IP field omitted when invalid
- Device continues to operate (sensors still work)
- Reconnection attempts continue

**Residual Risk**: Low - Invalid IP is handled gracefully

## Testing Risks

### 1. Hardware-Specific Functionality Not Tested
**Risk**: Wi-Fi scanning and selection logic not fully tested in native environment.

**Mitigation**:
- Core logic is simple and well-structured
- Comprehensive logging for hardware validation
- Code structure allows for mock testing
- Hardware validation recommended before production

**Residual Risk**: Medium - Requires hardware validation for full confidence

### 2. Edge Cases Not Covered
**Risk**: Unusual network configurations might not be handled correctly.

**Mitigation**:
- Algorithm handles common cases (priority, RSSI, fallback)
- Logging provides visibility into behavior
- Code is modular for easy fixes
- Can be extended based on real-world feedback

**Residual Risk**: Low - Common cases covered, can address edge cases as they arise

## Security Risks

### 1. Password Exposure in Logs
**Risk**: Wi-Fi passwords might be logged accidentally.

**Mitigation**:
- Passwords never logged
- Only SSID, priority, and RSSI are logged
- Passwords stored in gitignored Secrets.h
- Code review confirms no password logging

**Residual Risk**: None - Passwords never logged

### 2. IP Address Exposure
**Risk**: Publishing IP address might expose device location or network topology.

**Mitigation**:
- IP is local network address (not public)
- Already visible in network traffic
- Backend already has device identity
- No additional security risk beyond normal operation

**Residual Risk**: None - IP is already visible in network layer

## Deployment Risks

### 1. Configuration Errors
**Risk**: Users might misconfigure known networks in Secrets.h.

**Mitigation**:
- Clear example in Secrets.h.example
- Comments explain priority system
- Invalid configuration will be logged
- Device will continue to operate (just won't connect)

**Residual Risk**: Low - Clear documentation and logging

### 2. Migration from Single Network
**Risk**: Existing users with single-network configuration need to update Secrets.h.

**Mitigation**:
- Backward compatible with existing single-network defines
- Can use KNOWN_WIFI_NETWORKS_COUNT = 1
- Old WIFI_SSID_VALUE/WIFI_PASSWORD_VALUE still work if adapted
- Migration is optional (can keep old configuration)

**Residual Risk**: Very Low - Backward compatible design

## Overall Risk Assessment

**Total Risk Level**: LOW

**Summary**:
- Implementation risks are well-mitigated
- Operational risks are acceptable
- Security risks are minimal
- Deployment risks are low due to backward compatibility
- Main residual risk is hardware validation, which is expected for firmware

**Recommendation**: Safe to proceed with deployment after hardware validation.
