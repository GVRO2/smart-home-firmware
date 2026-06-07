# Firmware Governance

## Regra central

O firmware e um componente observacional local-first. Agentes devem preservar contrato MQTT, pinagem, seguranca e evidencia antes de declarar qualquer entrega como pronta.

A POC atual deve permanecer sem rele, sem automacao eletrica e sem comandos remotos.

## Como alterar firmware

1. Ler `README.md`, `docs/sdd/firmware-sdd.md`, `docs/architecture/firmware-hardware-wiring.md` e `docs/squad/firmware-agent-memory.md`.
2. Verificar `git status --short`.
3. Identificar se a mudanca afeta sensor, payload, pinagem, rede ou seguranca.
4. Alterar codigo com escopo minimo.
5. Atualizar docs e ADRs quando contrato ou decisao mudar.
6. Rodar gates aplicaveis e registrar evidencia.

## Como alterar documentacao

Docs canonicos vivem em `docs/sdd`, `docs/architecture`, `docs/governance`, `docs/decisions`, `docs/backlog` e `docs/squad`. Relatorios temporarios vivem em `docs/audit/tmp`.

## Decisoes

Decisoes duradouras devem virar ADR. Relatorio temporario nao substitui ADR.

## Evidencias

Toda validacao relevante deve gerar pasta em `docs/evidence/runs/<YYYYMMDD-HHMM-task-name>/` com comandos, resultados e riscos.

## Status

Use status explicito: passed, passed with warnings, failed ou blocked. Nao transformar falha real em warning.

## Drift firmware/backend

Mudancas em topico, payload, deviceId, room ou semantica de campos exigem comparacao com backend antes de aprovar.
