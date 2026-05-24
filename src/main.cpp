#include <Arduino.h>

#include "AppConfig.h"
#include "Bh1750LightSensor.h"
#include "DhtEnvironmentSensor.h"
#include "MqttMessageBuilder.h"
#include "MqttPublisher.h"
#include "PubSubMqttClientAdapter.h"
#include "Secrets.h"
#include "TimeProvider.h"
#include "WiFiConnection.h"
#include <WiFi.h>
#include <string>

WiFiConnection wifiConnection(WIFI_SSID_VALUE, WIFI_PASSWORD_VALUE);
TimeProvider timeProvider;
DhtEnvironmentSensor environmentSensor(DHT_PIN);
Bh1750LightSensor lightSensor(BH1750_I2C_ADDRESS, I2C_SDA_PIN, I2C_SCL_PIN);
PubSubMqttClientAdapter mqttClient(MQTT_HOST_VALUE, MQTT_PORT_VALUE);
MqttPublisher mqttPublisher(mqttClient, ROOM_SLUG, DEVICE_EXTERNAL_ID, MQTT_HOST_VALUE, MQTT_PORT_VALUE);

unsigned long lastPublishAt = 0;

namespace {
void printHexAddress(uint8_t address) {
    Serial.print("0x");
    if (address < 16) {
        Serial.print('0');
    }
    Serial.print(address, HEX);
}

void logBootConfig() {
    std::string topic = buildEnvironmentTopic(ROOM_SLUG, DEVICE_EXTERNAL_ID);

    Serial.print("[CONFIG] deviceId=");
    Serial.println(DEVICE_EXTERNAL_ID);
    Serial.print("[CONFIG] room=");
    Serial.println(ROOM_SLUG);
    Serial.print("[CONFIG] mqttHost=");
    Serial.println(MQTT_HOST_VALUE);
    Serial.print("[CONFIG] mqttPort=");
    Serial.println(MQTT_PORT_VALUE);
    Serial.print("[CONFIG] mqttTopic=");
    Serial.println(topic.c_str());
    Serial.print("[CONFIG] dhtPin=");
    Serial.println(DHT_PIN);
    Serial.print("[CONFIG] bh1750Sda=");
    Serial.println(I2C_SDA_PIN);
    Serial.print("[CONFIG] bh1750Scl=");
    Serial.println(I2C_SCL_PIN);
    Serial.print("[CONFIG] bh1750Address=");
    printHexAddress(BH1750_I2C_ADDRESS);
    Serial.println();
}

void logWifiStatus() {
    Serial.print("[WIFI] status=");
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("connected ip=");
        Serial.print(WiFi.localIP());
        Serial.print(" rssi=");
        Serial.print(WiFi.RSSI());
        Serial.println("dBm");
    } else {
        Serial.print("disconnected code=");
        Serial.println(WiFi.status());
    }
}

void logTimeStatus(bool ntpSynced, const String& measuredAt) {
    Serial.print("[TIME] ntpSynced=");
    Serial.print(ntpSynced ? "true" : "false");
    Serial.print(" measuredAt=");
    Serial.println(measuredAt.length() > 0 ? measuredAt : "(unavailable)");
}

void logEnvironmentReading(const EnvironmentReading& reading, const String& measuredAt) {
    Serial.println("[ENV] Ciclo de leitura iniciado");
    Serial.print("[ENV] deviceId=");
    Serial.print(DEVICE_EXTERNAL_ID);
    Serial.print(" room=");
    Serial.println(ROOM_SLUG);
    logWifiStatus();
    logTimeStatus(timeProvider.isReady(), measuredAt);
    Serial.println();

    Serial.print("[DHT22] status=");
    if (reading.hasTemperature && reading.hasHumidity) {
        Serial.print("valid temperatureCelsius=");
        Serial.print(reading.temperatureCelsius, 2);
        Serial.print(" humidityPercentage=");
        Serial.println(reading.humidityPercentage, 2);
    } else if (reading.hasTemperature || reading.hasHumidity) {
        Serial.print("partial");
        if (reading.hasTemperature) {
            Serial.print(" temperatureCelsius=");
            Serial.print(reading.temperatureCelsius, 2);
        } else {
            Serial.print(" temperatureCelsius=invalid");
        }
        if (reading.hasHumidity) {
            Serial.print(" humidityPercentage=");
            Serial.print(reading.humidityPercentage, 2);
        } else {
            Serial.print(" humidityPercentage=invalid");
        }
        Serial.println(" reason=partial_read");
    } else {
        Serial.println("invalid reason=nan_read");
    }

    Serial.print("[BH1750] status=");
    if (reading.hasLuminosity) {
        Serial.print("valid address=");
        printHexAddress(BH1750_I2C_ADDRESS);
        Serial.print(" luminosityLux=");
        Serial.println(reading.luminosityLux, 2);
    } else {
        Serial.print("invalid reason=");
        Serial.print(lightSensor.isReady() ? "read_failed" : "not_initialized");
        Serial.print(" address=");
        printHexAddress(BH1750_I2C_ADDRESS);
        Serial.println();
    }

    if (!reading.hasAnyValidValue()) {
        Serial.println("[ENV] no_valid_sensor_reading=true");
    }
    Serial.println();
}
}

void setup() {
    Serial.begin(115200);
    delay(1000);

    Serial.println("Iniciando Home AI Room Observer ESP32...");
    logBootConfig();

    environmentSensor.begin();
    lightSensor.begin();
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
    lightSensor.readInto(reading);
    String measuredAt = timeProvider.nowIsoUtc();

    logEnvironmentReading(reading, measuredAt);
    mqttPublisher.publishEnvironment(reading, std::string(measuredAt.c_str()));
}
