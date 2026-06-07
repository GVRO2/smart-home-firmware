# Firmware SDD - Home AI Room Observer

## 1. Objetivo

Definir o desenho tecnico do firmware ESP32 da POC Home AI Room Observer. O firmware coleta dados ambientais locais do quarto e publica medicoes via MQTT para o backend da rede residencial.

## 2. Escopo da POC atual

- ESP32 DevKit V1.
- DHT22/AM2302 para temperatura e umidade.
- BH1750/GY-302 para luminosidade.
- Wi-Fi local.
- Broker MQTT local.
- Payload ambiental observacional.
- Logs seriais para diagnostico.
- Audio local limitado via MAX98357A (reprodução de tons/alarme local).

## 3. Fora do escopo

- Rele, controle 127V/220V ou automacao eletrica.
- Comandos remotos para o ESP32.
- Broker MQTT exposto publicamente.
- PostgreSQL exposto publicamente.
- Microfone, wake word, TTS, streaming real, decodificacao real de PLAY_URL e IA embarcada (audio local limitado via MAX98357A esta dentro do escopo).
- OTA sem threat model e decisao formal.

## 4. Visao geral do firmware

O firmware inicializa sensores, tenta Wi-Fi sem travar o loop indefinidamente, sincroniza horario UTC por NTP, conecta ao MQTT local e publica payload JSON quando ha horario confiavel e pelo menos uma leitura sensorial valida.

## 5. Hardware alvo

ESP32 DevKit V1 operado em laboratorio/rede residencial local.

## 6. Sensores suportados

- DHT22/AM2302: temperatura e umidade.
- BH1750/GY-302: luminosidade em lux.

Sensores de presenca e som sao reservados no mapeamento de pinos (GPIO 27 e GPIO 34), mas seus drivers ainda nao foram implementados na POC atual.
O audio local e suportado conforme ADR-0004.

## 7. Pinagem oficial

- DHT22/AM2302 DATA: GPIO 4.
- BH1750 SDA: GPIO 21.
- BH1750 SCL: GPIO 22.
- BH1750 ADDR: GND para endereco `0x23`.
- Heartbeat LED: GPIO 2.
- Presence OUT: GPIO 27 (Reservado para teste fisico).
- Sound AO: GPIO 34 (Reservado para teste fisico).
- MAX98357A BCLK: GPIO 26.
- MAX98357A LRC/WS: GPIO 25.
- MAX98357A DIN: GPIO 33.
- MAX98357A SD: nao conectar na POC atual (SD e shutdown/enable, nao data input).

GPIO 4 e strapping pin; alterar essa pinagem exige ADR ou atualizacao explicita de docs e testes.

## 8. Comunicacao

O ESP32 usa Wi-Fi local em modo station e publica por MQTT no broker local configurado em `include/Secrets.h`.

## 9. MQTT

Topico oficial:

```text
home/bedroom/esp32-bedroom-01/environment
```

Wildcard esperado do backend quando aplicavel:

```text
home/+/+/environment
```

## 10. Payload publicado

Payload completo esperado:

```json
{
  "deviceId": "esp32-bedroom-01",
  "room": "bedroom",
  "temperatureCelsius": 27.4,
  "humidityPercentage": 62.5,
  "luminosityLux": 18.0,
  "measuredAt": "2026-05-21T20:30:00Z"
}
```

Campos de identidade e tempo sao obrigatorios quando ha publicacao. Campos sensoriais invalidos sao omitidos; valores falsos nao devem ser publicados silenciosamente.

## 11. Frequencia de coleta e publicacao

O intervalo atual e `PUBLISH_INTERVAL_MS = 5000`. Para uso real prolongado, a propria configuracao recomenda 30000 ms para reduzir trafego e leituras desnecessarias.

## 12. Estrategia de leitura dos sensores

O DHT22 e lido por `DhtEnvironmentSensor`. O BH1750 e lido por `Bh1750LightSensor` depois de `Wire.begin(21, 22)`. As leituras sao agregadas em `EnvironmentReading`.

## 13. Tratamento de falhas de sensor

- Temperatura valida: entre -40 C e 80 C.
- Umidade valida: entre 0% e 100%.
- Luminosidade valida: numero nao NaN e maior ou igual a 0.
- Campo invalido e omitido.
- Se nenhum campo sensorial e valido, o MQTT publish e cancelado.
- Falha de sensor nao deve travar o loop principal.

## 14. Reconexao Wi-Fi

`WiFiConnection` tenta reconectar com intervalo de retry e timeout curto. O loop continua vivo mesmo quando o Wi-Fi falha.

## 15. Reconexao MQTT

`MqttPublisher` tenta reconectar com intervalo de retry. Se MQTT estiver desconectado, a publicacao e pulada com log `reason=mqtt_disconnected`.

