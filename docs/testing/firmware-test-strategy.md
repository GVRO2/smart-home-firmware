# Firmware Test Strategy

## Principle

Mock para logica. Simulacao para contrato. Hardware real para fisica. Release somente com evidencia fisica.

## Levels

1. Native automatic tests run on the PC with `platformio test -e native`. They validate pure logic, payload contracts, parsers, queues, status JSON, and orchestration through mocks.
2. MQTT simulation runs on the PC with local broker tooling. It validates environment publications, backend audio commands, and ESP32 audio status messages without an ESP32.
3. Physical validation runs with the ESP32 DevKit V1 connected. It validates upload, boot, serial logs, Wi-Fi, MQTT, DHT22, BH1750, MAX98357A/I2S, speaker output, brownout behavior, and interactions between audio and environment publishing.
4. Release readiness runs automatic gates and requires current physical validation or recent compatible physical evidence.

`check-unreachable-unity-tests.ps1` protects the native runner from false success by verifying that no `RUN_TEST` appears after `return UNITY_END();`, that each test runner has a single Unity return, and that native output count can be compared with registered tests.

## Failure Interpretation

Native build or test failures are code failures. Missing PlatformIO is an environment block. Missing Mosquitto blocks only MQTT simulation. Missing ESP32 blocks physical validation and release readiness, but it does not fail automatic gates.

## Commands

```powershell
.\scripts\quality\check-native-tests.ps1
.\scripts\quality\check-unreachable-unity-tests.ps1
.\scripts\quality\check-mock-mqtt-contract.ps1
.\scripts\governance\run-firmware-approval-review.ps1 -Mode automatic
.\scripts\governance\run-firmware-approval-review.ps1 -Mode physical
.\scripts\governance\run-firmware-approval-review.ps1 -Mode release
```
