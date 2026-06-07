# Physical play_tone validation

Run date: 2026-06-04
Status: PASSED_FOR_PLAY_TONE_ONLY

## Operator result

Operator reported that the device played a single audible beep and that playback was smoother, without the previous buzzing noise.

Original operator note:

```text
Ele so executou um bib, mas agora mais liso e bomnito sem um zumbido
```

## MQTT command

Topic:

```text
home/bedroom/esp32-bedroom-01/audio/command
```

Payload:

```json
{
  "commandId": "56b2ff85-90ad-4d5a-baad-aca1969b6300",
  "type": "play_tone",
  "frequencyHz": 1000,
  "durationMs": 800,
  "volume": 20,
  "createdAt": "2026-06-04T21:10:41.182210400Z",
  "expiresAt": "2026-06-04T21:11:41.182210400Z"
}
```

## Serial evidence

Observed boot/runtime facts:

```text
[WIFI] connected ip=192.168.1.45
[MQTT] connected
[MQTT] audio_command_subscription topic=home/bedroom/esp32-bedroom-01/audio/command status=success
[AUDIO] service_init=ready i2s_init=lazy
```

Observed command/playback facts:

```text
[MQTT] message_received topic=home/bedroom/esp32-bedroom-01/audio/command length=203
[AUDIO] command=received commandId=4ea7b145-fae6-49be-87c4-c55bb3663ab5 type=play_tone
[AUDIO] i2s_driver_install=success i2s_set_pin=success
[AUDIO] tone_start frequencyHz=1000 durationMs=800 volume=20
```

Observed environmental publishing remained active after playback:

```text
[MQTT] publish=success
```

## Acceptance covered

- CA-007: `play_tone` regression check passed physically for the observed run.
- MQTT command subscription and command receive path were validated on COM5.
- I2S/MAX98357A output path produced audible sound according to operator confirmation.

## Acceptance not covered

- CA-001 backend WAV hosting was not tested in this run.
- CA-003/CA-004 `play_audio_url` HTTP WAV download and header validation were not tested in this run.
- CA-006 real WAV playback through `play_audio_url` was not tested in this run.
- CA-008 `stop_audio` during playback was not tested in this run.

## Notes

The pasted MQTT payload commandId differs from the serial log commandId. This evidence still proves a `play_tone` command was received and played, but future capture should keep the exact published payload and serial event from the same command in one artifact.
