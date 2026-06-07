# Commands

```powershell
Get-Command mosquitto_pub -ErrorAction SilentlyContinue
Get-Command mosquitto_sub -ErrorAction SilentlyContinue
where.exe mosquitto_pub
where.exe mosquitto_sub
Get-ChildItem -Path 'C:\Program Files\mosquitto','C:\Program Files (x86)\mosquitto' -Filter mosquitto*.exe -ErrorAction SilentlyContinue
[System.IO.Ports.SerialPort]::GetPortNames()
powershell -ExecutionPolicy Bypass -File scripts\dev\run-mock-mqtt-scenario.ps1 -RunName firmware-physical-audio-validation-mock-mqtt
powershell -ExecutionPolicy Bypass -File scripts\governance\run-firmware-approval-review.ps1 -Mode physical
```

Notes:

- A broader recursive search under `C:\Program Files` and `C:\Program Files (x86)` timed out before finding Mosquitto binaries.
- No upload, serial monitor, MQTT subscribe, MQTT publish, or audio command was attempted against ESP32 because the expected serial port `COM5` was not present.
