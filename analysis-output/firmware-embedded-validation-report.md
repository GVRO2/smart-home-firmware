# Firmware Embedded Validation Report

## Porta detectada
- `COM5`
- Hardware ID: `USB VID:PID=1A86:55D4 SER=5AC9039558 LOCATION=1-4`
- Descrição: `USB-Enhanced-SERIAL CH9102 (COM5)`

## 1. Arquivos criados
- Nenhum arquivo adicional de firmware foi criado nesta rodada.

## 2. Arquivos alterados
- [platformio.ini](platformio.ini)

## 3. Comandos executados
| Comando | Resultado | Observação |
|---|---|---|
| `pio device list` | Sucesso | Detectou `COM5` como porta do ESP32 |
| `pio run` | Sucesso | Build do `esp32dev` concluído |
| `pio test -e native` | Sucesso | 20 testes nativos passaram |
| `pio run --target upload` | Falhou | Porta correta usada, mas o chip respondeu com `Wrong boot mode detected (0x17)` |
| `pio device monitor -p COM5 -b 115200` | Sucesso | Serial conectou e exibiu inicialização e publicações MQTT |
| `pio test -e esp32dev` | Falhou | Erro no runner Unity do PlatformIO neste host, antes da etapa de upload |

## 4. Resultado do upload
- Upload tentou usar `COM5` corretamente.
- Falha observada: `Failed to connect to ESP32: Wrong boot mode detected (0x17)`. O chip precisa entrar em download mode para flash.
- O firmware já estava rodando no dispositivo, como confirmado pelo monitor serial.

## 5. Resultado do monitor serial
- O monitor serial abriu com sucesso em `COM5` a `115200` baud.
- Logs observados:
  - inicialização do firmware
  - conexão Wi-Fi bem-sucedida
  - sincronização NTP bem-sucedida
  - conexão MQTT bem-sucedida
  - publicações periódicas em `home/bedroom/esp32-bedroom-01/environment`
- Isso confirma que a execução embarcada está ativa no ESP32.

## 6. Resultado do MQTTX
- Não houve acesso direto à interface do MQTTX nesta workspace.
- A validação do broker foi feita com um subscriber MQTT local em Python no tópico `home/bedroom/esp32-bedroom-01/environment`.
- Resultado observado no payload vivo:
  - `deviceId`
  - `room`
  - `measuredAt`
  - `temperatureCelsius`
  - `humidityPercentage`
- O payload vivo não continha `counter`, `luminosityLux`, SSID nem senha.
- Foi observado também um payload retido antigo no broker com `counter` e `luminosityLux`; o payload vivo atual não carrega esses campos.

## 7. Pendências restantes
- Pressionar `BOOT` e/ou `RESET` manualmente no ESP32 para entrar em download mode e repetir `pio run --target upload`.
- Confirmar a visualização no MQTTX local, se você quiser a validação pela UI do cliente em vez do subscriber local.
- Se o runner embarcado continuar falhando neste host, validar `pio test -e esp32dev` em uma máquina com a instalação do PlatformIO sem esse erro do Unity.

## 8. Cobertura validada nesta rodada
- Porta serial correta identificada.
- Firmware compilando no `esp32dev`.
- Testes nativos passando.
- Monitor serial mostrando leitura e publicação MQTT reais.
- Payload vivo sem `counter`, `luminosityLux`, SSID ou senha.
