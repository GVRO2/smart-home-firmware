# Firmware Physical Audio Validation Evidence

Status: FIRMWARE_PHYSICAL_AUDIO_VALIDATION_BLOCKED

Physical validation could not start because the expected ESP32 serial port from `platformio.ini` is `COM5`, but only `COM1` was detected. MQTT local runtime validation was also blocked because `mosquitto_pub` and `mosquitto_sub` were not found in PATH or the standard Mosquitto install folders checked.

No physical success was declared. No audio success was declared. No streaming HTTP implementation was declared.

Detected blockers:

- `BLOCKED_BY_BROKER_TOOL_UNAVAILABLE`: `mosquitto_pub`
- `FIRMWARE_PHYSICAL_VALIDATION_BLOCKED_BY_MISSING_HARDWARE`: expected `COM5` was not present
- `AUDIO_PHYSICAL_VALIDATION_BLOCKED_BY_MISSING_AUDIO_HARDWARE`: MAX98357A/speaker could not be inspected or validated because the ESP32 target hardware was not reachable

Captured evidence:

- `serial-output.txt`: detected serial ports and missing `COM5`
- `mqtt-pub-output.txt`: mock MQTT scenario blocked by missing Mosquitto CLI
- `mqtt-sub-output.txt`: mock MQTT exit code
- `physical-approval-output.txt`: official physical approval review blocker
