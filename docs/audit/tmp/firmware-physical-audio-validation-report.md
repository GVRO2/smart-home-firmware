# Firmware Physical Audio Validation Report

## Status

FIRMWARE_PHYSICAL_AUDIO_VALIDATION_BLOCKED

## Objective

Move from automatic/mock validation toward real local MQTT and physical ESP32 audio validation without declaring release readiness before evidence.

## MQTT Tooling

`mosquitto_pub` and `mosquitto_sub` were not found through `Get-Command`, `where.exe`, or the standard Mosquitto install folders checked.

Result: `BLOCKED_BY_BROKER_TOOL_UNAVAILABLE`.

## MQTT Scenario

`scripts/dev/run-mock-mqtt-scenario.ps1` was executed and returned:

`BLOCKED_BY_BROKER_TOOL_UNAVAILABLE: mosquitto_pub`

No local MQTT publish/subscribe evidence was captured.

## Physical Hardware

Configured `monitor_port` and `upload_port`: `COM5`.

Detected serial ports: `COM1`.

Result: `FIRMWARE_PHYSICAL_VALIDATION_BLOCKED_BY_MISSING_HARDWARE`.

## Audio Hardware

MAX98357A and speaker could not be validated because the target ESP32 was not reachable on the configured port.

Result: `AUDIO_PHYSICAL_VALIDATION_BLOCKED_BY_MISSING_AUDIO_HARDWARE`.

## Evidence Captured

- `docs/evidence/runs/20260531-2137-firmware-physical-audio-validation/summary.md`
- `docs/evidence/runs/20260531-2137-firmware-physical-audio-validation/commands.md`
- `docs/evidence/runs/20260531-2137-firmware-physical-audio-validation/serial-output.txt`
- `docs/evidence/runs/20260531-2137-firmware-physical-audio-validation/mqtt-pub-output.txt`
- `docs/evidence/runs/20260531-2137-firmware-physical-audio-validation/mqtt-sub-output.txt`
- `docs/evidence/runs/20260531-2137-firmware-physical-audio-validation/play-tone-result.md`
- `docs/evidence/runs/20260531-2137-firmware-physical-audio-validation/brownout-check.md`

## Claims Not Made

- No physical boot success.
- No Wi-Fi connection success.
- No MQTT connection success.
- No DHT22/BH1750 real reading success.
- No audible `play_tone` success.
- No `stop_audio` or `set_volume` physical success.
- No brownout/reboot absence.
- No HTTP streaming implementation.

## Recommendation

Keep release readiness blocked. Install or locate Mosquitto CLI, connect the ESP32/audio hardware, then rerun the physical approval review with serial and MQTT evidence.
