# Firmware Quality Gates

## Build gate

Comando: `scripts/quality/check-firmware-build.ps1`.

Objetivo: compilar o firmware com PlatformIO e salvar saida em evidence. Falha de compilacao e bloqueante.

## Static analysis gate

Nao ha static analyzer dedicado configurado. Qualquer ferramenta futura deve ser documentada aqui antes de virar bloqueante.

## Formatting gate

Nao ha formatter bloqueante configurado. Manter estilo local de C++ e evitar refactors cosmeticos amplos.

## Firmware contract gate

Comando: `scripts/quality/check-firmware-contract.ps1`.

Valida `deviceId`, `room`, topico MQTT, campos do payload, pinagem DHT/BH1750 e omissao de campos invalidos.

## Documentation consistency gate

Comando: `scripts/quality/check-firmware-docs-consistency.ps1`.

Valida existencia dos docs canonicos e mencoes obrigatorias a POC atual, fora do escopo, seguranca local-first e contrato MQTT.

## Secrets scan gate

Comando: `scripts/quality/check-secrets.ps1`.

Procura `include/Secrets.h`, placeholders substituidos indevidamente, IP publico, tokens e chaves privadas.

## Hardware pin documentation gate

Coberto pelo contract gate e docs consistency gate.

## MQTT payload contract gate

Coberto pelo contract gate e testes nativos.

## Unity harness gate

Comando: `scripts/quality/check-unreachable-unity-tests.ps1`.

Falha se algum `RUN_TEST` estiver depois de `return UNITY_END();`, se houver mais de um `return UNITY_END();` no mesmo runner, se houver `return` antes do ultimo `RUN_TEST`, se um teste `should...` detectavel nao estiver registrado, ou se a contagem do output nativo nao bater com os `RUN_TEST` registrados quando um log de teste for informado.

## Test source sanity gate

Comando: `scripts/quality/check-test-source-sanity.ps1`.

Segue a skill importada `Test Harness Quality & Anti-False-Success`. Falha se o arquivo de teste tiver chaves aparentemente desbalanceadas, funcao de teste sobreposta, funcao duplicada, assertion solta fora de funcao, `RUN_TEST` depois de `return UNITY_END();` ou mais de um `return UNITY_END();`. A validacao e propositalmente conservadora e complementa compilacao e testes nativos.

## Evidence completeness gate

O approval review verifica se a pasta de evidence existe e se contem arquivos minimos.

## Audio gates

- `scripts/quality/check-audio-contract.ps1`: valida topicos `audio/command` e `audio/status`, tipos `play_audio_url`, `play_audio_stream`, `play_tone`, `stop_audio`, `set_volume`, estados `accepted`, `playing`, `completed`, `rejected`, `failed`, `commandId`, volume, URL local e max duration.
- `scripts/quality/check-audio-docs-consistency.ps1`: valida SDD, arquitetura, wiring, seguranca, ADR-0004, backlog e memoria da squad para audio.
- `scripts/quality/check-audio-security.ps1`: valida feature flag, bloqueio de URL publica, `stop_audio`, volume limit, max duration, topico especifico e ausencia de microfone/wake word/IA embarcada.
- `scripts/quality/check-audio-status-tests.ps1`: valida que status `accepted`, `playing`, `completed`, `stopped`, `rejected` e `failed` tenham testes separados, mocks limpos, acao explicita, topico/payload/reason coerentes, ausencia de teste agrupado obsoleto e executa `platformio test -e native`.
- `scripts/quality/check-audio-build-compatibility.ps1`: executa testes nativos e build `esp32dev`.
# Mock and Physical Validation Gates

Automatic approval review includes:

- `check-test-source-sanity.ps1`
- `check-audio-status-tests.ps1`
- `check-unreachable-unity-tests.ps1`
- `check-native-tests.ps1`
- `check-firmware-build.ps1`
- `check-firmware-contract.ps1`
- `check-audio-contract.ps1`
- `check-audio-security.ps1`
- `check-audio-docs-consistency.ps1`
- `check-firmware-docs-consistency.ps1`
- `check-secrets.ps1`
- `check-mock-mqtt-contract.ps1`
- `check-hardware-abstractions.ps1`
- `check-physical-validation-policy.ps1`

Automatic gates do not require ESP32 hardware. Physical and release modes are separate governance paths. A Unity harness failure is a hard approval-review failure, not a warning.
