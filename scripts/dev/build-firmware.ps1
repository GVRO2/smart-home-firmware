param(
    [string]$Platformio = "C:\Users\KABUM\.platformio\penv\Scripts\platformio.exe"
)

$ErrorActionPreference = "Stop"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
Push-Location $repo
try {
    if (-not (Test-Path -LiteralPath $Platformio)) {
        Write-Error "BLOCKED_BY_MISSING_TOOL platformio not found at $Platformio"
    }
    & $Platformio run
    exit $LASTEXITCODE
}
finally {
    Pop-Location
}
