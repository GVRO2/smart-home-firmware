#pragma once

#include "EnvironmentReading.h"

class EnvironmentSensor {
public:
    virtual ~EnvironmentSensor() = default;
    virtual void begin() = 0;
    virtual EnvironmentReading read() = 0;
};
