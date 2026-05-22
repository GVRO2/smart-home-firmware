#include "PubSubMqttClientAdapter.h"

PubSubMqttClientAdapter::PubSubMqttClientAdapter(const char* host, int port)
    : wifiClient_(), client_(wifiClient_) {
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

void PubSubMqttClientAdapter::loop() {
    client_.loop();
}

int PubSubMqttClientAdapter::state() {
    return client_.state();
}