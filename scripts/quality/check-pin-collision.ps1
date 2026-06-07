$ErrorActionPreference = "Stop"
$repo = Resolve-Path (Join-Path $PSScriptRoot "..\..")
$failures = New-Object System.Collections.Generic.List[string]

# 1. Parse AppConfig.h
$appConfigPath = Join-Path $repo "include\AppConfig.h"
if (-not (Test-Path -LiteralPath $appConfigPath)) {
    Write-Output "PIN_MAPPING_COLLISION_DETECTED"
    Write-Output "Missing AppConfig.h"
    exit 1
}

$text = Get-Content -Raw -LiteralPath $appConfigPath

function Get-Pin($Name) {
    if ($text -match "constexpr\s+int\s+$Name\s*=\s*(\d+);") {
        return [int]$Matches[1]
    }
    return $null
}

$DHT_PIN = Get-Pin "DHT_PIN"
$I2C_SDA_PIN = Get-Pin "I2C_SDA_PIN"
$I2C_SCL_PIN = Get-Pin "I2C_SCL_PIN"
$AUDIO_I2S_BCLK_PIN = Get-Pin "AUDIO_I2S_BCLK_PIN"
$AUDIO_I2S_LRC_PIN = Get-Pin "AUDIO_I2S_LRC_PIN"
$AUDIO_I2S_DIN_PIN = Get-Pin "AUDIO_I2S_DIN_PIN"
$HEARTBEAT_LED_PIN = Get-Pin "HEARTBEAT_LED_PIN"
$PRESENCE_SENSOR_PIN = Get-Pin "PRESENCE_SENSOR_PIN"
$SOUND_SENSOR_PIN = Get-Pin "SOUND_SENSOR_PIN"

if ($null -eq $DHT_PIN -or $null -eq $I2C_SDA_PIN -or $null -eq $I2C_SCL_PIN -or $null -eq $AUDIO_I2S_BCLK_PIN -or $null -eq $AUDIO_I2S_LRC_PIN -or $null -eq $AUDIO_I2S_DIN_PIN -or $null -eq $HEARTBEAT_LED_PIN -or $null -eq $PRESENCE_SENSOR_PIN -or $null -eq $SOUND_SENSOR_PIN) {
    $failures.Add("Could not parse all pin configurations in AppConfig.h")
} else {
    # Check collisions
    if ($AUDIO_I2S_DIN_PIN -eq $PRESENCE_SENSOR_PIN) {
        $failures.Add("AUDIO_I2S_DIN_PIN is equal to PRESENCE_SENSOR_PIN ($AUDIO_I2S_DIN_PIN)")
    }

    $sensorPins = @($DHT_PIN, $I2C_SDA_PIN, $I2C_SCL_PIN, $PRESENCE_SENSOR_PIN, $SOUND_SENSOR_PIN)
    if ($sensorPins -contains $AUDIO_I2S_BCLK_PIN) {
        $failures.Add("AUDIO_I2S_BCLK_PIN is equal to a sensor pin ($AUDIO_I2S_BCLK_PIN)")
    }
    if ($sensorPins -contains $AUDIO_I2S_LRC_PIN) {
        $failures.Add("AUDIO_I2S_LRC_PIN is equal to a sensor pin ($AUDIO_I2S_LRC_PIN)")
    }

    $outputPins = @($HEARTBEAT_LED_PIN, $AUDIO_I2S_BCLK_PIN, $AUDIO_I2S_LRC_PIN, $AUDIO_I2S_DIN_PIN)
    if ($outputPins -contains 34) {
        $failures.Add("Output pin uses GPIO34 (which is input-only)")
    }

    # Unique pins validation
    $allPins = @{
        "HEARTBEAT_LED_PIN" = $HEARTBEAT_LED_PIN
        "DHT_PIN" = $DHT_PIN
        "I2C_SDA_PIN" = $I2C_SDA_PIN
        "I2C_SCL_PIN" = $I2C_SCL_PIN
        "AUDIO_I2S_BCLK_PIN" = $AUDIO_I2S_BCLK_PIN
        "AUDIO_I2S_LRC_PIN" = $AUDIO_I2S_LRC_PIN
        "AUDIO_I2S_DIN_PIN" = $AUDIO_I2S_DIN_PIN
        "PRESENCE_SENSOR_PIN" = $PRESENCE_SENSOR_PIN
        "SOUND_SENSOR_PIN" = $SOUND_SENSOR_PIN
    }

    $grouped = $allPins.Keys | Group-Object { $allPins[$_] }
    foreach ($group in $grouped) {
        if ($group.Count -gt 1) {
            $names = $group.Group -join ", "
            $failures.Add("GPIO $($group.Name) is reused by multiple pins: $names")
        }
    }
}

