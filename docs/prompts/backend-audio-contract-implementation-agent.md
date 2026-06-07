# Backend Audio Contract Implementation Agent

You are implementing the backend side of the Home AI Room Observer audio contract.

## Goal

Let the backend/AI decide a spoken message, generate or retrieve local audio, host it on a local/private HTTP endpoint, publish an MQTT audio command to the ESP32, and record the ESP32 status response.

## Firmware Contract

Command topic:

```text
home/bedroom/esp32-bedroom-01/audio/command
```

Status topic:

```text
home/bedroom/esp32-bedroom-01/audio/status
```

Supported command types:

- `play_audio_url`
- `play_audio_stream`
- `play_tone`
- `stop_audio`
- `set_volume`

## Required Backend Behavior

- Generate text and TTS outside the ESP32.
- Host audio on a local/private `http://` URL.
- Never send public internet audio URLs by default.
- Never include credentials in MQTT payloads or URLs.
- Publish `commandId`, `type`, `requestedAt`, and `expiresAt` for URL/stream commands.
- Record status states `accepted`, `playing`, `completed`, `rejected`, and `failed`.
- Audit who/what requested the audio.
- Do not expose MQTT broker publicly.
- Do not expose ESP32 publicly.
- Prepare future authorization and quiet-hours policy before broader use.

## Out Of Scope

- Firmware changes.
- Microphone.
- Speech recognition.
- Wake word.
- AI in firmware.
- Electrical automation.

## Acceptance

- Backend publishes valid local-only command payloads.
- Backend rejects or refuses public audio URLs.
- Backend stores status responses by `commandId`.
- Backend treats `failed` and `rejected` as operational evidence, not success.
