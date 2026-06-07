#pragma once

#include <cstdint>

using MqttMessageCallback = void (*)(char* topic, uint8_t* payload, unsigned int length);

class IMqttClient {
public:
    virtual ~IMqttClient() = default;

    virtual bool connected() = 0;
    virtual bool connect(const char* clientId) = 0;
    virtual bool publish(const char* topic, const char* payload) = 0;
    virtual bool subscribe(const char* topic) = 0;
    virtual void setCallback(MqttMessageCallback callback) = 0;
    virtual void loop() = 0;
    virtual int state() = 0;
};
