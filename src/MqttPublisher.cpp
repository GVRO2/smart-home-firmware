#include <Arduino.h>

#include "MqttPublisher.h"

#include <string>

#include "MqttMessageBuilder.h"

namespace {
constexpr unsigned long MQTT_RETRY_INTERVAL_MS = 10000;
}

MqttPublisher::MqttPublisher(
    IMqttClient& client,
    const char* roomSlug,
    const char* deviceExternalId,
    const char* host,
    int port
)
    : client_(client),
      roomSlug_(roomSlug),
      deviceExternalId_(deviceExternalId),
      host_(host),
      port_(port),
      lastConnectAttemptAt_(0) {
}

void MqttPublisher::connect() {
    connectInternal();
}

void MqttPublisher::ensureConnected() {
    if (client_.connected()) {
        return;
    }

    unsigned long now = millis();
    if (now - lastConnectAttemptAt_ < MQTT_RETRY_INTERVAL_MS) {
        return;
    }

    connectInternal();
}

void MqttPublisher::loop() {
    if (client_.connected()) {
        client_.loop();
    }
}

bool MqttPublisher::publishEnvironment(const EnvironmentReading& reading, const std::string& measuredAtUtc) {
    if (!client_.connected()) {
        ensureConnected();
    }

    if (!client_.connected()) {
        Serial.print("[MQTT] connected=false state=");
        Serial.println(client_.state());
        Serial.println("[MQTT] publish=skipped reason=mqtt_disconnected");
        return false;
    }

    Serial.print("[MQTT] connected=true host=");
    Serial.print(host_);
    Serial.print(" port=");
    Serial.println(port_);

    if (!hasValidSensorValue(reading)) {
        Serial.println("[ENV] no_valid_sensor_reading=true");
        Serial.println("[MQTT] publish=skipped reason=no_valid_sensor_reading");
        return false;
    }

    if (measuredAtUtc.empty()) {
        Serial.println("[MQTT] publish=skipped reason=time_unavailable");
        return false;
    }

    std::string topic = buildEnvironmentTopic(roomSlug_, deviceExternalId_);
    std::string payload = buildEnvironmentPayloadJson(reading, roomSlug_, deviceExternalId_, measuredAtUtc.c_str());

    if (payload.empty()) {
        Serial.println("[MQTT] publish=skipped reason=payload_empty");
        return false;
    }

    Serial.print("[MQTT] topic=");
    Serial.println(topic.c_str());
    Serial.print("[MQTT] payload=");
    Serial.println(payload.c_str());

    bool published = client_.publish(topic.c_str(), payload.c_str());
    if (published) {
        Serial.print("[MQTT] publish=success");
        if (!reading.hasTemperature) {
            Serial.print(" warning=temperatureCelsius_omitted");
        }
        if (!reading.hasHumidity) {
            Serial.print(" warning=humidityPercentage_omitted");
        }
        if (!reading.hasLuminosity) {
            Serial.print(" warning=luminosityLux_omitted");
        }
        Serial.println();
    } else {
        Serial.println("[MQTT] publish=failed");
        Serial.print("[MQTT] state=");
        Serial.println(client_.state());
    }

    return published;
}

void MqttPublisher::connectInternal() {
    lastConnectAttemptAt_ = millis();

    if (client_.connected()) {
        return;
    }

    Serial.print("[MQTT] connect=attempt host=");
    Serial.print(host_);
    Serial.print(" port=");
    Serial.print(port_);
    Serial.print(" clientId=");
    Serial.println(deviceExternalId_);
    if (client_.connect(deviceExternalId_)) {
        Serial.println("[MQTT] connect=success");
    } else {
        Serial.print("[MQTT] connect=failed state=");
        Serial.println(client_.state());
        Serial.print("[MQTT] connect=failed host=");
        Serial.print(host_);
        Serial.print(" port=");
        Serial.println(port_);
    }
}
