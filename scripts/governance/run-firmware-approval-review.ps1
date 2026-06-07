param(
    [ValidateSet("automatic", "physical", "release")]
    [string]$Mode = "automatic"
)

$ErrorActionPreference = "Continue"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$stamp = Get-Date -Format "yyyyMMdd-HHmm"
$runDir = Join-Path $repo "docs\evidence\runs\$stamp-firmware-approval-review-$Mode"
$report = Join-Path $repo "docs\audit\tmp\firmware-approval-review-report.md"
New-Item -ItemType Directory -Force -Path $runDir | Out-Null
New-Item -ItemType Directory -Force -Path (Join-Path $runDir "logs") | Out-Null

function Write-EvidenceFiles($Status, $Rows, $Commands, $Warnings, $Failures) {
    "# Firmware Approval Review Evidence`n`nMode: $Mode`nStatus: $Status`n" | Set-Content -Encoding UTF8 (Join-Path $runDir "summary.md")
    ($Commands -join "`n") | Set-Content -Encoding UTF8 (Join-Path $runDir "commands.md")
    $Status | Set-Content -Encoding UTF8 (Join-Path $runDir "validation-result.md")
    $riskText = if ($Failures.Count -eq 0) { "No gate failures recorded." } else { ($Failures | ForEach-Object { "- $_" }) -join "`n" }
    $nextText = if ($Mode -eq "automatic") { "Next: run physical validation before release readiness." } else { "Next: resolve blockers and capture physical evidence." }
    $riskText | Set-Content -Encoding UTF8 (Join-Path $runDir "risks.md")
    $nextText | Set-Content -Encoding UTF8 (Join-Path $runDir "next-steps.md")

@"
# Firmware Approval Review Report

## 1. Status

$Status

## 2. Mode

$Mode

## 3. Gates

| Gate | Status | Exit Code | Evidence |
|---|---:|---:|---|
$($Rows -join "`n")

## 4. Evidence

- $runDir

## 5. Warnings

$($(if ($Warnings.Count -eq 0) { "- None" } else { ($Warnings | ForEach-Object { "- $_" }) -join "`n" }))

## 6. Failures

$($(if ($Failures.Count -eq 0) { "- None" } else { ($Failures | ForEach-Object { "- $_" }) -join "`n" }))
"@ | Set-Content -Encoding UTF8 $report
}

