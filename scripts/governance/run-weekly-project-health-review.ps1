param(
    [string]$BackendRepo = "C:\Users\KABUM\IdeaProjects\smart-home",
    [string]$FirmwareRepo = (Resolve-Path (Join-Path $PSScriptRoot "..\..")).Path,
    [string]$ReviewDate = (Get-Date -Format "yyyy-MM-dd"),
    [switch]$SkipBackendMaven
)

$ErrorActionPreference = "Continue"

function Write-TextFile {
    param([string]$Path, [string]$Content)
    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $Path) | Out-Null
    $Content | Set-Content -Encoding UTF8 -LiteralPath $Path
}

function Invoke-RecordedCommand {
    param(
        [string]$Name,
        [string]$WorkingDirectory,
        [string]$Command,
        [string]$LogPath
    )

    New-Item -ItemType Directory -Force -Path (Split-Path -Parent $LogPath) | Out-Null
    Push-Location $WorkingDirectory
    try {
        powershell -NoProfile -ExecutionPolicy Bypass -Command $Command *>&1 | Tee-Object -FilePath $LogPath
        $code = $LASTEXITCODE
    }
    finally {
        Pop-Location
    }
    return [pscustomobject]@{
        Name = $Name
        Command = $Command
        WorkingDirectory = $WorkingDirectory
        ExitCode = $code
        LogPath = $LogPath
    }
}

$stamp = Get-Date -Format "yyyyMMdd-HHmm"
$runDir = Join-Path $FirmwareRepo "docs\evidence\runs\$stamp-weekly-project-health-review"
$rawDir = Join-Path $runDir "raw"
New-Item -ItemType Directory -Force -Path $rawDir | Out-Null

$commands = New-Object System.Collections.Generic.List[object]
$blocked = $false
$foundRisks = $false

if (-not (Test-Path -LiteralPath $FirmwareRepo)) {
    Write-TextFile (Join-Path $runDir "summary.md") "WEEKLY_PROJECT_HEALTH_REVIEW_BLOCKED`nFirmware repository unavailable: $FirmwareRepo"
    exit 2
}

Push-Location $FirmwareRepo
try {
    git status --short | Set-Content -Encoding UTF8 (Join-Path $rawDir "firmware-git-status.txt")
    git branch --show-current | Set-Content -Encoding UTF8 (Join-Path $rawDir "firmware-branch.txt")
    git log --oneline -n 20 | Set-Content -Encoding UTF8 (Join-Path $rawDir "firmware-git-log.txt")
}
finally {
    Pop-Location
}

$firmwareReview = Invoke-RecordedCommand `
    -Name "firmware approval review automatic" `
    -WorkingDirectory $FirmwareRepo `
    -Command "powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1 -Mode automatic" `
    -LogPath (Join-Path $rawDir "firmware-approval-review-automatic.txt")
$commands.Add($firmwareReview)
if ($firmwareReview.ExitCode -ne 0) { $foundRisks = $true }

if (Test-Path -LiteralPath $BackendRepo) {
    Push-Location $BackendRepo
    try {
        git status --short | Set-Content -Encoding UTF8 (Join-Path $rawDir "backend-git-status.txt")
        git branch --show-current | Set-Content -Encoding UTF8 (Join-Path $rawDir "backend-branch.txt")
        git log --oneline -n 20 | Set-Content -Encoding UTF8 (Join-Path $rawDir "backend-git-log.txt")
    }
    finally {
        Pop-Location
    }

    if (Test-Path -LiteralPath (Join-Path $BackendRepo "scripts\governance\run-profile.ps1")) {
        $backendReview = Invoke-RecordedCommand `
            -Name "backend approval review" `
            -WorkingDirectory $BackendRepo `
            -Command "powershell -ExecutionPolicy Bypass -File .\scripts\governance\run-profile.ps1 -Profile approval-review" `
            -LogPath (Join-Path $rawDir "backend-approval-review.txt")
        $commands.Add($backendReview)
        if ($backendReview.ExitCode -ne 0) { $foundRisks = $true }
    }

    if (-not $SkipBackendMaven -and (Test-Path -LiteralPath (Join-Path $BackendRepo "mvnw.cmd"))) {
        $backendMaven = Invoke-RecordedCommand `
            -Name "backend maven clean verify" `
            -WorkingDirectory $BackendRepo `
            -Command ".\mvnw.cmd clean verify" `
            -LogPath (Join-Path $rawDir "backend-mvn-clean-verify.txt")
        $commands.Add($backendMaven)
        $mavenLog = Get-Content -Raw -LiteralPath $backendMaven.LogPath
        if ($backendMaven.ExitCode -ne 0 -and $mavenLog -notmatch "BUILD SUCCESS") { $foundRisks = $true }
    }
}
else {
    $blocked = $true
    Write-TextFile (Join-Path $rawDir "backend-unavailable.txt") "REPOSITORY_UNAVAILABLE: $BackendRepo"
}

$commandRows = $commands | ForEach-Object {
    "| $($_.Name) | `$($($_.Command))` | $($_.WorkingDirectory) | $($_.ExitCode) | $($_.LogPath) |"
}

Write-TextFile (Join-Path $runDir "commands.md") @"
# Commands

| Name | Command | Directory | Exit Code | Evidence |
|---|---|---|---:|---|
$($commandRows -join "`n")
"@

Write-TextFile (Join-Path $runDir "repositories-scanned.md") @"
# Repositories Scanned

- Firmware: $FirmwareRepo
- Backend: $(if (Test-Path -LiteralPath $BackendRepo) { $BackendRepo } else { "REPOSITORY_UNAVAILABLE: $BackendRepo" })
"@

Write-TextFile (Join-Path $runDir "summary.md") @"
# Weekly Project Health Review Evidence

Review date: $ReviewDate
Run: $runDir

Status: $(if ($blocked) { "WEEKLY_PROJECT_HEALTH_REVIEW_BLOCKED" } elseif ($foundRisks) { "WEEKLY_PROJECT_HEALTH_REVIEW_FOUND_RISKS" } else { "WEEKLY_PROJECT_HEALTH_REVIEW_PASSED_WITH_WARNINGS" })

This script collects evidence only. The narrative report must still be reviewed by an auditor before release claims are made.
"@

foreach ($name in @(
    "files-inspected.md",
    "gates-output.md",
    "git-status.md",
    "risk-register.md",
    "backlog-drift.md",
    "sdd-drift.md",
    "contract-drift.md",
    "security-findings.md",
    "next-actions.md"
)) {
    $path = Join-Path $runDir $name
    if (-not (Test-Path -LiteralPath $path)) {
        Write-TextFile $path "# $name`n`nGenerated evidence placeholder. Review raw logs in `raw/` and update the weekly report with conclusions."
    }
}

if ($blocked) {
    "WEEKLY_PROJECT_HEALTH_REVIEW_BLOCKED"
    exit 2
}

if ($foundRisks) {
    "WEEKLY_PROJECT_HEALTH_REVIEW_FOUND_RISKS"
    exit 1
}

"WEEKLY_PROJECT_HEALTH_REVIEW_PASSED_WITH_WARNINGS"
exit 0