## 16. Logs seriais

Logs esperados incluem `[BOOT]`, `[CONFIG]`, `[WIFI]`, `[TIME]`, `[DHT22]`, `[BH1750]`, `[ENV]` e `[MQTT]`. Logs devem registrar fatos operacionais e skip reasons, nunca secrets.

## 17. Configuracao e secrets

Constantes versionadas ficam em `include/AppConfig.h`. Credenciais locais ficam em `include/Secrets.h`, criado a partir de `include/Secrets.h.example` e ignorado pelo Git.

## 18. Seguranca

O broker MQTT, PostgreSQL e o ESP32 nao devem ser expostos a internet. MQTT sem TLS/autenticacao e aceitavel apenas na POC local controlada. Qualquer comando MQTT futuro exige autenticacao, autorizacao, threat model e ADR.

## 19. Limitacoes conhecidas

- Dependencia de NTP para `measuredAt`.
- Dependencia de COM local para upload/monitor.
- MQTT local sem TLS.
- GPIO 4 e strapping pin.
- Sem validacao fisica automatica nesta entrega.

## 20. Decisoes pendentes

- Periodo definitivo de publicacao em uso real.
- Autenticacao MQTT fora de laboratorio.
- Estrategia de ingestao backend para payload parcial.
- Validacao fisica ponta a ponta ESP32 -> MQTT -> backend -> PostgreSQL.

## 21. Evolucao futura

Itens futuros devem permanecer em Draft ate decisao formal: presenca, som, audio, alertas locais, OTA seguro, comandos MQTT e IA local.

## 22. Contrato com o backend

O backend deve consumir `home/+/+/environment` ou o topico especifico oficial e validar `deviceId`, `room`, `measuredAt` e campos sensoriais recebidos. Payload quebrado deve ser rejeitado; payload parcial deve ser aceito apenas se essa regra estiver documentada no backend.

## 23. Criterios de validacao

- `pio run` compila.
- Testes nativos passam.
- Contract gate passa.
- Docs consistency gate passa.
- Secrets gate nao encontra credenciais reais versionadas.
- Validacao fisica so pode ser declarada quando upload/monitor/MQTT real forem executados e evidenciados.

## 24. Rastreabilidade

DHT22/BH1750 -> `EnvironmentReading` -> `MqttMessageBuilder` -> `MqttPublisher` -> topico MQTT -> broker Mosquitto -> backend -> PostgreSQL.

## 25. Historico de alteracoes

- 2026-05-31: SDD inicial criado durante bootstrap de governanca do firmware.
- 2026-05-31: ADR-0004 adiciona saida de audio local controlada. Esta atualizacao substitui a marcacao anterior de audio como futuro: microfone, wake word, reconhecimento de voz e IA embarcada continuam fora do escopo; alto-falante I2S/MAX98357A passa a ser capability local limitada.

## 26. Audio local controlado

O firmware agora aceita comandos locais de audio em `home/bedroom/esp32-bedroom-01/audio/command` e publica status em `home/bedroom/esp32-bedroom-01/audio/status`.

Tipos suportados no contrato:

- `play_audio_url`
- `play_audio_stream`
- `play_tone`
- `stop_audio`
- `set_volume`

Estados publicados:

- `accepted`
- `playing`
- `completed`
- `rejected`
- `failed`

Regras de seguranca:

- `commandId` obrigatorio.
- Volume entre `0` e `100`.
- URL/stream apenas `http://` local/private network.
- `play_audio_url` aceita WAV/PCM 16-bit mono nesta etapa; MP3/AAC/Opus nao sao suportados.
- URL publica rejeitada por padrao.
- URL/stream exige `expiresAt`.
- Duracao maxima limitada por `AUDIO_MAX_DURATION_MS`.
- Sem credenciais no payload.
- Sem microfone, wake word, reconhecimento de voz, IA embarcada, rele ou automacao eletrica.

Pinagem MAX98357A:

- BCLK/SCK: GPIO 26.
- LRC/WS: GPIO 25.
- DIN: GPIO 33.
- SD: nao conectar na POC atual (SD e shutdown/enable, nao data input).
- VIN: 5V/VIN.
- GND: GND.
- SPK+ e SPK- devem ir apenas ao alto-falante; nunca ligar SPK- ao GND.

Estado atual: `play_tone`, `stop_audio`, `set_volume`, parser, fila e status MQTT estao implementados. `play_audio_url` tem implementacao firmware para baixar WAV HTTP local, validar header PCM 16-bit mono, configurar I2S e escrever audio em chunks. `play_audio_url` foi validado fisicamente para WAV/PCM local com evidencia em video, com observacao de audio levemente saturado/estourado. `play_audio_stream` continua nao implementado para streaming continuo. `stop_audio` durante reproducao WAV ainda exige evidencia fisica dedicada.
