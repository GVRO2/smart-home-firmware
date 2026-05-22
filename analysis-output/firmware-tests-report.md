# Firmware Tests Report

## 1. Arquivos criados
- [include/IMqttClient.h](include/IMqttClient.h)
- [include/MqttMessageBuilder.h](include/MqttMessageBuilder.h)
- [include/PubSubMqttClientAdapter.h](include/PubSubMqttClientAdapter.h)
- [include/TimeFormatting.h](include/TimeFormatting.h)
- [scripts/check_secrets.py](scripts/check_secrets.py)
- [scripts/native_toolchain.py](scripts/native_toolchain.py)
- [docs/firmware-testing.md](docs/firmware-testing.md)
- [test/test_native/test_environment_reading.cpp](test/test_native/test_environment_reading.cpp)
- [test/test_native/test_mqtt_contract.cpp](test/test_native/test_mqtt_contract.cpp)
- [test/test_native/MqttPublisher.native.cpp](test/test_native/MqttPublisher.native.cpp)
- [test/test_embedded/test_dht_sensor.cpp](test/test_embedded/test_dht_sensor.cpp)
- [test/test_embedded/test_wifi_mqtt_smoke.cpp](test/test_embedded/test_wifi_mqtt_smoke.cpp)

## 2. Arquivos alterados
- [platformio.ini](platformio.ini)
- [include/EnvironmentReading.h](include/EnvironmentReading.h)
- [include/DhtEnvironmentSensor.h](include/DhtEnvironmentSensor.h)
- [include/MqttPublisher.h](include/MqttPublisher.h)
- [include/Secrets.h.example](include/Secrets.h.example)
- [include/TimeProvider.h](include/TimeProvider.h)
- [src/DhtEnvironmentSensor.cpp](src/DhtEnvironmentSensor.cpp)
- [src/MqttPublisher.cpp](src/MqttPublisher.cpp)
- [src/PubSubMqttClientAdapter.cpp](src/PubSubMqttClientAdapter.cpp)
- [src/TimeProvider.cpp](src/TimeProvider.cpp)
- [src/main.cpp](src/main.cpp)
- [/memories/repo/home-ai-room-observer-esp32.md](/memories/repo/home-ai-room-observer-esp32.md)

## 3. Testes criados
| Teste | O que valida | Tipo |
|---|---|---|
| shouldAcceptReadingWhenTemperatureAndHumidityAreValid | Temperatura e umidade válidas são aceitas | Unitário nativo |
| shouldAcceptReadingWhenOnlyTemperatureIsValid | Umidade NaN não invalida a leitura quando temperatura é válida | Unitário nativo |
| shouldAcceptReadingWhenOnlyHumidityIsValid | Temperatura NaN não invalida a leitura quando umidade é válida | Unitário nativo |
| shouldRejectReadingWhenAllSensorValuesAreInvalid | Leitura sem valores válidos é rejeitada | Unitário nativo |
| shouldRejectReadingWhenTemperatureIsBelowMinimum | Limite mínimo de temperatura | Unitário nativo |
| shouldRejectReadingWhenTemperatureIsAboveMaximum | Limite máximo de temperatura | Unitário nativo |
| shouldRejectReadingWhenHumidityIsBelowMinimum | Limite mínimo de umidade | Unitário nativo |
| shouldRejectReadingWhenHumidityIsAboveMaximum | Limite máximo de umidade | Unitário nativo |
| shouldBuildOfficialMqttTopicWhenRoomAndDeviceAreProvided | Tópico oficial home/{roomSlug}/{deviceExternalId}/environment | Unitário nativo |
| shouldNotUseDeprecatedMqttTopicFormat | Ausência do tópico depreciado | Unitário nativo |
| shouldBuildPayloadWhenTemperatureAndHumidityAreValid | JSON com campos oficiais e parseável | Unitário nativo |
| shouldOmitTemperatureWhenTemperatureIsInvalid | Omissão de temperatura inválida | Unitário nativo |
| shouldOmitHumidityWhenHumidityIsInvalid | Omissão de umidade inválida | Unitário nativo |
| shouldNotBuildPayloadWhenNoSensorValueIsValid | Sem leitura válida, não gera payload | Unitário nativo |
| shouldRejectPublishWhenMeasuredAtIsEmpty | Timestamp vazio bloqueia publicação | Unitário nativo |
| shouldIncludeMeasuredAtWhenTimestampIsAvailable | Timestamp ISO-8601 UTC com Z | Unitário nativo |
| shouldPublishWhenReadingAndTimestampAreValid | Caminho feliz do publish sem rede real | Unitário nativo |
| shouldNotPublishWhenReadingIsInvalid | Leitura inválida não chama publish | Unitário nativo |
| shouldNotPublishWhenTimestampIsEmpty | Timestamp vazio não chama publish | Unitário nativo |
| shouldPublishToOfficialEnvironmentTopic | Publicação usa o tópico oficial | Unitário nativo |
| shouldCompileEmbeddedSmokeTestForDhtSensor | Smoke test embarcado mínimo para DHT | Embarcado |
| shouldCompileEmbeddedSmokeTestForWifiAndMqtt | Smoke test embarcado mínimo para Wi-Fi/MQTT | Embarcado |

## 4. Comandos executados
| Comando | Resultado | Observação |
|---|---|---|
| `& 'C:\Users\KABUM\.platformio\penv\Scripts\platformio.exe' run` | Sucesso | Build padrão restrito a `esp32dev` via `default_envs` |
| `py -3 scripts/check_secrets.py` | Sucesso | Template de segredos validado como placeholder |
| `& 'C:\Users\KABUM\.platformio\penv\Scripts\platformio.exe' test -e native` | Sucesso | Suíte nativa passou com 20 testes |
| `& 'C:\Users\KABUM\.platformio\penv\Scripts\platformio.exe' run --target upload` | Falhou | Auto-detectou `COM1`, mas não havia resposta serial do ESP32 |
| `& 'C:\Users\KABUM\.platformio\penv\Scripts\platformio.exe' test -e esp32dev` | Não executado | Não havia ESP32 físico conectado para validar HIL |

## 5. Cobertura comportamental
A cobertura nativa valida o contrato de leitura ambiental, a regra de validade de temperatura e umidade, a montagem do tópico MQTT, a montagem do JSON, a omissão de campos inválidos, o bloqueio por timestamp vazio e o caminho feliz de publicação sem rede real usando fake client.

O contrato de segredos também foi validado: `include/Secrets.h` permanece ignorado, `Secrets.h.example` usa placeholders e `src/main.cpp` não contém credenciais hardcoded.

## 6. Limitações
O ambiente Windows desta workspace não possui um `g++` do sistema, então o alvo native precisou do pacote `platformio/toolchain-gccmingw32` e do script `scripts/native_toolchain.py` para funcionar. O teste embarcado real e o upload dependem de um ESP32 físico e de uma porta serial válida; neste host o upload auto-detectou `COM1`, mas não havia dispositivo respondendo.

## 7. Próximos passos
1. Executar `pio test -e esp32dev` em uma máquina com o ESP32 conectado para smoke tests de hardware.
2. Ajustar `upload_port` e `monitor_port` se o ESP32 aparecer em outra porta serial.
3. Expandir os testes embarcados com uma verificação real de leitura do DHT e uma smoke test de MQTT, se houver infraestrutura disponível.
