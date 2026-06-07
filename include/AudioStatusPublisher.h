#pragma once

#include <string>

#include "IMqttClient.h"

class AudioStatusPublisher {
public:
    AudioStatusPublisher(IMqttClient& client, const char* roomSlug, const char* deviceExternalId);

    bool publishStatus(
        const std::string& commandId,
        const char* state,
        const char* message,
        const std::string& reportedAtUtc,
        const char* reason = nullptr
    );

private:
    IMqttClient& client_;
    const char* roomSlug_;
    const char* deviceExternalId_;
};
