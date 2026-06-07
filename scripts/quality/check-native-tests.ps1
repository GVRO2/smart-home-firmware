param(
    [string]$RunName = "firmware-native-tests"
)

$ErrorActionPreference = "Continue"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$stamp = Get-Date -Format "yyyyMMdd-HHmm"
$runDir = Join-Path $repo "docs\evidence\runs\$stamp-$RunName"
New-Item -ItemType Directory -Force -Path $runDir | Out-Null
$log = Join-Path $runDir "native-tests-output.txt"

$platformio = Get-Command platformio -ErrorAction SilentlyContinue
if (-not $platformio) {
    $candidate = Join-Path $env:USERPROFILE ".platformio\penv\Scripts\platformio.exe"
    if (Test-Path -LiteralPath $candidate) {
        $platformioPath = $candidate
    } else {
        "FIRMWARE_NATIVE_TESTS_BLOCKED_BY_MISSING_PLATFORMIO" | Tee-Object -FilePath $log
        exit 2
    }
} else {
    $platformioPath = $platformio.Source
}

$harnessGate = Join-Path $repo "scripts\quality\check-unreachable-unity-tests.ps1"
$harnessPreOutput = & powershell -ExecutionPolicy Bypass -File $harnessGate -TestPath "test\test_native" *>&1
$harnessPreCode = $LASTEXITCODE
"Command: powershell -ExecutionPolicy Bypass -File $harnessGate -TestPath test\test_native" | Tee-Object -FilePath $log
$harnessPreOutput | Tee-Object -FilePath $log -Append
if ($harnessPreCode -ne 0) {
    "exit_code=$harnessPreCode" | Tee-Object -FilePath $log -Append
    "FIRMWARE_NATIVE_TESTS_FAILED_BY_UNITY_HARNESS" | Tee-Object -FilePath $log -Append
    exit 1
}

Push-Location $repo
try {
    "Command: $platformioPath test -e native" | Tee-Object -FilePath $log -Append
    & $platformioPath test -e native *>&1 | Tee-Object -FilePath $log -Append
    $code = $LASTEXITCODE
}
finally {
    Pop-Location
}

$testFiles = Get-ChildItem -Path (Join-Path $repo "test\test_native") -Recurse -File -Include "*.cpp", "*.h"
$allText = ($testFiles | ForEach-Object { Get-Content -Raw -LiteralPath $_.FullName }) -join "`n"
$registeredCount = ([regex]::Matches($allText, 'RUN_TEST\s*\(')).Count
$nativeOutput = Get-Content -Raw -LiteralPath $log
$executedCount = ([regex]::Matches($nativeOutput, '(?m)^\s*test[^\r\n]*\s+\[PASSED\]\s*$')).Count
if ($executedCount -eq 0 -and $nativeOutput -match '(\d+)\s+Tests\s+0\s+Failures') {
    $executedCount = [int]$Matches[1]
}

"registered_run_test_count=$registeredCount" | Tee-Object -FilePath $log -Append
"executed_test_count=$executedCount" | Tee-Object -FilePath $log -Append

$harnessPostOutput = & powershell -ExecutionPolicy Bypass -File $harnessGate -TestPath "test\test_native" -NativeOutput $log *>&1
$harnessPostCode = $LASTEXITCODE
"Command: powershell -ExecutionPolicy Bypass -File $harnessGate -TestPath test\test_native -NativeOutput $log" | Tee-Object -FilePath $log -Append
$harnessPostOutput | Tee-Object -FilePath $log -Append

"exit_code=$code" | Tee-Object -FilePath $log -Append
if ($code -eq 0 -and $harnessPostCode -eq 0 -and $registeredCount -eq $executedCount -and $executedCount -gt 0) {
    "FIRMWARE_NATIVE_TESTS_PASSED" | Tee-Object -FilePath $log -Append
} else {
    "FIRMWARE_NATIVE_TESTS_FAILED" | Tee-Object -FilePath $log -Append
    if ($code -eq 0 -and $executedCount -eq 0) {
        "reason=no_executed_test_count_detected" | Tee-Object -FilePath $log -Append
    }
    if ($registeredCount -ne $executedCount) {
        "reason=registered_count_does_not_match_executed_count" | Tee-Object -FilePath $log -Append
    }
    if ($harnessPostCode -ne 0) {
        "reason=unity_harness_gate_failed_after_native_run" | Tee-Object -FilePath $log -Append
    }
}
exit $(if ($code -eq 0 -and $harnessPostCode -eq 0 -and $registeredCount -eq $executedCount -and $executedCount -gt 0) { 0 } else { 1 })
