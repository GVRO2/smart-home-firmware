#include "GpioPresenceSensor.h"

GpioPresenceSensor::GpioPresenceSensor(int pin) : pin_(pin), ready_(false) {}

void GpioPresenceSensor::begin() {
    pinMode(pin_, INPUT);
    ready_ = true;
}

PresenceReading GpioPresenceSensor::read() {
    PresenceReading reading;
    if (!ready_) {
        reading.isValid = false;
        return reading;
    }
    reading.presenceDetected = (digitalRead(pin_) == HIGH);
    reading.isValid = true;
    return reading;
}

void GpioPresenceSensor::readInto(EnvironmentReading& reading) {
    PresenceReading presence = read();
    if (presence.isValid) {
        reading.presenceDetected = presence.presenceDetected;
        reading.hasPresence = true;
    }
}

bool GpioPresenceSensor::isReady() const {
    return ready_;
}
