#include <Arduino.h>

#include "MqttPublisher.h"

#include <string>

#include "MqttMessageBuilder.h"

namespace {
constexpr unsigned long MQTT_RETRY_INTERVAL_MS = 10000;
}

MqttPublisher::MqttPublisher(IMqttClient& client, const char* roomSlug, const char* deviceExternalId)
    : client_(client), roomSlug_(roomSlug), deviceExternalId_(deviceExternalId), lastConnectAttemptAt_(0) {
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
        Serial.println("MQTT desconectado. Nao foi possivel publicar.");
        return false;
    }

    if (!hasValidSensorValue(reading)) {
        Serial.println("Nenhum valor sensorial valido para publicar.");
        return false;
    }

    if (measuredAtUtc.empty()) {
        Serial.println("Horario UTC indisponivel. Publicacao cancelada.");
        return false;
    }

    std::string topic = buildEnvironmentTopic(roomSlug_, deviceExternalId_);
    std::string payload = buildEnvironmentPayloadJson(reading, roomSlug_, deviceExternalId_, measuredAtUtc.c_str());

    if (payload.empty()) {
        Serial.println("Payload MQTT invalido. Publicacao cancelada.");
        return false;
    }

    bool published = client_.publish(topic.c_str(), payload.c_str());
    if (published) {
        Serial.print("Payload publicado em ");
        Serial.println(topic.c_str());
    } else {
        Serial.println("Falha ao publicar payload MQTT.");
    }

    return published;
}

void MqttPublisher::connectInternal() {
    lastConnectAttemptAt_ = millis();

    if (client_.connected()) {
        return;
    }

    Serial.println("Conectando no MQTT...");
    if (client_.connect(deviceExternalId_)) {
        Serial.println("MQTT conectado.");
    } else {
        Serial.print("Falha ao conectar no MQTT. Estado: ");
        Serial.println(client_.state());
    }
}
