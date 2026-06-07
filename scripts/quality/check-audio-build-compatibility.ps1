param(
    [string]$Platformio = "C:\Users\KABUM\.platformio\penv\Scripts\platformio.exe"
)

$ErrorActionPreference = "Continue"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$stamp = Get-Date -Format "yyyyMMdd-HHmm"
$runDir = Join-Path $repo "docs\evidence\runs\$stamp-firmware-audio-build-compatibility"
New-Item -ItemType Directory -Force -Path $runDir | Out-Null

Push-Location $repo
try {
    "Command: $Platformio test -e native`nCommand: $Platformio run" | Set-Content -Encoding UTF8 (Join-Path $runDir "commands.md")

    if (-not (Test-Path -LiteralPath $Platformio)) {
        "BLOCKED_BY_MISSING_TOOL platformio not found at $Platformio" | Set-Content -Encoding UTF8 (Join-Path $runDir "validation-result.md")
        exit 2
    }

    & $Platformio test -e native *>&1 | Tee-Object -FilePath (Join-Path $runDir "native-tests-output.txt")
    $nativeCode = $LASTEXITCODE
    & $Platformio run *>&1 | Tee-Object -FilePath (Join-Path $runDir "build-output.txt")
    $buildCode = $LASTEXITCODE

    if ($nativeCode -eq 0 -and $buildCode -eq 0) {
        "FIRMWARE_AUDIO_BUILD_COMPATIBILITY_PASSED" | Set-Content -Encoding UTF8 (Join-Path $runDir "validation-result.md")
        "FIRMWARE_AUDIO_BUILD_COMPATIBILITY_PASSED"
        exit 0
    }

    "FIRMWARE_AUDIO_BUILD_COMPATIBILITY_FAILED native=$nativeCode build=$buildCode" | Set-Content -Encoding UTF8 (Join-Path $runDir "validation-result.md")
    "FIRMWARE_AUDIO_BUILD_COMPATIBILITY_FAILED"
    exit 1
}
finally {
    Pop-Location
}
