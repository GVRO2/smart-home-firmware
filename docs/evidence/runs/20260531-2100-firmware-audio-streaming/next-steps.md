# Next Steps

1. Run upload and Serial Monitor with ESP32/MAX98357A connected.
2. Publish `play_tone`, `stop_audio`, and `set_volume` commands through real MQTT.
3. Capture `audio/status` with `mosquitto_sub`.
4. Select and benchmark an ESP32 audio decoder library before implementing HTTP MP3/WAV playback.
5. Sync backend implementation from `docs/prompts/backend-audio-contract-implementation-agent.md`.
