#include "WiFiConnection.h"

#include <WiFi.h>

namespace {
constexpr unsigned long WIFI_RETRY_INTERVAL_MS = 10000;
constexpr unsigned long WIFI_CONNECT_TIMEOUT_MS = 6000;
constexpr unsigned long WIFI_CONNECT_POLL_MS = 250;
}

WiFiConnection::WiFiConnection(const char* ssid, const char* password)
    : ssid_(ssid), password_(password), lastConnectAttemptAt_(0) {}

void WiFiConnection::connect() {
    connectInternal();
}

void WiFiConnection::ensureConnected() {
    if (isConnected()) {
        return;
    }

    unsigned long now = millis();
    if (lastConnectAttemptAt_ != 0 && now - lastConnectAttemptAt_ < WIFI_RETRY_INTERVAL_MS) {
        return;
    }

    connectInternal();
}

bool WiFiConnection::isConnected() const {
    return WiFi.status() == WL_CONNECTED;
}

void WiFiConnection::connectInternal() {
    lastConnectAttemptAt_ = millis();

    if (isConnected()) {
        return;
    }

    Serial.println("[WIFI] connect_attempt");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid_, password_);

    unsigned long startAt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAt < WIFI_CONNECT_TIMEOUT_MS) {
        delay(WIFI_CONNECT_POLL_MS);
    }

    if (isConnected()) {
        Serial.print("[WIFI] connected ip=");
        Serial.println(WiFi.localIP());
    } else {
        Serial.print("[WIFI] connect_failed status=");
        Serial.println(WiFi.status());
    }
}
