# Physical Audio URL Video Validation Report

## Executive summary

Status: `PASSED_WITH_AUDIO_QUALITY_OBSERVATION`

The ESP32 `play_audio_url` path has physical evidence of successful local WAV playback through the I2S/MAX98357A speaker path. The operator provided a video artifact and confirmed that the audio played. The observed limitation is audio quality: the output was slightly clipped/overdriven.

This report does not claim final audio polish. It classifies the feature as functionally validated for audible WAV playback, with follow-up tuning required for gain/clipping.

## Evidence package

Evidence run:

```text
docs/evidence/runs/20260604-1847-physical-audio-url-video-validation/
```

Video artifact:

```text
docs/evidence/runs/20260604-1847-physical-audio-url-video-validation/artifacts/whatsapp-video-20260604-184727.mp4
```

Artifact hash:

```text
SHA256=8E630F1DF5F941A55215C7973A8E06954AB2A52F9534CE63B4996DF8FCB67CC0
SizeBytes=1116960
```

Operator statement:

```text
Segue o video, o audio ficou um pouco estourado mas funcionou.
```

## Tested scenario

The validated scenario is the local-first audio URL playback path:

```text
Backend/local HTTP WAV endpoint
-> MQTT play_audio_url command
-> ESP32 command handling
-> HTTP GET from ESP32
-> WAV/PCM header validation
-> I2S/MAX98357A playback
-> Audible speaker output
```

The test used a WAV URL in the local network:

```text
http://192.168.1.200:8089/test-001.wav
```

## Result by acceptance criterion

| Criterion | Result | Notes |
| --- | --- | --- |
| CA-001 - Backend hosts WAV locally | PARTIAL | Local HTTP WAV URL was available and consumed by ESP32; backend implementation itself is outside this firmware repo. |
| CA-002 - MQTT sends URL command | PASSED | Prior serial evidence showed `play_audio_url` command receipt. |
| CA-003 - ESP32 downloads WAV | PASSED | Prior serial evidence showed HTTP 200. |
| CA-004 - ESP32 validates supported WAV | PASSED | Prior serial evidence showed valid 16 kHz mono 16-bit WAV header. |
| CA-005 - Unsupported format rejection | PASSED_AUTOMATED | Native tests cover unsupported `mp3` command format and stereo WAV rejection. |
| CA-006 - Real speaker playback | PASSED_WITH_OBSERVATION | Video plus operator confirmation prove audio output; audio was slightly clipped. |
| CA-007 - `play_tone` regression | PASSED_PREVIOUS_PHYSICAL_RUN | Separate physical tone evidence exists. |
| CA-008 - Stop during playback | NOT_VALIDATED | Still needs a deliberate `stop_audio` test during WAV playback. |

## Quality observation

The playback is functional, but the audio was reported as slightly saturated. Likely causes to investigate:

- Command volume too high for the specific WAV amplitude.
- Source WAV already clipped before playback.
- Firmware gain mapping too aggressive for `play_audio_url`.
- MAX98357A/speaker/power chain reaching practical output limits.

## Recommended next actions

1. Repeat the same WAV with `volume=10` and `volume=15`.
2. Inspect or regenerate `test-001.wav` with normalized peaks below 0 dBFS.
3. Add a conservative gain ceiling for URL playback if clipping persists.
4. Run CA-008 by sending `stop_audio` during a longer WAV and capturing serial plus audible stop behavior.

## Final determination

`play_audio_url` should now be documented as physically working for local WAV/PCM playback, with a remaining audio-quality tuning item. `play_audio_stream`, MP3, AAC, Opus, wake word, microphone, and bidirectional audio remain out of scope/not implemented.
