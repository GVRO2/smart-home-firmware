# Firmware Backlog

## Product Goal

Transformar o firmware ESP32 em componente observacional confiavel do Home AI Room Observer, com dados ambientais rastreaveis ate o backend.

## EPIC TEST - Firmware Mock, Simulation, and Physical Validation

### STORY-TEST-001 - Definir estrategia de testes sem ESP32

- Tipo: Technical Story
- Prioridade: P0
- Objetivo: separar testes automáticos, simulacao MQTT, validacao fisica e release readiness.
- Escopo: documentos em `docs/testing` e gates de politica.
- Fora do escopo: substituir validacao fisica por mock.
- Requisitos funcionais: explicar quando usar cada nivel e como interpretar bloqueios.
- Requisitos tecnicos: comandos PowerShell reproduziveis.
- Requisitos nao funcionais: auditavel e sem falso sucesso.
- Criterios de aceite: `firmware-test-strategy.md` existe e cita os quatro niveis.
- Cenarios negativos: hardware ausente bloqueia apenas modo fisico/release.
- Evidencias esperadas: output de `check-physical-validation-policy.ps1`.
- Definition of Ready: firmware repo acessivel.
- Definition of Done: estrategia revisada e versionada.

### STORY-TEST-002 - Criar abstracoes mockaveis para hardware e infraestrutura

- Tipo: Technical Story
- Prioridade: P0
- Objetivo: permitir regra de negocio sem dependencia direta de hardware.
- Escopo: `EnvironmentSensor`, `LightSensor`, `AudioOutputDriver`, `MqttPublisherPort`, `ClockProvider`, `NetworkConnection`.
- Fora do escopo: refatoracao ampla de pinagem.
- Requisitos funcionais: drivers reais ficam atras de portas.
- Requisitos tecnicos: headers em `include`.
- Requisitos nao funcionais: baixo impacto no firmware existente.
- Criterios de aceite: `check-hardware-abstractions.ps1` passa.
- Cenarios negativos: teste native nao deve depender de I2S real.
- Evidencias esperadas: gate de abstracoes.
- Definition of Ready: interfaces atuais identificadas.
- Definition of Done: portas e equivalentes mock existem.

### STORY-TEST-003 - Criar mocks nativos para sensores, audio, MQTT e relogio

- Tipo: Technical Story
- Prioridade: P0
- Objetivo: executar fluxos sem ESP32, Wi-Fi, broker ou sensores reais.
- Escopo: mocks em `include/mocks`.
- Fora do escopo: simular comportamento eletrico.
- Requisitos funcionais: controlar leituras, status e falhas.
- Requisitos tecnicos: compilar em `env:native`.
- Requisitos nao funcionais: deterministico.
- Criterios de aceite: testes native usam mocks.
- Cenarios negativos: mock nao pode declarar sucesso fisico.
- Evidencias esperadas: `native-tests-output.txt`.
- Definition of Ready: portas criadas.
- Definition of Done: mock de cada porta critica criado.

### STORY-TEST-004 - Criar testes native para contratos e regras sem hardware

- Tipo: Technical Story
- Prioridade: P0
- Objetivo: cobrir payload ambiental, parser, fila, status e orquestracao de audio.
- Escopo: `test/test_native`.
- Fora do escopo: upload ou monitor serial.
- Requisitos funcionais: rejeitar JSON invalido, URL publica, volume invalido, fila cheia e comando expirado.
- Requisitos tecnicos: `platformio test -e native`.
- Requisitos nao funcionais: sem dependencia de Serial.
- Criterios de aceite: native tests passam ou falham com exit code real.
- Cenarios negativos: build/test falho nao vira warning.
- Evidencias esperadas: `native-tests-output.txt`.
- Definition of Ready: mocks disponiveis.
- Definition of Done: fluxo mock de comando ate status coberto.

### STORY-TEST-005 - Criar simuladores MQTT para ambiente e audio

