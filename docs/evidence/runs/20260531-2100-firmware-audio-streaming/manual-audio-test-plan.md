# Manual Audio Test Plan

## Preconditions

- ESP32 DevKit V1 connected on the configured COM port.
- MAX98357A wired with BCLK/SCK GPIO 26, LRC/WS GPIO 25 and DIN/SD GPIO 27.
- One speaker connected to SPK+ and SPK- only.
- Broker MQTT reachable on the local network.
- Local backend/TTS HTTP endpoint available only for URL/stream tests.

## Steps

1. Upload firmware with `scripts\dev\upload-firmware.ps1`.
2. Open Serial Monitor with `scripts\dev\monitor-serial.ps1`.
3. Confirm boot logs and no brownout/reboot.
4. Confirm `[AUDIO] i2s_init=success`.
5. Publish `play_tone` with `frequencyHz=880`, `durationMs=500`, `volume=40`.
6. Confirm audible tone.
7. Confirm `accepted`, `playing`, and `completed` status on `home/bedroom/esp32-bedroom-01/audio/status`.
8. Publish `set_volume` and confirm `accepted` plus `completed`.
9. Publish long `play_tone`, then `stop_audio`, and confirm playback stops.
10. Publish `play_audio_url` with a short local HTTP file and record current limitation if driver returns `failed`.
11. Publish public URL and confirm parser rejects it.
12. Publish expired command and confirm `rejected`.
13. Restart broker or Wi-Fi and confirm MQTT resubscription.
14. Confirm environmental publishing continues before and after audio commands.

## Expected Blockers To Record

- `BLOCKED_BY_MISSING_HARDWARE`
- `BLOCKED_BY_PORT_UNAVAILABLE`
- `BLOCKED_BY_BROKER_UNAVAILABLE`
- `BLOCKED_BY_BACKEND_UNAVAILABLE`
