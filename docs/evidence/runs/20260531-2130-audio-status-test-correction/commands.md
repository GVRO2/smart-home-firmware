# Commands

| Command | Exit Code | Resultado |
|---|---:|---|
| `powershell -ExecutionPolicy Bypass -File scripts\quality\check-audio-status-tests.ps1 -RunName audio-status-test-correction-gate` | 1 | Initial sandboxed PlatformIO run blocked by `.platformio\platforms.lock` permission |
| `powershell -ExecutionPolicy Bypass -File scripts\quality\check-audio-status-tests.ps1 -RunName audio-status-test-correction-gate` | 0 | AUDIO_STATUS_TESTS_PASSED |
| `powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1 -Mode automatic` | 0 | FIRMWARE_APPROVAL_REVIEW_PASSED_WITH_WARNINGS |
| `powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1 -Mode release` | 1 | FIRMWARE_RELEASE_READINESS_BLOCKED_BY_MISSING_PHYSICAL_EVIDENCE |
