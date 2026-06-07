#pragma once

#include <Arduino.h>
#include "PresenceSensor.h"

class GpioPresenceSensor : public PresenceSensor {
public:
    GpioPresenceSensor(int pin);
    void begin() override;
    PresenceReading read() override;
    void readInto(EnvironmentReading& reading) override;
    bool isReady() const override;

private:
    int pin_;
    bool ready_;
};
