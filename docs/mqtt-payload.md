# MQTT Payload

Topico oficial:

```text
home/bedroom/esp32-bedroom-01/environment
```

Payload completo esperado:

```json
{
  "deviceId": "esp32-bedroom-01",
  "room": "bedroom",
  "temperatureCelsius": 27.4,
  "humidityPercentage": 62.5,
  "luminosityLux": 123.0,
  "measuredAt": "2026-05-24T12:00:00Z"
}
```

Regras:

- `deviceId` vem de `DEVICE_EXTERNAL_ID`;
- `room` vem de `ROOM_SLUG`;
- `temperatureCelsius` e `humidityPercentage` vem do DHT22/AM2302;
- `luminosityLux` vem do GY-302/BH1750;
- `measuredAt` usa horario UTC via NTP;
- campos de sensores com leitura invalida sao omitidos;
- se nenhuma leitura sensorial for valida, o firmware nao publica.

Validacao local sugerida:

```powershell
mosquitto_sub -h localhost -p 1883 -t "home/bedroom/esp32-bedroom-01/environment" -v
```

Use o host local da sua rede. Nao exponha o broker MQTT publicamente.
