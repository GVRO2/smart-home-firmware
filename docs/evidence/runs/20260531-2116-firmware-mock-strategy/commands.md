# Commands

| Command | Directory | Exit Code | Result |
|---|---|---:|---|
| `powershell -ExecutionPolicy Bypass -File scripts\quality\check-hardware-abstractions.ps1` | `C:\Users\KABUM\OneDrive\Documents\PlatformIO\Projects\home-ai-room-observer-esp32` | 0 | FIRMWARE_HARDWARE_ABSTRACTIONS_GATE_PASSED |
| `powershell -ExecutionPolicy Bypass -File scripts\quality\check-mock-mqtt-contract.ps1` | `C:\Users\KABUM\OneDrive\Documents\PlatformIO\Projects\home-ai-room-observer-esp32` | 0 | FIRMWARE_MOCK_MQTT_CONTRACT_GATE_PASSED |
| `powershell -ExecutionPolicy Bypass -File scripts\quality\check-physical-validation-policy.ps1` | `C:\Users\KABUM\OneDrive\Documents\PlatformIO\Projects\home-ai-room-observer-esp32` | 0 | FIRMWARE_PHYSICAL_VALIDATION_POLICY_GATE_PASSED |
| `C:\Users\KABUM\.platformio\penv\Scripts\platformio.exe test -e native` | `C:\Users\KABUM\OneDrive\Documents\PlatformIO\Projects\home-ai-room-observer-esp32` | 0 | 55 tests passed |
| `powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1 -Mode automatic` | `C:\Users\KABUM\OneDrive\Documents\PlatformIO\Projects\home-ai-room-observer-esp32` | 0 | FIRMWARE_APPROVAL_REVIEW_PASSED_WITH_WARNINGS |
| `powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1 -Mode physical` | `C:\Users\KABUM\OneDrive\Documents\PlatformIO\Projects\home-ai-room-observer-esp32` | 1 | FIRMWARE_PHYSICAL_VALIDATION_BLOCKED_BY_MISSING_HARDWARE |
| `powershell -ExecutionPolicy Bypass -File scripts\dev\run-mock-mqtt-scenario.ps1` | `C:\Users\KABUM\OneDrive\Documents\PlatformIO\Projects\home-ai-room-observer-esp32` | 1 | BLOCKED_BY_BROKER_TOOL_UNAVAILABLE: mosquitto_pub |
| `powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1 -Mode release` | `C:\Users\KABUM\OneDrive\Documents\PlatformIO\Projects\home-ai-room-observer-esp32` | 1 | FIRMWARE_RELEASE_READINESS_BLOCKED_BY_MISSING_PHYSICAL_EVIDENCE |
