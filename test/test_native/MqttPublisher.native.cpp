#include "MqttPublisher.h"

#include <string>

#include "MqttMessageBuilder.h"

MqttPublisher::MqttPublisher(
    IMqttClient& client,
    const char* roomSlug,
    const char* deviceExternalId,
    const char* host,
    int port
)
    : client_(client),
      roomSlug_(roomSlug),
      deviceExternalId_(deviceExternalId),
      host_(host),
      port_(port),
      lastConnectAttemptAt_(0) {
}

void MqttPublisher::connect() {
    connectInternal();
}

void MqttPublisher::ensureConnected() {
    if (!client_.connected()) {
        connectInternal();
    }
}

void MqttPublisher::loop() {
    if (client_.connected()) {
        client_.loop();
    }
}

bool MqttPublisher::publishEnvironment(const EnvironmentReading& reading, const std::string& measuredAtUtc) {
    if (!client_.connected()) {
        ensureConnected();
    }

    if (!client_.connected()) {
        return false;
    }

    if (!hasValidSensorValue(reading)) {
        return false;
    }

    if (measuredAtUtc.empty()) {
        return false;
    }

    std::string topic = buildEnvironmentTopic(roomSlug_, deviceExternalId_);
    std::string payload = buildEnvironmentPayloadJson(reading, roomSlug_, deviceExternalId_, measuredAtUtc);

    if (payload.empty()) {
        return false;
    }

    return client_.publish(topic.c_str(), payload.c_str());
}

void MqttPublisher::connectInternal() {
    client_.connect(deviceExternalId_);
}
