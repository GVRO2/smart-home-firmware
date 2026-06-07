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

Require-Text "include\AppConfig.h" 'AUDIO_PLAYBACK_ENABLED' "audio feature flag missing"
Require-Text "include\AppConfig.h" 'AUDIO_MAX_DURATION_MS' "audio max duration missing"
Require-Text "src\AudioCommandParser.cpp" 'http://' "http local URL policy missing"
Require-Text "src\AudioCommandParser.cpp" '192\.168\.|10\.|172\.' "private-network URL allowlist missing"
Require-Text "src\AudioCommandParser.cpp" 'public_or_invalid' "public URL rejection reason missing"
Require-Text "src\AudioCommandParser.cpp" 'missing_command_id' "missing commandId rejection missing"
Require-Text "src\AudioCommandParser.cpp" 'invalid_volume' "volume limit rejection missing"
Require-Text "src\AudioPlaybackService.cpp" 'StopAudio' "stop_audio handling missing"
Require-Text "src\main.cpp" 'audioCommandTopic != topic' "specific audio topic guard missing"
Require-Text "docs\architecture\firmware-security.md" 'Nao adicionar microfone' "microphone prohibition missing"
Require-Text "docs\architecture\firmware-security.md" 'wake word' "wake word prohibition missing"
Require-Text "docs\architecture\firmware-security.md" 'Nao expor ESP32' "public exposure prohibition missing"

if ($failures.Count -gt 0) {
    "FIRMWARE_AUDIO_SECURITY_GATE_FAILED"
    $failures | ForEach-Object { "- $_" }
    exit 1
}

"FIRMWARE_AUDIO_SECURITY_GATE_PASSED"
exit 0
