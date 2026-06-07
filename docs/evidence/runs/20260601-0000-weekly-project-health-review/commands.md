# Commands

| Command | Directory | Exit Code | Evidence | Result |
|---|---|---:|---|---|
| `git status --short` | firmware repo | 0 | `raw/firmware-git-status.txt` | Captured dirty tree |
| `git branch --show-current` | firmware repo | 0 | `raw/firmware-branch.txt` | `main` |
| `git log --oneline -n 20` | firmware repo | 0 | `raw/firmware-git-log.txt` | Captured recent commits |
| `powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1 -Mode automatic` | firmware repo | 1 | `raw/firmware-approval-review-automatic.txt` | Initial sandbox run blocked by PlatformIO global permissions |
| `powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1 -Mode automatic` | firmware repo | 0 | `raw/firmware-approval-review-automatic-elevated.txt` | Passed with warnings |
| `git status --short` | backend repo | 0 | `raw/backend-git-status.txt` | Captured clean tree |
| `git branch --show-current` | backend repo | 0 | `raw/backend-branch.txt` | `main` |
| `git log --oneline -n 20` | backend repo | 0 | `raw/backend-git-log.txt` | Captured recent commits |
| `.\scripts\governance\run-profile.ps1 -Profile approval-review` | backend repo | 1 | `raw/backend-approval-review.txt` | Blocked by PowerShell execution policy |
| `powershell -ExecutionPolicy Bypass -File .\scripts\governance\run-profile.ps1 -Profile approval-review` | backend repo | 1 | `raw/backend-approval-review-bypass.txt` | Blocked by sandbox write permissions |
| `powershell -ExecutionPolicy Bypass -File .\scripts\governance\run-profile.ps1 -Profile approval-review` | backend repo | 0 | `raw/backend-approval-review-elevated.txt` | Passed |
| `.\mvnw.cmd clean verify` | backend repo | 1 | `raw/backend-mvn-clean-verify.txt` | Maven reported `BUILD SUCCESS`, wrapper returned 1 due stderr/warnings |

## Environment Notes

Sandboxed runs could not write to PlatformIO global cache or backend evidence directories. The affected validation commands were rerun with user-approved elevation.
