# Risks

- `check-test-source-sanity.ps1` is intentionally conservative and regex/brace-depth based; compiler and native tests remain mandatory.
- `include/Secrets.h` exists locally and produces a warning, but it is expected to remain untracked and ignored.
- Release remains blocked without real physical validation evidence for ESP32 hardware, serial logs, sensors, I2S/MAX98357A, and speaker output.
