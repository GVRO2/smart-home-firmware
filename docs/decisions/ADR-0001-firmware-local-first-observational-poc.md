# ADR-0001 - Firmware Local-First Observational POC

## Status

Accepted

## Contexto

A POC atual mede ambiente do quarto e envia dados ao backend local. Ela nao deve controlar cargas eletricas nem aceitar comandos externos.

## Decisao

O firmware permanece local-first e observacional:

- Wi-Fi local.
- MQTT local.
- Sem rele.
- Sem automacao eletrica.
- Sem comandos remotos.
- Sem broker publico.

## Consequencias

Qualquer mudanca que introduza comando, rele, OTA, audio interativo ou automacao eletrica exige nova historia, threat model e ADR.
