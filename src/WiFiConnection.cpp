#include "WiFiConnection.h"

#include <WiFi.h>

namespace {
constexpr unsigned long WIFI_RETRY_INTERVAL_MS = 10000;
constexpr unsigned long WIFI_SCAN_INTERVAL_MS = 30000;
constexpr unsigned long WIFI_CONNECT_TIMEOUT_MS = 6000;
constexpr unsigned long WIFI_CONNECT_POLL_MS = 250;
constexpr int WIFI_SCAN_NETWORKS_MAX = 10;
}

WiFiConnection::WiFiConnection(KnownWifiNetworkProvider& networkProvider)
    : networkProvider_(networkProvider), lastConnectAttemptAt_(0), lastScanAttemptAt_(0) {}

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

String WiFiConnection::getLocalIpAddress() {
    if (!isConnected()) {
        return "";
    }

    IPAddress ip = WiFi.localIP();
    String ipString = ip.toString();

    if (ipString == "0.0.0.0" || ipString.isEmpty()) {
        return "";
    }

    return ipString;
}

void WiFiConnection::connectInternal() {
    lastConnectAttemptAt_ = millis();

    if (isConnected()) {
        return;
    }

    Serial.println("[WIFI] WIFI_SCAN_STARTED");
    bool connected = connectToBestKnownWifi();

    if (connected) {
        Serial.print("[WIFI] WIFI_CONNECTED ssid=");
        Serial.print(WiFi.SSID());
        Serial.print(" ip=");
        Serial.print(WiFi.localIP());
        Serial.print(" rssi=");
        Serial.print(WiFi.RSSI());
        Serial.println("dBm");
    } else {
        Serial.println("[WIFI] WIFI_CONNECTION_FAILED");
    }
}

bool WiFiConnection::connectToBestKnownWifi() {
    int networkCount = networkProvider_.getNetworkCount();
    if (networkCount == 0) {
        Serial.println("[WIFI] WIFI_NO_KNOWN_NETWORK_AVAILABLE");
        return false;
    }

    unsigned long now = millis();
    if (lastScanAttemptAt_ != 0 && now - lastScanAttemptAt_ < WIFI_SCAN_INTERVAL_MS) {
        Serial.println("[WIFI] scan_skipped reason=recent_scan");
    } else {
        lastScanAttemptAt_ = now;
        int n = WiFi.scanNetworks();
        Serial.print("[WIFI] scan_found=");
        Serial.println(n);

        if (n == 0) {
            Serial.println("[WIFI] WIFI_NO_KNOWN_NETWORK_AVAILABLE");
            return false;
        }

        if (n > WIFI_SCAN_NETWORKS_MAX) {
            n = WIFI_SCAN_NETWORKS_MAX;
        }

        struct NetworkCandidate {
            KnownWifiNetwork network;
            int rssi;
            bool found;
        };

        NetworkCandidate candidates[10];
        int candidateCount = 0;

        for (int i = 0; i < n && candidateCount < 10; ++i) {
            String ssid = WiFi.SSID(i);
            int rssi = WiFi.RSSI(i);

            for (int j = 0; j < networkCount; ++j) {
                KnownWifiNetwork known = networkProvider_.getNetwork(j);
                if (ssid == known.ssid) {
                    Serial.print("[WIFI] WIFI_KNOWN_NETWORK_FOUND ssid=");
                    Serial.print(known.ssid);
                    Serial.print(" priority=");
                    Serial.print(known.priority);
                    Serial.print(" rssi=");
                    Serial.print(rssi);
                    Serial.println("dBm");

                    candidates[candidateCount].network = known;
                    candidates[candidateCount].rssi = rssi;
                    candidates[candidateCount].found = true;
                    candidateCount++;
                    break;
                }
            }
        }

        WiFi.scanDelete();

        if (candidateCount == 0) {
            Serial.println("[WIFI] WIFI_NO_KNOWN_NETWORK_AVAILABLE");
            return false;
        }

        for (int i = 0; i < candidateCount - 1; ++i) {
            for (int j = i + 1; j < candidateCount; ++j) {
                if (candidates[j].network.priority > candidates[i].network.priority ||
                    (candidates[j].network.priority == candidates[i].network.priority && candidates[j].rssi > candidates[i].rssi)) {
                    NetworkCandidate temp = candidates[i];
                    candidates[i] = candidates[j];
                    candidates[j] = temp;
                }
            }
        }

        for (int i = 0; i < candidateCount; ++i) {
            Serial.print("[WIFI] WIFI_CONNECTING ssid=");
            Serial.print(candidates[i].network.ssid);
            Serial.print(" priority=");
            Serial.print(candidates[i].network.priority);
            Serial.print(" rssi=");
            Serial.print(candidates[i].rssi);
            Serial.println("dBm");

            if (tryConnectToNetwork(candidates[i].network)) {
                return true;
            }

            Serial.print("[WIFI] connect_failed ssid=");
            Serial.println(candidates[i].network.ssid);
        }

        return false;
    }

    for (int i = 0; i < networkCount; ++i) {
        KnownWifiNetwork network = networkProvider_.getNetwork(i);
        Serial.print("[WIFI] WIFI_CONNECTING ssid=");
        Serial.print(network.ssid);
        Serial.print(" priority=");
        Serial.println(network.priority);

        if (tryConnectToNetwork(network)) {
            return true;
        }

        Serial.print("[WIFI] connect_failed ssid=");
        Serial.println(network.ssid);
    }

    return false;
}

bool WiFiConnection::tryConnectToNetwork(const KnownWifiNetwork& network) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(network.ssid, network.password);

    unsigned long startAt = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAt < WIFI_CONNECT_TIMEOUT_MS) {
        delay(WIFI_CONNECT_POLL_MS);
    }

    return isConnected();
}
