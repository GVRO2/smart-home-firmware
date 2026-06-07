#pragma once

#include "LightSensor.h"

class MockLightSensor final : public LightSensor {
public:
    bool ready = true;
    bool hasLuminosity = false;
    float luminosityLux = 0.0f;
    int beginCalls = 0;
    int readCalls = 0;

    void begin() override {
        ++beginCalls;
    }

    void readInto(EnvironmentReading& reading) override {
        ++readCalls;
        reading.hasLuminosity = hasLuminosity;
        if (hasLuminosity) {
            reading.luminosityLux = luminosityLux;
        }
    }

    bool isReady() const override {
        return ready;
    }
};