# 2. Check documentation alignment
function Check-Doc-Pin($DocPath, $Pattern, $ExpectedPin, $Message) {
    $fullPath = Join-Path $repo $DocPath
    if (-not (Test-Path -LiteralPath $fullPath)) {
        $script:failures.Add("Missing doc file: $DocPath")
        return
    }
    $docText = Get-Content -Raw -LiteralPath $fullPath
    if ($docText -match $Pattern) {
        $foundPin = [int]$Matches[1]
        if ($foundPin -ne $ExpectedPin) {
            $script:failures.Add("$Message (Doc: $foundPin vs Firmware: $ExpectedPin)")
        }
    } else {
        $script:failures.Add("Could not find pattern for $Message in $DocPath")
    }
}

# Verify wiring doc
Check-Doc-Pin "docs\architecture\firmware-hardware-wiring.md" 'DATA\s*\|\s*GPIO\s*(\d+)' $DHT_PIN "DHT_PIN in wiring doc"
Check-Doc-Pin "docs\architecture\firmware-hardware-wiring.md" 'SDA\s*\|\s*GPIO\s*(\d+)' $I2C_SDA_PIN "I2C_SDA_PIN in wiring doc"
Check-Doc-Pin "docs\architecture\firmware-hardware-wiring.md" 'SCL\s*\|\s*GPIO\s*(\d+)' $I2C_SCL_PIN "I2C_SCL_PIN in wiring doc"
Check-Doc-Pin "docs\architecture\firmware-hardware-wiring.md" 'BCLK/SCK\s*\|\s*GPIO\s*(\d+)' $AUDIO_I2S_BCLK_PIN "AUDIO_I2S_BCLK_PIN in wiring doc"
Check-Doc-Pin "docs\architecture\firmware-hardware-wiring.md" 'LRC/WS\s*\|\s*GPIO\s*(\d+)' $AUDIO_I2S_LRC_PIN "AUDIO_I2S_LRC_PIN in wiring doc"
Check-Doc-Pin "docs\architecture\firmware-hardware-wiring.md" 'DIN\s*\|\s*GPIO\s*(\d+)' $AUDIO_I2S_DIN_PIN "AUDIO_I2S_DIN_PIN in wiring doc"
Check-Doc-Pin "docs\architecture\firmware-hardware-wiring.md" 'Presence OUT\s*\|\s*GPIO\s*(\d+)' $PRESENCE_SENSOR_PIN "PRESENCE_SENSOR_PIN in wiring doc"
Check-Doc-Pin "docs\architecture\firmware-hardware-wiring.md" 'Sound AO\s*\|\s*GPIO\s*(\d+)' $SOUND_SENSOR_PIN "SOUND_SENSOR_PIN in wiring doc"

# Verify SDD doc
Check-Doc-Pin "docs\sdd\firmware-sdd.md" 'DHT22/AM2302 DATA:\s*GPIO\s*(\d+)' $DHT_PIN "DHT_PIN in SDD doc"
Check-Doc-Pin "docs\sdd\firmware-sdd.md" 'BH1750 SDA:\s*GPIO\s*(\d+)' $I2C_SDA_PIN "I2C_SDA_PIN in SDD doc"
Check-Doc-Pin "docs\sdd\firmware-sdd.md" 'BH1750 SCL:\s*GPIO\s*(\d+)' $I2C_SCL_PIN "I2C_SCL_PIN in SDD doc"
Check-Doc-Pin "docs\sdd\firmware-sdd.md" 'BCLK/SCK:\s*GPIO\s*(\d+)' $AUDIO_I2S_BCLK_PIN "AUDIO_I2S_BCLK_PIN in SDD doc"
Check-Doc-Pin "docs\sdd\firmware-sdd.md" 'LRC/WS:\s*GPIO\s*(\d+)' $AUDIO_I2S_LRC_PIN "AUDIO_I2S_LRC_PIN in SDD doc"
Check-Doc-Pin "docs\sdd\firmware-sdd.md" 'DIN:\s*GPIO\s*(\d+)' $AUDIO_I2S_DIN_PIN "AUDIO_I2S_DIN_PIN in SDD doc"
Check-Doc-Pin "docs\sdd\firmware-sdd.md" 'Presence OUT:\s*GPIO\s*(\d+)' $PRESENCE_SENSOR_PIN "PRESENCE_SENSOR_PIN in SDD doc"
Check-Doc-Pin "docs\sdd\firmware-sdd.md" 'Sound AO:\s*GPIO\s*(\d+)' $SOUND_SENSOR_PIN "SOUND_SENSOR_PIN in SDD doc"

