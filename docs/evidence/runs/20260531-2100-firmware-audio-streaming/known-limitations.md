# Known Limitations

- Physical ESP32/MAX98357A audio was not validated in this run.
- MQTT broker command/status capture was not executed.
- Backend/PostgreSQL ingestion was not executed.
- `play_audio_url` and `play_audio_stream` validate command shape and local/private URL policy, but HTTP audio decoding is not implemented.
- `include/Secrets.h` exists locally and remains acceptable only if untracked/ignored.
