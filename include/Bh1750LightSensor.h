#pragma once

#include <Arduino.h>
#include <BH1750.h>
#include <Wire.h>

#include "EnvironmentReading.h"
#include "LightSensor.h"

class Bh1750LightSensor : public LightSensor {
public:
    Bh1750LightSensor(uint8_t address, int sdaPin, int sclPin);

    void begin() override;
    void readInto(EnvironmentReading& reading) override;
    bool isReady() const override;

private:
    BH1750 lightMeter_;
    uint8_t address_;
    int sdaPin_;
    int sclPin_;
    bool ready_;
};
