# Release Validation Policy

Mocks nao aprovam release sozinhos. Release readiness requires all automatic gates plus one of:

1. Physical validation executed for the current version; or
2. Recent compatible physical evidence.

Evidence is incompatible when pinout changes, MQTT contract changes, sensor libraries change, audio libraries change, I2S pins change, power topology changes, or the physical hardware differs from the release target.

If no compatible physical evidence exists, release readiness returns `FIRMWARE_RELEASE_READINESS_BLOCKED_BY_MISSING_PHYSICAL_EVIDENCE`.
