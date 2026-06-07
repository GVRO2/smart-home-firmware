# Firmware Architecture

## Modulos

- `src/main.cpp`: orquestra boot, loop, heartbeat, sensores, tempo e MQTT.
- `WiFiConnection`: conectividade Wi-Fi local com timeout e retry.
- `TimeProvider`: sincronizacao NTP e geracao de timestamp UTC ISO-8601.
- `DhtEnvironmentSensor`: leitura e validacao basica do DHT22/AM2302.
- `Bh1750LightSensor`: inicializacao I2C e leitura do BH1750/GY-302.
- `EnvironmentReading`: estrutura de leitura e validadores de faixa.
- `MqttMessageBuilder`: topico e payload JSON.
- `MqttPublisher`: conexao MQTT, publicacao e logs de skip/failure.
- `AppConfig`: constantes versionadas.
- `Secrets`: credenciais locais nao versionadas.

## Fluxo principal

1. `setup()` inicia Serial, LED, sensores e logs de configuracao.
2. `loop()` processa heartbeat.
3. `WiFiConnection.ensureConnected()` tenta manter Wi-Fi.
4. `TimeProvider.ensureSynced()` tenta manter horario UTC valido.
5. `MqttPublisher.ensureConnected()` tenta manter MQTT.
6. A cada `PUBLISH_INTERVAL_MS`, sensores sao lidos.
7. `MqttPublisher.publishEnvironment()` valida leitura, tempo, monta payload e publica.

## Inicializacao

O firmware nao deve depender do Serial Monitor para operar. Falhas de sensor ou rede sao registradas e o loop continua.

## Leitura de sensores

O DHT22 preenche temperatura/umidade. O BH1750 complementa luminosidade no mesmo `EnvironmentReading`. Cada campo tem flag `has*` para evitar valor fabricado.

## Montagem de payload

`buildEnvironmentPayloadJson()` inclui identidade, sala, timestamp e apenas campos sensoriais validos.

## Publicacao MQTT

`buildEnvironmentTopic()` monta `home/{room}/{deviceId}/environment`. A publicacao e cancelada quando MQTT esta desconectado, tempo esta indisponivel ou nao ha leitura valida.

## Logs

Logs devem permitir diagnostico sem revelar credenciais. Skip reasons sao parte do contrato operacional: `mqtt_disconnected`, `no_valid_sensor_reading`, `time_unavailable` e `payload_empty`.

## Separacao desejada

Novas responsabilidades devem entrar como classes pequenas em `include/` e `src/`, com testes nativos quando nao dependerem de hardware. Evitar inserir logica de sensores, payload e rede diretamente em `loop()`.

## Audio local

Novos modulos:

- `AudioCommandParser`: parse e validacao de comandos MQTT de audio.
- `AudioPlaybackQueue`: fila bounded com prioridade e suporte a interrupcao.
- `AudioPlaybackService`: orquestracao de comando, estado e status.
- `AudioStatusPublisher`: publicacao de `audio/status`.
- `I2sAudioOutputDriver`: saida I2S para MAX98357A e tom de teste.

O fluxo de audio roda ao lado do fluxo ambiental. `MqttPublisher.loop()` processa callbacks de comando; `AudioPlaybackService.loop()` avanca playback sem impedir a publicacao ambiental. O firmware assina apenas `home/bedroom/esp32-bedroom-01/audio/command` e publica status em `home/bedroom/esp32-bedroom-01/audio/status`.

`play_tone` usa I2S/MAX98357A para validacao minima de hardware. `stop_audio` interrompe o driver e limpa a fila. `set_volume` atualiza o volume. `play_audio_url` aceita WAV/PCM 16-bit mono por HTTP local, valida o header e envia PCM ao I2S em chunks. `play_audio_stream` continua validado no contrato, mas sem streaming continuo implementado.
