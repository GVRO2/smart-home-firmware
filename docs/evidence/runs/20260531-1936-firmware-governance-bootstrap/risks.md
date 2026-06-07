# Risks

- Physical ESP32 validation is not implied by documentation bootstrap.
- COM5 upload/monitor may be blocked by local hardware state.
- Backend consumption of partial payload still needs runtime evidence.
- `include/Secrets.h` exists locally; acceptable while ignored/untracked, but must never be committed.
- MQTT remains local-only and unauthenticated for POC.
