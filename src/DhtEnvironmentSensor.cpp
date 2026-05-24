#include "DhtEnvironmentSensor.h"

#include "AppConfig.h"

DhtEnvironmentSensor::DhtEnvironmentSensor(int pin)
    : dht_(pin, DHT_TYPE) {}

void DhtEnvironmentSensor::begin() {
    dht_.begin();
}

EnvironmentReading DhtEnvironmentSensor::read() {
    EnvironmentReading reading{};

    float temperature = dht_.readTemperature();
    float humidity = dht_.readHumidity();

    reading.hasTemperature = EnvironmentReading::isValidTemperature(temperature);
    reading.hasHumidity = EnvironmentReading::isValidHumidity(humidity);

    if (reading.hasTemperature) {
        reading.temperatureCelsius = temperature;
    }

    if (reading.hasHumidity) {
        reading.humidityPercentage = humidity;
    }

    return reading;
}
