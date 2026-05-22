#pragma once

#include <Arduino.h>
#include <PubSubClient.h>
#include <WiFi.h>

#include "IMqttClient.h"

class PubSubMqttClientAdapter final : public IMqttClient {
public:
    PubSubMqttClientAdapter(const char* host, int port);

    bool connected() override;
    bool connect(const char* clientId) override;
    bool publish(const char* topic, const char* payload) override;
    void loop() override;
    int state() override;

private:
    WiFiClient wifiClient_;
    PubSubClient client_;
};