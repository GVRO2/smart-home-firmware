# Next Actions

## 24-48h

1. Connect ESP32 hardware and verify COM port.
2. Run `powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1 -Mode physical`.
3. Capture serial, MQTT subscribe, MQTT command/status and backend persistence evidence.
4. Fix Maven/PowerShell capture so stderr warnings do not mask successful Maven exit code.

## Next Week

1. Run a cross-repo backend/firmware integration scenario.
2. Remove stale SDD wording that calls all audio future.
3. Decide or defer HTTP audio decoder implementation explicitly.
