#pragma once

#include <Arduino.h>
#include <DHT.h>

#include "EnvironmentSensor.h"
#include "EnvironmentReading.h"

class DhtEnvironmentSensor : public EnvironmentSensor {
public:
    explicit DhtEnvironmentSensor(int pin);

    void begin() override;
    EnvironmentReading read() override;

private:
    DHT dht_;
};
