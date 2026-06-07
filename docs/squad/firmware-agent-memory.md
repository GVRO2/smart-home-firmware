# Firmware Agent Memory

## Hardware oficial

ESP32 DevKit V1 com DHT22/AM2302 e BH1750/GY-302.

## Sensores atuais

- DHT22/AM2302: temperatura e umidade.
- BH1750/GY-302: luminosidade.

Presenca e som sao reservados no mapeamento de pinos, mas seus drivers nao estao implementados. Microfone continua fora do escopo. O audio local e suportado conforme ADR-0004.

## Pinagem oficial

- DHT22 DATA: GPIO 4.
- BH1750 SDA: GPIO 21.
- BH1750 SCL: GPIO 22.
- BH1750 address: `0x23`.
- LED heartbeat: GPIO 2.
- Presence OUT: GPIO 27.
- Sound AO: GPIO 34.
- MAX98357A BCLK/SCK: GPIO 26.
- MAX98357A LRC/WS: GPIO 25.
- MAX98357A DIN: GPIO 33.
- MAX98357A SD: nao conectar na POC atual (SD e shutdown/enable, nao data input).

## MQTT

- Topic: `home/bedroom/esp32-bedroom-01/environment`.
- Device ID: `esp32-bedroom-01`.
- Room: `bedroom`.

## Payload

Campos: `deviceId`, `room`, `measuredAt`, `temperatureCelsius`, `humidityPercentage`, `luminosityLux`. Campos sensoriais invalidos sao omitidos. Sem leitura sensorial valida ou sem NTP, nao publicar.

## Seguranca

Local-first. Sem broker publico, sem PostgreSQL publico, sem comando remoto, sem rele e sem automacao eletrica na POC.

## Relacao com backend

Backend deve consumir topico especifico ou wildcard `home/+/+/environment`, validar payload e persistir historico ambiental.

## Status atual

Firmware funcional base existe. Esta governanca cria SDD, docs, scripts, ADRs, backlog, prompts e approval review.

## Proximos riscos

- Validar hardware real.
- Confirmar consumo backend de payload parcial.
- Melhorar autenticacao MQTT antes de uso fora de laboratorio.

## Audio local ADR-0004

- Audio command topic: `home/bedroom/esp32-bedroom-01/audio/command`.
- Audio status topic: `home/bedroom/esp32-bedroom-01/audio/status`.
- MAX98357A BCLK/SCK: GPIO 26.
- MAX98357A LRC/WS: GPIO 25.
- MAX98357A DIN: GPIO 33.
- MAX98357A SD: nao conectar na POC atual (SD e shutdown/enable, nao data input).
- `play_tone`, `stop_audio`, `set_volume`, parser, fila e status MQTT implementados.
- `play_audio_url` e `play_audio_stream` validam URL local e contrato, mas decodificacao HTTP ainda exige decisao de biblioteca.
- Sem microfone, wake word, reconhecimento de voz, IA embarcada, rele ou broker publico.
# Firmware Mock Strategy Memory

- Use native tests for logic and contracts without ESP32.
- Use MQTT simulation scripts for backend/firmware contract traffic without hardware.
- Treat missing hardware as a physical/release blocker, not an automatic gate failure.
- Never claim DHT22, BH1750, I2S, MAX98357A, speaker, brownout, or COM-port validation without real evidence.
- Preserve invalid sensor behavior: omit invalid readings rather than inventing values.
- Test harness edits must follow `Test Harness Quality & Anti-False-Success` from `governance-kit.yaml`.
- Audio status tests require independent `accepted`, `playing`, `completed`, `stopped`, `rejected`, and `failed` coverage with clean mocks, explicit publish action, topic validation, payload validation, and reason presence/absence checks.