- Tipo: Technical Story
- Prioridade: P1
- Objetivo: validar contrato MQTT sem ESP32.
- Escopo: scripts em `scripts/dev`.
- Fora do escopo: expor broker publicamente.
- Requisitos funcionais: publicar ambiente, comando de audio e status.
- Requisitos tecnicos: usar `mosquitto_pub` e detectar ferramenta ausente.
- Requisitos nao funcionais: logs com topico, payload, comando e exit code.
- Criterios de aceite: `check-mock-mqtt-contract.ps1` passa.
- Cenarios negativos: broker ausente retorna bloqueio especifico.
- Evidencias esperadas: output do cenario mock.
- Definition of Ready: contrato MQTT documentado.
- Definition of Done: scripts e docs criados.

### STORY-TEST-006 - Separar approval review em automatic, physical e release

- Tipo: Quality Gate
- Prioridade: P0
- Objetivo: evitar que gates automaticos dependam de hardware.
- Escopo: `run-firmware-approval-review.ps1`.
- Fora do escopo: aprovar release sem evidencia fisica.
- Requisitos funcionais: `-Mode automatic`, `-Mode physical`, `-Mode release`.
- Requisitos tecnicos: registrar evidencia por modo.
- Requisitos nao funcionais: status explicitos.
- Criterios de aceite: automatic nao exige ESP32; physical e release bloqueiam corretamente.
- Cenarios negativos: ausencia de hardware nao vira sucesso fisico.
- Evidencias esperadas: `approval-review-automatic-output.txt`.
- Definition of Ready: gates automaticos existentes.
- Definition of Done: modos implementados e documentados.

### STORY-TEST-007 - Criar politica de validacao fisica e release readiness

- Tipo: Governance Story
- Prioridade: P0
- Objetivo: definir quando evidencia fisica e obrigatoria.
- Escopo: docs de politica e gate de consistencia.
- Fora do escopo: inventar evidencia ou apagar historico.
- Requisitos funcionais: listar invalidadores de evidencia antiga.
- Requisitos tecnicos: gate verifica textos obrigatorios.
- Requisitos nao funcionais: fail closed.
- Criterios de aceite: release sem evidencia retorna bloqueio especifico.
- Cenarios negativos: mock-only release rejeitado.
- Evidencias esperadas: policy gate e relatorio final.
- Definition of Ready: docs de testing criados.
- Definition of Done: politica versionada e gateada.

## EPIC 1 - Firmware Observacional do Quarto

### FEATURE 1 - Leitura ambiental

#### Story: Ler temperatura e umidade do DHT22/AM2302

- Status: Implemented.
- Subtasks: manter validacao de faixa; evidenciar leitura real.

#### Story: Ler luminosidade do BH1750/GY-302

- Status: Implemented.
- Subtasks: manter endereco `0x23`; evidenciar lux real.

#### Story: Tratar falha de leitura sem travar firmware

- Status: Implemented with further evidence needed.
- Subtasks: registrar skip reasons; criar teste fisico de sensor desconectado.

#### Story: Normalizar valores antes de publicar

- Status: Implemented.
- Subtasks: alinhar backend sobre payload parcial.

### FEATURE 2 - Publicacao MQTT

#### Story: Conectar ao Wi-Fi local

- Status: Implemented.
- Subtasks: evidenciar reconexao real.

#### Story: Conectar ao broker MQTT local

- Status: Implemented.
- Subtasks: evidenciar broker real.

#### Story: Publicar payload ambiental valido

- Status: Implemented.
- Subtasks: capturar `mosquitto_sub`.

#### Story: Reconectar apos queda de Wi-Fi/MQTT

- Status: Implemented with further evidence needed.
- Subtasks: teste controlado de queda.

#### Story: Registrar logs seriais uteis

- Status: Implemented.
- Subtasks: manter logs sem secrets.

### FEATURE 3 - Contrato com backend

#### Story: Alinhar topico MQTT com backend

- Status: In progress.
- Subtasks: comparar backend consumer.

