param(
    [string]$RunName = "audio-status-tests"
)

$ErrorActionPreference = "Continue"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$stamp = Get-Date -Format "yyyyMMdd-HHmm"
$runDir = Join-Path $repo "docs\evidence\runs\$stamp-$RunName"
New-Item -ItemType Directory -Force -Path $runDir | Out-Null
$log = Join-Path $runDir "audio-status-tests-output.txt"
$testPath = Join-Path $repo "test\test_native\test_mqtt_contract.cpp"
$text = Get-Content -Raw -LiteralPath $testPath
$failures = New-Object System.Collections.Generic.List[string]

function Add-Failure($Message) {
    $script:failures.Add($Message)
    $Message | Tee-Object -FilePath $log -Append
}

function Require-Pattern($Pattern, $Message) {
    if ($text -notmatch $Pattern) {
        Add-Failure $Message
    }
}

function Get-FunctionBody($Text, $FunctionName) {
    $match = [regex]::Match($Text, "(?m)^\s*void\s+$FunctionName\s*\(\s*\)\s*\{")
    if (-not $match.Success) {
        return $null
    }

    $depth = 0
    for ($i = $match.Index; $i -lt $Text.Length; ++$i) {
        if ($Text[$i] -eq "{") {
            ++$depth
        } elseif ($Text[$i] -eq "}") {
            --$depth
            if ($depth -eq 0) {
                return $Text.Substring($match.Index, $i - $match.Index + 1)
            }
        }
    }

    return $null
}

"Checking audio status tests in $testPath" | Tee-Object -FilePath $log

Require-Pattern 'void\s+shouldPublishAcceptedAudioStatus\s*\(\)' "Missing shouldPublishAcceptedAudioStatus"
Require-Pattern 'void\s+shouldPublishPlayingAudioStatus\s*\(\)' "Missing shouldPublishPlayingAudioStatus"
Require-Pattern 'void\s+shouldPublishCompletedAudioStatus\s*\(\)' "Missing shouldPublishCompletedAudioStatus"
Require-Pattern 'void\s+shouldPublishStoppedAudioStatus\s*\(\)' "Missing shouldPublishStoppedAudioStatus"
Require-Pattern 'void\s+shouldPublishRejectedAudioStatus\s*\(\)' "Missing shouldPublishRejectedAudioStatus"
Require-Pattern 'void\s+shouldPublishFailedAudioStatus\s*\(\)' "Missing shouldPublishFailedAudioStatus"

foreach ($state in @("accepted", "playing", "completed", "stopped", "rejected", "failed")) {
    Require-Pattern "`"$state`"" "Missing state literal: $state"
}

$acceptedBody = Get-FunctionBody $text "shouldPublishAcceptedAudioStatus"
if ($null -eq $acceptedBody) {
    Add-Failure "Could not inspect shouldPublishAcceptedAudioStatus"
} else {
    if ($acceptedBody -notmatch 'publisher\.publishStatus\s*\(') {
        Add-Failure "accepted test does not explicitly call publishStatus"
    }
    if ($acceptedBody -notmatch '"accepted"') {
        Add-Failure "accepted test does not validate accepted state"
    }
    if ($acceptedBody -match 'audio_url_unreachable') {
        Add-Failure "accepted test validates failed reason audio_url_unreachable"
    }
    if ($acceptedBody -match 'expired_command') {
        Add-Failure "accepted test validates rejected reason expired_command"
    }
    if ($acceptedBody -notmatch 'assertAudioStatusPayload\s*\(') {
        Add-Failure "accepted test does not validate payload and reason absence through helper"
    }
}

$expectedTests = @(
    @{ Name = "shouldPublishAcceptedAudioStatus"; State = "accepted"; Reason = $null },
    @{ Name = "shouldPublishPlayingAudioStatus"; State = "playing"; Reason = $null },
    @{ Name = "shouldPublishCompletedAudioStatus"; State = "completed"; Reason = $null },
    @{ Name = "shouldPublishStoppedAudioStatus"; State = "stopped"; Reason = $null },
    @{ Name = "shouldPublishRejectedAudioStatus"; State = "rejected"; Reason = "expired_command" },
    @{ Name = "shouldPublishFailedAudioStatus"; State = "failed"; Reason = "audio_url_unreachable" }
)

foreach ($test in $expectedTests) {
    $name = $test.Name
    $body = Get-FunctionBody $text $name
    if ($null -ne $body) {
        if ($body -notmatch 'FakeMqttClient client;') { Add-Failure "$name does not create a clean FakeMqttClient" }
        if ($body -notmatch 'AudioStatusPublisher publisher') { Add-Failure "$name does not create a clean AudioStatusPublisher" }
        if ($body -notmatch 'publisher\.publishStatus\s*\(') { Add-Failure "$name does not explicitly call publishStatus" }
        if ($body -notmatch 'lastTopic\.c_str\(\)') { Add-Failure "$name does not validate topic" }
        if ($body -notmatch 'assertAudioStatusPayload\s*\(') { Add-Failure "$name does not validate payload through assertAudioStatusPayload" }
        if ($body -notmatch "`"$($test.State)`"") { Add-Failure "$name does not validate expected state $($test.State)" }
        if ($null -eq $test.Reason) {
            if ($body -match 'audio_url_unreachable|expired_command') { Add-Failure "$name mixes error reason into non-error status" }
        } elseif ($body -notmatch $test.Reason) {
            Add-Failure "$name does not validate expected reason $($test.Reason)"
        }
    }
}

