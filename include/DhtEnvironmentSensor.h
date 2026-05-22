#pragma once

#include <Arduino.h>
#include <DHT.h>

#include "EnvironmentReading.h"

class DhtEnvironmentSensor {
public:
    explicit DhtEnvironmentSensor(int pin);

    void begin();
    EnvironmentReading read();

private:
    DHT dht_;
};
