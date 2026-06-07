# Contract Drift

## Environmental Contract

Status: CONTRACT_DRIFT_NONE

- Firmware topic: `home/bedroom/esp32-bedroom-01/environment`.
- Backend subscription: `home/+/+/environment`.
- Shared payload: `deviceId`, `room`, `measuredAt`, `temperatureCelsius`, `humidityPercentage`, `luminosityLux`.
- Partial sensor payload is documented on firmware and supported by backend constraints/tests.

## Audio Contract

Status: CONTRACT_DRIFT_MEDIUM

Firmware has command/status contract and tests. Backend has safe advisory architecture, but this review did not execute real backend command publishing or firmware status ingestion.
