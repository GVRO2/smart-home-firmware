# Known Limitations

- No ESP32 upload, serial monitor, audible speaker validation, brownout check, or real sensor/MQTT hardware validation was executed.
- `play_audio_url` and `play_audio_stream` validate contract and local/private URL policy, but production playback remains stubbed because HTTP audio download and decoding are not implemented.
- MQTT scenario execution is blocked on this machine by missing Mosquitto CLI tooling.
- A sandboxed PlatformIO run failed because PlatformIO could not access its user cache/lock files; rerun outside the sandbox passed.
