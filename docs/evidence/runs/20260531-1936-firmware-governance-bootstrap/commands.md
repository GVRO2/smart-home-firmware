# Commands

Comandos iniciais executados:

| Command | Directory | Exit Code | Result |
|---|---|---:|---|
| `git status --short` | repo firmware | 0 | clean |
| `rg --files` | repo firmware | 0 | files listed |
| `Get-Content platformio.ini` | repo firmware | 0 | PlatformIO stack detected |
| `rg --files docs scripts` | backend repo | 1 | output returned, command ended after pipeline/select handling |
| `powershell -ExecutionPolicy Bypass -File scripts\quality\check-firmware-contract.ps1` | repo firmware | 0 | FIRMWARE_CONTRACT_GATE_PASSED |
| `powershell -ExecutionPolicy Bypass -File scripts\quality\check-firmware-docs-consistency.ps1` | repo firmware | 0 | FIRMWARE_DOCS_CONSISTENCY_PASSED after doc term correction |
| `powershell -ExecutionPolicy Bypass -File scripts\quality\check-secrets.ps1` | repo firmware | 0 | FIRMWARE_SECRETS_GATE_PASSED_WITH_WARNINGS |
| `powershell -ExecutionPolicy Bypass -File scripts\quality\check-firmware-build.ps1 -RunName firmware-governance-build` | repo firmware | 0 | PlatformIO build passed after escalated run; sandbox run was blocked by PlatformIO global cache permissions |
| `powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1` | repo firmware | 0 | FIRMWARE_APPROVAL_REVIEW_PASSED_WITH_WARNINGS |
| `powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1` | repo firmware | 0 | rerun after report path fix; evidence `20260531-2033-firmware-approval-review` |
| `C:\Users\KABUM\.platformio\penv\Scripts\platformio.exe test -e native` | repo firmware | 0 | 24 native test cases passed |
