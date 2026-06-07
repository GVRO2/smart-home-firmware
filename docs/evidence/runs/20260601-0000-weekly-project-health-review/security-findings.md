# Security Findings

| Severity | Finding | Evidence | Recommendation |
|---|---|---|---|
| Medium | MQTT without TLS/auth is acceptable only for local lab POC. | Firmware SDD and security docs. | Do not expose broker; define auth/TLS before wider deployment. |
| Medium | `include/Secrets.h` exists locally. | Secrets gate warning. | Keep ignored and inspect before any commit. |
| Medium | Audio commands must remain local and bounded. | ADR-0004, parser tests, audio security gate. | Keep URL public rejection and no microphone/wake word boundary. |

No P0 security issue was confirmed in this review.
