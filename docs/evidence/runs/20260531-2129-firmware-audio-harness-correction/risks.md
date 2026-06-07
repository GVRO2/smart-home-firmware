# Risks

- False success risk is reduced by the new Unity harness gate, but future test layouts should keep runners simple and auditable.
- Release remains blocked without physical ESP32 evidence for sensors, MQTT, I2S/MAX98357A, speaker output, and power stability.
- Streaming implementation can increase RAM, flash, latency, and failure surface once an HTTP decoder library is selected.
- The local `include/Secrets.h` warning should remain monitored so secrets are not committed.
