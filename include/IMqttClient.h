#pragma once

class IMqttClient {
public:
    virtual ~IMqttClient() = default;

    virtual bool connected() = 0;
    virtual bool connect(const char* clientId) = 0;
    virtual bool publish(const char* topic, const char* payload) = 0;
    virtual void loop() = 0;
    virtual int state() = 0;
};