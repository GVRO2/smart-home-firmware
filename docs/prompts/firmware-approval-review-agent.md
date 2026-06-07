# Firmware Approval Review Agent Prompt

Atue como agente de approval review do firmware.

Execute ou verifique:

- Test source sanity gate.
- Audio status tests gate.
- Unreachable Unity tests gate.
- Native tests gate.
- Build gate.
- Contract gate.
- Docs consistency gate.
- Secrets gate.
- Evidence completeness gate.

Use a skill `Test Harness Quality & Anti-False-Success` para qualquer gate ou evidencia envolvendo testes C/C++/C#. Funcao de teste sobreposta, teste inalcançavel, `RUN_TEST` obsoleto, fixture contaminada ou assert incoerente deve falhar o approval review como `FIRMWARE_APPROVAL_REVIEW_FAILED`.

Classifique o resultado como passed, passed with warnings, failed ou blocked. Nao confunda approval review com validacao fisica ou release readiness.
# Firmware Approval Review Agent

Run the approval review with an explicit mode:

```powershell
.\scripts\governance\run-firmware-approval-review.ps1 -Mode automatic
.\scripts\governance\run-firmware-approval-review.ps1 -Mode physical
.\scripts\governance\run-firmware-approval-review.ps1 -Mode release
```

`automatic` validates build, native tests, contracts, docs, secrets, mocks, hardware abstractions, and physical/release policy. It must not require ESP32 hardware.

`physical` requires real ESP32 evidence and reports `FIRMWARE_PHYSICAL_VALIDATION_BLOCKED_BY_MISSING_HARDWARE` when hardware is unavailable.

`release` runs automatic gates and requires compatible physical evidence. It reports `FIRMWARE_RELEASE_READINESS_BLOCKED_BY_MISSING_PHYSICAL_EVIDENCE` when evidence is absent.
