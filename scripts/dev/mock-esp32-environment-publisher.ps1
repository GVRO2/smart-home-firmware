param(
    [string]$BrokerHost = "localhost",
    [int]$BrokerPort = 1883,
    [string]$DeviceId = "esp32-bedroom-01",
    [string]$Room = "bedroom",
    [double]$TemperatureCelsius = 27.4,
    [double]$HumidityPercentage = 62.5,
    [double]$LuminosityLux = 18.0,
    [int]$Count = 1,
    [int]$IntervalMs = 1000
)

$ErrorActionPreference = "Stop"
$topic = "home/$Room/$DeviceId/environment"

for ($i = 0; $i -lt $Count; $i++) {
    $payload = [ordered]@{
        deviceId = $DeviceId
        room = $Room
        temperatureCelsius = $TemperatureCelsius
        humidityPercentage = $HumidityPercentage
        luminosityLux = $LuminosityLux
        measuredAt = (Get-Date).ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ssZ")
    } | ConvertTo-Json -Compress

    $command = "mosquitto_pub -h $BrokerHost -p $BrokerPort -t $topic -m $payload"
    "topic=$topic"
    "payload=$payload"
    "command=$command"

    & mosquitto_pub -h $BrokerHost -p $BrokerPort -t $topic -m $payload
    $code = $LASTEXITCODE
    "exit_code=$code"
    if ($code -ne 0) { exit $code }
    if ($i -lt ($Count - 1)) { Start-Sleep -Milliseconds $IntervalMs }
}
