#include "Bh1750LightSensor.h"

Bh1750LightSensor::Bh1750LightSensor(uint8_t address, int sdaPin, int sclPin)
    : lightMeter_(address), address_(address), sdaPin_(sdaPin), sclPin_(sclPin), ready_(false) {}

void Bh1750LightSensor::begin() {
    Wire.begin(sdaPin_, sclPin_);

    ready_ = lightMeter_.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, address_, &Wire);
    if (ready_) {
        Serial.print("[BH1750] init=success address=0x");
        Serial.print(address_, HEX);
        Serial.print(" sda=");
        Serial.print(sdaPin_);
        Serial.print(" scl=");
        Serial.print(sclPin_);
        Serial.println(" mode=CONTINUOUS_HIGH_RES_MODE");
    } else {
        Serial.print("[BH1750] init=failed address=0x");
        Serial.print(address_, HEX);
        Serial.println(" check_wiring=true");
    }
}

void Bh1750LightSensor::readInto(EnvironmentReading& reading) {
    if (!ready_) {
        return;
    }

    float luminosityLux = lightMeter_.readLightLevel();
    reading.hasLuminosity = EnvironmentReading::isValidLuminosity(luminosityLux);

    if (reading.hasLuminosity) {
        reading.luminosityLux = luminosityLux;
    }
}

bool Bh1750LightSensor::isReady() const {
    return ready_;
}
