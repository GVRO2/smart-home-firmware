# Gates Output

## Firmware

- Native tests: PASS, 57/57.
- ESP32 build: PASS, `esp32dev SUCCESS`.
- Contract gate: PASS.
- Audio contract gate: PASS.
- Audio security gate: PASS.
- Audio docs consistency gate: PASS.
- Firmware docs consistency gate: PASS.
- Secrets gate: PASSED_WITH_WARNINGS because `include/Secrets.h` exists locally and is ignored.
- Mock MQTT contract gate: PASS.
- Hardware abstractions gate: PASS.
- Physical validation policy gate: PASS.
- Approval review automatic: FIRMWARE_APPROVAL_REVIEW_PASSED_WITH_WARNINGS.

## Backend

- Approval review: PASS, `VALIDATION_PROFILE_EXIT_CODE: 0`.
- Maven: output reported `BUILD SUCCESS`.
- Surefire: 186 tests, 0 failures, 0 errors, 0 skipped.
- Failsafe: 44 tests, 0 failures, 0 errors, 0 skipped.
- Wrapper caveat: shell command returned 1 because stderr/warnings were surfaced as PowerShell `NativeCommandError`.
