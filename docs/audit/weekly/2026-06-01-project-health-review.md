# Weekly Project Health Review - Home AI Room Observer

## 1. Status geral

Status: PROJECT_HEALTH_ORANGE

Resumo executivo:

- Backend e firmware foram analisados em conjunto.
- Firmware approval review automatic passou com warnings.
- Build ESP32 passou e 57 testes native passaram.
- Backend approval review passou com exit code 0.
- Maven `clean verify` registrou `BUILD SUCCESS`, com 186 testes unitarios e 44 testes de integracao sem falhas, mas o wrapper PowerShell retornou 1 por stderr/warnings e logs MQTT.
- Contrato ambiental backend/firmware esta alinhado em topico e payload principal.
- Release readiness de firmware segue bloqueada por ausencia de evidencia fisica recente.
- Audio local esta governado, mas `play_audio_url` e `play_audio_stream` seguem sem decoder HTTP real.
- Nao ha evidencia de MQTT fisico ESP32 -> broker -> backend -> PostgreSQL nesta revisao.
- A decisao recomendada e continuar com warnings e corrigir P1 antes de qualquer release fisico.

## 2. Escopo da revisao

- Data da revisao: 2026-06-01
- Periodo analisado: ultima semana disponivel nos repositorios, com foco em evidencias de 2026-05-24 a 2026-06-01.
- Repositorios analisados:
  - Firmware: `C:\Users\KABUM\OneDrive\Documents\PlatformIO\Projects\home-ai-room-observer-esp32`
  - Backend: `C:\Users\KABUM\IdeaProjects\smart-home`
- Branches analisadas:
  - Firmware: `main`
  - Backend: `main`
- Commit base:
  - Firmware: `d7808e1 Feedback com led adicionado`
  - Backend: `32d4e34 docs(audit): close contextual AI safe audio P0 review`
- Ferramentas disponiveis: Git, PowerShell, PlatformIO, Maven wrapper, governance profile runner.
- Ferramentas indisponiveis ou bloqueadas: validacao fisica ESP32, COM5/hardware nesta revisao, broker real com `mosquitto_pub/sub` end-to-end nesta revisao.

## 3. Sumario executivo

### 3.1 Principais conclusoes

O projeto tem boa cobertura automatica para firmware e backend, mas nao pode ser tratado como release-ready. Os gates automaticos de firmware passaram com warning de secret local ignorado, e o backend passou approval review. O Maven reportou build success, mas a execucao mostrou ruído operacional real: warnings de Mockito/JDK, logs MQTT de desconexao e `Surefire is going to kill self fork JVM`.

### 3.2 Principais riscos

- P1: validacao fisica de firmware/audio ausente.
- P1: fluxo ESP32 -> MQTT -> backend -> PostgreSQL sem evidencia real nova nesta revisao.
- P1: audio HTTP/stream documentado e validado por contrato, mas sem decoder/playback real.
- P2: wrapper PowerShell pode retornar falha mesmo quando Maven reporta `BUILD SUCCESS`.
- P2: `include/Secrets.h` existe localmente; gate aceita por estar ignorado, mas requer vigilancia.

### 3.3 Principais recomendacoes

Priorizar uma rodada fisica com hardware conectado, broker local e backend ativo. Depois corrigir o ruído do wrapper Maven/PowerShell para evitar falso vermelho. Manter `play_audio_url` e `play_audio_stream` como stub ate prova real de decoder.

## 4. Metricas da semana

| Metrica | Valor | Status | Observacao |
|---|---:|---|---|
| Builds firmware executados | 1 | PASS | `esp32dev SUCCESS` em PlatformIO |
| Testes firmware native | 57/57 | PASS | Approval review automatic elevado |
| Gates firmware automaticos | 13 | WARN | Todos passaram; secrets gate com warning |
| Backend approval review | 1 | PASS | `VALIDATION_PROFILE_EXIT_CODE: 0` |
| Testes backend unitarios | 186/186 | PASS | Maven surefire sem falhas |
| Testes backend integracao | 44/44 | PASS_WITH_WARNINGS | Maven failsafe sem falhas, com ruído MQTT/JDK |
| Evidencias novas | 1 run semanal | OK | `docs/evidence/runs/20260601-0000-weekly-project-health-review` |
| Evidencias fisicas recentes | 0 | MISSING | Physical review bloqueado por hardware ausente |
| Gaps P0 | 0 | OK | Nenhum P0 confirmado nesta revisao |
| Gaps P1 | 3 | RISK | Fisico, end-to-end real, audio HTTP/stream |
| ADRs pendentes | 0 criticos | OK | ADR-0004 cobre audio local limitado |
| Decisoes pendentes | 3 | WARN | Periodo de publicacao, auth MQTT, payload parcial |
| Contratos com drift | 0 criticos | OK | Ambiental alinhado; audio backend ainda parcial/documental |
| Stubs ativos | 2 | WARN | `play_audio_url`, `play_audio_stream` sem decoder |
| Secrets warnings | 1 | WARN | `include/Secrets.h` local ignorado |

