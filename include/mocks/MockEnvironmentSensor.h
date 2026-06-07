#pragma once

#include "EnvironmentSensor.h"

class MockEnvironmentSensor final : public EnvironmentSensor {
public:
    EnvironmentReading nextReading;
    int beginCalls = 0;
    int readCalls = 0;

    void begin() override {
        ++beginCalls;
    }

    EnvironmentReading read() override {
        ++readCalls;
        return nextReading;
    }
};
