# Autonomous Squad Operating Model

## Entrada de tarefa

Toda tarefa deve informar objetivo, escopo, arquivos provaveis, impacto em hardware/MQTT/backend e criterios de aceite.

## Leitura de contexto

O agente deve ler `README.md`, SDD, agent memory, ADRs e docs de arquitetura antes de alterar codigo.

## Decisao de escopo

Separar POC atual de itens futuros. Audio, IA, presenca, OTA, comandos remotos e automacao eletrica exigem decisao formal.

## Execucao

Implementar mudancas pequenas, preservar contratos e evitar refactor sem necessidade.

## Validacao

Rodar gates locais compativeis com a mudanca. Validacao fisica deve ser explicitamente evidenciada.

## Evidencia

Registrar comandos, saidas, exit codes, riscos e proximos passos em `docs/evidence/runs/`.

## Approval review

Executar `scripts/governance/run-firmware-approval-review.ps1` antes de declarar governanca pronta.

## Correcao de gaps

Falhas devem gerar acao corretiva ou bloqueio declarado. Nao mascarar falhas como sucesso parcial.

## Memoria do projeto

Atualizar `docs/squad/firmware-agent-memory.md` quando uma decisao estavel mudar.
