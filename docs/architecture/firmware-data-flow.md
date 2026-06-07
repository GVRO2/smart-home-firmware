# Firmware Data Flow

```text
DHT22/AM2302 + BH1750/GY-302
  -> ESP32 firmware
  -> EnvironmentReading
  -> JSON payload
  -> MQTT topic home/bedroom/esp32-bedroom-01/environment
  -> Mosquitto local
  -> Backend Spring Boot
  -> PostgreSQL
```

## Origem dos dados

- `temperatureCelsius`: DHT22/AM2302.
- `humidityPercentage`: DHT22/AM2302.
- `luminosityLux`: BH1750/GY-302.
- `deviceId` e `room`: `include/AppConfig.h`.
- `measuredAt`: NTP UTC via `TimeProvider`.

## Transformacao

Cada leitura e validada antes de entrar no payload. Campo invalido e omitido; valor falso nao e inventado.

## Publicacao

O topico oficial e `home/bedroom/esp32-bedroom-01/environment`. O backend pode assinar `home/+/+/environment`.

## Contrato

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

## Validacoes

- `deviceId` e `room` devem ser estaveis.
- `measuredAt` deve ser UTC ISO-8601.
- Temperatura: -40 C a 80 C.
- Umidade: 0% a 100%.
- Luminosidade: >= 0 lux.

## Rejeicoes esperadas no backend

O backend deve rejeitar JSON malformado, topico fora do contrato, identidade ausente, timestamp invalido e campos fora de faixa. Payload parcial deve ser aceito somente se a regra estiver explicitamente documentada no backend.

## Evidencias esperadas

- Build do firmware.
- Testes nativos de contrato.
- Serial output com leitura e skip reasons.
- `mosquitto_sub` ou log equivalente mostrando payload real.
- Evidencia backend de ingestao e persistencia quando disponivel.

## Fluxo de audio

```text
Backend/AI
  -> gera texto ou escolhe alerta
  -> TTS/backend local hospeda audio em HTTP local
  -> MQTT home/bedroom/esp32-bedroom-01/audio/command
  -> AudioCommandParser
  -> AudioPlaybackQueue
  -> AudioPlaybackService
  -> I2sAudioOutputDriver
  -> MAX98357A
  -> alto-falante
  -> MQTT home/bedroom/esp32-bedroom-01/audio/status
```

MQTT carrega apenas controle e status. `play_audio_url` e `play_audio_stream` devem apontar para `http://` local/private network. Audio bruto nao deve ser transportado por MQTT por padrao. Nesta entrega, `play_tone` valida I2S; `play_audio_url` baixa WAV/PCM 16-bit mono por HTTP local e escreve PCM no I2S em chunks; `play_audio_stream` permanece sem streaming continuo implementado.
