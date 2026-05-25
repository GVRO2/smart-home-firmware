# ESP32 Standalone Boot Reliability Report

Data: 2026-05-24
Projeto: Home AI Room Observer ESP32

## Status final

Parcialmente validado em software. Firmware atualizado para boot autonomo sem dependencia de Serial Monitor, com comportamento nao bloqueante para Wi-Fi/MQTT/NTP, heartbeat visual e logs de diagnostico de boot.

Validacao de hardware em campo ainda pendente (sem acesso fisico ao ESP32/broker nesta sessao).

## Causa provavel encontrada

A causa mais provavel era combinacao de inicializacao com etapas potencialmente longas no setup e ausencia de indicadores visuais fora do Serial, gerando percepcao de que so funciona ao abrir monitor (evento que costuma resetar placa por DTR/RTS).

Risco adicional identificado: DHT22 em GPIO 4 (pino de strapping do ESP32) pode afetar boot em alguns cenarios eletricos (pull-up/pull-down e tempo de subida de energia).

## Diagnostico obrigatorio (10 itens)

1. Existe while(!Serial)?
- Nao encontrado.

2. Existe logica dependente de Serial para continuar?
- Nao havia bloqueio direto por Serial.
- Havia fluxo de setup com operacoes de rede em sequencia, que podiam atrasar inicio funcional.

3. Firmware trava esperando Wi-Fi indefinidamente?
- Nao indefinidamente, mas tentava por janelas longas.
- Ajustado para timeout curto e retentativa periodica sem travar permanentemente.

4. Firmware trava esperando MQTT indefinidamente?
- Nao indefinidamente.
- Mantido nao bloqueante com retentativa periodica e logs padronizados.

5. Boot bloqueia se BH1750 ou DHT22 falhar?
- Nao bloqueia. Sensores invalidos nao derrubam loop.
- Publicacao e omitida quando nao ha leitura valida.

6. Existem delays excessivos no setup?
- Reduzido setup para inicializacao rapida de perifericos e logs.
- Conexoes de rede migradas para o loop via ensure*.

7. Ha watchdog ou heartbeat?
- Heartbeat LED adicionado no loop.
- Sem watchdog dedicado novo nesta alteracao.

8. DHT22 no GPIO 4 pode interferir no boot?
- Sim, potencialmente, pois GPIO 4 e pino de strapping.
- Firmware agora loga aviso de risco no boot.

9. Existe log suficiente para boot/Wi-Fi/MQTT/publish?
- Sim, com prefixos [BOOT], [WIFI], [MQTT], [TIME], [SENSOR], [ENV].

10. Existe LED heartbeat sem Serial?
- Sim. LED heartbeat com padroes de status:
  - 1 piscada: boot iniciado
  - 2 piscadas: Wi-Fi conectado
  - 3 piscadas: MQTT conectado
  - 4 piscadas: publish realizado

## Arquivos alterados

- include/AppConfig.h
- src/main.cpp
- src/WiFiConnection.cpp
- src/MqttPublisher.cpp
- src/TimeProvider.cpp
- README.md

## Mudancas implementadas

- Remocao de dependencia de conexoes no setup (Wi-Fi/NTP/MQTT).
- Primeira tentativa imediata no loop para Wi-Fi/MQTT/NTP (sem esperar janela inicial).
- Wi-Fi com timeout reduzido (6s) e retentativa controlada.
- NTP somente quando Wi-Fi esta conectado; timeout reduzido.
- Logs padronizados:
  - [BOOT] firmware_start
  - [BOOT] config_loaded
  - [WIFI] connect_attempt
  - [WIFI] connected ip=...
  - [MQTT] connect_attempt ...
  - [MQTT] connected
  - [SENSOR] bh1750_init=success|failed
  - [SENSOR] dht_ready=true
- Heartbeat LED no GPIO 2 (configuravel em AppConfig.h).
- Aviso de risco no boot para DHT em pino de strapping.

## Logs antes/depois

Antes (exemplos):
- Conectando no Wi-Fi...
- Wi-Fi conectado.
- Falha ao conectar no Wi-Fi.
- Sincronizando horario NTP...

Depois (exemplos):
- [BOOT] firmware_start
- [BOOT] config_loaded
- [BOOT] warning=dht_pin_is_strapping pin=4
- [WIFI] connect_attempt
- [WIFI] connected ip=192.168.x.x
- [WIFI] connect_failed status=...
- [TIME] ntp_sync_attempt
- [TIME] ntp_synced=true
- [MQTT] connect_attempt host=... port=... clientId=...
- [MQTT] connected
- [MQTT] publish=success

## Evidencias de validacao executadas nesta sessao

- Build firmware ESP32: sucesso (pio run)
- Testes nativos: sucesso (24/24)

## Testes fisicos obrigatorios (pendentes)

1. Upload do firmware.
2. Desconectar USB de dados e energizar apenas por fonte/cabo.
3. Nao abrir Serial Monitor e verificar recebimento no Mosquitto/backend/PostgreSQL.
4. Abrir Serial Monitor depois e confirmar logs.
5. Pressionar EN/RST e confirmar comportamento identico.
6. Repetir power-cycle 3 vezes e confirmar publish autonomo.

## Resultado esperado para criterios de aceite

- CA-001: atendido em nivel de codigo; validar em hardware.
- CA-002: atendido em nivel de codigo; validar em hardware.
- CA-003: atendido em nivel de codigo (falhas nao bloqueantes).
- CA-004: atendido (heartbeat LED implementado).
- CA-005: atendido em nivel de codigo (logs serial preservados, nao obrigatorios).

## Pendencias

- Executar bateria de testes fisicos obrigatorios com dispositivo energizado sem abrir monitor serial.
- Se houver instabilidade de boot persistente, considerar migrar DHT22 do GPIO 4 para GPIO 27 (ou outro pino nao-strapping) e repetir validacao.
