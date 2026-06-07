$ErrorActionPreference = "Stop"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$failures = New-Object System.Collections.Generic.List[string]

function Require-Text($Path, $Pattern, $Message) {
    $full = Join-Path $repo $Path
    if (-not (Test-Path -LiteralPath $full)) {
        $script:failures.Add("Missing file: $Path")
        return
    }
    $text = Get-Content -Raw -LiteralPath $full
    if ($text -notmatch $Pattern) {
        $script:failures.Add($Message)
    }
}

Require-Text "docs\testing\firmware-test-strategy.md" 'Mock para logica.*Simulacao para contrato.*Hardware real para fisica.*Release somente com evidencia fisica' "test strategy must state the validation principle"
Require-Text "docs\testing\physical-validation-policy.md" 'ESP32 real' "physical policy must require real ESP32"
Require-Text "docs\testing\release-validation-policy.md" 'Mocks nao aprovam release sozinhos' "release policy must reject mock-only release"
Require-Text "docs\testing\firmware-mock-strategy.md" 'Mocks nao substituem validacao fisica' "mock strategy must state mock limitations"
Require-Text "docs\testing\firmware-mqtt-simulation.md" 'mosquitto_pub' "MQTT simulation docs must mention mosquitto_pub"

if ($failures.Count -gt 0) {
    "FIRMWARE_PHYSICAL_VALIDATION_POLICY_GATE_FAILED"
    $failures | ForEach-Object { "- $_" }
    exit 1
}

"FIRMWARE_PHYSICAL_VALIDATION_POLICY_GATE_PASSED"
exit 0
