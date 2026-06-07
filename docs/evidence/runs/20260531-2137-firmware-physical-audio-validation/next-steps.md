# Next Steps

1. Install Mosquitto CLI or add existing `mosquitto_pub.exe` and `mosquitto_sub.exe` to PATH.
2. Connect the ESP32 on `COM5` or update `platformio.ini` to the actual detected ESP32 port.
3. Confirm DHT22, BH1750, MAX98357A, and speaker wiring before running physical validation.
4. Rerun `scripts/dev/run-mock-mqtt-scenario.ps1`.
5. Rerun `scripts/governance/run-firmware-approval-review.ps1 -Mode physical`.
6. Capture serial output proving boot, Wi-Fi, MQTT, DHT22, BH1750, environment publish, `play_tone`, `audio/status`, `stop_audio`, `set_volume`, and no brownout/reboot.
