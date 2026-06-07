# Firmware Agent Operating Rules

- Nunca alterar pinagem sem atualizar docs, SDD e ADR aplicavel.
- Nunca alterar payload sem atualizar contrato, testes e docs.
- Nunca alterar topico sem verificar backend.
- Nunca adicionar automacao eletrica sem historia aprovada e ADR.
- Nunca commitar credenciais reais.
- A POC deve permanecer sem comandos remotos.
- Sempre registrar evidencia para validacoes relevantes.
- Sempre classificar status final.
- Sempre indicar bloqueios de ambiente.
- Sempre preservar escopo observacional da POC.
- Nunca declarar validacao fisica sem ESP32 real, logs e evidence.
- Nunca expor broker MQTT, PostgreSQL ou ESP32 publicamente para esta POC.
- Qualquer alteracao em testes C/C++/C# deve seguir a skill `Test Harness Quality & Anti-False-Success`.
- Nunca aprovar teste que apenas passa; validar registro, alcance, fixture limpa, acao explicita e asserts coerentes.
