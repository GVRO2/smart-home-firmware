#include "PubSubMqttClientAdapter.h"

#include "AppConfig.h"

PubSubMqttClientAdapter::PubSubMqttClientAdapter(const char* host, int port)
    : wifiClient_(), client_(wifiClient_) {
    client_.setBufferSize(MQTT_PACKET_BUFFER_SIZE);
    client_.setServer(host, port);
}

bool PubSubMqttClientAdapter::connected() {
    return client_.connected();
}

bool PubSubMqttClientAdapter::connect(const char* clientId) {
    return client_.connect(clientId);
}

bool PubSubMqttClientAdapter::publish(const char* topic, const char* payload) {
    return client_.publish(topic, payload);
}

bool PubSubMqttClientAdapter::subscribe(const char* topic) {
    return client_.subscribe(topic);
}

void PubSubMqttClientAdapter::setCallback(MqttMessageCallback callback) {
    client_.setCallback(callback);
}

void PubSubMqttClientAdapter::loop() {
    client_.loop();
}

int PubSubMqttClientAdapter::state() {
    return client_.state();
}
