# Known Limitations

- Physical validation was not executed.
- MQTT scenario execution was attempted but blocked because `mosquitto_pub` is unavailable in this environment.
- Existing audio URL/stream playback remains dependent on the real `I2sAudioOutputDriver` implementation and physical MAX98357A/speaker validation.
- `include/Secrets.h` exists locally and is reported by the secrets gate as a warning because it must remain untracked/ignored.