#### Story: Alinhar payload JSON com backend

- Status: In progress.
- Subtasks: validar payload parcial.

#### Story: Criar evidencia de publicacao valida

- Status: Pending hardware.

#### Story: Criar evidencia de consumo pelo backend

- Status: Pending backend runtime.

### FEATURE 4 - Governanca e qualidade

#### Story: Criar SDD do firmware

- Status: Implemented by governance bootstrap.

#### Story: Criar documentacao de pinagem

- Status: Implemented by governance bootstrap.

#### Story: Criar harness local

- Status: Implemented by governance bootstrap.

#### Story: Criar quality gates basicos

- Status: Implemented by governance bootstrap.

#### Story: Criar politica de evidencias

- Status: Implemented by governance bootstrap.

#### Story: Criar memoria persistida do agente

- Status: Implemented by governance bootstrap.

### FEATURE 5 - Itens futuros

Todas as historias abaixo sao P3/Draft e nao prontas para desenvolvimento sem decisao formal:

- Integrar sensor de presenca.
- Integrar sensor de som.
- Integrar amplificador e alto-falantes.
- Criar alertas locais.
- Avaliar OTA seguro.
- Avaliar comandos MQTT futuros.
- Avaliar IA local.

## EPIC 2 - Local AI Audio Communication

### FEATURE - Audio Output via I2S

#### STORY-AUDIO-001 - Documentar decisao de saida de audio local

- Prioridade: P0.
- Tipo: Architecture / ADR.
- Status: Implemented.
- Contexto: backend/IA precisa emitir comunicacao audivel local sem transformar o ESP32 em assistente de voz.
- Objetivo: criar ADR-0004 com decisao MQTT controle + HTTP local + I2S/MAX98357A.
- Escopo: ADR, riscos, mitigacoes, limites de produto.
- Fora do escopo: microfone, wake word, IA embarcada, rele, broker publico.
- Requisitos funcionais: decisao deve declarar `play_tone`, URL/stream, stop, volume e status.
- Requisitos tecnicos: referenciar topicos e pinos.
- Nao funcionais: local-first, fail closed, evidencia fisica obrigatoria.
- Aceite Dado/Quando/Entao: Dado firmware observacional, Quando audio for aprovado, Entao ADR-0004 existe e limita escopo.
- Cenarios negativos: ausencia de ADR bloqueia audio.
- Seguranca: sem internet publica, sem microfone.
- Testes: gate de docs.
- Evidencias esperadas: ADR e approval review.
- Definition of Ready: necessidade de audio local definida.
- Definition of Done: ADR versionada e citada em SDD/docs.

#### STORY-AUDIO-002 - Implementar contrato MQTT de comando/status de audio

- Prioridade: P0.
- Tipo: Technical Story.
- Status: Implemented.
- Contexto: backend precisa mandar intencao e receber estado.
- Objetivo: criar `audio/command` e `audio/status`.
- Escopo: topicos, tipos e estados.
- Fora do escopo: audio bruto por MQTT.
- Requisitos funcionais: `accepted`, `playing`, `completed`, `rejected`, `failed`.
- Requisitos tecnicos: topicos derivados de room/device.
- Nao funcionais: nao quebrar `environment`.
- Aceite Dado/Quando/Entao: Dado MQTT conectado, Quando comando chega, Entao status e publicado.
- Cenarios negativos: topico generico nao autorizado.
- Seguranca: topico especifico do dispositivo.
- Testes: native topic/status tests.
- Evidencias esperadas: native output e gates.
- Definition of Ready: ADR-0004 aceita.
- Definition of Done: topicos em codigo, docs e gates.

#### STORY-AUDIO-003 - Implementar parser validado de comandos de audio

