#pragma once

#include "PresenceReading.h"
#include "EnvironmentReading.h"

class PresenceSensor {
public:
    virtual ~PresenceSensor() = default;
    virtual void begin() = 0;
    virtual PresenceReading read() = 0;
    virtual void readInto(EnvironmentReading& reading) = 0;
    virtual bool isReady() const = 0;
};
