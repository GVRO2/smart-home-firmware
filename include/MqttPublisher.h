#pragma once

#include <string>

#include "EnvironmentReading.h"
#include "IMqttClient.h"
#include "MqttPublisherPort.h"

class MqttPublisher : public MqttPublisherPort {
public:
    MqttPublisher(
        IMqttClient& client,
        const char* roomSlug,
        const char* deviceExternalId,
        const char* host,
        int port
    );

    void connect() override;
    void ensureConnected() override;
    void loop() override;
    void setAudioCommandTopic(const char* topic);
    void setLocalIpAddress(const std::string& ipAddress);
    bool publishEnvironment(const EnvironmentReading& reading, const std::string& measuredAtUtc) override;

private:
    void connectInternal();
    void subscribeAudioCommands();

    IMqttClient& client_;
    const char* roomSlug_;
    const char* deviceExternalId_;
    const char* host_;
    int port_;
    const char* audioCommandTopic_;
    std::string localIpAddress_;
    unsigned long lastConnectAttemptAt_;
};
