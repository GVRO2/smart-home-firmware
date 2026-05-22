#include "WiFiConnection.h"

#include <WiFi.h>

namespace {
constexpr unsigned long WIFI_RETRY_INTERVAL_MS = 10000;
constexpr unsigned long WIFI_CONNECT_TIMEOUT_MS = 20000;
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
    if (now - lastConnectAttemptAt_ < WIFI_RETRY_INTERVAL_MS) {
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

    Serial.println("Conectando no Wi-Fi...");
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid_, password_);

    unsigned long startAt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAt < WIFI_CONNECT_TIMEOUT_MS) {
        delay(500);
        Serial.print('.');
    }
    Serial.println();

    if (isConnected()) {
        Serial.println("Wi-Fi conectado.");
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
    } else {
        Serial.println("Falha ao conectar no Wi-Fi.");
    }
}
