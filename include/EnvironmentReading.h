#pragma once

#include <cmath>

struct EnvironmentReading {
    float temperatureCelsius = NAN;
    float humidityPercentage = NAN;
    bool hasTemperature = false;
    bool hasHumidity = false;

    static bool isValidTemperature(float temperature) {
        return !std::isnan(temperature) && temperature >= -40.0f && temperature <= 80.0f;
    }

    static bool isValidHumidity(float humidity) {
        return !std::isnan(humidity) && humidity >= 0.0f && humidity <= 100.0f;
    }

    bool hasAnyValidValue() const {
        return hasTemperature || hasHumidity;
    }
};
