#pragma once

#include <string>

#include "EnvironmentReading.h"
#include "IMqttClient.h"

class MqttPublisher {
public:
    MqttPublisher(IMqttClient& client, const char* roomSlug, const char* deviceExternalId);

    void connect();
    void ensureConnected();
    void loop();
    bool publishEnvironment(const EnvironmentReading& reading, const std::string& measuredAtUtc);

private:
    void connectInternal();

    IMqttClient& client_;
    const char* roomSlug_;
    const char* deviceExternalId_;
    unsigned long lastConnectAttemptAt_;
};
