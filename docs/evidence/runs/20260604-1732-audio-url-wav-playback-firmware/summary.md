# Audio URL WAV playback firmware evidence

Run date: 2026-06-04
Status: PARTIAL

## Scope completed

- Replaced the production `play_audio_url` stub with a firmware path for local HTTP WAV playback.
- Added WAV/PCM header validation for RIFF/WAVE, PCM format, mono channel, 16-bit samples, non-empty data, and bounded data size.
- Added asynchronous ESP32 playback task for URL playback so `stop_audio` can signal playback stop while chunks are being downloaded/written.
- Kept `play_audio_stream` out of scope; continuous streaming remains unimplemented.
- Updated native tests for `requestId`, unsupported `mp3` format rejection, WAV header acceptance/rejection, and service-to-driver URL startup.

## Commands executed

```text
& 'C:\Users\KABUM\.platformio\penv\Scripts\platformio.exe' test -e native
& 'C:\Users\KABUM\.platformio\penv\Scripts\platformio.exe' run
.\scripts\quality\check-unreachable-unity-tests.ps1 -Path test\test_native
powershell -NoProfile -ExecutionPolicy Bypass -File .\scripts\quality\check-unreachable-unity-tests.ps1 -Path test\test_native
```

## Results

```text
native PlatformIO tests: 69 test cases, 69 succeeded
esp32dev firmware build: SUCCESS
direct harness gate: ENVIRONMENT_FAILURE PSSecurityException because PowerShell script execution is disabled
harness gate with ExecutionPolicy Bypass: UNITY_TEST_HARNESS_PASSED, registered_run_test_count=71, defined_test_function_count=71
```

## Manual validation status

Physical playback was not validated in this run. No ESP32 serial monitor, local backend WAV endpoint, broker publish, or audible speaker evidence was captured.

Required before closing physical acceptance:

- Backend URL returning a supported WAV file over LAN-reachable HTTP.
- MQTT command using the ESP32-reachable backend IP, not `localhost`.
- Serial log showing HTTP 200, valid WAV header, playback start, and playback completion or controlled stop.
- Operator confirmation that audio was physically audible on the MAX98357A speaker.

## Current implementation status

- `play_tone`: implemented, build-preserved.
- `stop_audio`: implemented, signals URL playback stop and clears queue.
- `set_volume`: implemented, volume is applied as PCM gain for URL playback.
- `play_audio_url`: implemented for local HTTP WAV/PCM 16-bit mono in firmware, not physically validated.
- `play_audio_stream`: not implemented for continuous streaming.
- MP3/AAC/Opus: not supported.
