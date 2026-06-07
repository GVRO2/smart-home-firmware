# Firmware Security

## Baseline

O firmware e local-first. O ESP32 se conecta a Wi-Fi local e publica em broker MQTT local. A POC atual e observacional e nao executa comandos remotos.

## Nao exposicao

- Nao expor o broker MQTT a internet.
- Nao expor PostgreSQL a internet.
- Nao expor o ESP32 para comandos externos diretos.
- Nao abrir porta publica para OTA ou debug.

## Credenciais

Credenciais reais ficam em `include/Secrets.h`, ignorado pelo Git. O template `include/Secrets.h.example` deve conter apenas placeholders.

## MQTT

MQTT sem TLS/autenticacao e aceitavel somente em rede local controlada. Para uso fora de laboratorio, exigir autenticacao MQTT, segmentacao de rede ou TLS conforme decisao formal.

## OTA futuro

OTA nao esta implementado. Qualquer OTA deve ter threat model, autenticacao, rollback seguro, evidencia e ADR.

## Comandos remotos

Comandos MQTT para o ESP32 sao fora do escopo. Qualquer comando remoto futuro precisa de autenticacao/autorizacao, allowlist de comandos, logs e validacao de impacto fisico.

## Audio futuro

Audio/alto-falante nao deve virar interface de comando sem decisao formal. Microfone aumenta superficie de privacidade e deve ser tratado como novo produto, nao como extensao trivial.

## Audio local ADR-0004

ADR-0004 autoriza apenas saida de audio local por I2S/MAX98357A. Audio nao executa comandos, nao escuta ambiente e nao autoriza automacao eletrica.

Regras obrigatorias:

- Aceitar apenas `home/bedroom/esp32-bedroom-01/audio/command`.
- Publicar resultado em `home/bedroom/esp32-bedroom-01/audio/status`.
- Exigir `commandId`.
- Rejeitar URL publica por padrao.
- Aceitar apenas `http://` local/private network para URL/stream.
- Rejeitar volume menor que 0 ou maior que 100.
- Limitar duracao maxima por `AUDIO_MAX_DURATION_MS`.
- Suportar `stop_audio`.
- Registrar `accepted`, `playing`, `completed`, `rejected` e `failed`.
- Nao incluir credenciais em payload.
- Nao adicionar microfone, wake word, reconhecimento de voz ou IA embarcada.
- Nao expor ESP32 nem broker MQTT a internet.

Riscos especificos: spam sonoro, susto, privacidade, horario noturno, brownout, ruido eletrico e URL indisponivel. Politica de silencio/noturno fica como trabalho futuro.

## Rede IoT

Recomendado manter o ESP32 em rede segregada ou VLAN IoT quando a POC evoluir, com broker acessivel apenas pelos componentes necessarios.