# Verify squad memory doc
Check-Doc-Pin "docs\squad\firmware-agent-memory.md" 'DHT22 DATA:\s*GPIO\s*(\d+)' $DHT_PIN "DHT_PIN in squad memory doc"
Check-Doc-Pin "docs\squad\firmware-agent-memory.md" 'BH1750 SDA:\s*GPIO\s*(\d+)' $I2C_SDA_PIN "I2C_SDA_PIN in squad memory doc"
Check-Doc-Pin "docs\squad\firmware-agent-memory.md" 'BH1750 SCL:\s*GPIO\s*(\d+)' $I2C_SCL_PIN "I2C_SCL_PIN in squad memory doc"
Check-Doc-Pin "docs\squad\firmware-agent-memory.md" 'MAX98357A BCLK/SCK:\s*GPIO\s*(\d+)' $AUDIO_I2S_BCLK_PIN "AUDIO_I2S_BCLK_PIN in squad memory doc"
Check-Doc-Pin "docs\squad\firmware-agent-memory.md" 'MAX98357A LRC/WS:\s*GPIO\s*(\d+)' $AUDIO_I2S_LRC_PIN "AUDIO_I2S_LRC_PIN in squad memory doc"
Check-Doc-Pin "docs\squad\firmware-agent-memory.md" 'MAX98357A DIN:\s*GPIO\s*(\d+)' $AUDIO_I2S_DIN_PIN "AUDIO_I2S_DIN_PIN in squad memory doc"
Check-Doc-Pin "docs\squad\firmware-agent-memory.md" 'Presence OUT:\s*GPIO\s*(\d+)' $PRESENCE_SENSOR_PIN "PRESENCE_SENSOR_PIN in squad memory doc"
Check-Doc-Pin "docs\squad\firmware-agent-memory.md" 'Sound AO:\s*GPIO\s*(\d+)' $SOUND_SENSOR_PIN "SOUND_SENSOR_PIN in squad memory doc"

# 3. Global scan for dangerous patterns
Write-Output "Running global safety scan for dangerous patterns..."
$activeFiles = Get-ChildItem -Path $repo -Recurse -File | Where-Object {
    $_.FullName -notmatch '\\\.git\\' -and
    $_.FullName -notmatch '\\\.pio\\' -and
    $_.FullName -notmatch '\\\.cache\\' -and
    $_.FullName -notmatch '\\\.venv\\' -and
    $_.FullName -notmatch '\\docs\\evidence\\' -and
    $_.FullName -notmatch '\\docs\\audit\\' -and
    $_.Name -ne "check-pin-collision.ps1"
}

foreach ($file in $activeFiles) {
    $relativePath = $file.FullName.Substring($repo.Length + 1)
    $lines = Get-Content -Path $file.FullName
    $lineNum = 0
    foreach ($line in $lines) {
        $lineNum++
        
        # Pattern 1: DIN/SD
        if ($line -match 'DIN/SD') {
            $failures.Add("Dangerous pattern 'DIN/SD' found in ${relativePath}:${lineNum} -> '$line'")
        }
        
        # Pattern 2: GPIO 27 associated with audio
        if ($line -match 'GPIO\s*27' -and ($line -match 'audio' -or $line -match 'i2s' -or $line -match 'din' -or $line -match 'speaker' -or $line -match 'max98357a')) {
            $failures.Add("Dangerous association of 'GPIO 27' with audio found in ${relativePath}:${lineNum} -> '$line'")
        }
        
        # Pattern 3: /speaker
        if ($line -match '/speaker') {
            $failures.Add("Dangerous pattern '/speaker' found in ${relativePath}:${lineNum} -> '$line'")
        }
        
        # Pattern 4: audio real validated
        if ($line -match 'audio real validated') {
            $failures.Add("Dangerous pattern 'audio real validated' found in ${relativePath}:${lineNum} -> '$line'")
        }
        
        # Pattern 5: REAL_ON_HARDWARE
        if ($line -match 'REAL_ON_HARDWARE') {
            $failures.Add("Dangerous pattern 'REAL_ON_HARDWARE' found in ${relativePath}:${lineNum} -> '$line'")
        }
    }
}

if ($failures.Count -gt 0) {
    Write-Output "PIN_MAPPING_COLLISION_DETECTED"
    $failures | ForEach-Object { Write-Output "- $_" }
    exit 1
}

Write-Output "PIN_MAPPING_VALIDATED"
exit 0
