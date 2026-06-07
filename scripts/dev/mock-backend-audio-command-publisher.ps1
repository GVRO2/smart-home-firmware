param(
    [string]$BrokerHost = "localhost",
    [int]$BrokerPort = 1883,
    [ValidateSet("play_tone", "play_audio_url", "play_audio_stream", "stop_audio", "set_volume")]
    [string]$CommandType = "play_tone",
    [string]$CommandId = "cmd-local-test-001",
    [string]$DeviceId = "esp32-bedroom-01",
    [string]$Room = "bedroom",
    [int]$Volume = 40,
    [int]$FrequencyHz = 880,
    [int]$DurationMs = 500,
    [string]$AudioUrl = "http://192.168.0.10:8080/audio/messages/msg-001.mp3",
    [string]$StreamUrl = "http://192.168.0.10:8080/audio/stream/session-001"
)

$ErrorActionPreference = "Stop"
$topic = "home/$Room/$DeviceId/audio/command"
$expiresAt = (Get-Date).ToUniversalTime().AddMinutes(5).ToString("yyyy-MM-ddTHH:mm:ssZ")
$requestedAt = (Get-Date).ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ssZ")

$body = [ordered]@{
    commandId = $CommandId
    type = $CommandType
    source = "mock-backend"
    volume = $Volume
    requestedAt = $requestedAt
}

if ($CommandType -eq "play_tone") {
    $body.frequencyHz = $FrequencyHz
    $body.durationMs = $DurationMs
}
if ($CommandType -eq "play_audio_url") {
    $body.audioUrl = $AudioUrl
    $body.format = "mp3"
    $body.expiresAt = $expiresAt
}
if ($CommandType -eq "play_audio_stream") {
    $body.streamUrl = $StreamUrl
    $body.format = "mp3"
    $body.expiresAt = $expiresAt
}

if (-not $body.commandId -or -not $body.type) {
    "FIRMWARE_MOCK_AUDIO_COMMAND_INVALID_CONTRACT"
    exit 1
}

$payload = $body | ConvertTo-Json -Compress
$command = "mosquitto_pub -h $BrokerHost -p $BrokerPort -t $topic -m $payload"
"topic=$topic"
"payload=$payload"
"command=$command"
& mosquitto_pub -h $BrokerHost -p $BrokerPort -t $topic -m $payload
$code = $LASTEXITCODE
"exit_code=$code"
exit $code
