param(
    [string]$RunName = "firmware-build",
    [string]$Platformio = "C:\Users\KABUM\.platformio\penv\Scripts\platformio.exe"
)

$ErrorActionPreference = "Continue"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$stamp = Get-Date -Format "yyyyMMdd-HHmm"
$runDir = Join-Path $repo "docs\evidence\runs\$stamp-$RunName"
New-Item -ItemType Directory -Force -Path $runDir | Out-Null
$output = Join-Path $runDir "build-output.txt"

Push-Location $repo
try {
    "# Firmware Build Evidence`n" | Set-Content -Encoding UTF8 (Join-Path $runDir "summary.md")
    "Command: $Platformio run`nDirectory: $repo`n" | Set-Content -Encoding UTF8 (Join-Path $runDir "commands.md")

    if (-not (Test-Path -LiteralPath $Platformio)) {
        "BLOCKED_BY_MISSING_TOOL platformio not found at $Platformio" | Set-Content -Encoding UTF8 $output
        "Status: BLOCKED_BY_MISSING_TOOL" | Set-Content -Encoding UTF8 (Join-Path $runDir "validation-result.md")
        exit 2
    }

    & $Platformio run *>&1 | Tee-Object -FilePath $output
    $code = $LASTEXITCODE
    "Exit Code: $code" | Add-Content -Encoding UTF8 (Join-Path $runDir "commands.md")
    if ($code -eq 0) {
        "Status: PASSED" | Set-Content -Encoding UTF8 (Join-Path $runDir "validation-result.md")
    } else {
        "Status: FAILED" | Set-Content -Encoding UTF8 (Join-Path $runDir "validation-result.md")
    }
    exit $code
}
finally {
    Pop-Location
}
