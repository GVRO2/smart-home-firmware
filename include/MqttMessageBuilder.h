#pragma once

#include <ArduinoJson.h>

#include <string>

#include "EnvironmentReading.h"

inline std::string buildEnvironmentTopic(const char* roomSlug, const char* deviceExternalId) {
    return std::string("home/") + roomSlug + "/" + deviceExternalId + "/environment";
}

inline bool hasValidSensorValue(const EnvironmentReading& reading) {
    return reading.hasAnyValidValue();
}

inline bool canBuildEnvironmentPayload(const EnvironmentReading& reading, const std::string& measuredAtUtc) {
    return hasValidSensorValue(reading) && !measuredAtUtc.empty();
}

inline std::string buildEnvironmentPayloadJson(
    const EnvironmentReading& reading,
    const char* roomSlug,
    const char* deviceExternalId,
    const std::string& measuredAtUtc,
    const std::string& ipAddress = ""
) {
    if (!canBuildEnvironmentPayload(reading, measuredAtUtc)) {
        return std::string();
    }

    JsonDocument document;
    document["deviceId"] = deviceExternalId;
    document["room"] = roomSlug;
    document["measuredAt"] = measuredAtUtc;

    if (reading.hasTemperature) {
        document["temperatureCelsius"] = reading.temperatureCelsius;
    }

    if (reading.hasHumidity) {
        document["humidityPercentage"] = reading.humidityPercentage;
    }

    if (reading.hasLuminosity) {
        document["luminosityLux"] = reading.luminosityLux;
    }

    if (reading.hasPresence) {
        document["presenceDetected"] = reading.presenceDetected;
    }

    if (!ipAddress.empty()) {
        document["ipAddress"] = ipAddress;
    }

    std::string payload;
    serializeJson(document, payload);
    return payload;
}
