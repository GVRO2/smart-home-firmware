#pragma once

#include <Arduino.h>
#include <BH1750.h>
#include <Wire.h>

#include "EnvironmentReading.h"

class Bh1750LightSensor {
public:
    Bh1750LightSensor(uint8_t address, int sdaPin, int sclPin);

    void begin();
    void readInto(EnvironmentReading& reading);
    bool isReady() const;

private:
    BH1750 lightMeter_;
    uint8_t address_;
    int sdaPin_;
    int sclPin_;
    bool ready_;
};
