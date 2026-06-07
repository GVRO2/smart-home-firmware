powershell -ExecutionPolicy Bypass -File scripts\quality\check-test-source-sanity.ps1 
powershell -ExecutionPolicy Bypass -File scripts\quality\check-audio-status-tests.ps1 -RunName firmware-approval-audio-status-tests
powershell -ExecutionPolicy Bypass -File scripts\quality\check-unreachable-unity-tests.ps1 
powershell -ExecutionPolicy Bypass -File scripts\quality\check-native-tests.ps1 -RunName firmware-approval-native-tests
powershell -ExecutionPolicy Bypass -File scripts\quality\check-firmware-build.ps1 -RunName firmware-approval-build
powershell -ExecutionPolicy Bypass -File scripts\quality\check-firmware-contract.ps1 
powershell -ExecutionPolicy Bypass -File scripts\quality\check-audio-contract.ps1 
powershell -ExecutionPolicy Bypass -File scripts\quality\check-audio-security.ps1 
powershell -ExecutionPolicy Bypass -File scripts\quality\check-audio-docs-consistency.ps1 
powershell -ExecutionPolicy Bypass -File scripts\quality\check-firmware-docs-consistency.ps1 
powershell -ExecutionPolicy Bypass -File scripts\quality\check-secrets.ps1 
powershell -ExecutionPolicy Bypass -File scripts\quality\check-mock-mqtt-contract.ps1 
powershell -ExecutionPolicy Bypass -File scripts\quality\check-hardware-abstractions.ps1 
powershell -ExecutionPolicy Bypass -File scripts\quality\check-physical-validation-policy.ps1 
