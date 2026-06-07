# Physical sensor and MQTT validation

Run date: 2026-06-04
Status: PASSED_OPERATIONAL_SENSOR_MQTT_VALIDATION

## Evidence source

This report is based on the serial monitor logs provided by the operator in the same validation session that exercised `play_tone` and `play_audio_url`.

The logs show a real ESP32 running on COM5, connected to Wi-Fi, connected to the local MQTT broker, reading sensors, and publishing environment payloads repeatedly.

## Hardware/runtime context observed

```text
--- Terminal on COM5 | 115200 8-N-1
[BOOT] firmware_start
[BOOT] config_loaded
[CONFIG] deviceId=esp32-bedroom-01
[CONFIG] room=bedroom
[CONFIG] mqttHost=192.168.1.200
[CONFIG] mqttPort=1883
[CONFIG] mqttTopic=home/bedroom/esp32-bedroom-01/environment
[CONFIG] dhtPin=4
[CONFIG] bh1750Sda=21
[CONFIG] bh1750Scl=22
[CONFIG] bh1750Address=0x23
[WIFI] connected ip=192.168.1.45
[MQTT] connected
```

## Sensor initialization evidence

```text
[SENSOR] dht_ready=true
[BH1750] init=success address=0x23 sda=21 scl=22 mode=CONTINUOUS_HIGH_RES_MODE
[SENSOR] bh1750_init=success
[SENSOR] presence_ready=true
```

## Sensor reading evidence

Representative valid readings from the captured serial logs:

```text
[DHT22] status=valid temperatureCelsius=21.40 humidityPercentage=80.20
[BH1750] status=valid address=0x23 luminosityLux=140.00
[PRESENCE] status=valid presenceDetected=true
```

```text
[DHT22] status=valid temperatureCelsius=21.80 humidityPercentage=77.80
[BH1750] status=valid address=0x23 luminosityLux=136.67
[PRESENCE] status=valid presenceDetected=false
```

```text
[DHT22] status=valid temperatureCelsius=21.80 humidityPercentage=78.10
[BH1750] status=valid address=0x23 luminosityLux=114.17
[PRESENCE] status=valid presenceDetected=false
```

Observed ranges in the provided logs:

| Field | Observed range/status |
| --- | --- |
| `temperatureCelsius` | 21.40 to 21.80 |
| `humidityPercentage` | 77.50 to 80.20 |
| `luminosityLux` | 106.67 to 140.00 |
| `presenceDetected` | both `true` and `false` observed |

## MQTT publication evidence

The firmware repeatedly published to the official environment topic:

```text
[MQTT] topic=home/bedroom/esp32-bedroom-01/environment
[MQTT] payload={"deviceId":"esp32-bedroom-01","room":"bedroom","measuredAt":"2026-06-04T21:09:19Z","temperatureCelsius":21.4,"humidityPercentage":80.2,"luminosityLux":140,"presenceDetected":true}
[MQTT] publish=success
```

```text
[MQTT] payload={"deviceId":"esp32-bedroom-01","room":"bedroom","measuredAt":"2026-06-04T21:33:01Z","temperatureCelsius":21.8,"humidityPercentage":77.8,"luminosityLux":136.6667,"presenceDetected":false}
[MQTT] publish=success
```

```text
[MQTT] payload={"deviceId":"esp32-bedroom-01","room":"bedroom","measuredAt":"2026-06-04T21:34:16Z","temperatureCelsius":21.8,"humidityPercentage":78.1,"luminosityLux":114.1667,"presenceDetected":false}
[MQTT] publish=success
```

## Continuity during audio validation

The logs show environment publishing continued before, during, and after audio operations:

```text
[AUDIO] playback=start format=wav pcm_s16le
[MQTT] payload={"deviceId":"esp32-bedroom-01","room":"bedroom","measuredAt":"2026-06-04T21:33:16Z","temperatureCelsius":21.8,"humidityPercentage":77.6,"luminosityLux":136.6667,"presenceDetected":true}
[MQTT] publish=success
[AUDIO] playback=completed bytesWritten=441600
```

## Acceptance covered

- ESP32 booted and ran the configured firmware on COM5.
- Wi-Fi connected with local IP `192.168.1.45`.
- MQTT connected to local broker `192.168.1.200:1883`.
- DHT22/AM2302 temperature and humidity readings were valid.
- BH1750/GY-302 luminosity readings were valid at address `0x23`.
- Presence sensor produced valid `true` and `false` states.
- Environment payloads included `deviceId`, `room`, `measuredAt`, `temperatureCelsius`, `humidityPercentage`, `luminosityLux`, and `presenceDetected`.
- Environment publishing continued while audio playback was active.

## Not validated

- Absolute calibration accuracy of temperature, humidity, luminosity, or presence detection.
- Long-duration soak behavior beyond the captured serial window.
- Sensor-disconnect fail-closed behavior in this physical run.

## Final classification

The physical sensor and MQTT environment publication path is validated operationally for this hardware session. The evidence supports release-readiness claims for sensor startup, valid readings, official environment topic, JSON payload shape, and successful MQTT publication, with calibration and disconnect-resilience tests still separate follow-up items.
