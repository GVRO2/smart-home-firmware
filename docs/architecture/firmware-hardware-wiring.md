# Firmware Hardware Wiring

## Hardware atual

- ESP32 DevKit V1.
- DHT22/AM2302.
- BH1750/GY-302.

## BH1750 / GY-302

| BH1750/GY-302 | ESP32 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| SDA | GPIO 21 |
| SCL | GPIO 22 |
| ADDR | GND |

Endereco oficial: `0x23`. Se ADDR for ligado em 3V3, o endereco vira `0x5C` e o firmware deve ser alterado com docs e testes.

## DHT22 / AM2302

| DHT22/AM2302 | ESP32 |
|---|---|
| VCC | 3V3 |
| GND | GND |
| DATA | GPIO 4 |

Sensor cru deve usar resistor pull-up de 4.7k a 10k entre DATA e 3V3. Modulos prontos podem ja incluir pull-up.

## Audio - MAX98357A I2S

| MAX98357A | ESP32 |
|---|---|
| VIN | 5V/VIN |
| GND | GND |
| BCLK/SCK | GPIO 26 |
| LRC/WS | GPIO 25 |
| DIN | GPIO 33 |
| SD | nao conectar na POC atual |
| SPK+ | positivo do alto-falante |
| SPK- | negativo do alto-falante |

O pino SD do MAX98357A e shutdown/enable (nao data input) e deve ser deixado desconectado na POC atual para que o chip utilize seu pull-up interno e opere sempre habilitado. O pino DIN e o data input e deve ser conectado ao GPIO 33.

Com um unico MAX98357A, usar um alto-falante. Para dois alto-falantes, preferir dois amplificadores ou documentar claramente a limitacao. Nunca ligar SPK- ao GND se o modulo usar saida diferencial.

## Sensores Adicionais (Reservas Físicas)

| Sensor | ESP32 |
|---|---|
| Presence OUT | GPIO 27 |
| Sound AO | GPIO 34 |

Regras eletricas:

- Nao alimentar alto-falante diretamente pelo ESP32.
- Nao ligar dois alto-falantes em paralelo sem validacao eletrica.
- Validar consumo de corrente do amplificador e do alto-falante.
- Registrar brownout, ruido, travamento ou reboot no evidence run.
- Adicionar capacitor ou mitigacao de fonte somente com evidencia.

## Regras de mudanca

Qualquer alteracao de pinagem deve atualizar `include/AppConfig.h`, este documento, o SDD, ADR aplicavel e gates de contrato.