## 5. Gaps criticos e prioritarios

| ID | Severidade | Area | Gap | Evidencia | Impacto | Acao recomendada |
|---|---|---|---|---|---|---|
| WHG-2026-06-01-001 | P1 | Firmware/Release | Sem evidencia fisica recente para ESP32, sensores, MQTT e audio | `FIRMWARE_PHYSICAL_VALIDATION_BLOCKED_BY_MISSING_HARDWARE`; `FIRMWARE_RELEASE_READINESS_BLOCKED_BY_MISSING_PHYSICAL_EVIDENCE` | Bloqueia release real | Conectar ESP32, liberar COM, rodar modo physical e capturar serial/MQTT |
| WHG-2026-06-01-002 | P1 | Integracao | Sem evidencia nova de ESP32 -> MQTT -> backend -> PostgreSQL real | Maven valida backend, mas nao substitui `mosquitto_pub/sub` e hardware | Risco de drift operacional | Rodar broker/backend/firmware juntos e salvar subscribe/persistencia |
| WHG-2026-06-01-003 | P1 | Audio | `play_audio_url` e `play_audio_stream` nao tem decoder HTTP/playback real | SDD e backlog declaram limitacao/stub | Risco de claim falso de streaming | Manter bloqueado ate spike de biblioteca, build, teste e validacao fisica |
| WHG-2026-06-01-004 | P2 | DX/Gates | Maven `BUILD SUCCESS` veio com wrapper exit 1 por stderr/warnings | Raw `backend-mvn-clean-verify.txt` | Pode gerar falso vermelho | Ajustar captura de stderr/exit code para separar warning de falha |
| WHG-2026-06-01-005 | P2 | Security/Ops | `include/Secrets.h` local existe | Secrets gate passou com warning | Risco se `.gitignore` mudar | Manter ignorado e revisar antes de commit |

## 6. Backend health

### 6.1 Build e testes

Backend approval review passou: `VALIDATION_PROFILE_EXIT_CODE: 0`.

`mvnw clean verify` registrou `BUILD SUCCESS`, `Tests run: 186, Failures: 0, Errors: 0, Skipped: 0` em surefire e `Tests run: 44, Failures: 0, Errors: 0, Skipped: 0` em failsafe. O wrapper PowerShell retornou 1 por stderr/warnings, entao a confiabilidade funcional e forte, mas a confiabilidade de automacao precisa ajuste.

### 6.2 Arquitetura

ArchitectureRulesTest rodou com 14 testes sem falhas. O backend mantem separacao para dominio, aplicacao, infraestrutura MQTT, REST e persistencia.

### 6.3 Seguranca

Approval review passou e docs de seguranca existem. A regra de audio consultivo permanece coerente: backend nao deve usar LLM como autoridade operacional e nao deve emitir claims de execucao.

### 6.4 Docs/SDD

Docs e ADRs recentes cobrem Home Brain, policy engine e audio advisory. Nao foi encontrada falha P0 documental nesta revisao.

### 6.5 Backlog

Backlog nao foi alterado nesta revisao. O principal drift potencial e cross-repo: firmware ja tem contrato de audio command/status; backend tem advisory seguro, mas evidencia de publicacao real de comando/status ainda precisa ser atualizada quando essa etapa entrar em escopo.

### 6.6 Evidencias

Evidence run do backend foi criado pelo profile runner: `20260531-2135-approval-review`. A copia resumida esta em `raw/backend-approval-review-elevated.txt`.

### 6.7 Riscos

O Maven gerou logs recorrentes de `Lost connection` no adaptador MQTT durante testes, mas sem falha de suite. Tratar como risco operacional P2 ate confirmar que e comportamento esperado do ambiente de teste.

## 7. Firmware health

### 7.1 Build e testes

