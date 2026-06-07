# ADR-0003 - Hardware Pinout Baseline

## Status

Accepted

## Contexto

Pinagem incorreta pode impedir boot, quebrar leitura ou danificar componentes.

## Decisao

Pinagem oficial:

- ESP32 DevKit V1.
- DHT22/AM2302 DATA em GPIO 4.
- BH1750/GY-302 SDA em GPIO 21.
- BH1750/GY-302 SCL em GPIO 22.
- BH1750/GY-302 ADDR em GND para `0x23`.
- Audio e alto-falantes: FUTURE / NOT IMPLEMENTED.

## Consequencias

GPIO 4 e strapping pin e deve continuar visivel como risco. Alterar pinagem exige atualizacao de codigo, docs, SDD e ADR ou nova ADR.
