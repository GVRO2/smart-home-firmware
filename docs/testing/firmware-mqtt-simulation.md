# Firmware MQTT Simulation

MQTT simulation validates contract traffic without ESP32 hardware.

## Environment Publisher

```powershell
.\scripts\dev\mock-esp32-environment-publisher.ps1 -BrokerHost localhost -BrokerPort 1883
```

Publishes to `home/bedroom/esp32-bedroom-01/environment` with `deviceId`, `room`, `temperatureCelsius`, `humidityPercentage`, `luminosityLux`, and `measuredAt`.

## Backend Audio Command Publisher

```powershell
.\scripts\dev\mock-backend-audio-command-publisher.ps1 -CommandType play_tone
.\scripts\dev\mock-backend-audio-command-publisher.ps1 -CommandType play_audio_url
.\scripts\dev\mock-backend-audio-command-publisher.ps1 -CommandType play_audio_stream
.\scripts\dev\mock-backend-audio-command-publisher.ps1 -CommandType stop_audio
.\scripts\dev\mock-backend-audio-command-publisher.ps1 -CommandType set_volume
```

Publishes to `home/bedroom/esp32-bedroom-01/audio/command`.

## ESP32 Audio Status Publisher

```powershell
.\scripts\dev\mock-esp32-audio-status-publisher.ps1 -State accepted
.\scripts\dev\mock-esp32-audio-status-publisher.ps1 -State playing
.\scripts\dev\mock-esp32-audio-status-publisher.ps1 -State completed
.\scripts\dev\mock-esp32-audio-status-publisher.ps1 -State rejected
.\scripts\dev\mock-esp32-audio-status-publisher.ps1 -State failed
.\scripts\dev\mock-esp32-audio-status-publisher.ps1 -State stopped
```

Publishes to `home/bedroom/esp32-bedroom-01/audio/status`.

## Broker Tools

Use `mosquitto_pub` and `mosquitto_sub` against a local broker. If these tools are unavailable, the scenario reports `BLOCKED_BY_BROKER_TOOL_UNAVAILABLE`. If the broker is not running, it reports `BLOCKED_BY_BROKER_UNAVAILABLE`.
