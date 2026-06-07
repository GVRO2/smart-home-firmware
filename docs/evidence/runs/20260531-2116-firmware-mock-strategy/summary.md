# Firmware Mock Strategy Evidence

Status: FIRMWARE_MOCK_STRATEGY_IMPLEMENTED_WITH_WARNINGS

The firmware now has a documented four-level validation strategy, mockable ports, native mocks, expanded native tests, MQTT simulation scripts, automatic quality gates, and approval review modes for `automatic`, `physical`, and `release`.

Automatic validation passed with one warning from the existing secrets gate: local `include/Secrets.h` exists and is acceptable only because it is untracked/ignored.

No physical ESP32 validation was executed in this run.

Additional mode checks:

- Physical mode returned `FIRMWARE_PHYSICAL_VALIDATION_BLOCKED_BY_MISSING_HARDWARE`.
- Mock MQTT scenario returned `BLOCKED_BY_BROKER_TOOL_UNAVAILABLE: mosquitto_pub`.
- Release mode returned `FIRMWARE_RELEASE_READINESS_BLOCKED_BY_MISSING_PHYSICAL_EVIDENCE` after the release evidence check was tightened to reject blocked physical runs.
