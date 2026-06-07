# Firmware Audio Streaming Implementation Report

## 1. Status final

FIRMWARE_AUDIO_CONTROL_AND_TONE_IMPLEMENTED_WITH_STREAMING_STUBS

## 2. Objetivo

Adicionar capability local controlada de audio ao ESP32 sem quebrar o contrato ambiental nem introduzir microfone, wake word, IA embarcada, rele ou exposicao publica.

## 3. Escopo implementado

- Parser de comandos de audio.
- MQTT `audio/command` e `audio/status`.
- Status `accepted`, `playing`, `completed`, `rejected`, `failed`.
- Fila bounded com prioridade e interrupcao.
- `play_tone` via I2S/MAX98357A.
- `stop_audio`.
- `set_volume`.
- Validacao local/private URL para URL/stream.
- Stubs explicitos para `play_audio_url` e `play_audio_stream`: o firmware valida contrato e publica falha controlada quando o driver nao consegue iniciar esses modos.
- Gates, ADR, docs, backlog, prompt de backend e evidence run.

## 4. Fora do escopo preservado

- Microfone.
- Reconhecimento de voz.
- Wake word.
- IA no firmware.
- Rele ou automacao eletrica.
- MQTT publico.
- ESP32 exposto a internet.
- Decodificacao HTTP MP3/WAV/stream completa.

## 5. Arquivos criados

Ver `docs/evidence/runs/20260531-2100-firmware-audio-streaming/files-created.md`.

## 6. Arquivos alterados

Ver `docs/evidence/runs/20260531-2100-firmware-audio-streaming/files-updated.md`.

## 7. Arquitetura implementada

MQTT recebe intencao local, `AudioCommandParser` valida, `AudioPlaybackQueue` organiza, `AudioPlaybackService` orquestra, `I2sAudioOutputDriver` toca tom I2S e `AudioStatusPublisher` publica status. O driver atual nao baixa nem decodifica audio HTTP; `play_audio_url` e `play_audio_stream` sao caminhos de contrato com stub de falha controlada.

## 8. Contratos MQTT de audio

- Command: `home/bedroom/esp32-bedroom-01/audio/command`.
- Status: `home/bedroom/esp32-bedroom-01/audio/status`.
- Types: `play_audio_url`, `play_audio_stream`, `play_tone`, `stop_audio`, `set_volume`.

## 9. Pinagem de audio

- BCLK/SCK: GPIO 26.
- LRC/WS: GPIO 25.
- DIN/SD: GPIO 27.
- VIN: 5V/VIN.
- GND: GND.
- SPK+/SPK- apenas no alto-falante.

## 10. Seguranca

URL publica e rejeitada por padrao. Apenas `http://` local/private network e aceito. `commandId` e obrigatorio, volume e limitado a `0..100`, URL/stream exige `expiresAt`, existe max duration e `stop_audio`.

## 11. Testes criados

Testes nativos cobrem parser, command/status topics, subscription, queue, status payloads e preservacao do contrato ambiental.

## 12. Gates criados/atualizados

- `check-audio-contract.ps1`.
- `check-audio-docs-consistency.ps1`.
- `check-audio-security.ps1`.
- `check-audio-build-compatibility.ps1`.
- `check-firmware-contract.ps1`.
- `run-firmware-approval-review.ps1`.

## 13. Evidencias geradas

- `docs/evidence/runs/20260531-2100-firmware-audio-streaming/`.
- `docs/evidence/runs/20260531-2100-firmware-approval-review/`.
- `docs/evidence/runs/20260531-2100-firmware-audio-build-compatibility/`.

## 14. Comandos executados

Ver `docs/evidence/runs/20260531-2100-firmware-audio-streaming/commands.md`.

## 15. Resultado dos gates

| Gate | Status | Exit Code | Observacao |
|---|---:|---:|---|
| Native tests | PASSED | 0 | 44/44 na execucao original; revisado posteriormente porque o harness precisava de gate anti-codigo-morto |
| ESP32 build | PASSED | 0 | RAM 14.5%, Flash 65.0% |
| Firmware contract | PASSED | 0 | Ambiente + audio pins/topics |
| Docs consistency | PASSED | 0 | Docs canonicos |
| Secrets | PASSED_WITH_WARNINGS | 0 | `include/Secrets.h` local untracked |
| Audio contract | PASSED | 0 | Types/status/URL/max duration |
| Audio docs consistency | PASSED | 0 | ADR/docs/backlog |
| Audio security | PASSED | 0 | Local-only policy |
| Approval review | PASSED_WITH_WARNINGS | 0 | Warning de secret local e validacao fisica pendente |

## 16. Resultado dos testes nativos

44 testes passaram em `env:native` na execucao original. Esse numero foi reavaliado pelo gate de harness em `docs/audit/tmp/firmware-audio-harness-correction-report.md`.

## 17. Resultado dos testes fisicos

BLOCKED_BY_MISSING_HARDWARE. Upload, Serial Monitor, tom audivel, brownout e reboot nao foram validados nesta execucao.

## 18. Resultado MQTT real

BLOCKED_BY_BROKER_UNAVAILABLE. `mosquitto_sub` e `mosquitto_pub` reais nao foram executados nesta execucao.

## 19. Impacto no contrato ambiental

Contrato `home/bedroom/esp32-bedroom-01/environment` preservado. Payload ambiental continua omitindo campos invalidos e nao inventa leituras.

## 20. Impacto no backend

Backend precisa implementar endpoint local de audio/TTS, publicar comandos e registrar status. Prompt criado em `docs/prompts/backend-audio-contract-implementation-agent.md`.

## 21. Riscos encontrados

- PlatformIO dentro do sandbox falhou por permissao em `platforms.lock`; rerun aprovado fora do sandbox passou.
- Audio HTTP real ainda nao implementado.

## 22. Riscos residuais

- Validacao fisica pendente.
- MQTT real pendente.
- Backend/PostgreSQL pendentes.
- Autenticacao/TLS MQTT futura.
- Quiet-hours futura.

## 23. Decisoes pendentes

- Biblioteca de decodificacao HTTP MP3/WAV/stream.
- Politica de silencio/noturno.
- Evidencia fisica do MAX98357A.

## 24. Proximas acoes recomendadas

1. Validar `play_tone` com ESP32/MAX98357A.
2. Capturar MQTT real de command/status.
3. Implementar backend/TTS local.
4. Avaliar biblioteca de audio antes de URL/stream real.

## 25. Aprovacao recomendada

APPROVED_WITH_WARNINGS
