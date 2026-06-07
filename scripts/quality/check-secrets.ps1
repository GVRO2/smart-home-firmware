$ErrorActionPreference = "Stop"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$failures = New-Object System.Collections.Generic.List[string]
$warnings = New-Object System.Collections.Generic.List[string]

$secretsPath = Join-Path $repo "include\Secrets.h"
if (Test-Path -LiteralPath $secretsPath) {
    Push-Location $repo
    try {
        $trackedSecrets = git ls-files -- include/Secrets.h
        if ($trackedSecrets) {
            $failures.Add("include/Secrets.h is tracked by Git")
        } else {
            $warnings.Add("include/Secrets.h exists locally; acceptable only because it is untracked/ignored")
        }
    }
    finally {
        Pop-Location
    }
}

$files = Get-ChildItem -Path $repo -Recurse -File |
    Where-Object {
        $_.FullName -notmatch '\\\.git\\' -and
        $_.FullName -notmatch '\\\.pio\\' -and
        $_.FullName -notmatch '\\\.venv\\'
    }

foreach ($file in $files) {
    $text = Get-Content -Raw -LiteralPath $file.FullName -ErrorAction SilentlyContinue
    if ($text -match '-----BEGIN (RSA |OPENSSH |EC |DSA )?PRIVATE KEY-----') {
        $failures.Add("Private key pattern in $($file.FullName)")
    }
    if ($text -match '(?i)(api[_-]?key|token|password|senha)\s*[:=]\s*["''][^"'']{12,}') {
        $warnings.Add("Potential credential pattern in $($file.FullName)")
    }
    if ($text -match '\b(?!10\.|127\.|172\.(1[6-9]|2[0-9]|3[0-1])\.|192\.168\.)([1-9][0-9]{0,2}\.){3}[0-9]{1,3}\b') {
        $warnings.Add("Potential public IP in $($file.FullName)")
    }
}

if ($failures.Count -gt 0) {
    "FIRMWARE_SECRETS_GATE_FAILED"
    $failures | ForEach-Object { "- $_" }
    $warnings | ForEach-Object { "WARNING: $_" }
    exit 1
}

if ($warnings.Count -gt 0) {
    "FIRMWARE_SECRETS_GATE_PASSED_WITH_WARNINGS"
    $warnings | ForEach-Object { "WARNING: $_" }
    exit 0
}

"FIRMWARE_SECRETS_GATE_PASSED"
exit 0
