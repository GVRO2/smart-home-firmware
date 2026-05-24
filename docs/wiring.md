# Pinagem ESP32 - Home AI Room Observer Firmware

## DHT22 / AM2302

| Sensor | Pino | ESP32 |
|---|---|---|
| DHT22 | VCC | 3V3 |
| DHT22 | GND | GND |
| DHT22 | DATA | GPIO 4 |

Sensor cru de 4 pinos precisa de resistor pull-up de 4.7k a 10k entre DATA e
VCC. Modulos prontos podem ja incluir esse resistor.

## GY-302 V322 / BH1750

| Sensor | Pino | ESP32 |
|---|---|---|
| GY-302 | VCC | 3V3 |
| GY-302 | GND | GND |
| GY-302 | SCL | GPIO 22 |
| GY-302 | SDA | GPIO 21 |
| GY-302 | ADDR | GND |

## Endereco I2C

| ADDR | Endereco |
|---|---|
| GND | `0x23` |
| 3V3 | `0x5C` |

O firmware usa `0x23`; nao deixe ADDR solto.
