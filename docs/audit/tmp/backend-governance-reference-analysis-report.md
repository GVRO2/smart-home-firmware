# Backend Governance Reference Analysis Report

## 1. Status

BACKEND_REFERENCE_ANALYZED_WITH_WARNINGS

## 2. Localizacao do repo irmao

Repo analisado: `C:\Users\KABUM\IdeaProjects\smart-home`.

Busca realizada:

- `C:\Users\KABUM\OneDrive\Documents\PlatformIO\Projects`: nao havia backend irmao ao lado do firmware.
- `C:\Users\KABUM\IdeaProjects`: encontrado `smart-home`, com docs, scripts, evidence, backlog, security e agent tooling do Home AI Room Observer.

## 3. Estrutura documental encontrada

- `docs/documentation-governance.md`: precedencia documental, anti-fake-success e regras de evidence.
- `docs/evidence/README.md`, `docs/evidence/evidence-index.md`, `docs/evidence/evidence-log.md`: politica e historico de evidencia.
- `docs/backlog/README.md` e backlog segmentado por diagnostico, historias, sequenciamento, matrizes e checklist.
- `docs/decisions/pending-decisions.md`: registro de decisoes e pendencias.
- `docs/security/*`: threat model e baseline de seguranca.
- `docs/governance/*`: regras para agentes, gates, dependencias e governanca.
- `docs/backend/*`, `docs/api/*`, `docs/runtime/*`: documentacao tecnica e runtime.

## 4. Estrutura de governanca encontrada

- Scripts de agentes em `scripts/agent/`.
- Scripts de evidence em `scripts/evidence/`.
- Scripts de security em `scripts/security/`.
- Scripts de dev em `scripts/dev/`.
- Politicas de nao mascarar falhas, nao declarar sucesso sem comando e nao tratar approval review como release readiness.
- Uso de evidence com comandos, stdout/stderr, exit code e manifesto quando aplicavel.

## 5. Estrutura de SDD encontrada

O backend organiza especificacoes, backlog, contratos e documentacao por area. Para o firmware, o equivalente adequado e um SDD mais compacto e direto, cobrindo hardware, sensores, MQTT, falhas, seguranca local-first e rastreabilidade.

## 6. Estrutura de evidencias encontrada

Padrao central:

- evidence prova execucao, nao define regra.
- comandos precisam ter exit code e saida registravel.
- falha nao pode virar warning por narrativa.
- evidence run vive em `docs/evidence/runs/`.

Para firmware, o formato foi adaptado para `summary.md`, `commands.md`, `logs/`, `build-output.txt`, `validation-result.md`, `risks.md` e `next-steps.md`.

## 7. Estrutura de backlog encontrada

O backend usa decomposicao profissional com objetivo, epicos, features, historias, subtarefas, gates e status. Para firmware, a decomposicao foi adaptada a POC observacional: sensores, MQTT, contrato backend, governanca/qualidade e itens futuros em Draft/P3.

## 8. Padroes reutilizaveis no firmware

- Evidencia como prova objetiva, nao como fonte normativa.
- Relatorios temporarios em `docs/audit/tmp/`.
- ADRs para decisoes duradouras.
- Regras explicitas para agentes.
- Approval review separado de release readiness.
- Gates simples, executaveis e com exit code confiavel.
- Separacao entre escopo atual e futuro.
- Seguranca local-first e nao exposicao publica.

## 9. Padroes que NAO devem ser copiados

- Maven, Spring, OpenAPI e Testcontainers nao se aplicam diretamente ao firmware.
- Gates REST/frontend nao devem ser transportados sem adaptacao.
- Runtime backend com PostgreSQL nao deve virar requisito de build do firmware.
- Release readiness backend nao substitui validacao fisica com ESP32 real.

## 10. Recomendacoes para adaptacao

1. Usar PlatformIO como build gate do firmware.
2. Criar gate de contrato lendo constantes reais do codigo e docs canonicos.
3. Manter secrets scan local simples e conservador.
4. Classificar upload/monitor como validacao fisica/manual.
5. Registrar qualquer ausencia de ferramenta como `BLOCKED_BY_MISSING_TOOL`.
6. Manter POC sem rele, comandos remotos, broker publico ou automacao eletrica.
