#pragma once

#include <string>

#include "MqttPublisherPort.h"

class MockMqttPublisher final : public MqttPublisherPort {
public:
    bool publishResult = true;
    int connectCalls = 0;
    int ensureConnectedCalls = 0;
    int loopCalls = 0;
    int publishEnvironmentCalls = 0;
    EnvironmentReading lastReading;
    std::string lastMeasuredAtUtc;

    void connect() override {
        ++connectCalls;
    }

    void ensureConnected() override {
        ++ensureConnectedCalls;
    }

    void loop() override {
        ++loopCalls;
    }

    bool publishEnvironment(const EnvironmentReading& reading, const std::string& measuredAtUtc) override {
        ++publishEnvironmentCalls;
        lastReading = reading;
        lastMeasuredAtUtc = measuredAtUtc;
        return publishResult;
    }
};