if ($text -match 'shouldPublishPlayingCompletedAndStoppedAudioStatuses') {
    Add-Failure "Combined playing/completed/stopped status test remains; split tests are required"
}

if ($text -match 'RUN_TEST\s*\(\s*shouldPublishPlayingCompletedAndStoppedAudioStatuses\s*\)\s*;') {
    Add-Failure "RUN_TEST registers obsolete grouped playing/completed/stopped status test"
}

$functionMatches = [regex]::Matches($text, '(?m)^\s*void\s+(shouldPublish[A-Za-z0-9_]*AudioStatus)\s*\(\s*\)\s*\{')
$duplicates = ($functionMatches | ForEach-Object { $_.Groups[1].Value }) | Group-Object | Where-Object { $_.Count -gt 1 }
foreach ($duplicate in $duplicates) {
    Add-Failure "duplicate audio status function definition: $($duplicate.Name)"
}

$lines = Get-Content -LiteralPath $testPath
$returnUnityEndLine = -1
for ($i = 0; $i -lt $lines.Count; ++$i) {
    if ($lines[$i] -match 'return\s+UNITY_END\s*\(\s*\)\s*;') {
        if ($returnUnityEndLine -ge 0) {
            Add-Failure "more than one return UNITY_END() in test main"
        }
        $returnUnityEndLine = $i
    }
}
if ($returnUnityEndLine -ge 0) {
    for ($i = $returnUnityEndLine + 1; $i -lt $lines.Count; ++$i) {
        if ($lines[$i] -match 'RUN_TEST\s*\(') {
            Add-Failure "RUN_TEST appears after return UNITY_END() at line $($i + 1)"
        }
    }
}

if ($text -match 'void\s+shouldPublishRejectedAudioStatus\s*\(\)\s*\{\s*void\s+shouldPublishPlayingAudioStatus\s*\(') {
    Add-Failure "shouldPublishPlayingAudioStatus appears nested in shouldPublishRejectedAudioStatus"
}
if ($text -match 'void\s+shouldPublishFailedAudioStatus\s*\(\)\s*\{\s*void\s+shouldPublishCompletedAudioStatus\s*\(') {
    Add-Failure "shouldPublishCompletedAudioStatus appears nested in shouldPublishFailedAudioStatus"
}
if ($text -match 'void\s+shouldPublishPlayingCompletedAndStoppedAudioStatuses\s*\(\)\s*\{\s*void\s+shouldPublishStoppedAudioStatus\s*\(') {
    Add-Failure "shouldPublishStoppedAudioStatus appears nested in obsolete grouped status test"
}

if ($failures.Count -gt 0) {
    "AUDIO_STATUS_TESTS_FAILED" | Tee-Object -FilePath $log -Append
    exit 1
}

$platformio = Get-Command platformio -ErrorAction SilentlyContinue
if (-not $platformio) {
    $candidate = Join-Path $env:USERPROFILE ".platformio\penv\Scripts\platformio.exe"
    if (Test-Path -LiteralPath $candidate) {
        $platformioPath = $candidate
    } else {
        "AUDIO_STATUS_TESTS_BLOCKED_BY_MISSING_PLATFORMIO" | Tee-Object -FilePath $log -Append
        exit 2
    }
} else {
    $platformioPath = $platformio.Source
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

"exit_code=$code" | Tee-Object -FilePath $log -Append
if ($code -eq 0) {
    "AUDIO_STATUS_TESTS_PASSED" | Tee-Object -FilePath $log -Append
} else {
    "AUDIO_STATUS_TESTS_FAILED" | Tee-Object -FilePath $log -Append
}
exit $code
