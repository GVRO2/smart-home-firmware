param(
    [string]$TestPath = "test\test_native\test_mqtt_contract.cpp"
)

$ErrorActionPreference = "Continue"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$target = Join-Path $repo $TestPath
$failures = New-Object System.Collections.Generic.List[string]
$warnings = New-Object System.Collections.Generic.List[string]

function Add-Failure($Message) {
    $script:failures.Add($Message)
}

function Get-FunctionBody($Text, $FunctionName) {
    $match = [regex]::Match($Text, "(?m)^\s*void\s+$FunctionName\s*\(\s*\)\s*\{")
    if (-not $match.Success) {
        return $null
    }

    $depth = 0
    $inString = $false
    $escape = $false
    for ($i = $match.Index; $i -lt $Text.Length; ++$i) {
        $char = $Text[$i]
        if ($inString) {
            if ($escape) {
                $escape = $false
            } elseif ($char -eq "\") {
                $escape = $true
            } elseif ($char -eq '"') {
                $inString = $false
            }
            continue
        }
        if ($char -eq '"') {
            $inString = $true
            continue
        }

        if ($char -eq "{") {
            ++$depth
        } elseif ($char -eq "}") {
            --$depth
            if ($depth -eq 0) {
                return $Text.Substring($match.Index, $i - $match.Index + 1)
            }
        }
    }

    return $null
}

if (-not (Test-Path -LiteralPath $target)) {
    "TEST_SOURCE_SANITY_FAILED"
    "- test source not found: $TestPath"
    exit 1
}

$text = Get-Content -Raw -LiteralPath $target
$lines = Get-Content -LiteralPath $target

$open = 0
$close = 0
$inString = $false
$escape = $false
foreach ($char in $text.ToCharArray()) {
    if ($inString) {
        if ($escape) {
            $escape = $false
        } elseif ($char -eq "\") {
            $escape = $true
        } elseif ($char -eq '"') {
            $inString = $false
        }
        continue
    }
    if ($char -eq '"') {
        $inString = $true
        continue
    }
    if ($char -eq "{") { ++$open }
    if ($char -eq "}") { ++$close }
}
if ($open -ne $close) {
    Add-Failure "brace count is not balanced: open=$open close=$close"
}

$returnUnityEndCount = ([regex]::Matches($text, "return\s+UNITY_END\s*\(\s*\)\s*;")).Count
if ($returnUnityEndCount -gt 1) {
    Add-Failure "more than one return UNITY_END() was found"
}

$returnUnityEndLine = -1
for ($i = 0; $i -lt $lines.Count; ++$i) {
    if ($lines[$i] -match "return\s+UNITY_END\s*\(\s*\)\s*;") {
        $returnUnityEndLine = $i
        break
    }
}
if ($returnUnityEndLine -ge 0) {
    for ($i = $returnUnityEndLine + 1; $i -lt $lines.Count; ++$i) {
        if ($lines[$i] -match "RUN_TEST\s*\(") {
            Add-Failure "RUN_TEST appears after return UNITY_END() at line $($i + 1)"
        }
    }
}

$functionMatches = [regex]::Matches($text, "(?m)^\s*void\s+(should[A-Za-z0-9_]*|test[A-Za-z0-9_]*)\s*\(\s*\)\s*\{")
$functionNames = $functionMatches | ForEach-Object { $_.Groups[1].Value }
$duplicates = $functionNames | Group-Object | Where-Object { $_.Count -gt 1 }
foreach ($duplicate in $duplicates) {
    Add-Failure "duplicate test function definition: $($duplicate.Name)"
}

foreach ($match in $functionMatches) {
    $name = $match.Groups[1].Value
    $body = Get-FunctionBody $text $name
    if ($null -eq $body) {
        Add-Failure "could not resolve body for $name"
        continue
    }

    $nested = [regex]::Matches($body, "(?m)^\s*void\s+(should[A-Za-z0-9_]*|test[A-Za-z0-9_]*)\s*\(\s*\)\s*\{")
    if ($nested.Count -gt 1) {
        Add-Failure "$name contains another test function signature"
    }
}

$assertionDepth = 0
for ($i = 0; $i -lt $lines.Count; ++$i) {
    $line = $lines[$i]
    if ($assertionDepth -eq 0 -and $line -match "\bTEST_ASSERT_[A-Z0-9_]+\s*\(") {
        Add-Failure "assertion appears outside a function at line $($i + 1)"
    }

    foreach ($char in $line.ToCharArray()) {
        if ($char -eq "{") { ++$assertionDepth }
        if ($char -eq "}") { --$assertionDepth }
        if ($assertionDepth -lt 0) {
            Add-Failure "orphan closing brace near line $($i + 1)"
            $assertionDepth = 0
        }
    }
}

if ($text -match "void\s+shouldPublishRejectedAudioStatus\s*\(\)\s*\{\s*void\s+shouldPublishPlayingAudioStatus\s*\(") {
    Add-Failure "shouldPublishPlayingAudioStatus appears nested in shouldPublishRejectedAudioStatus"
}
if ($text -match "void\s+shouldPublishFailedAudioStatus\s*\(\)\s*\{\s*void\s+shouldPublishCompletedAudioStatus\s*\(") {
    Add-Failure "shouldPublishCompletedAudioStatus appears nested in shouldPublishFailedAudioStatus"
}
if ($text -match "void\s+shouldPublishPlayingCompletedAndStoppedAudioStatuses\s*\(\)\s*\{\s*void\s+shouldPublishStoppedAudioStatus\s*\(") {
    Add-Failure "shouldPublishStoppedAudioStatus appears nested in obsolete grouped status test"
}

$warnings.Add("regex and brace-depth sanity checks are conservative; compiler and native tests remain required")

"checked_source=$TestPath"
"test_function_count=$($functionMatches.Count)"
if ($failures.Count -gt 0) {
    "TEST_SOURCE_SANITY_FAILED"
    $failures | ForEach-Object { "- $_" }
    exit 1
}

if ($warnings.Count -gt 0) {
    "TEST_SOURCE_SANITY_PASSED_WITH_WARNINGS"
    $warnings | ForEach-Object { "warning=$_" }
    exit 0
}

"TEST_SOURCE_SANITY_PASSED"
exit 0
