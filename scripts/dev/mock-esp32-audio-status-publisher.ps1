param(
    [string]$BrokerHost = "localhost",
    [int]$BrokerPort = 1883,
    [string]$DeviceId = "esp32-bedroom-01",
    [string]$Room = "bedroom",
    [ValidateSet("accepted", "playing", "completed", "rejected", "failed", "stopped")]
    [string]$State = "completed",
    [string]$CommandId = "cmd-local-test-001",
    [string]$Reason = "",
    [string]$Message = "Audio playback completed"
)

$ErrorActionPreference = "Stop"
$topic = "home/$Room/$DeviceId/audio/status"
$body = [ordered]@{
    deviceId = $DeviceId
    room = $Room
    commandId = $CommandId
    state = $State
    message = $Message
    reportedAt = (Get-Date).ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ssZ")
}
if ($Reason) { $body.reason = $Reason }

$payload = $body | ConvertTo-Json -Compress
$command = "mosquitto_pub -h $BrokerHost -p $BrokerPort -t $topic -m $payload"
"topic=$topic"
"payload=$payload"
"command=$command"
& mosquitto_pub -h $BrokerHost -p $BrokerPort -t $topic -m $payload
$code = $LASTEXITCODE
"exit_code=$code"
exit $code
