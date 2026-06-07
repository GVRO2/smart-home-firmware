# Risks

- Physical firmware behavior remains unvalidated in this run.
- MQTT runtime behavior remains unvalidated because Mosquitto CLI tooling is unavailable.
- Audio hardware wiring, audible `play_tone`, `stop_audio`, `set_volume`, and `audio/status` publication remain unproven on real hardware.
- Brownout/reboot absence remains unproven.
- Release readiness must remain blocked until compatible physical evidence exists.
