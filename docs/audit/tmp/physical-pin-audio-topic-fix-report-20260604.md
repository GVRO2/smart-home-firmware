# Relatório de Correção P0: Colisão de Pinos, Alinhamento MQTT e Evidências

**Data de Execução:** 2026-06-04
**Veredito Final:** `PHYSICAL_TEST_READY_WITH_WARNINGS`

---

## 1. Sumário executivo

Este relatório apresenta os resultados das ações de correção P0 governadas pela Squad Autônoma para o projeto **Home AI Room Observer**. O objetivo principal foi remover os bloqueadores de pinagem física e de desalinhamento de tópicos MQTT de áudio que impediam a realização segura do primeiro teste com hardware real. Todas as correções foram implementadas, testadas por suítes nativas e do backend, e validadas através de novos gates de qualidade.

---

## 2. Escopo executado

- **Correção da pinagem do firmware ESP32:** Atualização do pino I2S DIN de GPIO 27 para GPIO 33, liberando o GPIO 27 para o sensor de presença.
- **Reserva de pinos de sensores:** Declaração explícita de `PRESENCE_SENSOR_PIN = 27` e `SOUND_SENSOR_PIN = 34` no firmware.
- **Gate de colisão de pinos:** Implementação de script de validação de qualidade `check-pin-collision.ps1` e teste unitário nativo de regressão `shouldBeCollisionFreeAndSafe`.
- **Alinhamento de Tópicos MQTT:** Padronização dos tópicos `home/{room}/{device}/audio/command` e `home/{room}/{device}/audio/status` no firmware e backend (Spring Boot), com depreciação total do tópico legado `/speaker`.
- **Validação de testes:** Execução de todas as suítes de testes automáticos (58 testes nativos de firmware, 193 testes unitários e 45 testes de integração de backend).
- **Geração de evidências:** Criação de logs de execução e validações no diretório de runs oficial do projeto.

---

## 3. Fora do escopo

- Teste físico com ESP32 real conectado (não autorizado nesta rodada).
- Implementação física real de drivers de presença e som (drivers continuam como gaps/warnings controlados).
- Implementação de decodificadores HTTP WAV/MP3 reais (links de áudio URL/Stream continuam como stubs de sinalização).
- Exposição pública de brokers MQTT ou bancos PostgreSQL.

---

## 4. Arquivos alterados

### Repositório Firmware (`home-ai-room-observer-esp32`)
- `include/AppConfig.h`
- `scripts/quality/check-firmware-contract.ps1`
- `scripts/quality/check-audio-docs-consistency.ps1`
- `docs/architecture/firmware-hardware-wiring.md`
- `docs/sdd/firmware-sdd.md`
- `docs/squad/firmware-agent-memory.md`
- `test/test_native/test_mqtt_contract.cpp`
- `test/test_native/test_pin_collision.cpp` (Novo arquivo)
- `scripts/quality/check-pin-collision.ps1` (Novo arquivo)
- `scripts/governance/run-firmware-approval-review.ps1`

### Repositório Backend (`smart-home`)
- `src/main/resources/application.yml`
- `src/main/java/br/com/homeai/roomobserver/interfaces/rest/config/HomeBrainProperties.java`
- `src/main/java/br/com/homeai/roomobserver/application/usecase/HomeBrainSettings.java`
- `src/test/java/br/com/homeai/roomobserver/interfaces/rest/config/HomeBrainPropertiesTest.java`
- `src/test/java/br/com/homeai/roomobserver/application/usecase/AudioPublisherTest.java`

---

## 5. Correção de pin mapping

O mapeamento de pinos do microcontrolador ESP32 DevKit V1 foi corrigido no arquivo `AppConfig.h` e sincronizado com os esquemas de fiação físicos recomendados pelo usuário:
- **MAX98357A DIN/SD:** Redirecionado para o **GPIO 33** (anteriormente alocado no GPIO 27).
- **Presença OUT:** Alocado com segurança no **GPIO 27** (reservado no firmware, resolvendo o conflito com a linha I2S).
- **Som Analógico AO:** Reservado no **GPIO 34** (pino exclusivo de entrada).

---

## 6. Confirmação de ausência de colisões

