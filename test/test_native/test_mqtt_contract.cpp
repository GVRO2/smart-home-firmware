#include <unity.h>

#include <ArduinoJson.h>

#include <string>

#include "EnvironmentReading.h"
#include "IMqttClient.h"
#include "MqttMessageBuilder.h"
#include "MqttPublisher.h"
#include "TimeFormatting.h"

namespace {
class FakeMqttClient final : public IMqttClient {
public:
    bool connectedValue = false;
    bool connectResult = true;
    bool publishResult = true;
    int stateValue = -1;
    int connectCalls = 0;
    int publishCalls = 0;
    int loopCalls = 0;
    std::string lastClientId;
    std::string lastTopic;
    std::string lastPayload;

    bool connected() override {
        return connectedValue;
    }

    bool connect(const char* clientId) override {
        ++connectCalls;
        lastClientId = clientId;
        connectedValue = connectResult;
        return connectResult;
    }

    bool publish(const char* topic, const char* payload) override {
        ++publishCalls;
        lastTopic = topic;
        lastPayload = payload;
        return publishResult;
    }

    void loop() override {
        ++loopCalls;
    }

    int state() override {
        return stateValue;
    }
};

void setUp() {}
void tearDown() {}
}

void shouldAcceptReadingWhenTemperatureAndHumidityAreValid();
void shouldAcceptReadingWhenOnlyTemperatureIsValid();
void shouldAcceptReadingWhenOnlyHumidityIsValid();
void shouldRejectReadingWhenAllSensorValuesAreInvalid();
void shouldRejectReadingWhenTemperatureIsBelowMinimum();
void shouldRejectReadingWhenTemperatureIsAboveMaximum();
void shouldRejectReadingWhenHumidityIsBelowMinimum();
void shouldRejectReadingWhenHumidityIsAboveMaximum();

void shouldBuildOfficialMqttTopicWhenRoomAndDeviceAreProvided() {
    std::string topic = buildEnvironmentTopic("bedroom", "esp32-bedroom-01");

    TEST_ASSERT_EQUAL_STRING("home/bedroom/esp32-bedroom-01/environment", topic.c_str());
}

void shouldNotUseDeprecatedMqttTopicFormat() {
    std::string topic = buildEnvironmentTopic("bedroom", "esp32-bedroom-01");

    TEST_ASSERT_NOT_EQUAL(0, topic.compare("home/bedroom/environment"));
}

void shouldBuildPayloadWhenTemperatureAndHumidityAreValid() {
    EnvironmentReading reading;
    reading.temperatureCelsius = 27.4f;
    reading.humidityPercentage = 62.5f;
    reading.hasTemperature = true;
    reading.hasHumidity = true;

    std::string payload = buildEnvironmentPayloadJson(
        reading,
        "bedroom",
        "esp32-bedroom-01",
        "2026-05-21T20:30:00Z"
    );

    TEST_ASSERT_FALSE(payload.empty());

    JsonDocument document;
    DeserializationError error = deserializeJson(document, payload.c_str());
    TEST_ASSERT_EQUAL(DeserializationError::Ok, error.code());

    TEST_ASSERT_EQUAL_STRING("esp32-bedroom-01", document["deviceId"]);
    TEST_ASSERT_EQUAL_STRING("bedroom", document["room"]);
    TEST_ASSERT_EQUAL_FLOAT(27.4f, document["temperatureCelsius"]);
    TEST_ASSERT_EQUAL_FLOAT(62.5f, document["humidityPercentage"]);
    TEST_ASSERT_EQUAL_STRING("2026-05-21T20:30:00Z", document["measuredAt"]);
    TEST_ASSERT_TRUE(document["luminosityLux"].isNull());
    TEST_ASSERT_TRUE(document["counter"].isNull());
    TEST_ASSERT_FALSE(payload.find("SEU_WIFI") != std::string::npos);
    TEST_ASSERT_FALSE(payload.find("SUA_SENHA") != std::string::npos);
}

void shouldOmitTemperatureWhenTemperatureIsInvalid() {
    EnvironmentReading reading;
    reading.humidityPercentage = 62.5f;
    reading.hasHumidity = true;

    std::string payload = buildEnvironmentPayloadJson(
        reading,
        "bedroom",
        "esp32-bedroom-01",
        "2026-05-21T20:30:00Z"
    );

    JsonDocument document;
    deserializeJson(document, payload.c_str());

    TEST_ASSERT_TRUE(document["temperatureCelsius"].isNull());
    TEST_ASSERT_FALSE(document["humidityPercentage"].isNull());
}

void shouldOmitHumidityWhenHumidityIsInvalid() {
    EnvironmentReading reading;
    reading.temperatureCelsius = 27.4f;
    reading.hasTemperature = true;

    std::string payload = buildEnvironmentPayloadJson(
        reading,
        "bedroom",
        "esp32-bedroom-01",
        "2026-05-21T20:30:00Z"
    );

    JsonDocument document;
    deserializeJson(document, payload.c_str());

    TEST_ASSERT_FALSE(document["temperatureCelsius"].isNull());
    TEST_ASSERT_TRUE(document["humidityPercentage"].isNull());
}

