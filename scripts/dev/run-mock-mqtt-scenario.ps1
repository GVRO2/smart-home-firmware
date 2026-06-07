param(
    [string]$BrokerHost = "localhost",
    [int]$BrokerPort = 1883,
    [string]$RunName = "firmware-mock-mqtt-scenario"
)

$ErrorActionPreference = "Continue"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$stamp = Get-Date -Format "yyyyMMdd-HHmm"
$runDir = Join-Path $repo "docs\evidence\runs\$stamp-$RunName"
New-Item -ItemType Directory -Force -Path $runDir | Out-Null
$log = Join-Path $runDir "mock-mqtt-scenario-output.txt"

if (-not (Get-Command mosquitto_pub -ErrorAction SilentlyContinue)) {
    "BLOCKED_BY_BROKER_TOOL_UNAVAILABLE: mosquitto_pub" | Tee-Object -FilePath $log
    exit 2
}
if (-not (Get-Command mosquitto_sub -ErrorAction SilentlyContinue)) {
    "BLOCKED_BY_BROKER_TOOL_UNAVAILABLE: mosquitto_sub" | Tee-Object -FilePath $log
    exit 2
}

Push-Location $repo
try {
    & mosquitto_pub -h $BrokerHost -p $BrokerPort -t "home/healthcheck" -m "ping" *>&1 | Tee-Object -FilePath $log -Append
    if ($LASTEXITCODE -ne 0) {
        "BLOCKED_BY_BROKER_UNAVAILABLE" | Tee-Object -FilePath $log -Append
        exit 2
    }

    & .\scripts\dev\mock-esp32-environment-publisher.ps1 -BrokerHost $BrokerHost -BrokerPort $BrokerPort *>&1 | Tee-Object -FilePath $log -Append
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & .\scripts\dev\mock-backend-audio-command-publisher.ps1 -BrokerHost $BrokerHost -BrokerPort $BrokerPort *>&1 | Tee-Object -FilePath $log -Append
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
    & .\scripts\dev\mock-esp32-audio-status-publisher.ps1 -BrokerHost $BrokerHost -BrokerPort $BrokerPort *>&1 | Tee-Object -FilePath $log -Append
    if ($LASTEXITCODE -ne 0) { exit $LASTEXITCODE }
}
finally {
    Pop-Location
}

"FIRMWARE_MOCK_MQTT_SCENARIO_PASSED" | Tee-Object -FilePath $log -Append
exit 0