- Prioridade: P0.
- Tipo: Technical Story.
- Status: Implemented.
- Contexto: payload de comando nao confiavel deve falhar fechado.
- Objetivo: validar JSON, tipo, `commandId`, volume, expiracao e URL local.
- Escopo: parser sem hardware e sem MQTT direto.
- Fora do escopo: playback.
- Requisitos funcionais: aceitar comandos validos e rejeitar invalidos.
- Requisitos tecnicos: ArduinoJson, `AudioCommandParseResult`.
- Nao funcionais: testavel em native.
- Aceite Dado/Quando/Entao: Dado URL publica, Quando parser rodar, Entao rejeita.
- Cenarios negativos: sem `commandId`, tipo desconhecido, volume fora de faixa, expirado.
- Seguranca: URL local/private only.
- Testes: parser native tests.
- Evidencias esperadas: 44 native tests passing.
- Definition of Ready: contrato definido.
- Definition of Done: parser e testes verdes.

#### STORY-AUDIO-004 - Implementar fila e orquestracao de reproducao

- Prioridade: P1.
- Tipo: Technical Story.
- Status: Implemented.
- Contexto: audio nao deve bloquear sensores.
- Objetivo: fila bounded com prioridade e interrupcao.
- Escopo: queue, service loop, status.
- Fora do escopo: decoder HTTP.
- Requisitos funcionais: enfileirar, rejeitar cheia, respeitar prioridade, limpar em stop.
- Requisitos tecnicos: `AudioPlaybackQueue`, `AudioPlaybackService`.
- Nao funcionais: loop principal continua.
- Aceite Dado/Quando/Entao: Dado fila cheia, Quando novo comando chegar, Entao publica failed.
- Cenarios negativos: interrupcao limpa fila.
- Seguranca: max duration e stop.
- Testes: queue native tests.
- Evidencias esperadas: native output.
- Definition of Ready: parser pronto.
- Definition of Done: service compila em `esp32dev`.

#### STORY-AUDIO-005 - Implementar driver I2S para MAX98357A

- Prioridade: P1.
- Tipo: Technical Story.
- Status: Implemented with physical evidence pending.
- Contexto: ESP32 precisa gerar saida I2S.
- Objetivo: encapsular I2S em driver.
- Escopo: init I2S, pinos, volume, stop, loop.
- Fora do escopo: decoder MP3/WAV.
- Requisitos funcionais: inicializar I2S e tocar tom.
- Requisitos tecnicos: `driver/i2s.h`, compile guard native.
- Nao funcionais: nao travar sensores.
- Aceite Dado/Quando/Entao: Dado build esp32dev, Quando compilar, Entao driver linka.
- Cenarios negativos: init falha publica failure.
- Seguranca: nao alimentar alto-falante por GPIO.
- Testes: build esp32dev.
- Evidencias esperadas: build-output e manual audio plan.
- Definition of Ready: pinagem definida.
- Definition of Done: build passa; validacao fisica ainda pendente.

#### STORY-AUDIO-006 - Implementar play_tone para validacao fisica minima

- Prioridade: P1.
- Tipo: Technical Story.
- Status: Implemented with physical evidence pending.
- Contexto: tom curto reduz dependencia de backend/TTS.
- Objetivo: tocar senoide I2S por duracao limitada.
- Escopo: frequencia, duracao, volume.
- Fora do escopo: audio falado.
- Requisitos funcionais: `play_tone` aceito e executado.
- Requisitos tecnicos: `startTone` e `loop`.
- Nao funcionais: max duration.
- Aceite Dado/Quando/Entao: Dado comando `play_tone`, Quando recebido, Entao status vai para `playing` e depois `completed`.
- Cenarios negativos: frequencia/duracao invalidas rejeitadas.
- Seguranca: volume limitado.
- Testes: parser e build; fisico manual pendente.
- Evidencias esperadas: serial e audibilidade.
- Definition of Ready: driver I2S.
- Definition of Done: codigo e testes; DoD fisico pendente.

#### STORY-AUDIO-007 - Implementar play_audio_url via HTTP local

