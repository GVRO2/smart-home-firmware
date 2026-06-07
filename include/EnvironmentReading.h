#pragma once

#include <cmath>

struct EnvironmentReading {
    float temperatureCelsius = NAN;
    float humidityPercentage = NAN;
    float luminosityLux = NAN;
    bool presenceDetected = false;
    bool hasTemperature = false;
    bool hasHumidity = false;
    bool hasLuminosity = false;
    bool hasPresence = false;

    static bool isValidTemperature(float temperature) {
        return !std::isnan(temperature) && temperature >= -40.0f && temperature <= 80.0f;
    }

    static bool isValidHumidity(float humidity) {
        return !std::isnan(humidity) && humidity >= 0.0f && humidity <= 100.0f;
    }

    static bool isValidLuminosity(float luminosity) {
        return !std::isnan(luminosity) && luminosity >= 0.0f;
    }

    bool hasAnyValidValue() const {
        return hasTemperature || hasHumidity || hasLuminosity || hasPresence;
    }
};
