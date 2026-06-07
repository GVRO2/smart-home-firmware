#pragma once

#include "EnvironmentReading.h"

class LightSensor {
public:
    virtual ~LightSensor() = default;
    virtual void begin() = 0;
    virtual void readInto(EnvironmentReading& reading) = 0;
    virtual bool isReady() const = 0;
};
