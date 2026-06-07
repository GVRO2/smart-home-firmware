# Firmware Release Readiness

## Criterios para POC pronta

- Firmware compila em `esp32dev`.
- Testes nativos passam.
- ESP32 conecta ao Wi-Fi local.
- ESP32 conecta ao broker MQTT local.
- DHT22/AM2302 le temperatura e umidade validas ou registra falha sem travar.
- BH1750/GY-302 le luminosidade valida ou registra falha sem travar.
- Payload JSON valido e publicado no topico oficial.
- Audio command/status documentado e validado por gates.
- `play_tone` I2S evidenciado antes de declarar prontidao de alto-falante.
- `play_audio_url`/`play_audio_stream` com decodificacao HTTP evidenciados antes de declarar streaming pronto.
- Reconexao Wi-Fi/MQTT evidenciada apos queda controlada.
- Sensor invalido nao trava o loop.
- Documentacao canonica atualizada.
- Evidencia registrada em `docs/evidence/runs/`.
- Contrato com backend validado.

## Limite do approval review

`scripts/governance/run-firmware-approval-review.ps1` valida governanca local automatizavel. Ele nao prova upload, hardware real, MQTT real ou persistencia backend sem evidencia fisica adicional.

Para audio, o approval review tambem nao prova audibilidade, ausencia de brownout, consumo eletrico ou reproducao HTTP real. Esses itens exigem evidence run fisico.

## Status possiveis

- READY_FOR_POC_WITH_EVIDENCE.
- READY_FOR_POC_WITH_WARNINGS.
- NOT_READY.
- BLOCKED_BY_ENVIRONMENT.
# Firmware Release Readiness

Firmware release readiness is not granted by mocks alone. A release candidate must pass automatic gates and include current or recent compatible physical evidence for ESP32 boot, sensors, MQTT, and audio hardware when audio is in scope.

Missing physical evidence blocks release readiness with `FIRMWARE_RELEASE_READINESS_BLOCKED_BY_MISSING_PHYSICAL_EVIDENCE`.
