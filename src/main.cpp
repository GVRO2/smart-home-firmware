#include <Arduino.h>

#include "AppConfig.h"
#include "DhtEnvironmentSensor.h"
#include "MqttPublisher.h"
#include "PubSubMqttClientAdapter.h"
#include "Secrets.h"
#include "TimeProvider.h"
#include "WiFiConnection.h"
#include <string>

WiFiConnection wifiConnection(WIFI_SSID_VALUE, WIFI_PASSWORD_VALUE);
TimeProvider timeProvider;
DhtEnvironmentSensor environmentSensor(DHT_PIN);
PubSubMqttClientAdapter mqttClient(MQTT_HOST_VALUE, MQTT_PORT_VALUE);
MqttPublisher mqttPublisher(mqttClient, ROOM_SLUG, DEVICE_EXTERNAL_ID);

unsigned long lastPublishAt = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("Iniciando Home AI Room Observer ESP32...");

    environmentSensor.begin();
    wifiConnection.connect();
    timeProvider.sync();
    mqttPublisher.connect();
}

void loop() {
    wifiConnection.ensureConnected();
    timeProvider.ensureSynced();
    mqttPublisher.ensureConnected();
    mqttPublisher.loop();

    unsigned long now = millis();
    if (now - lastPublishAt < PUBLISH_INTERVAL_MS) {
        return;
    }

    lastPublishAt = now;

    EnvironmentReading reading = environmentSensor.read();
    String measuredAt = timeProvider.nowIsoUtc();

    mqttPublisher.publishEnvironment(reading, std::string(measuredAt.c_str()));
}