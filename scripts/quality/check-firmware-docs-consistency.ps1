$ErrorActionPreference = "Stop"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$required = @(
    "docs\sdd\firmware-sdd.md",
    "docs\architecture\firmware-hardware-wiring.md",
    "docs\governance\firmware-governance.md",
    "docs\backlog\firmware-backlog.md",
    "docs\squad\firmware-agent-memory.md",
    "docs\governance\evidence-policy.md",
    "docs\governance\quality-gates.md"
)
$failures = New-Object System.Collections.Generic.List[string]

foreach ($file in $required) {
    if (-not (Test-Path -LiteralPath (Join-Path $repo $file))) {
        $failures.Add("Missing required doc: $file")
    }
}

$combined = ""
foreach ($file in $required) {
    $path = Join-Path $repo $file
    if (Test-Path -LiteralPath $path) {
        $combined += "`n" + (Get-Content -Raw -LiteralPath $path)
    }
}

foreach ($term in @("POC", "fora do escopo", "local-first", "home/bedroom/esp32-bedroom-01/environment", "sem rele", "sem comandos remotos")) {
    if ($combined -notmatch [regex]::Escape($term)) {
        $failures.Add("Missing required documentation term: $term")
    }
}

if ($failures.Count -gt 0) {
    "FIRMWARE_DOCS_CONSISTENCY_FAILED"
    $failures | ForEach-Object { "- $_" }
    exit 1
}

"FIRMWARE_DOCS_CONSISTENCY_PASSED"
exit 0
