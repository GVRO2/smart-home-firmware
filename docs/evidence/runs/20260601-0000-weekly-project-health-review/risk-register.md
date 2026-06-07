# Risk Register

| ID | Severity | Area | Risk | Evidence | Status |
|---|---|---|---|---|---|
| WHG-2026-06-01-001 | P1 | Firmware/Release | Release could be claimed without physical ESP32 evidence. | Physical/release validation blocked by missing evidence. | Open |
| WHG-2026-06-01-002 | P1 | Integration | No current real ESP32 -> MQTT -> backend -> PostgreSQL evidence. | No `mosquitto_pub/sub` or physical run in this review. | Open |
| WHG-2026-06-01-003 | P1 | Audio | HTTP/stream audio can be overclaimed. | SDD/backlog state decoder as pending. | Open |
| WHG-2026-06-01-004 | P2 | DX/Gates | Maven wrapper result can be misread. | `BUILD SUCCESS` with wrapper exit 1. | Open |
| WHG-2026-06-01-005 | P2 | Security/Ops | Local secrets file exists. | Secrets gate warning. | Open |
