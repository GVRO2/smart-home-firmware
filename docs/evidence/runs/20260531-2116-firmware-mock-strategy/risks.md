# Risks

- A passing native test does not prove sensor wiring, I2S wiring, speaker output, brownout stability, Wi-Fi RF behavior, or COM-port reliability.
- Release readiness remains blocked until compatible physical evidence is captured.
- The current mock MQTT scripts depend on `mosquitto_pub`/`mosquitto_sub` for full scenario execution; `mosquitto_pub` was not available in this environment.
