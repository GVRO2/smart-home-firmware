#pragma once

#include <string>

#include "EnvironmentReading.h"

class MqttPublisherPort {
public:
    virtual ~MqttPublisherPort() = default;
    virtual void connect() = 0;
    virtual void ensureConnected() = 0;
    virtual void loop() = 0;
    virtual bool publishEnvironment(const EnvironmentReading& reading, const std::string& measuredAtUtc) = 0;
};