function Invoke-Gate($Name, $Script, $ArgsList, $Rows, $Commands, $Warnings, $Failures) {
    $log = Join-Path $runDir ("logs\" + ($Name -replace ' ', '-').ToLowerInvariant() + ".txt")
    $command = "powershell -ExecutionPolicy Bypass -File $Script $($ArgsList -join ' ')"
    $Commands.Add($command)
    Push-Location $repo
    try {
        powershell -ExecutionPolicy Bypass -File $Script @ArgsList *>&1 | Tee-Object -FilePath $log
        $code = $LASTEXITCODE
    }
    finally {
        Pop-Location
    }
    $output = Get-Content -Raw -LiteralPath $log
    $status = if ($code -eq 0 -and $output -match "WARNING|PASSED_WITH_WARNINGS") { "PASSED_WITH_WARNINGS" } elseif ($code -eq 0) { "PASSED" } elseif ($code -eq 2) { "BLOCKED" } else { "FAILED" }
    $Rows.Add("| $Name | $status | $code | $log |")
    if ($status -eq "PASSED_WITH_WARNINGS") { $Warnings.Add("$Name passed with warnings") }
    if ($status -eq "FAILED") { $Failures.Add("$Name failed") }
    if ($status -eq "BLOCKED") { $Failures.Add("$Name blocked") }
}

$rows = New-Object System.Collections.Generic.List[string]
$warnings = New-Object System.Collections.Generic.List[string]
$failures = New-Object System.Collections.Generic.List[string]
$commands = New-Object System.Collections.Generic.List[string]

if ($Mode -eq "automatic" -or $Mode -eq "release") {
    $gates = @(
        @{ Name = "Test Source Sanity"; Script = "scripts\quality\check-test-source-sanity.ps1"; Args = @() },
        @{ Name = "Audio Status Tests"; Script = "scripts\quality\check-audio-status-tests.ps1"; Args = @("-RunName", "firmware-approval-audio-status-tests") },
        @{ Name = "Unreachable Unity Tests"; Script = "scripts\quality\check-unreachable-unity-tests.ps1"; Args = @() },
        @{ Name = "Native Tests"; Script = "scripts\quality\check-native-tests.ps1"; Args = @("-RunName", "firmware-approval-native-tests") },
        @{ Name = "Build"; Script = "scripts\quality\check-firmware-build.ps1"; Args = @("-RunName", "firmware-approval-build") },
        @{ Name = "Contract"; Script = "scripts\quality\check-firmware-contract.ps1"; Args = @() },
        @{ Name = "Pin Collision Check"; Script = "scripts\quality\check-pin-collision.ps1"; Args = @() },
        @{ Name = "Audio Contract"; Script = "scripts\quality\check-audio-contract.ps1"; Args = @() },
        @{ Name = "Audio Security"; Script = "scripts\quality\check-audio-security.ps1"; Args = @() },
        @{ Name = "Audio Docs Consistency"; Script = "scripts\quality\check-audio-docs-consistency.ps1"; Args = @() },
        @{ Name = "Docs Consistency"; Script = "scripts\quality\check-firmware-docs-consistency.ps1"; Args = @() },
        @{ Name = "Secrets"; Script = "scripts\quality\check-secrets.ps1"; Args = @() },
        @{ Name = "Mock MQTT Contract"; Script = "scripts\quality\check-mock-mqtt-contract.ps1"; Args = @() },
        @{ Name = "Hardware Abstractions"; Script = "scripts\quality\check-hardware-abstractions.ps1"; Args = @() },
        @{ Name = "Physical Validation Policy"; Script = "scripts\quality\check-physical-validation-policy.ps1"; Args = @() }
    )

    foreach ($gate in $gates) {
        Invoke-Gate $gate.Name $gate.Script $gate.Args $rows $commands $warnings $failures
    }
}

if ($Mode -eq "physical") {
    $commands.Add("Physical validation requires ESP32 upload, serial monitor, sensor, MQTT, and audio evidence.")
    $portLine = Select-String -Path (Join-Path $repo "platformio.ini") -Pattern '^upload_port\s*=\s*(.+)$' | Select-Object -First 1
    $port = if ($portLine) { $portLine.Matches[0].Groups[1].Value.Trim() } else { "COM5" }
    $portPresent = [System.IO.Ports.SerialPort]::GetPortNames() -contains $port
    if (-not $portPresent) {
        $status = "FIRMWARE_PHYSICAL_VALIDATION_BLOCKED_BY_MISSING_HARDWARE"
        $rows.Add("| Physical Hardware | BLOCKED | 2 | Port $port not present |")
        $failures.Add($status)
        Write-EvidenceFiles $status $rows $commands $warnings $failures
        $status
        exit 2
    }

    $status = "FIRMWARE_PHYSICAL_VALIDATION_BLOCKED_BY_PORT_UNAVAILABLE"
    $rows.Add("| Physical Hardware | BLOCKED | 2 | Port $port present but physical validation must be run manually with evidence |")
    $failures.Add($status)
    Write-EvidenceFiles $status $rows $commands $warnings $failures
    $status
    exit 2
}

if ($Mode -eq "release" -and $failures.Count -eq 0) {
    $physicalEvidence = Get-ChildItem -Path (Join-Path $repo "docs\evidence\runs") -Directory -ErrorAction SilentlyContinue |
        Where-Object {
            $_.Name -match "physical|hardware" -and
            (Test-Path -LiteralPath (Join-Path $_.FullName "validation-result.md")) -and
            ((Get-Content -Raw -LiteralPath (Join-Path $_.FullName "validation-result.md")) -match 'FIRMWARE_PHYSICAL_VALIDATION_PASSED')
        } |
        Select-Object -First 1
    if (-not $physicalEvidence) {
        $status = "FIRMWARE_RELEASE_READINESS_BLOCKED_BY_MISSING_PHYSICAL_EVIDENCE"
        $rows.Add("| Physical Evidence | BLOCKED | 2 | No compatible physical evidence directory found |")
        $failures.Add($status)
        Write-EvidenceFiles $status $rows $commands $warnings $failures
        $status
        exit 2
    }
}

$status = if ($failures.Count -eq 0 -and $warnings.Count -eq 0) {
    "FIRMWARE_APPROVAL_REVIEW_PASSED"
} elseif ($failures.Count -eq 0) {
    "FIRMWARE_APPROVAL_REVIEW_PASSED_WITH_WARNINGS"
} elseif (($failures -join " ") -match "BLOCKED") {
    "FIRMWARE_APPROVAL_REVIEW_BLOCKED"
} else {
    "FIRMWARE_APPROVAL_REVIEW_FAILED"
}

Write-EvidenceFiles $status $rows $commands $warnings $failures
$status
exit $(if ($failures.Count -eq 0) { 0 } else { 1 })
