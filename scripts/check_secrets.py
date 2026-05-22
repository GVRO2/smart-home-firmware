from __future__ import annotations

import re
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]
GITIGNORE = ROOT / ".gitignore"
SECRETS_EXAMPLE = ROOT / "include" / "Secrets.h.example"
MAIN_CPP = ROOT / "src" / "main.cpp"


def fail(message: str) -> int:
    print(message)
    return 1


def is_placeholder(value: str) -> bool:
    normalized = value.strip().upper()
    return any(token in normalized for token in ("SEU", "SUA", "YOUR", "PLACEHOLDER", "TODO"))


def main() -> int:
    if not GITIGNORE.exists():
        return fail(".gitignore nao encontrado.")

    gitignore_text = GITIGNORE.read_text(encoding="utf-8")
    if "include/Secrets.h" not in gitignore_text:
        return fail("include/Secrets.h nao esta protegido no .gitignore.")

    if not SECRETS_EXAMPLE.exists():
        return fail("include/Secrets.h.example nao encontrado.")

    secrets_text = SECRETS_EXAMPLE.read_text(encoding="utf-8")
    secret_patterns = {
        "WIFI_SSID_VALUE": r'WIFI_SSID_VALUE\s*\"([^\"]+)\"',
        "WIFI_PASSWORD_VALUE": r'WIFI_PASSWORD_VALUE\s*\"([^\"]+)\"',
        "MQTT_HOST_VALUE": r'MQTT_HOST_VALUE\s*\"([^\"]+)\"',
    }

    for name, pattern in secret_patterns.items():
        match = re.search(pattern, secrets_text)
        if not match:
            return fail(f"{name} nao encontrado em include/Secrets.h.example.")
        if not is_placeholder(match.group(1)):
            return fail(f"{name} em include/Secrets.h.example precisa ser placeholder, nao segredo real.")

    if not MAIN_CPP.exists():
        return fail("src/main.cpp nao encontrado.")

    main_text = MAIN_CPP.read_text(encoding="utf-8")
    suspicious_patterns = [
        r'WIFI_SSID_VALUE\s*\"[^\"]+\"',
        r'WIFI_PASSWORD_VALUE\s*\"[^\"]+\"',
        r'MQTT_HOST_VALUE\s*\"[^\"]+\"',
        r'192\.168\.\d+\.\d+',
    ]

    for pattern in suspicious_patterns:
        if re.search(pattern, main_text):
            return fail("src/main.cpp contem credenciais ou host hardcoded.")

    print("Secrets check passed.")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())