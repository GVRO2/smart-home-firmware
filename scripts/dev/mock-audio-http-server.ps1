param(
    [int]$Port = 8080,
    [string]$Directory = "docs\test-assets\audio"
)

$ErrorActionPreference = "Stop"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$fullDirectory = Join-Path $repo $Directory

if (-not (Test-Path -LiteralPath $fullDirectory)) {
    New-Item -ItemType Directory -Force -Path $fullDirectory | Out-Null
}

$python = Get-Command python -ErrorAction SilentlyContinue
if (-not $python) {
    $python = Get-Command py -ErrorAction SilentlyContinue
}
if (-not $python) {
    "BLOCKED_BY_MISSING_TOOL: python"
    exit 2
}

"Serving local audio test files only."
"directory=$fullDirectory"
"port=$Port"
Get-ChildItem -LiteralPath $fullDirectory | ForEach-Object { "file=$($_.Name)" }
& $python.Source -m http.server $Port --bind 127.0.0.1 --directory $fullDirectory
exit $LASTEXITCODE