- **AUDIO_I2S_DIN_PIN (GPIO 33) vs PRESENCE_SENSOR_PIN (GPIO 27):** Diferentes (resolvido).
- **AUDIO_I2S_BCLK_PIN (GPIO 26) / LRC_PIN (GPIO 25) vs Sensores (GPIO 4, 21, 22, 27, 34):** Sem sobreposições.
- **Saídas físicas vs GPIO 34:** Nenhum sinal de saída (LED, I2S) utiliza o GPIO 34 (que é exclusivo de entrada, prevenindo curtos-circuitos).
- **Consistência Documental:** Mapeamentos no `firmware-hardware-wiring.md`, `firmware-sdd.md` and `firmware-agent-memory.md` foram totalmente alinhados e validados pelo script automatizado.
- **Saída da Validação:** `PIN_MAPPING_VALIDATED`.

---

## 7. Correção/alinhamento de tópicos MQTT

- **Tópico de Comando de Áudio:** Configurado como `home/{room}/{device}/audio/command` no firmware e backend (Spring Boot).
- **Tópico de Status de Áudio:** Configurado como `home/{room}/{device}/audio/status` no firmware e backend.
- **Depreciação:** O tópico antigo `home/{room}/{device}/speaker` foi completamente removido do arquivo de propriedades `application.yml` e das classes/testes java de configuração do backend.

---

## 8. Classificação real das capacidades de áudio

As capacidades do subsistema de áudio foram auditadas e classificadas conforme a realidade técnica da entrega:

| Capacidade | Classificação | Rationale |
|---|---|---|
| **PLAY_TONE** | `IMPLEMENTED_NOT_PHYSICALLY_VALIDATED` | Gerador de onda senoidal I2S DMA codificado e unitariamente testado, mas sem execução em hardware real neste run. |
| **PLAY_NOTIFICATION** | `NOT_FOUND` | Sem armazenamento de arquivos locais ou player integrado. |
| **PLAY_URL** | `STUB_ONLY` | Parser e validação local/privada codificados, mas decodificação HTTP real é stub que emite status `failed`. |
| **STREAM** | `STUB_ONLY` | Validação de link de streaming presente, mas buffering e decodificação reais de stream são stubs que emitem status `failed`. |
| **TTS** | `NOT_FOUND` | Text-to-Speech não faz parte do escopo da POC atual. |
| **STOP_AUDIO** | `IMPLEMENTED_NOT_PHYSICALLY_VALIDATED` | Fila e comando de interrupção codificados e testados. |
| **SET_VOLUME** | `IMPLEMENTED_NOT_PHYSICALLY_VALIDATED` | Lógica de volume (0-100) implementada e validada. |
| **AUDIO_STATUS** | `IMPLEMENTED_NOT_PHYSICALLY_VALIDATED` | Publicação MQTT de status (`accepted`, `playing`, `completed`, etc.) codificada e testada. |

---

## 9. Revisão SOLID/Clean Code

A implementação seguiu as boas práticas de engenharia:
- **SRP:** Lógicas de colisão de pinos, mapeamento e documentação separadas de responsabilidades de runtime.
- **OCP:** A validação de colisão é centralizada no gate script e nos testes unitários, permitindo adicionar pinos sem alterar o motor de execução.
- **LSP:** Interface do driver I2S mantida, mocks funcionam sem alterações de comportamento para os testes nativos.
- **DIP:** Componentes do firmware dependem de interfaces abstratas, dissociadas de pinos físicos em runtime.
- **Clean Code:** Uso de constantes explícitas (`DHT_PIN`, `AUDIO_I2S_DIN_PIN`), sem "magic numbers" nos drivers, documentação sincronizada e livre de inflações de status (stubs de áudio devidamente identificados).

---

## 10. Revisão da Squad Autônoma

- **Firmware Engineer:** PASSED (Mapeamento corrigido, barramento I2S ajustado para GPIO 33, GPIO 27 liberado).
- **Backend Engineer:** PASSED (Tópicos MQTT sincronizados nas configurações e testes java, depreciando `/speaker`).
- **QA Review:** PASSED (Testes de regressão adicionados e executados com sucesso em ambas as pontas).
- **Architecture Review:** PASSED (SOLID e Clean Code verificados e aprovados).
- **Governance Review:** PASSED (Relatório, runs de evidências gerados, veredito emitido conforme regras de conformidade).

---

## 11. Testes executados

