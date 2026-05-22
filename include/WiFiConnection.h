#pragma once

#include <Arduino.h>

class WiFiConnection {
public:
    WiFiConnection(const char* ssid, const char* password);

    void connect();
    void ensureConnected();
    bool isConnected() const;

private:
    void connectInternal();

    const char* ssid_;
    const char* password_;
    unsigned long lastConnectAttemptAt_;
};
