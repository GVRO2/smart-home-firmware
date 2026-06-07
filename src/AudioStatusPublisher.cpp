#include "AudioStatusPublisher.h"

#include <ArduinoJson.h>

#include "AudioConfig.h"

AudioStatusPublisher::AudioStatusPublisher(IMqttClient& client, const char* roomSlug, const char* deviceExternalId)
    : client_(client), roomSlug_(roomSlug), deviceExternalId_(deviceExternalId) {
}

bool AudioStatusPublisher::publishStatus(
    const std::string& commandId,
    const char* state,
    const char* message,
    const std::string& reportedAtUtc,
    const char* reason
) {
    if (!client_.connected()) {
        return false;
    }

    JsonDocument document;
    document["deviceId"] = deviceExternalId_;
    document["room"] = roomSlug_;
    document["commandId"] = commandId;
    document["state"] = state;
    if (reason != nullptr && reason[0] != '\0') {
        document["reason"] = reason;
    }
    document["message"] = message;
    document["reportedAt"] = reportedAtUtc;

    std::string payload;
    serializeJson(document, payload);
    std::string topic = buildAudioStatusTopic(roomSlug_, deviceExternalId_);
    return client_.publish(topic.c_str(), payload.c_str());
}