- **Firmware Native Tests (PlatformIO):** `pio test -e native` executado. 58/58 testes passados (incluindo o novo teste `shouldBeCollisionFreeAndSafe`).
- **Quality Gates (Firmware):** `run-firmware-approval-review.ps1` executado com sucesso (Status: `FIRMWARE_APPROVAL_REVIEW_PASSED_WITH_WARNINGS` devido à presença do arquivo local `Secrets.h` ignorado).
- **Backend Build & Verification (Maven):** `.\mvnw.cmd clean verify` executado no backend. BUILD SUCCESS (193 testes unitários e 45 de integração passados com sucesso).

---

## 12. Evidências geradas

As evidências foram persistidas na pasta `docs/evidence/runs/20260604-physical-pin-audio-topic-fix/`:
- `command-execution-log.txt` (Comandos e resultados reproduzíveis)
- `native-tests-run.txt` (Resultado da execução dos testes de firmware)
- `approval-review-gates.txt` (Resultados individuais de cada gate de governança)
- `pin-mapping-validation.txt` (Lista de pinos e validações elétricas/de colisão)
- `mqtt-topic-validation.txt` (Verificação de alinhamento e depreciações de tópicos)
- `audio-readiness-classification.txt` (Classificação realista e sem inflações de áudio)
- `solid-clean-code-review.txt` (Avaliação estática contra princípios de arquitetura limpa)
- `autonomous-squad-review.txt` (Parecer formal dos papéis da Squad Autônoma)
- `changed-files.txt` (Lista completa de arquivos novos e modificados)

---

## 13. Gaps remanescentes

- **P1:** Ausência de drivers de sensores reais no firmware para leitura do sensor de presença (GPIO 27) e som (GPIO 34).
- **P1:** Decodificadores reais de áudio URL/Stream ainda são stubs no firmware (I2S DMA executa apenas `play_tone` local).
- **P1:** O backend possui a propriedade de tópico MQTT de áudio configurada, mas não possui a lógica java ativa (publisher outbound real) de envio de comandos para o broker Mosquitto.

---

## 14. Riscos para o primeiro teste físico

- **Reset de Brownout:** Alto risco de brownout se o amplificador I2S (MAX98357A) for energizado pelo regulador de 3.3V ou pela porta USB de um PC com volume alto. Deve-se limitar o volume e usar fonte externa ou capacitor apropriado.
- **Instabilidade no Wi-Fi/DMA:** A leitura síncrona do DHT22 ou perda de sinal Wi-Fi pode congelar temporariamente o processamento e provocar stutters/interrupções na execução DMA de tons I2S.
- **Erros de Montagem Manual:** Trocas acidentais na fiação de 3.3V/5V na protoboard podem danificar fisicamente o regulador interno da placa ESP32.

---

## 15. Plano recomendado de teste físico incremental

1. Subir ESP32 sem áudio ativo (`AUDIO_PLAYBACK_ENABLED = false` em `AppConfig.h`).
2. Validar serial boot (confirmando que a placa inicializa sem boot loops).
3. Validar DHT22 (acompanhar logs ambientais via monitor serial).
4. Validar BH1750 (confirmar leituras de luminosidade em lux correspondentes à luz da sala).
5. Validar MQTT publish de ambiente (broker Mosquitto local recebe as leituras).
6. Validar backend recebendo (verificar logs do Spring Boot processando a ingestão).
7. Validar PostgreSQL persistindo (conferir dados gravados na tabela `environmental_measurements`).
8. Energizar MAX98357A sem tocar áudio (confirmar ausência de estalos ou ruídos no alto-falante).
9. Tocar `PLAY_TONE` local em volume baixo (usando valor seguro como 15 ou 20).
10. Enviar `PLAY_TONE` via comando MQTT para o tópico `audio/command`.
11. Validar publicação de `audio/status` pelo firmware para o broker.
12. Só depois avaliar presença e som, pois os drivers reais são gaps a serem implementados.

---

## 16. Veredito final

```text
PHYSICAL_TEST_READY_WITH_WARNINGS
```

### Justificativa:
As colisões graves de hardware de GPIO foram completamente sanadas no firmware, e os tópicos MQTT de comando/status estão alinhados entre os sistemas, passando por todas as validações unitárias e de contrato de qualidade automáticas. Contudo, o início dos testes físicos deve prosseguir com **warnings**, dado que os drivers de presença/som ainda são gaps (não implementados no firmware) e os comandos de áudio por URL/Stream ainda consistem em stubs de verificação estrutural.
