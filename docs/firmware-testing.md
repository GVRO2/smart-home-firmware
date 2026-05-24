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

## Validacao manual do BH1750

Com o GY-302 V322 conectado em SDA GPIO 21, SCL GPIO 22 e ADDR no GND, abra:

- `pio device monitor`

Logs esperados:

- `BH1750 inicializado no endereco I2C 0x23`
- ou `Falha ao inicializar BH1750 no endereco I2C 0x23`

Quando a leitura for valida, o payload MQTT deve incluir `luminosityLux`.
