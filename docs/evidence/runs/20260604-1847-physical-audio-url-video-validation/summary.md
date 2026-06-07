# Physical audio URL video validation

Run date: 2026-06-04
Status: PASSED_WITH_AUDIO_QUALITY_OBSERVATION

## Evidence artifact

Video artifact stored in this evidence run:

```text
docs/evidence/runs/20260604-1847-physical-audio-url-video-validation/artifacts/whatsapp-video-20260604-184727.mp4
```

Original source path:

```text
C:\Users\KABUM\Downloads\WhatsApp Video 2026-06-04 at 18.47.27.mp4
```

File metadata:

```text
SizeBytes=1116960
SHA256=8E630F1DF5F941A55215C7973A8E06954AB2A52F9534CE63B4996DF8FCB67CC0
OriginalLastWriteTime=2026-06-04 18:47:44
```

`ffprobe` was not available in the local shell, so codec/duration metadata was not extracted during this run.

## Operator confirmation

The operator confirmed the `play_audio_url` physical test worked. The audio was audible but slightly clipped/overdriven.

Original note:

```text
Segue o video, o audio ficou um pouco estourado mas funcionou.
```

## Tested behavior

The test validates physical output for the local-first audio URL path:

```text
MQTT play_audio_url command
-> ESP32 receives command
-> ESP32 performs local HTTP GET for WAV
-> ESP32 validates WAV/PCM header
-> ESP32 writes PCM to I2S/MAX98357A
-> Speaker emits audible audio
```

## Acceptance impact

| Acceptance criterion | Result | Evidence basis |
| --- | --- | --- |
| CA-002 - MQTT command with URL | PASSED | Prior serial log showed `command=received commandId=audio-url-test-001 type=play_audio_url`. |
| CA-003 - ESP32 downloads WAV | PASSED | Prior serial log showed `http_get=status code=200`. |
| CA-004 - ESP32 validates supported WAV | PASSED | Prior serial log showed `wav_header=valid sampleRateHz=16000 channels=1 bitsPerSample=16 dataBytes=220800`. |
| CA-006 - Real speaker playback | PASSED_WITH_OBSERVATION | Operator-provided video and confirmation prove audible output; audio quality was slightly clipped. |
| CA-007 - Tone regression | PASSED in previous run | Separate physical `play_tone` evidence exists. |
| CA-008 - Stop during playback | NOT_VALIDATED | No stop-during-WAV evidence was provided in this run. |

## Audio quality observation

The playback path is functional, but the captured audio was reported as slightly overdriven. This is not a blocker for proving end-to-end playback, but it should be treated as a tuning issue before calling the experience polished.

Recommended tuning checks:

- Re-test the same WAV at lower command volume, for example `volume=10` or `volume=15`.
- Confirm the WAV file itself is not already clipped before the ESP32 receives it.
- If clipping persists, lower firmware PCM gain mapping or add a safer maximum output gain for `play_audio_url`.

## Final classification

`play_audio_url` is physically validated for WAV/PCM local playback with a known audio-quality issue: audible output works, but clipping/saturation needs tuning.
