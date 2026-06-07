$ErrorActionPreference = "Stop"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$failures = New-Object System.Collections.Generic.List[string]

$required = @(
    "docs\sdd\firmware-sdd.md",
    "docs\architecture\firmware-architecture.md",
    "docs\architecture\firmware-data-flow.md",
    "docs\architecture\firmware-hardware-wiring.md",
    "docs\architecture\firmware-security.md",
    "docs\decisions\ADR-0004-audio-output-local-ai-communication.md",
    "docs\backlog\firmware-backlog.md",
    "docs\squad\firmware-agent-memory.md"
)

$combined = ""
foreach ($file in $required) {
    $path = Join-Path $repo $file
    if (-not (Test-Path -LiteralPath $path)) {
        $failures.Add("Missing required audio doc: $file")
    } else {
        $combined += "`n" + (Get-Content -Raw -LiteralPath $path)
    }
}

foreach ($term in @(
    "ADR-0004",
    "MAX98357A",
    "GPIO 26",
    "GPIO 25",
    "GPIO 33",
    "home/bedroom/esp32-bedroom-01/audio/command",
    "home/bedroom/esp32-bedroom-01/audio/status",
    "play_audio_url",
    "play_audio_stream",
    "play_tone",
    "stop_audio",
    "set_volume",
    "URL publica",
    "microfone",
    "wake word"
)) {
    if ($combined -notmatch [regex]::Escape($term)) {
        $failures.Add("Missing audio documentation term: $term")
    }
}

if ($failures.Count -gt 0) {
    "FIRMWARE_AUDIO_DOCS_CONSISTENCY_FAILED"
    $failures | ForEach-Object { "- $_" }
    exit 1
}

"FIRMWARE_AUDIO_DOCS_CONSISTENCY_PASSED"
exit 0
