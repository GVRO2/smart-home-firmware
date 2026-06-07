# Commands

```powershell
powershell -ExecutionPolicy Bypass -File scripts\quality\check-unreachable-unity-tests.ps1
powershell -ExecutionPolicy Bypass -File scripts\quality\check-unreachable-unity-tests.ps1 -TestPath test\test_native
powershell -ExecutionPolicy Bypass -File scripts\quality\check-native-tests.ps1 -RunName firmware-audio-harness-correction-native
powershell -ExecutionPolicy Bypass -File scripts\quality\check-mock-mqtt-contract.ps1
powershell -ExecutionPolicy Bypass -File scripts\quality\check-audio-docs-consistency.ps1
powershell -ExecutionPolicy Bypass -File scripts\quality\check-firmware-build.ps1 -RunName firmware-audio-harness-correction-build
powershell -ExecutionPolicy Bypass -File scripts\dev\run-mock-mqtt-scenario.ps1 -RunName firmware-audio-harness-correction-mock-mqtt
powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1 -Mode automatic
```

The native test and ESP32 build commands were rerun outside the sandbox because PlatformIO could not access `C:\Users\KABUM\.platformio\platforms.lock` from the sandbox.