Build `esp32dev` passou em 14.52s. Testes native passaram: 57/57. Gates de contrato, docs, audio, mock MQTT, abstracoes e politica fisica passaram.

### 7.2 Mock strategy

Mocks nativos estao presentes para sensores, MQTT, relogio, network e audio. O projeto separa automatic, physical e release review, evitando que mock vire sucesso fisico.

### 7.3 Validacao fisica

Bloqueada. Evidencia existente declara explicitamente `FIRMWARE_PHYSICAL_VALIDATION_BLOCKED_BY_MISSING_HARDWARE` e release readiness bloqueado por falta de evidencia fisica.

### 7.4 Audio

Audio local controlado esta governado por ADR-0004. `play_tone`, `stop_audio`, `set_volume`, parser, fila e status estao cobertos por testes/build. `play_audio_url` e `play_audio_stream` validam contrato e URL local, mas playback HTTP real segue pendente.

### 7.5 MQTT

Contrato ambiental oficial: `home/{room}/{deviceId}/environment`; backend assina `home/+/+/environment`. Firmware publica `deviceId`, `room`, campos sensoriais validos e `measuredAt`. Payload parcial e aceito desde que tenha ao menos uma leitura valida.

### 7.6 Seguranca

Secrets gate passou com warning porque `include/Secrets.h` existe localmente e esta ignorado. Audio rejeita URL publica, exige `commandId`, restringe volume/duracao e mantem microfone/wake word/automacao fora do escopo.

### 7.7 Docs/SDD

Há drift textual baixo no SDD: a secao antiga ainda diz que audio e futuro, mas o historico e a secao 26 corrigem o estado atual. Classificacao: SDD_DRIFT_LOW.

### 7.8 Backlog

Backlog reconhece audio URL/stream como parcial/pendente e validacao fisica como bloqueada. Classificacao: BACKLOG_HEALTH_WARNING.

### 7.9 Evidencias

Evidence automatico recente existe. Evidence fisico real nao existe nesta revisao.

### 7.10 Riscos

Principal risco: confundir build/teste/mock com release fisico. O proprio projeto ja tem gates para bloquear isso.

## 8. Integracao backend -> firmware

### 8.1 Contrato ambiental

CONTRACT_DRIFT_NONE para contrato principal. Firmware usa `home/bedroom/esp32-bedroom-01/environment`; backend configura `home/+/+/environment`; payload inclui `deviceId`, `room`, `measuredAt`, `temperatureCelsius`, `humidityPercentage`, `luminosityLux`.

### 8.2 Contrato de audio

CONTRACT_DRIFT_MEDIUM. Firmware documenta e testa `audio/command` e `audio/status`. Backend tem audio advisory seguro, mas a prova de publicacao/ingestao real do comando/status nao foi executada nesta revisao.

### 8.3 Drift detectado

- Ambiental: nenhum drift critico.
- Audio: drift medio por falta de evidencia operacional cross-repo.
- SDD: baixo, por texto legado dizendo audio futuro antes da secao corretiva.

### 8.4 Acoes recomendadas

Rodar um teste integrado com backend emitindo comando de audio seguro para broker local, firmware consumindo e publicando status, e backend/UI registrando o status ou bloqueio.

## 9. SDD, ADRs e decisoes

### 9.1 SDD drift

SDD_DRIFT_LOW. O SDD tem secao corretiva de audio, mas tambem conserva texto anterior dizendo que audio e futuro.

### 9.2 ADRs novas

ADR-0004 cobre audio local controlado, MQTT apenas para comando/status e HTTP local/private como transporte futuro.

### 9.3 ADRs ausentes

Nenhuma ADR P0 ausente detectada para o escopo implementado.

### 9.4 Decisoes pendentes antigas

Periodo definitivo de publicacao, autenticacao MQTT fora de laboratorio e estrategia backend para payload parcial continuam pendentes.

## 10. Seguranca e superficie de ataque

### 10.1 Riscos encontrados

- SECURITY_RISK_MEDIUM: MQTT local sem TLS/autenticacao aceitavel apenas em POC local.
- SECURITY_RISK_MEDIUM: secrets locais existem e dependem de `.gitignore`.
- SECURITY_RISK_MEDIUM: audio command exige manter broker local fechado.

### 10.2 Mitigacoes existentes

Secrets gate, `.gitignore`, URL local/private only, sem microfone/wake word, sem rele/automacao, guardrail backend para mensagens consultivas.

### 10.3 Mitigacoes ausentes

