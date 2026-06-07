$ErrorActionPreference = "Stop"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$failures = New-Object System.Collections.Generic.List[string]

function Require-Path($Path) {
    if (-not (Test-Path -LiteralPath (Join-Path $repo $Path))) {
        $script:failures.Add("Missing file: $Path")
    }
}

@(
    "include\EnvironmentSensor.h",
    "include\LightSensor.h",
    "include\AudioOutputDriver.h",
    "include\MqttPublisherPort.h",
    "include\ClockProvider.h",
    "include\NetworkConnection.h",
    "include\mocks\MockEnvironmentSensor.h",
    "include\mocks\MockLightSensor.h",
    "include\mocks\MockAudioOutputDriver.h",
    "include\mocks\MockMqttPublisher.h",
    "include\mocks\MockClockProvider.h",
    "include\mocks\MockNetworkConnection.h"
) | ForEach-Object { Require-Path $_ }

$nativeTest = Get-Content -Raw -LiteralPath (Join-Path $repo "test\test_native\test_mqtt_contract.cpp")
if ($nativeTest -notmatch 'MockAudioOutputDriver') {
    $failures.Add("native audio tests do not use MockAudioOutputDriver")
}

if ($failures.Count -gt 0) {
    "FIRMWARE_HARDWARE_ABSTRACTIONS_GATE_FAILED"
    $failures | ForEach-Object { "- $_" }
    exit 1
}

"FIRMWARE_HARDWARE_ABSTRACTIONS_GATE_PASSED"
exit 0
