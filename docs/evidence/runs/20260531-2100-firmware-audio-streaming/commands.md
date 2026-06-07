# Commands

```powershell
& 'C:\Users\KABUM\.platformio\penv\Scripts\platformio.exe' test -e native
& 'C:\Users\KABUM\.platformio\penv\Scripts\platformio.exe' run
powershell -ExecutionPolicy Bypass -File scripts\quality\check-audio-contract.ps1
powershell -ExecutionPolicy Bypass -File scripts\quality\check-audio-docs-consistency.ps1
powershell -ExecutionPolicy Bypass -File scripts\quality\check-audio-security.ps1
powershell -ExecutionPolicy Bypass -File scripts\quality\check-audio-build-compatibility.ps1
powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1
```

Initial sandboxed PlatformIO runs hit `PermissionError: C:\Users\KABUM\.platformio\platforms.lock`; the same checks passed when rerun outside the sandbox with approval.
