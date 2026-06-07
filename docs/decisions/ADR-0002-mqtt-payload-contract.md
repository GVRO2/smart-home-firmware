# ADR-0002 - MQTT Payload Contract

## Status

Accepted

## Contexto

O backend precisa receber medicoes ambientais rastreaveis e estaveis.

## Decisao

Topico oficial:

```text
home/bedroom/esp32-bedroom-01/environment
```

Payload completo:

```json
{
  "deviceId": "esp32-bedroom-01",
  "room": "bedroom",
  "temperatureCelsius": 27.4,
  "humidityPercentage": 62.5,
  "luminosityLux": 18.0,
  "measuredAt": "2026-05-21T20:30:00Z"
}
```

Campos sensoriais invalidos sao omitidos. Se nenhuma leitura sensorial for valida, nao publicar. Se `measuredAt` estiver indisponivel, nao publicar.

## Consequencias

Mudancas em topico, identidade, semantica de campo ou payload parcial exigem sincronizacao com backend, testes e evidencia.
