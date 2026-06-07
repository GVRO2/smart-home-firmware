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

Require-Text "include\AppConfig.h" 'DEVICE_EXTERNAL_ID\s*=\s*"esp32-bedroom-01"' "deviceId constant mismatch"
Require-Text "include\AppConfig.h" 'ROOM_SLUG\s*=\s*"bedroom"' "room constant mismatch"
Require-Text "include\AppConfig.h" 'DHT_PIN\s*=\s*4' "DHT pin mismatch"
Require-Text "include\AppConfig.h" 'I2C_SDA_PIN\s*=\s*21' "BH1750 SDA pin mismatch"
Require-Text "include\AppConfig.h" 'I2C_SCL_PIN\s*=\s*22' "BH1750 SCL pin mismatch"
Require-Text "include\AppConfig.h" 'BH1750_I2C_ADDRESS\s*=\s*0x23' "BH1750 address mismatch"
Require-Text "include\AppConfig.h" 'AUDIO_I2S_BCLK_PIN\s*=\s*26' "audio BCLK pin mismatch"
Require-Text "include\AppConfig.h" 'AUDIO_I2S_LRC_PIN\s*=\s*25' "audio LRC pin mismatch"
Require-Text "include\AppConfig.h" 'AUDIO_I2S_DIN_PIN\s*=\s*33' "audio DIN pin mismatch"
Require-Text "include\MqttMessageBuilder.h" 'home/.*environment' "MQTT topic builder missing environment path"
Require-Text "include\AudioConfig.h" 'audio/command' "MQTT audio command topic builder missing"
Require-Text "include\AudioConfig.h" 'audio/status' "MQTT audio status topic builder missing"
Require-Text "include\MqttMessageBuilder.h" 'temperatureCelsius' "temperature field missing"
Require-Text "include\MqttMessageBuilder.h" 'humidityPercentage' "humidity field missing"
Require-Text "include\MqttMessageBuilder.h" 'luminosityLux' "luminosity field missing"
Require-Text "include\MqttMessageBuilder.h" 'measuredAt' "measuredAt field missing"
Require-Text "docs\sdd\firmware-sdd.md" 'home/bedroom/esp32-bedroom-01/environment' "SDD missing official topic"
Require-Text "docs\architecture\firmware-hardware-wiring.md" 'GPIO 21' "wiring doc missing SDA"
Require-Text "docs\architecture\firmware-hardware-wiring.md" 'GPIO 22' "wiring doc missing SCL"
Require-Text "docs\architecture\firmware-hardware-wiring.md" 'GPIO 26' "wiring doc missing audio BCLK"
Require-Text "docs\architecture\firmware-hardware-wiring.md" 'GPIO 25' "wiring doc missing audio LRC"
Require-Text "docs\architecture\firmware-hardware-wiring.md" 'GPIO 33' "wiring doc missing audio DIN"

if ($failures.Count -gt 0) {
    "FIRMWARE_CONTRACT_GATE_FAILED"
    $failures | ForEach-Object { "- $_" }
    exit 1
}

"FIRMWARE_CONTRACT_GATE_PASSED"
exit 0
