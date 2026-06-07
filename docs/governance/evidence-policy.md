# Firmware Evidence Policy

## Estrutura

Cada execucao relevante deve criar:

```text
docs/evidence/runs/<YYYYMMDD-HHMM-task-name>/
```

Arquivos esperados:

- `summary.md`
- `commands.md`
- `validation-result.md`
- `risks.md`
- `next-steps.md`
- `logs/`
- `serial-output.txt`, quando aplicavel
- `mqtt-output.txt`, quando aplicavel
- `build-output.txt`, quando aplicavel

## Comandos

Registrar comando, diretorio, exit code, resultado e trecho relevante da saida. Comando nao executado deve constar como `NOT_EXECUTED` com motivo.

## Falhas

Falhas reais permanecem falhas. Ambiente indisponivel deve ser classificado como bloqueio quando impedir o gate.

## Imutabilidade

Evidence historica nao deve ser apagada. Se uma execucao estiver errada, gerar nova evidence e apontar a correcao.
