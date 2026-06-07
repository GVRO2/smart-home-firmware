# Weekly Health Gaps Backlog

## Open Gaps

| ID | First Seen | Last Seen | Severity | Area | Description | Recommended Action | Status |
|---|---|---|---|---|---|---|
| WHG-2026-06-01-002 | 2026-06-01 | 2026-06-01 | P1 | Integracao | Sem evidencia nova de ESP32 -> MQTT -> backend -> PostgreSQL real. | Rodar teste end-to-end com broker/backend/firmware e salvar evidencia. | Open |
| WHG-2026-06-01-003 | 2026-06-01 | 2026-06-04 | P1 | Audio | `play_audio_url` tem decoder WAV/PCM firmware e evidencia fisica de playback local em video; audio ficou levemente saturado e `stop_audio` durante WAV ainda nao foi validado. `play_audio_stream` segue sem streaming continuo. | Ajustar ganho/volume do WAV, validar `stop_audio` durante reproducao e manter MP3/streaming fora do escopo ate nova decisao. | Open |
| WHG-2026-06-01-004 | 2026-06-01 | 2026-06-01 | P2 | DX/Gates | Maven reportou `BUILD SUCCESS`, mas o wrapper PowerShell retornou 1 por stderr/warnings. | Ajustar captura de stderr e exit code para evitar falso vermelho. | Open |
| WHG-2026-06-01-005 | 2026-06-01 | 2026-06-01 | P2 | Security/Ops | `include/Secrets.h` existe localmente e depende de `.gitignore`. | Manter ignorado e revisar antes de commit. | Open |

## Resolved Gaps

| ID | First Seen | Resolved At | Evidence | Notes |
|---|---|---|---|---|
| WHG-2026-06-01-001 | 2026-06-01 | 2026-06-04 | `docs/evidence/runs/20260604-2134-physical-sensor-mqtt-validation/summary.md`; `docs/evidence/runs/20260604-1847-physical-audio-url-video-validation/summary.md`; `docs/evidence/runs/20260604-2110-physical-play-tone-validation/summary.md` | ESP32, sensores, MQTT e audio local passaram a ter evidencia fisica recente. Permanecem gaps separados para backend/PostgreSQL real, tuning de audio e `stop_audio` durante WAV. |
