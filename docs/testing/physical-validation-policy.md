# Physical Validation Policy

Physical validation requires an ESP32 real connected to the PC, the actual DHT22/AM2302, BH1750/GY-302, MAX98357A/I2S amplifier when audio is in scope, and a real speaker for audible checks.

## Required Evidence

Capture the command, directory, exit code, serial output, observed COM port, firmware version or commit, sensor readings, MQTT topics and payloads, audio command/status traffic, and notes about brownout or reboot behavior.

## Required Checks

- Upload succeeds to the selected COM port.
- Boot reaches the main loop.
- Wi-Fi connects or reports a real failure.
- MQTT connects and publishes environment payloads.
- DHT22 readings are real or logged as invalid without fabricated values.
- BH1750 readings are real or omitted when invalid.
- I2S/MAX98357A playback is physically audible for `play_tone`.
- `stop_audio` and `set_volume` are observed on real hardware.
- Audio does not stop periodic environmental publishing.

If the COM port is unavailable, report `FIRMWARE_PHYSICAL_VALIDATION_BLOCKED_BY_PORT_UNAVAILABLE`. If hardware is missing, report `FIRMWARE_PHYSICAL_VALIDATION_BLOCKED_BY_MISSING_HARDWARE`.
