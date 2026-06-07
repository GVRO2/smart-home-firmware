# Physical play_audio_url first playback and repeat-start fix

Run date: 2026-06-04
Status: PARTIAL_WITH_CONFIRMED_FIRST_WAV_PLAYBACK

## Operator result

Operator reported that this `play_audio_url` command worked on the first send, but the same command did not play audio on the second send.

## MQTT command

```json
{
  "type": "play_audio_url",
  "audioUrl": "http://192.168.1.200:8089/test-001.wav",
  "format": "wav",
  "encoding": "pcm_s16le",
  "sampleRateHz": 16000,
  "channels": 1,
  "volume": 20,
  "requestId": "audio-url-test-001",
  "expiresAt": "2026-07-04T21:33:10.439Z"
}
```

## First playback serial evidence

```text
[MQTT] message_received topic=home/bedroom/esp32-bedroom-01/audio/command length=234
[AUDIO] command=received commandId=audio-url-test-001 type=play_audio_url
[AUDIO] i2s_driver_install=success i2s_set_pin=success
[AUDIO] http_get=start url=http://192.168.1.200:8089/test-001.wav
[AUDIO] http_get=status code=200
[AUDIO] wav_header=valid sampleRateHz=16000 channels=1 bitsPerSample=16 dataBytes=220800
[AUDIO] playback=start format=wav pcm_s16le
[AUDIO] playback=completed bytesWritten=441600
```

Environmental publishing continued during and after playback:

```text
[MQTT] publish=success
```

## Repeat failure serial evidence

The second and third command attempts were received but did not start HTTP playback:

```text
[MQTT] message_received topic=home/bedroom/esp32-bedroom-01/audio/command length=234
[AUDIO] command=received commandId=audio-url-test-001 type=play_audio_url
```

Missing expected line:

```text
[AUDIO] http_get=start url=http://192.168.1.200:8089/test-001.wav
```

## Root cause

`AudioPlaybackService` only cleared `hasCurrent_` when a command was playing at the beginning of `loop()` and then stopped after `driver_.loop()`. URL playback runs in a separate task, so it can complete between service loop iterations. In that case `driver_.isPlaying()` was already false at the next service loop, `hasCurrent_` stayed true, and subsequent queued URL commands were never started.

## Fix

`AudioPlaybackService::loop()` now clears the current command whenever `hasCurrent_` is true and the driver is no longer playing after `driver_.loop()`.

Regression coverage added:

```text
shouldAllowSecondAudioUrlAfterFirstCompletesBetweenLoops
```

## Verification after fix

```text
native PlatformIO tests: 70 test cases, 70 succeeded
esp32dev firmware build: SUCCESS
```

## Acceptance covered

- CA-003: ESP32 downloaded WAV from HTTP and got HTTP 200.
- CA-004: ESP32 validated supported WAV PCM 16-bit mono 16 kHz.
- CA-006: first `play_audio_url` physical playback was operator-confirmed.

## Acceptance still pending

- Re-test second consecutive `play_audio_url` on flashed firmware with this fix.
- CA-008: `stop_audio` during URL playback remains pending.
