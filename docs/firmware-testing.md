# Firmware Testing

## Testes unitários nativos

Use para validar regras puras sem ESP32 físico:

- `pio test -e native`
- cobre `EnvironmentReading`, tópico MQTT, payload JSON e contrato de publicação

## Testes embarcados

Use no ESP32 para smoke tests e integração com hardware:

- `pio test -e esp32dev`
- cobre leitura real do DHT e smoke tests de Wi-Fi/MQTT

## Validação de segredos

Execute o check local de segredos e placeholders:

- Windows: `py -3 scripts/check_secrets.py`

## Build e upload

- `pio run`
- `pio run --target upload`