Sem auth/TLS MQTT para uso fora de laboratorio. Sem evidence real de rede isolada nesta revisao.

### 10.4 Riscos P0/P1

Nenhum P0 confirmado. P1 para release fisico sem evidencia, se tentado.

## 11. Evidencias e confiabilidade dos claims

### 11.1 Claims com evidencia forte

- Firmware compila para ESP32.
- 57 testes native passam.
- Backend approval review passa.
- Backend Maven reporta build success e suites sem falhas.
- Contrato ambiental documentado e testado.

### 11.2 Claims com evidencia fraca

- Robustez operacional MQTT em runtime real.
- Reconexao real ESP32 em Wi-Fi/MQTT.
- Integração backend/firmware em ambiente fisico atual.

### 11.3 Claims sem evidencia

- Audio audivel real.
- `play_audio_url`/`play_audio_stream` com decoder HTTP real.
- Persistencia PostgreSQL a partir de leitura fisica atual do ESP32.

### 11.4 Evidencias antigas ou invalidas

Evidencias de stress MQTT de 2026-05-24 sao uteis historicamente, mas nao substituem run fisico atual apos mudancas de audio/governanca.

## 12. Operacao local e DX

### 12.1 Scripts

Firmware tem approval review automatic/physical/release. Backend tem profile runner governado.

### 12.2 Dependencias

PlatformIO e Maven funcionam quando executados com permissao para caches e repositorios externos ao workspace.

### 12.3 Ferramentas ausentes

Nenhuma ferramenta essencial ausente para validacao automatica. Hardware/COM e broker real end-to-end nao foram disponibilizados nesta revisao.

### 12.4 Bloqueios recorrentes

Sandbox bloqueia PlatformIO global e escrita no repo backend; validacoes foram rerodadas com permissao elevada. PowerShell transforma stderr de Maven em erro de wrapper.

## 13. Tendencias da semana

### 13.1 Melhorias detectadas

Governanca de firmware amadureceu: gates automaticos, modos de approval review, mock strategy, contrato de audio e bloqueios fisicos explicitos.

### 13.2 Regressoes detectadas

Nenhuma regressao funcional confirmada. Há ruído operacional no Maven/PowerShell e logs MQTT.

### 13.3 Riscos acumulando

Audio esta crescendo em escopo e precisa de validacao fisica antes de qualquer claim de release.

### 13.4 Divida tecnica aumentando

Stub HTTP audio e captura de exit code do Maven sao as dividas principais.

## 14. Plano de acao recomendado

### 14.1 Proximas 24-48h

1. Conectar ESP32 e validar COM.
2. Rodar `run-firmware-approval-review.ps1 -Mode physical`.
3. Capturar serial, MQTT command/status e broker subscribe.
4. Ajustar wrapper de Maven para separar stderr warning de exit code real.

### 14.2 Proxima semana

1. Rodar teste cross-repo ESP32 -> MQTT -> backend -> PostgreSQL.
2. Atualizar SDD removendo texto legado de audio como futuro absoluto.
3. Criar evidencia backend para audio command/status quando o escopo de backend for executado.

### 14.3 Proximas 2-4 semanas

1. Decidir biblioteca de decoder HTTP audio ou remover claim de URL/stream do release.
2. Definir auth/TLS MQTT para alem do laboratorio.
3. Formalizar periodo definitivo de publicacao.

## 15. Decisao recomendada

Escolha: CONTINUE_WITH_WARNINGS

Justificativa: automatic gates e builds estao fortes, mas release fisico segue bloqueado por evidencia ausente. Nao ha P0 ativo confirmado; ha P1s que devem ser resolvidos antes de release real.

## 16. Anexos

- Evidence run: `docs/evidence/runs/20260601-0000-weekly-project-health-review`
- Comandos executados: `docs/evidence/runs/20260601-0000-weekly-project-health-review/commands.md`
- Relatorios relacionados:
  - `docs/evidence/runs/20260531-2129-firmware-approval-review-release/validation-result.md`
  - `docs/evidence/runs/20260531-2117-firmware-approval-review-physical/validation-result.md`
- Arquivos criticos inspecionados:
  - `platformio.ini`
  - `scripts/governance/run-firmware-approval-review.ps1`
  - `docs/sdd/firmware-sdd.md`
  - `docs/backlog/firmware-backlog.md`
  - Backend `pom.xml`
  - Backend `governance-kit.yaml`
