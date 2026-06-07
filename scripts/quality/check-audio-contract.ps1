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

Require-Text "include\AudioConfig.h" 'audio/command' "Audio command topic missing in code"
Require-Text "include\AudioConfig.h" 'audio/status' "Audio status topic missing in code"
Require-Text "include\AudioCommand.h" 'play_audio_url' "play_audio_url missing"
Require-Text "include\AudioCommand.h" 'play_audio_stream' "play_audio_stream missing"
Require-Text "include\AudioCommand.h" 'play_tone' "play_tone missing"
Require-Text "include\AudioCommand.h" 'stop_audio' "stop_audio missing"
Require-Text "include\AudioCommand.h" 'set_volume' "set_volume missing"
Require-Text "src\AudioCommandParser.cpp" 'commandId' "commandId validation missing"
Require-Text "src\AudioCommandParser.cpp" 'invalid_volume' "volume validation missing"
Require-Text "src\AudioCommandParser.cpp" 'isLocalHttpUrl' "local URL validation missing"
Require-Text "include\AppConfig.h" 'AUDIO_MAX_DURATION_MS' "max duration missing"
Require-Text "src\AudioStatusPublisher.cpp" 'accepted|state' "status publisher missing state"
Require-Text "docs\sdd\firmware-sdd.md" 'accepted' "accepted state missing in docs"
Require-Text "docs\sdd\firmware-sdd.md" 'playing' "playing state missing in docs"
Require-Text "docs\sdd\firmware-sdd.md" 'completed' "completed state missing in docs"
Require-Text "docs\sdd\firmware-sdd.md" 'rejected' "rejected state missing in docs"
Require-Text "docs\sdd\firmware-sdd.md" 'failed' "failed state missing in docs"

if ($failures.Count -gt 0) {
    "FIRMWARE_AUDIO_CONTRACT_GATE_FAILED"
    $failures | ForEach-Object { "- $_" }
    exit 1
}

"FIRMWARE_AUDIO_CONTRACT_GATE_PASSED"
exit 0
