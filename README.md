# Home AI Room Observer ESP32 Firmware

Firmware PlatformIO/Arduino para a POC local-first Home AI Room Observer.
O ESP32 le sensores ambientais locais e publica medicoes via MQTT para o backend
da rede residencial.

Escopo atual:

- leitura de temperatura e umidade com DHT22/AM2302;
- leitura de luminosidade em lux com GY-302 V322/BH1750;
- publicacao MQTT observacional;
- sem rele, comando remoto, automacao eletrica ou exposicao publica.

## Hardware suportado

- ESP32 DevKit V1;
- DHT22 ou AM2302 no GPIO 4;
- GY-302 V322/BH1750 via I2C, SDA GPIO 21, SCL GPIO 22, endereco `0x23`;
- Wi-Fi local;
- broker MQTT local.

## Pinagem

### DHT22 / AM2302

| Sensor | Pino | ESP32 |
|---|---|---|
| DHT22 | VCC | 3V3 |
| DHT22 | GND | GND |
| DHT22 | DATA | GPIO 4 |

Se o DHT22 for sensor cru de 4 pinos, use resistor pull-up de 4.7k a 10k
entre DATA e VCC. Modulos prontos geralmente ja trazem pull-up integrado.

### GY-302 V322 / BH1750

| Sensor | Pino | ESP32 |
|---|---|---|
| GY-302 | VCC | 3V3 |
| GY-302 | GND | GND |
| GY-302 | SCL | GPIO 22 |
| GY-302 | SDA | GPIO 21 |
| GY-302 | ADDR | GND |

| ADDR | Endereco I2C |
|---|---|
| GND | `0x23` |
| 3V3 | `0x5C` |

Nao deixe ADDR solto. O firmware usa `0x23` por padrao.

## Configuracao

1. Copie `include/Secrets.h.example` para `include/Secrets.h`.
2. Preencha SSID, senha Wi-Fi, host MQTT e porta MQTT no arquivo local.
3. Mantenha `include/Secrets.h` fora do Git; ele ja esta no `.gitignore`.

Configuracoes versionadas ficam em `include/AppConfig.h`:

- `ROOM_SLUG`: sala publicada no payload;
- `DEVICE_EXTERNAL_ID`: identificador externo do dispositivo;
- `PUBLISH_INTERVAL_MS`: intervalo de publicacao;
- `DHT_PIN`, `I2C_SDA_PIN`, `I2C_SCL_PIN`, `BH1750_I2C_ADDRESS`.

## Payload MQTT

Topico:

```text
home/bedroom/esp32-bedroom-01/environment
```

Payload quando todas as leituras sao validas:

```json
{
  "deviceId": "esp32-bedroom-01",
  "room": "bedroom",
  "measuredAt": "2026-05-24T12:00:00Z",
  "temperatureCelsius": 27.4,
  "humidityPercentage": 62.5,
  "luminosityLux": 123.0
}
```

Se um sensor falhar, o firmware nao inventa dados. Campos sem leitura valida sao
omitidos do JSON. Se nenhuma leitura sensorial for valida, o payload nao e
publicado.

## Build, upload e monitor

```powershell
pio run
pio run --target upload
pio device monitor
```

O projeto usa `COM5` em `platformio.ini`. Ajuste `upload_port` e `monitor_port`
se o ESP32 aparecer em outra porta.

## Testes

```powershell
pio test -e native
py -3 scripts/check_secrets.py
```

Os testes nativos cobrem contrato MQTT, payload JSON, validacao de leituras e
publicacao. Os testes embarcados sao smoke tests para compilacao no ESP32.

## Logs esperados no Serial Monitor

Inicializacao bem-sucedida do BH1750:

```text
BH1750 inicializado no endereco I2C 0x23
```

Falha de inicializacao ou leitura:

```text
Falha ao inicializar BH1750 no endereco I2C 0x23
BH1750 indisponivel. Luminosidade nao sera publicada neste ciclo.
Leitura BH1750 invalida.
```

## Troubleshooting rapido

- BH1750 nao inicializa: confirme VCC em 3V3, GND comum, SDA GPIO 21, SCL GPIO 22 e ADDR no GND.
- Lux ausente no payload: verifique o log Serial; o firmware omite `luminosityLux` quando a leitura e invalida.
- DHT invalido: confirme GPIO 4, alimentacao 3V3 e pull-up no DATA quando o sensor nao for modulo pronto.
- Sem publicacao: confira Wi-Fi, NTP e MQTT; o firmware cancela publicacao se nao houver horario UTC valido.
- Broker externo: nao exponha o MQTT publicamente para esta POC; mantenha o broker na rede local.
