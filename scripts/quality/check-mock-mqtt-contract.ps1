$ErrorActionPreference = "Stop"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$failures = New-Object System.Collections.Generic.List[string]

function Require-Text($Path, $Pattern, $Message) {
    $full = Join-Path $repo $Path
    if (-not (Test-Path -LiteralPath $full)) {
        $script:failures.Add("Missing file: $Path")
        return
    }
    $text = Get-Content -Raw -LiteralPath $full
    if ($text -notmatch $Pattern) {
        $script:failures.Add($Message)
    }
}

Require-Text "scripts\dev\mock-esp32-environment-publisher.ps1" 'home/\$Room/\$DeviceId/environment' "environment mock topic is missing"
Require-Text "scripts\dev\mock-esp32-environment-publisher.ps1" 'temperatureCelsius' "environment mock missing temperature"
Require-Text "scripts\dev\mock-esp32-environment-publisher.ps1" 'humidityPercentage' "environment mock missing humidity"
Require-Text "scripts\dev\mock-esp32-environment-publisher.ps1" 'luminosityLux' "environment mock missing luminosity"
Require-Text "scripts\dev\mock-backend-audio-command-publisher.ps1" 'home/\$Room/\$DeviceId/audio/command' "audio command mock topic is missing"
Require-Text "scripts\dev\mock-backend-audio-command-publisher.ps1" 'commandId' "audio command mock missing commandId"
Require-Text "scripts\dev\mock-backend-audio-command-publisher.ps1" 'play_tone' "audio command mock missing play_tone"
Require-Text "scripts\dev\mock-esp32-audio-status-publisher.ps1" 'home/\$Room/\$DeviceId/audio/status' "audio status mock topic is missing"
Require-Text "scripts\dev\mock-esp32-audio-status-publisher.ps1" 'accepted.*playing.*completed.*rejected.*failed.*stopped' "audio status mock missing status set"
Require-Text "scripts\dev\run-mock-mqtt-scenario.ps1" 'BLOCKED_BY_BROKER_TOOL_UNAVAILABLE' "scenario missing tool-block status"
Require-Text "scripts\dev\run-mock-mqtt-scenario.ps1" 'BLOCKED_BY_BROKER_UNAVAILABLE' "scenario missing broker-block status"

$scriptText = Get-ChildItem -Path (Join-Path $repo "scripts\dev") -Filter "*.ps1" | Get-Content -Raw
if ($scriptText -match 'https?://(?!192\.168\.|10\.|127\.0\.0\.1|localhost)') {
    $failures.Add("mock scripts contain a public hardcoded URL")
}

if ($failures.Count -gt 0) {
    "FIRMWARE_MOCK_MQTT_CONTRACT_GATE_FAILED"
    $failures | ForEach-Object { "- $_" }
    exit 1
}

"FIRMWARE_MOCK_MQTT_CONTRACT_GATE_PASSED"
exit 0
