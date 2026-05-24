# Troubleshooting

## BH1750 nao inicializa

- confirme VCC em `3V3`, nao em 5V;
- confirme GND comum entre sensor e ESP32;
- confirme SDA em GPIO 21 e SCL em GPIO 22;
- confirme ADDR ligado ao GND para endereco `0x23`;
- confira se a dependencia `claws/BH1750` foi instalada pelo PlatformIO.

Log esperado de falha:

```text
Falha ao inicializar BH1750 no endereco I2C 0x23
```

## `luminosityLux` nao aparece no payload

O firmware nao publica valor falso. Quando o BH1750 esta ausente ou retorna
leitura invalida, `luminosityLux` e omitido.

Logs possiveis:

```text
BH1750 indisponivel. Luminosidade nao sera publicada neste ciclo.
Leitura BH1750 invalida.
```

## DHT invalido

- confirme DATA em GPIO 4;
- use pull-up de 4.7k a 10k entre DATA e VCC quando o sensor for cru;
- aguarde alguns ciclos, pois leituras DHT podem falhar isoladamente.

## MQTT nao publica

- confirme Wi-Fi local;
- confirme host/porta em `include/Secrets.h`;
- confirme que o broker aceita conexao local;
- confira NTP: sem horario UTC valido, a publicacao e cancelada.

## Secrets

`include/Secrets.h` deve existir localmente e nao deve ser commitado. Use
`include/Secrets.h.example` como modelo.
