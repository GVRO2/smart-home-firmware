# Weekly Health Review Policy

## Purpose

The weekly health review keeps the Home AI Room Observer aligned across firmware, backend, contracts, backlog, SDD, security and evidence.

## Schedule

Recommended cadence: every Monday at 09:00 local time.

External automation is not enabled by this repository change. If Task Scheduler, GitHub Actions or CI automation is later approved, it should call:

```powershell
powershell -ExecutionPolicy Bypass -File scripts\governance\run-weekly-project-health-review.ps1
```

## Evidence Rules

- Do not declare physical validation without ESP32 upload, serial output and MQTT evidence.
- Do not declare backend validation without command output or an evidence run.
- Do not treat mock evidence as release readiness.
- Store weekly reports in `docs/audit/weekly`.
- Store weekly evidence in `docs/evidence/runs/<YYYYMMDD-HHMM-weekly-project-health-review>`.

## Exit Codes

- `0`: green/yellow without P0/P1 blocking risks.
- `1`: orange/red with P0/P1 risk or gate failure.
- `2`: blocked by missing repository, unavailable essential tool or insufficient context.
