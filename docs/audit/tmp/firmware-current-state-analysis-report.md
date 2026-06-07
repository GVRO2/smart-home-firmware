# Firmware Current State Analysis Report

## 1. Status

FIRMWARE_REPO_ANALYZED_WITH_WARNINGS

O repositorio foi analisado como firmware PlatformIO/Arduino para ESP32. Ha implementacao funcional para DHT22/AM2302, BH1750/GY-302, Wi-Fi, NTP, MQTT e testes nativos, mas ainda faltava uma camada canonica de SDD, governanca, evidencia, ADRs e approval review.

## 2. Stack detectada

- Linguagem: C/C++.
- Framework: Arduino sobre ESP32.
- Build system: PlatformIO.
- Ambientes: `esp32dev` e `native`.
- Bibliotecas firmware: `PubSubClient`, `ArduinoJson`, `DHT sensor library`, `Adafruit Unified Sensor`, `BH1750`.
- Testes: Unity no ambiente `native`, com stubs para MQTT.

## 3. Estrutura encontrada

- `src/main.cpp`: bootstrap, loop, sensores, Wi-Fi, NTP, MQTT e logs.
- `include/AppConfig.h`: constantes versionadas de device, room, pinos, intervalo e BH1750.
- `include/Secrets.h.example`: template de credenciais locais.
- `include/MqttMessageBuilder.h`: topico e payload JSON.
- `src/MqttPublisher.cpp`: conexao MQTT, publicacao e skip reasons.
- `src/DhtEnvironmentSensor.cpp`: leitura DHT22/AM2302 e validacao de faixa.
- `src/Bh1750LightSensor.cpp`: inicializacao I2C e leitura lux.
- `test/test_native/`: testes de contrato MQTT, leitura e publicacao.
- `docs/`: docs iniciais de wiring, MQTT, troubleshooting e testes.
- `analysis-output/`: relatorios historicos de execucoes anteriores.

## 4. Funcionalidades existentes

- Leitura de temperatura e umidade via DHT22/AM2302 no GPIO 4.
- Leitura de luminosidade via BH1750/GY-302 em I2C SDA GPIO 21, SCL GPIO 22, endereco `0x23`.
- Conexao Wi-Fi local com retry temporizado.
- Sincronizacao NTP antes de publicar `measuredAt`.
- Publicacao MQTT local observacional.
- Omissao de campos invalidos em vez de inventar valores.
- Skip de publicacao quando nenhuma leitura sensorial e valida.
- Logs seriais para boot, configuracao, Wi-Fi, tempo, DHT22, BH1750 e MQTT.
- Testes nativos para contrato e builder.

## 5. Contratos existentes

- Device ID: `esp32-bedroom-01`.
- Room: `bedroom`.
- Topico MQTT: `home/bedroom/esp32-bedroom-01/environment`.
- Payload: `deviceId`, `room`, `measuredAt`, e campos sensoriais validos entre `temperatureCelsius`, `humidityPercentage`, `luminosityLux`.
- Frequencia atual: `PUBLISH_INTERVAL_MS = 5000`.
- Pinagem: DHT22 DATA GPIO 4; BH1750 SDA GPIO 21, SCL GPIO 22, ADDR GND para `0x23`.
- Tempo: NTP UTC; sem horario confiavel, nao publica.

## 6. Lacunas

- SDD principal do firmware ausente.
- Arquitetura, data flow, seguranca e hardware wiring ainda nao estavam canonizados em `docs/architecture/`.
- Governanca de agentes, quality gates e release readiness ausentes.
- Politica formal de evidencia ausente.
- ADRs do firmware ausentes.
- Backlog inicial do firmware ausente.
- Prompts persistidos ausentes.
- Approval review automatizado ausente.
- Evidence run padronizada para bootstrap ausente antes desta entrega.

## 7. Riscos

- `DHT_PIN = 4` e um strapping pin do ESP32; o firmware ja registra warning no boot.
- Credenciais reais dependem de `include/Secrets.h`; o arquivo esta no `.gitignore`, mas o gate deve vigiar vazamentos.
- Publicacao depende de NTP; queda prolongada de tempo impede payload mesmo com sensor valido.
- Upload/monitor dependem de `COM5`, que historicamente pode estar bloqueada ou em boot mode errado.
- MQTT sem TLS/autenticacao e aceitavel apenas em rede local controlada para POC.
- Nenhum comando remoto ou rele deve ser introduzido sem decisao formal.

## 8. Inconsistencias

- Nao ha stack C# neste repositorio. A stack real detectada e C/C++ com Arduino/PlatformIO.
- O contrato recomendado pelo prompt coincide com o contrato do codigo para topico, device, room e campos. A diferenca operacional e que o firmware omite campos sensoriais invalidos e nao publica sem NTP valido.

## 9. Recomendacoes

1. Criar SDD e ADRs como fontes canonicas.
2. Criar gates PowerShell simples para build, contrato, docs e secrets.
3. Usar approval review como agregador local, sem declarar validacao fisica.
4. Manter firmware funcional intacto nesta fase.
5. Registrar evidencias com comandos, exit codes e riscos.
6. Validar qualquer mudanca futura de payload contra backend antes de merge.

## 10. Proximos passos

- Executar `scripts/governance/run-firmware-approval-review.ps1`.
- Quando houver hardware conectado, executar upload e monitor serial com autorizacao humana.
- Alinhar consumo backend e evidenciar ingestao ate PostgreSQL.
- Avaliar autenticacao MQTT antes de sair de laboratorio.