- Prioridade: P1.
- Tipo: Technical Story.
- Status: Partial.
- Contexto: backend/TTS deve hospedar arquivo local.
- Objetivo: reproduzir URL local via I2S.
- Escopo implementado: contrato, parser local-only, fila e status de falha.
- Fora do escopo atual: decodificacao HTTP MP3/WAV.
- Requisitos funcionais: rejeitar URL publica e expirada.
- Requisitos tecnicos: futura biblioteca avaliada separadamente.
- Nao funcionais: timeout e max duration.
- Aceite Dado/Quando/Entao: Dado URL local valida, Quando decoder existir, Entao reproduzir e completar.
- Cenarios negativos: URL invalida publica `rejected` ou `failed`.
- Seguranca: local/private `http://`.
- Testes: parser/security gates.
- Evidencias esperadas: future HTTP playback run.
- Definition of Ready: biblioteca escolhida.
- Definition of Done: pendente para streaming real.

#### STORY-AUDIO-008 - Avaliar e implementar play_audio_stream

- Prioridade: P2.
- Tipo: Spike / Technical Story.
- Status: Pending.
- Contexto: stream pode aumentar latencia/memoria.
- Objetivo: decidir viabilidade tecnica.
- Escopo: benchmark de biblioteca e RAM/flash.
- Fora do escopo: liberar sem evidencia.
- Requisitos funcionais: stream interrompivel.
- Requisitos tecnicos: max duration e timeout.
- Nao funcionais: nao derrubar MQTT/sensores.
- Aceite Dado/Quando/Entao: Dado benchmark aprovado, Quando implementado, Entao status e evidencia existem.
- Cenarios negativos: memoria insuficiente bloqueia.
- Seguranca: URL local e expiracao.
- Testes: native/build/manual.
- Evidencias esperadas: spike report.
- Definition of Ready: biblioteca candidata.
- Definition of Done: decisao documentada.

#### STORY-AUDIO-009 - Criar evidence run de audio fisico

- Prioridade: P1.
- Tipo: Quality Gate.
- Status: Blocked by hardware/runtime evidence.
- Contexto: nao declarar audio fisico sem ESP32/MAX98357A.
- Objetivo: executar upload, serial, MQTT e testes de tom/stop/volume.
- Escopo: evidence run manual.
- Fora do escopo: backend real se indisponivel.
- Requisitos funcionais: registrar boot, I2S init, comando recebido e status.
- Requisitos tecnicos: scripts dev e mosquitto quando disponivel.
- Nao funcionais: evidenciar brownout/reboot.
- Aceite Dado/Quando/Entao: Dado hardware conectado, Quando plano rodar, Entao outputs ficam anexados.
- Cenarios negativos: COM/broker indisponivel vira bloqueio explicito.
- Seguranca: sem secrets em logs.
- Testes: manual-audio-test-plan.
- Evidencias esperadas: serial-output, mqtt-command-output, mqtt-status-output.
- Definition of Ready: hardware e broker disponiveis.
- Definition of Done: evidencia fisica real.

#### STORY-AUDIO-010 - Criar contrato para backend/IA gerar e publicar comandos de audio

- Prioridade: P1.
- Tipo: Cross-repo Contract Story.
- Status: Implemented as prompt/documentation.
- Contexto: backend deve gerar TTS e publicar comandos.
- Objetivo: orientar implementacao no repo backend.
- Escopo: prompt e contrato.
- Fora do escopo: alterar backend neste repo.
- Requisitos funcionais: endpoint local, MQTT command, status ingest.
- Requisitos tecnicos: nao expor broker, auditar commandId.
- Nao funcionais: autorizacao futura e quiet-hours.
- Aceite Dado/Quando/Entao: Dado agente backend, Quando ler prompt, Entao implementa contrato sem firmware changes.
- Cenarios negativos: URL publica deve ser rejeitada.
- Seguranca: sem credenciais em payload.
- Testes: contrato backend futuro.
- Evidencias esperadas: prompt versionado.
- Definition of Ready: firmware contract publicado.
- Definition of Done: prompt criado.
