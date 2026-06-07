# Physical Sensor and MQTT Validation Report

## Executive summary

Status: `PASSED_OPERATIONAL_SENSOR_MQTT_VALIDATION`

The ESP32 firmware was physically observed running on COM5 with Wi-Fi, MQTT, DHT22/AM2302, BH1750/GY-302, and presence sensing active. The serial logs show valid sensor readings, official environment-topic publication, JSON payloads containing the expected fields, and successful MQTT publish results.

This is an operational validation report. It proves the sensors and MQTT publication path are working in the captured hardware session. It does not claim laboratory-grade calibration or long-duration soak coverage.

## Evidence package

Evidence run:

```text
docs/evidence/runs/20260604-2134-physical-sensor-mqtt-validation/
```

Source evidence:

```text
Operator-provided serial monitor logs from COM5 at 115200 baud.
```

## Runtime environment observed

| Item | Observed value |
| --- | --- |
| Serial port | COM5 |
| Baud | 115200 |
| Device ID | `esp32-bedroom-01` |
| Room | `bedroom` |
| Firmware local IP | `192.168.1.45` |
| MQTT broker | `192.168.1.200:1883` |
| Environment topic | `home/bedroom/esp32-bedroom-01/environment` |
| DHT pin | GPIO 4 |
| BH1750 SDA/SCL | GPIO 21 / GPIO 22 |
| BH1750 address | `0x23` |

## Initialization results

The logs show all relevant sensors initialized:

```text
[SENSOR] dht_ready=true
[BH1750] init=success address=0x23 sda=21 scl=22 mode=CONTINUOUS_HIGH_RES_MODE
[SENSOR] bh1750_init=success
[SENSOR] presence_ready=true
```

Wi-Fi and MQTT were also established:

```text
[WIFI] connected ip=192.168.1.45
[MQTT] connected
```

## Sensor validation results

| Sensor | Result | Evidence |
| --- | --- | --- |
| DHT22/AM2302 temperature | PASSED | Repeated `status=valid`, observed 21.40 C to 21.80 C. |
| DHT22/AM2302 humidity | PASSED | Repeated `status=valid`, observed 77.50% to 80.20%. |
| BH1750/GY-302 luminosity | PASSED | Repeated `status=valid`, observed 106.67 lux to 140.00 lux. |
| Presence sensor | PASSED | Valid `presenceDetected=true` and `presenceDetected=false` both observed. |

Representative log excerpt:

```text
[DHT22] status=valid temperatureCelsius=21.40 humidityPercentage=80.20
[BH1750] status=valid address=0x23 luminosityLux=140.00
[PRESENCE] status=valid presenceDetected=true
```

Second representative state:

```text
[DHT22] status=valid temperatureCelsius=21.80 humidityPercentage=77.80
[BH1750] status=valid address=0x23 luminosityLux=136.67
[PRESENCE] status=valid presenceDetected=false
```

## MQTT contract validation

The firmware published to the official environment topic:

```text
home/bedroom/esp32-bedroom-01/environment
```

Representative payload:

```json
{
  "deviceId": "esp32-bedroom-01",
  "room": "bedroom",
  "measuredAt": "2026-06-04T21:09:19Z",
  "temperatureCelsius": 21.4,
  "humidityPercentage": 80.2,
  "luminosityLux": 140,
  "presenceDetected": true
}
```

The logs repeatedly showed:

```text
[MQTT] publish=success
```

## Audio interaction observation

The environmental publication loop remained active during audio validation. This matters because audio playback must not starve sensor collection or MQTT publishing.

Representative sequence:

```text
[AUDIO] playback=start format=wav pcm_s16le
[MQTT] payload={"deviceId":"esp32-bedroom-01","room":"bedroom","measuredAt":"2026-06-04T21:33:16Z","temperatureCelsius":21.8,"humidityPercentage":77.6,"luminosityLux":136.6667,"presenceDetected":true}
[MQTT] publish=success
[AUDIO] playback=completed bytesWritten=441600
```

## Passed criteria

- Physical ESP32 boot and runtime logs captured.
- Wi-Fi connectivity confirmed.
- MQTT broker connectivity confirmed.
- DHT22/AM2302 reads valid temperature and humidity.
- BH1750/GY-302 initializes and reads valid luminosity.
- Presence sensor publishes valid `true` and `false` states.
- Official MQTT topic and JSON contract are respected.
- MQTT publication succeeds across repeated cycles.
- Audio playback did not stop environment publication in the captured window.

## Limits and residual risk

- This report does not validate absolute sensor calibration.
- This report does not prove multi-hour stability.
- This report does not validate physical sensor-disconnect behavior.
- DHT pin GPIO 4 is still logged as a strapping-pin warning and should remain tracked as a hardware caution.

## Final determination

The physical sensor stack is validated operationally for the current hardware setup. DHT22/AM2302, BH1750/GY-302, presence sensing, Wi-Fi, MQTT connectivity, and environment payload publication are working with real runtime evidence.
