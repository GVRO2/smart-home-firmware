# Next Steps

1. Install Mosquitto CLI tools or document the local broker tooling path, then rerun `scripts/dev/run-mock-mqtt-scenario.ps1`.
2. Run physical validation on ESP32 with DHT22, BH1750, MAX98357A/I2S, and speaker, then save serial and MQTT evidence.
3. Keep `play_audio_url` and `play_audio_stream` documented as stubs until a decoder library is implemented and physically validated.
4. Treat any future `RUN_TEST` count mismatch as a blocking approval review failure.
