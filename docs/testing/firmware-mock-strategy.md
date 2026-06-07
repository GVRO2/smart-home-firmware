# Firmware Mock Strategy

The firmware keeps business rules behind mockable ports:

- `EnvironmentSensor`
- `LightSensor`
- `AudioOutputDriver`
- `MqttPublisherPort`
- `ClockProvider`
- `NetworkConnection`
- `AudioCommandSource`
- `AudioStatusSink`

Native tests use mocks and fakes so they can run without Wi-Fi, Serial, broker, sensors, I2S, amplifier, or speaker. Mocks are allowed to prove parser behavior, payload shape, queue order, status publication, and service orchestration.

Mocks nao substituem validacao fisica. A mock can prove that `play_tone` asks the driver to start a tone, but only a real ESP32 plus MAX98357A and speaker can prove audible output, pin wiring, power stability, and brownout-free playback.

Unavailable playback must report controlled failure such as `audio_playback_unavailable`; it must not be documented as real playback success without physical evidence.

Current implementation status:

- `play_tone`, `stop_audio`, `set_volume`, parser, queue, and MQTT status publication are implemented for automatic native validation.
- `play_audio_url` validates contract and local/private URL policy, then depends on the production driver to download WAV/PCM 16-bit mono by HTTP, validate the WAV header, and write PCM chunks to I2S.
- `play_audio_stream` validates contract and local/private URL policy, but continuous streaming remains unimplemented.
- Release cannot claim physical audio playback success until real ESP32 evidence proves download, decoding, audible playback, status transitions, and stable environmental publishing during playback.
