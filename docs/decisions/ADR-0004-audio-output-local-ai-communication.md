# ADR-0004 - Audio Output for Local AI Communication

## Status

Accepted with implementation warnings.

## Context

The backend/AI needs a local, bounded way to communicate short spoken or audible responses to the user. The ESP32 firmware was previously observational only. Audio output changes the product surface but must not turn the firmware into an AI engine, voice assistant, microphone device, or electrical automation controller.

## Decision

Implement local audio output on ESP32 through I2S and a MAX98357A-compatible amplifier. MQTT is used only for control commands and status. HTTP local/private URLs are allowed as the future transport for generated audio files or streams, but raw audio over MQTT is not the default transport.

Implemented now:

- `play_tone`
- `stop_audio`
- `set_volume`
- `play_audio_url` command validation and status failure when playback is unavailable
- `play_audio_stream` command validation and status failure when playback is unavailable
- `audio/command`
- `audio/status`

## Scope

- Local MQTT command topic: `home/bedroom/esp32-bedroom-01/audio/command`.
- Local MQTT status topic: `home/bedroom/esp32-bedroom-01/audio/status`.
- Parser validation for `commandId`, command type, volume, expiration, local/private HTTP URL and max duration.
- I2S tone playback for physical wiring validation.
- Queue, interruption, stop, and status publishing.

## Out Of Scope

- Microphone.
- Speech recognition.
- Wake word.
- AI in firmware.
- Relay, 127V/220V, or electrical automation.
- Public MQTT broker.
- Public ESP32 endpoint.
- Public internet audio URL by default.
- Full MP3/WAV/PCM HTTP decoding in this delivery.

## Consequences

- The device now has a controlled output channel.
- The backend remains responsible for deciding messages, generating TTS, hosting local audio, and auditing intent.
- The firmware remains responsible for validating commands, local-only URL policy, queueing, I2S output, and status.
- Physical validation is required before declaring speaker readiness.
- URL/stream decoding needs a separate library decision and evidence run.

## Risks

- Sound spam or startling the user.
- Night-time disturbance.
- Privacy expectations around spoken responses.
- Brownout or voltage drop when the amplifier starts.
- URL unavailable or too slow.
- MQTT reconnect during command delivery.
- Insufficient heap/flash for an audio decoding library.
- Miswired MAX98357A, especially SPK- tied to GND.

## Mitigations

- Local-only HTTP URL validation.
- Public URLs rejected by default.
- `commandId` required.
- `expiresAt` required for URL and stream playback.
- Volume limited to `0..100`.
- Max duration: `AUDIO_MAX_DURATION_MS`.
- `stop_audio` command.
- Audio status topic for `accepted`, `playing`, `completed`, `rejected`, and `failed`.
- Feature flag: `AUDIO_PLAYBACK_ENABLED`.
- No microphone, wake word, recognition, relay, or public exposure.