void shouldNotBuildPayloadWhenNoSensorValueIsValid() {
    EnvironmentReading reading;

    std::string payload = buildEnvironmentPayloadJson(
        reading,
        "bedroom",
        "esp32-bedroom-01",
        "2026-05-21T20:30:00Z"
    );

    TEST_ASSERT_TRUE(payload.empty());
}

void shouldRejectPublishWhenMeasuredAtIsEmpty() {
    EnvironmentReading reading;
    reading.temperatureCelsius = 27.4f;
    reading.hasTemperature = true;

    TEST_ASSERT_FALSE(canBuildEnvironmentPayload(reading, std::string()));
}

void shouldIncludeMeasuredAtWhenTimestampIsAvailable() {
    std::string measuredAt = formatUtcIsoUtc(1779395400);

    TEST_ASSERT_FALSE(measuredAt.empty());
    TEST_ASSERT_TRUE(measuredAt.back() == 'Z');
}

void shouldPublishWhenReadingAndTimestampAreValid() {
    FakeMqttClient client;
    MqttPublisher publisher(client, "bedroom", "esp32-bedroom-01");

    EnvironmentReading reading;
    reading.temperatureCelsius = 27.4f;
    reading.humidityPercentage = 62.5f;
    reading.hasTemperature = true;
    reading.hasHumidity = true;

    bool published = publisher.publishEnvironment(reading, std::string("2026-05-21T20:30:00Z"));

    TEST_ASSERT_TRUE(published);
    TEST_ASSERT_EQUAL(1, client.publishCalls);
    TEST_ASSERT_EQUAL_STRING("home/bedroom/esp32-bedroom-01/environment", client.lastTopic.c_str());
    TEST_ASSERT_TRUE(client.lastPayload.find("\"deviceId\":\"esp32-bedroom-01\"") != std::string::npos);
}

void shouldNotPublishWhenReadingIsInvalid() {
    FakeMqttClient client;
    MqttPublisher publisher(client, "bedroom", "esp32-bedroom-01");

    EnvironmentReading reading;

    bool published = publisher.publishEnvironment(reading, std::string("2026-05-21T20:30:00Z"));

    TEST_ASSERT_FALSE(published);
    TEST_ASSERT_EQUAL(0, client.publishCalls);
}

void shouldNotPublishWhenTimestampIsEmpty() {
    FakeMqttClient client;
    MqttPublisher publisher(client, "bedroom", "esp32-bedroom-01");

    EnvironmentReading reading;
    reading.temperatureCelsius = 27.4f;
    reading.hasTemperature = true;

    bool published = publisher.publishEnvironment(reading, std::string());

    TEST_ASSERT_FALSE(published);
    TEST_ASSERT_EQUAL(0, client.publishCalls);
}

void shouldPublishToOfficialEnvironmentTopic() {
    FakeMqttClient client;
    MqttPublisher publisher(client, "bedroom", "esp32-bedroom-01");

    EnvironmentReading reading;
    reading.temperatureCelsius = 27.4f;
    reading.hasTemperature = true;

    bool published = publisher.publishEnvironment(reading, std::string("2026-05-21T20:30:00Z"));

    TEST_ASSERT_TRUE(published);
    TEST_ASSERT_EQUAL_STRING("home/bedroom/esp32-bedroom-01/environment", client.lastTopic.c_str());
}

int main(int argc, char** argv) {
    UNITY_BEGIN();
    RUN_TEST(shouldAcceptReadingWhenTemperatureAndHumidityAreValid);
    RUN_TEST(shouldAcceptReadingWhenOnlyTemperatureIsValid);
    RUN_TEST(shouldAcceptReadingWhenOnlyHumidityIsValid);
    RUN_TEST(shouldRejectReadingWhenAllSensorValuesAreInvalid);
    RUN_TEST(shouldRejectReadingWhenTemperatureIsBelowMinimum);
    RUN_TEST(shouldRejectReadingWhenTemperatureIsAboveMaximum);
    RUN_TEST(shouldRejectReadingWhenHumidityIsBelowMinimum);
    RUN_TEST(shouldRejectReadingWhenHumidityIsAboveMaximum);
    RUN_TEST(shouldBuildOfficialMqttTopicWhenRoomAndDeviceAreProvided);
    RUN_TEST(shouldNotUseDeprecatedMqttTopicFormat);
    RUN_TEST(shouldBuildPayloadWhenTemperatureAndHumidityAreValid);
    RUN_TEST(shouldOmitTemperatureWhenTemperatureIsInvalid);
    RUN_TEST(shouldOmitHumidityWhenHumidityIsInvalid);
    RUN_TEST(shouldNotBuildPayloadWhenNoSensorValueIsValid);
    RUN_TEST(shouldRejectPublishWhenMeasuredAtIsEmpty);
    RUN_TEST(shouldIncludeMeasuredAtWhenTimestampIsAvailable);
    RUN_TEST(shouldPublishWhenReadingAndTimestampAreValid);
    RUN_TEST(shouldNotPublishWhenReadingIsInvalid);
    RUN_TEST(shouldNotPublishWhenTimestampIsEmpty);
    RUN_TEST(shouldPublishToOfficialEnvironmentTopic);
    return UNITY_END();
}