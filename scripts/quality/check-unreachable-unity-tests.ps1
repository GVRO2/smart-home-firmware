param(
    [string]$NativeOutput = "",
    [string]$TestPath = "test"
)

$ErrorActionPreference = "Stop"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$failures = New-Object System.Collections.Generic.List[string]
$warnings = New-Object System.Collections.Generic.List[string]

function Add-Failure($Message) {
    $script:failures.Add($Message)
}

function Count-Matches($Text, $Pattern) {
    return ([regex]::Matches($Text, $Pattern)).Count
}

$testRoot = Join-Path $repo $TestPath
if (-not (Test-Path -LiteralPath $testRoot)) {
    "UNITY_TEST_HARNESS_FAILED"
    "- test path not found: $TestPath"
    exit 1
}
$testFiles = Get-ChildItem -Path $testRoot -Recurse -File -Include "*.cpp", "*.h"
$allText = ($testFiles | ForEach-Object { Get-Content -Raw -LiteralPath $_.FullName }) -join "`n"
$registeredMatches = [regex]::Matches($allText, 'RUN_TEST\s*\(\s*([A-Za-z_][A-Za-z0-9_]*)\s*\)')
$registeredNames = $registeredMatches | ForEach-Object { $_.Groups[1].Value }
$registeredSet = @{}
$registeredNames | ForEach-Object { $registeredSet[$_] = $true }

foreach ($file in $testFiles) {
    $repoPath = $repo.Path.TrimEnd('\')
    $relativePath = $file.FullName
    if ($relativePath.StartsWith($repoPath)) {
        $relativePath = $relativePath.Substring($repoPath.Length).TrimStart('\')
    }
    $lines = Get-Content -LiteralPath $file.FullName
    $text = $lines -join "`n"
    $unityBeginCount = Count-Matches $text 'UNITY_BEGIN\s*\('
    $unityEndCount = Count-Matches $text 'return\s+UNITY_END\s*\(\s*\)\s*;'
    $runTestIndexes = New-Object System.Collections.Generic.List[int]
    $unityEndIndexes = New-Object System.Collections.Generic.List[int]

    for ($i = 0; $i -lt $lines.Count; ++$i) {
        if ($lines[$i] -match 'RUN_TEST\s*\(') {
            $runTestIndexes.Add($i)
        }
        if ($lines[$i] -match 'return\s+UNITY_END\s*\(\s*\)\s*;') {
            $unityEndIndexes.Add($i)
        }
    }

    if ($unityBeginCount -gt 1) {
        Add-Failure "$relativePath has more than one UNITY_BEGIN()"
    }
    if ($unityEndCount -gt 1) {
        Add-Failure "$relativePath has more than one return UNITY_END();"
    }
    if ($unityBeginCount -gt 0 -and $unityEndCount -eq 0) {
        Add-Failure "$relativePath has UNITY_BEGIN() but no return UNITY_END();"
    }
    if ($unityEndCount -gt 0 -and $unityBeginCount -eq 0) {
        Add-Failure "$relativePath has return UNITY_END(); but no UNITY_BEGIN()"
    }

    if ($runTestIndexes.Count -gt 0 -and $unityEndIndexes.Count -gt 0) {
        $lastRunTest = ($runTestIndexes | Measure-Object -Maximum).Maximum
        $firstUnityEnd = ($unityEndIndexes | Measure-Object -Minimum).Minimum
        if ($firstUnityEnd -lt $lastRunTest) {
            Add-Failure "$relativePath has RUN_TEST after return UNITY_END();"
        }

        $beginIndex = -1
        for ($i = 0; $i -lt $lines.Count; ++$i) {
            if ($lines[$i] -match 'UNITY_BEGIN\s*\(') {
                $beginIndex = $i
                break
            }
        }
        if ($beginIndex -ge 0) {
            for ($i = $beginIndex + 1; $i -lt $lastRunTest; ++$i) {
                if ($lines[$i] -match '^\s*return\b' -and $lines[$i] -notmatch 'UNITY_END') {
                    Add-Failure "$relativePath has return before the last RUN_TEST at line $($i + 1)"
                }
            }
        }
    }
}

$definedMatches = [regex]::Matches($allText, '(?m)^\s*void\s+(should[A-Za-z0-9_]*)\s*\(\s*\)\s*\{')
$definedNames = $definedMatches | ForEach-Object { $_.Groups[1].Value } | Sort-Object -Unique
foreach ($name in $definedNames) {
    if (-not $registeredSet.ContainsKey($name)) {
        Add-Failure "test function is defined but not registered with RUN_TEST: $name"
    }
}

$definedSet = @{}
$definedNames | ForEach-Object { $definedSet[$_] = $true }
foreach ($name in ($registeredNames | Sort-Object -Unique)) {
    if (-not $definedSet.ContainsKey($name)) {
        Add-Failure "RUN_TEST references a function without a detectable definition: $name"
    }
}

if ($NativeOutput) {
    if (-not (Test-Path -LiteralPath $NativeOutput)) {
        Add-Failure "native output file not found for count comparison: $NativeOutput"
    } else {
        $output = Get-Content -Raw -LiteralPath $NativeOutput
        $executedCount = ([regex]::Matches($output, '(?m)^\s*test[^\r\n]*\s+\[PASSED\]\s*$')).Count
        if ($executedCount -eq 0 -and $output -match '(\d+)\s+Tests\s+0\s+Failures') {
            $executedCount = [int]$Matches[1]
        }
        if ($executedCount -ne $registeredMatches.Count) {
            Add-Failure "registered RUN_TEST count ($($registeredMatches.Count)) does not match native output executed count ($executedCount)"
        }
    }
}

"registered_run_test_count=$($registeredMatches.Count)"
"defined_test_function_count=$($definedNames.Count)"
if ($warnings.Count -gt 0) {
    $warnings | ForEach-Object { "warning=$_"; }
}
if ($failures.Count -gt 0) {
    "UNITY_TEST_HARNESS_FAILED"
    $failures | ForEach-Object { "- $_" }
    exit 1
}
if ($warnings.Count -gt 0) {
    "UNITY_TEST_HARNESS_PASSED_WITH_WARNINGS"
    exit 0
}

"UNITY_TEST_HARNESS_PASSED"
exit 0
