#include <unity.h>

#include <ArduinoJson.h>

#include <string>

#include "AudioCommandParser.h"
#include "AudioConfig.h"
#include "AudioPlaybackQueue.h"
#include "AudioStatusPublisher.h"
#include "EnvironmentReading.h"
#include "IMqttClient.h"
#include "mocks/MockAudioOutputDriver.h"
#include "MqttMessageBuilder.h"
#include "MqttPublisher.h"
#include "TimeFormatting.h"
#include "WavPcmAudio.h"

#include "../../src/AudioCommandParser.cpp"
#include "../../src/AudioPlaybackQueue.cpp"
#include "../../src/AudioPlaybackService.cpp"
#include "../../src/AudioStatusPublisher.cpp"

namespace {
class FakeMqttClient final : public IMqttClient {
public:
    bool connectedValue = false;
    bool connectResult = true;
    bool publishResult = true;
    int stateValue = -1;
    int connectCalls = 0;
    int publishCalls = 0;
    int subscribeCalls = 0;
    int loopCalls = 0;
    std::string lastClientId;
    std::string lastTopic;
    std::string lastPayload;
    std::string lastSubscription;

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

    bool subscribe(const char* topic) override {
        ++subscribeCalls;
        lastSubscription = topic;
        return true;
    }

    void setCallback(MqttMessageCallback) override {
    }

    void loop() override {
        ++loopCalls;
    }

    int state() override {
        return stateValue;
    }
};

void assertAudioStatusPayload(
    const std::string& payload,
    const char* expectedState,
    const char* expectedCommandId,
    const char* expectedMessage,
    const char* expectedReportedAt,
    const char* expectedReason = nullptr
) {
    JsonDocument document;
    DeserializationError error = deserializeJson(document, payload.c_str());
    TEST_ASSERT_EQUAL(DeserializationError::Ok, error.code());
    TEST_ASSERT_EQUAL_STRING("esp32-bedroom-01", document["deviceId"]);
    TEST_ASSERT_EQUAL_STRING("bedroom", document["room"]);
    TEST_ASSERT_EQUAL_STRING(expectedCommandId, document["commandId"]);
    TEST_ASSERT_EQUAL_STRING(expectedState, document["state"]);
    TEST_ASSERT_EQUAL_STRING(expectedMessage, document["message"]);
    TEST_ASSERT_EQUAL_STRING(expectedReportedAt, document["reportedAt"]);
    if (expectedReason == nullptr) {
        TEST_ASSERT_TRUE(document["reason"].isNull());
    } else {
        TEST_ASSERT_EQUAL_STRING(expectedReason, document["reason"]);
    }
}

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
void shouldAcceptReadingWhenLuminosityIsValid();
void shouldRejectReadingWhenLuminosityIsInvalid();
void shouldAcceptReadingWhenPresenceIsValid();
void shouldAcceptReadingWhenPresenceIsFalse();
void shouldBeCollisionFreeAndSafe();

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

void shouldBuildPayloadWithLuminosityWhenLightReadingIsValid() {
    EnvironmentReading reading;
    reading.temperatureCelsius = 27.4f;
    reading.humidityPercentage = 62.5f;
    reading.luminosityLux = 123.0f;
    reading.hasTemperature = true;
    reading.hasHumidity = true;
    reading.hasLuminosity = true;

    std::string payload = buildEnvironmentPayloadJson(
        reading,
        "bedroom",
        "esp32-bedroom-01",
        "2026-05-24T12:00:00Z"
    );

    TEST_ASSERT_FALSE(payload.empty());

    JsonDocument document;
    DeserializationError error = deserializeJson(document, payload.c_str());
    TEST_ASSERT_EQUAL(DeserializationError::Ok, error.code());

    TEST_ASSERT_EQUAL_STRING("esp32-bedroom-01", document["deviceId"]);
    TEST_ASSERT_EQUAL_STRING("bedroom", document["room"]);
    TEST_ASSERT_EQUAL_FLOAT(27.4f, document["temperatureCelsius"]);
    TEST_ASSERT_EQUAL_FLOAT(62.5f, document["humidityPercentage"]);
    TEST_ASSERT_EQUAL_FLOAT(123.0f, document["luminosityLux"]);
    TEST_ASSERT_EQUAL_STRING("2026-05-24T12:00:00Z", document["measuredAt"]);
}

void shouldOmitLuminosityWhenLightReadingIsInvalid() {
    EnvironmentReading reading;
    reading.temperatureCelsius = 27.4f;
    reading.hasTemperature = true;

    std::string payload = buildEnvironmentPayloadJson(
        reading,
        "bedroom",
        "esp32-bedroom-01",
        "2026-05-24T12:00:00Z"
    );

    JsonDocument document;
    deserializeJson(document, payload.c_str());

    TEST_ASSERT_FALSE(document["temperatureCelsius"].isNull());
    TEST_ASSERT_TRUE(document["luminosityLux"].isNull());
}

void shouldBuildPayloadWithPresenceTrueWhenPresenceDetectedIsNotNull() {
    EnvironmentReading reading;
    reading.presenceDetected = true;
    reading.hasPresence = true;

    std::string payload = buildEnvironmentPayloadJson(
        reading,
        "bedroom",
        "esp32-bedroom-01",
        "2026-05-24T12:00:00Z"
    );

    TEST_ASSERT_FALSE(payload.empty());

    JsonDocument document;
    DeserializationError error = deserializeJson(document, payload.c_str());
    TEST_ASSERT_EQUAL(DeserializationError::Ok, error.code());

    TEST_ASSERT_TRUE(document["presenceDetected"].as<bool>());
}

void shouldBuildPayloadWithPresenceFalseWhenPresenceDetectedIsNotNull() {
    EnvironmentReading reading;
    reading.presenceDetected = false;
    reading.hasPresence = true;

    std::string payload = buildEnvironmentPayloadJson(
        reading,
        "bedroom",
        "esp32-bedroom-01",
        "2026-05-24T12:00:00Z"
    );

    TEST_ASSERT_FALSE(payload.empty());

    JsonDocument document;
    DeserializationError error = deserializeJson(document, payload.c_str());
    TEST_ASSERT_EQUAL(DeserializationError::Ok, error.code());

    TEST_ASSERT_FALSE(document["presenceDetected"].as<bool>());
}

void shouldOmitPresenceWhenPresenceIsAbsent() {
    EnvironmentReading reading;
    reading.temperatureCelsius = 27.4f;
    reading.hasTemperature = true;

    std::string payload = buildEnvironmentPayloadJson(
        reading,
        "bedroom",
        "esp32-bedroom-01",
        "2026-05-24T12:00:00Z"
    );

    JsonDocument document;
    deserializeJson(document, payload.c_str());

    TEST_ASSERT_TRUE(document["presenceDetected"].isNull());
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
    MqttPublisher publisher(client, "bedroom", "esp32-bedroom-01", "192.168.1.200", 1883);

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
    MqttPublisher publisher(client, "bedroom", "esp32-bedroom-01", "192.168.1.200", 1883);

    EnvironmentReading reading;

    bool published = publisher.publishEnvironment(reading, std::string("2026-05-21T20:30:00Z"));

    TEST_ASSERT_FALSE(published);
    TEST_ASSERT_EQUAL(0, client.publishCalls);
}

void shouldNotPublishWhenTimestampIsEmpty() {
    FakeMqttClient client;
    MqttPublisher publisher(client, "bedroom", "esp32-bedroom-01", "192.168.1.200", 1883);

    EnvironmentReading reading;
    reading.temperatureCelsius = 27.4f;
    reading.hasTemperature = true;

    bool published = publisher.publishEnvironment(reading, std::string());

    TEST_ASSERT_FALSE(published);
    TEST_ASSERT_EQUAL(0, client.publishCalls);
}

void shouldPublishToOfficialEnvironmentTopic() {
    FakeMqttClient client;
    MqttPublisher publisher(client, "bedroom", "esp32-bedroom-01", "192.168.1.200", 1883);

    EnvironmentReading reading;
    reading.temperatureCelsius = 27.4f;
    reading.hasTemperature = true;

    bool published = publisher.publishEnvironment(reading, std::string("2026-05-21T20:30:00Z"));

    TEST_ASSERT_TRUE(published);
    TEST_ASSERT_EQUAL_STRING("home/bedroom/esp32-bedroom-01/environment", client.lastTopic.c_str());
}

void shouldBuildAudioCommandAndStatusTopics() {
    TEST_ASSERT_EQUAL_STRING(
        "home/bedroom/esp32-bedroom-01/audio/command",
        buildAudioCommandTopic("bedroom", "esp32-bedroom-01").c_str()
    );
    TEST_ASSERT_EQUAL_STRING(
        "home/bedroom/esp32-bedroom-01/audio/status",
        buildAudioStatusTopic("bedroom", "esp32-bedroom-01").c_str()
    );
}

void shouldSubscribeAudioCommandTopicAfterMqttConnect() {
    FakeMqttClient client;
    MqttPublisher publisher(client, "bedroom", "esp32-bedroom-01", "192.168.1.200", 1883);

    publisher.setAudioCommandTopic("home/bedroom/esp32-bedroom-01/audio/command");
    publisher.connect();

    TEST_ASSERT_EQUAL(1, client.subscribeCalls);
    TEST_ASSERT_EQUAL_STRING("home/bedroom/esp32-bedroom-01/audio/command", client.lastSubscription.c_str());
}

void shouldParseValidPlayAudioUrlCommand() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse(
        "{\"commandId\":\"cmd-1\",\"type\":\"play_audio_url\",\"audioUrl\":\"http://192.168.0.10:8080/audio/msg.wav\",\"format\":\"wav\",\"volume\":70,\"expiresAt\":\"2026-05-31T23:59:00Z\",\"requestedAt\":\"2026-05-31T23:50:00Z\"}",
        "2026-05-31T23:50:01Z"
    );

    TEST_ASSERT_TRUE(result.ok);
    TEST_ASSERT_EQUAL_STRING("cmd-1", result.command.commandId.c_str());
    TEST_ASSERT_EQUAL(AudioCommandType::PlayAudioUrl, result.command.type);
    TEST_ASSERT_EQUAL(70, result.command.volume);
}

void shouldParsePlayAudioUrlRequestIdAlias() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse(
        "{\"requestId\":\"audio-test-001\",\"type\":\"play_audio_url\",\"audioUrl\":\"http://192.168.0.10:8080/api/audio/messages/test-001.wav\",\"format\":\"wav\",\"volume\":20,\"expiresAt\":\"2026-05-31T23:59:00Z\"}",
        "2026-05-31T23:50:01Z"
    );

    TEST_ASSERT_TRUE(result.ok);
    TEST_ASSERT_EQUAL_STRING("audio-test-001", result.command.commandId.c_str());
    TEST_ASSERT_EQUAL_STRING("http://192.168.0.10:8080/api/audio/messages/test-001.wav", result.command.audioUrl.c_str());
}

void shouldRejectUnsupportedPlayAudioUrlFormat() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse(
        "{\"commandId\":\"cmd-1\",\"type\":\"play_audio_url\",\"audioUrl\":\"http://192.168.0.10:8080/audio/msg.mp3\",\"format\":\"mp3\",\"expiresAt\":\"2026-05-31T23:59:00Z\"}",
        "2026-05-31T23:50:01Z"
    );

    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_STRING("unsupported_audio_format", result.reason.c_str());
}

void shouldRejectAudioCommandWithoutCommandId() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse("{\"type\":\"play_tone\",\"frequencyHz\":880,\"durationMs\":500}", "2026-05-31T23:50:01Z");

    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_STRING("missing_command_id", result.reason.c_str());
}

void shouldRejectUnknownAudioCommandType() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse("{\"commandId\":\"cmd-1\",\"type\":\"dance\"}", "2026-05-31T23:50:01Z");

    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_STRING("unknown_command_type", result.reason.c_str());
}

void shouldRejectAudioVolumeBelowMinimum() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse("{\"commandId\":\"cmd-1\",\"type\":\"set_volume\",\"volume\":-1}", "2026-05-31T23:50:01Z");

    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_STRING("invalid_volume", result.reason.c_str());
}

void shouldRejectAudioVolumeAboveMaximum() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse("{\"commandId\":\"cmd-1\",\"type\":\"set_volume\",\"volume\":101}", "2026-05-31T23:50:01Z");

    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_STRING("invalid_volume", result.reason.c_str());
}

void shouldRejectInvalidAudioJson() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse("{", "2026-05-31T23:50:01Z");

    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_STRING("invalid_json", result.reason.c_str());
}

void shouldRejectInvalidToneFrequency() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse("{\"commandId\":\"cmd-1\",\"type\":\"play_tone\",\"frequencyHz\":10,\"durationMs\":500}", "2026-05-31T23:50:01Z");

    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_STRING("invalid_frequency", result.reason.c_str());
}

void shouldRejectInvalidToneDuration() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse("{\"commandId\":\"cmd-1\",\"type\":\"play_tone\",\"frequencyHz\":880,\"durationMs\":0}", "2026-05-31T23:50:01Z");

    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_STRING("invalid_duration", result.reason.c_str());
}

void shouldRejectHttpsAudioUrl() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse(
        "{\"commandId\":\"cmd-1\",\"type\":\"play_audio_url\",\"audioUrl\":\"https://192.168.0.10/audio.mp3\",\"expiresAt\":\"2026-05-31T23:59:00Z\"}",
        "2026-05-31T23:50:01Z"
    );

    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_STRING("public_or_invalid_audio_url", result.reason.c_str());
}

void shouldAcceptLocalAudioStreamCommand() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse(
        "{\"commandId\":\"cmd-stream\",\"type\":\"play_audio_stream\",\"streamUrl\":\"http://10.0.0.8:8080/audio/stream/session-001\",\"volume\":40,\"expiresAt\":\"2026-05-31T23:59:00Z\"}",
        "2026-05-31T23:50:01Z"
    );

    TEST_ASSERT_TRUE(result.ok);
    TEST_ASSERT_EQUAL(AudioCommandType::PlayAudioStream, result.command.type);
    TEST_ASSERT_EQUAL_STRING("http://10.0.0.8:8080/audio/stream/session-001", result.command.streamUrl.c_str());
}

void shouldRejectStreamWithoutExpiresAt() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse(
        "{\"commandId\":\"cmd-stream\",\"type\":\"play_audio_stream\",\"streamUrl\":\"http://10.0.0.8:8080/audio/stream/session-001\"}",
        "2026-05-31T23:50:01Z"
    );

    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_STRING("missing_expires_at", result.reason.c_str());
}

void shouldRejectPublicAudioUrl() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse(
        "{\"commandId\":\"cmd-1\",\"type\":\"play_audio_url\",\"audioUrl\":\"http://example.com/audio.mp3\",\"expiresAt\":\"2026-05-31T23:59:00Z\"}",
        "2026-05-31T23:50:01Z"
    );

    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_STRING("public_or_invalid_audio_url", result.reason.c_str());
}

void shouldRejectUrlWithoutHost() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse(
        "{\"commandId\":\"cmd-1\",\"type\":\"play_audio_url\",\"audioUrl\":\"http:///audio.mp3\",\"expiresAt\":\"2026-05-31T23:59:00Z\"}",
        "2026-05-31T23:50:01Z"
    );

    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_STRING("public_or_invalid_audio_url", result.reason.c_str());
}

void shouldRejectExpiredAudioCommand() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse(
        "{\"commandId\":\"cmd-1\",\"type\":\"play_audio_url\",\"audioUrl\":\"http://192.168.0.10/audio.mp3\",\"expiresAt\":\"2026-05-31T23:40:00Z\"}",
        "2026-05-31T23:50:01Z"
    );

    TEST_ASSERT_FALSE(result.ok);
    TEST_ASSERT_EQUAL_STRING("expired_command", result.reason.c_str());
}

void shouldAcceptSupportedWavPcmHeader() {
    const uint8_t wav[] = {
        'R','I','F','F', 40,0,0,0, 'W','A','V','E',
        'f','m','t',' ', 16,0,0,0,
        1,0,
        1,0,
        0x80,0x3E,0,0,
        0x00,0x7D,0,0,
        2,0,
        16,0,
        'd','a','t','a', 4,0,0,0,
        0,0, 0,0
    };
    WavPcmInfo info;

    TEST_ASSERT_TRUE(parseWavPcmHeader(wav, sizeof(wav), info));
    TEST_ASSERT_EQUAL(16000, info.sampleRateHz);
    TEST_ASSERT_EQUAL(1, info.channels);
    TEST_ASSERT_EQUAL(16, info.bitsPerSample);
    TEST_ASSERT_EQUAL(4, info.dataBytes);
}

void shouldRejectUnsupportedStereoWavHeader() {
    const uint8_t wav[] = {
        'R','I','F','F', 40,0,0,0, 'W','A','V','E',
        'f','m','t',' ', 16,0,0,0,
        1,0,
        2,0,
        0x80,0x3E,0,0,
        0x00,0xFA,0,0,
        4,0,
        16,0,
        'd','a','t','a', 4,0,0,0,
        0,0, 0,0
    };
    WavPcmInfo info;

    TEST_ASSERT_FALSE(parseWavPcmHeader(wav, sizeof(wav), info));
}

void shouldAcceptStopAudioCommand() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse("{\"commandId\":\"cmd-1\",\"type\":\"stop_audio\",\"requestedAt\":\"2026-05-31T23:50:00Z\"}", "2026-05-31T23:50:01Z");

    TEST_ASSERT_TRUE(result.ok);
    TEST_ASSERT_EQUAL(AudioCommandType::StopAudio, result.command.type);
}

void shouldAcceptSetVolumeCommand() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse("{\"commandId\":\"cmd-1\",\"type\":\"set_volume\",\"volume\":60}", "2026-05-31T23:50:01Z");

    TEST_ASSERT_TRUE(result.ok);
    TEST_ASSERT_EQUAL(AudioCommandType::SetVolume, result.command.type);
    TEST_ASSERT_EQUAL(60, result.command.volume);
}

void shouldAcceptPlayToneCommand() {
    AudioCommandParser parser;
    AudioCommandParseResult result = parser.parse("{\"commandId\":\"cmd-1\",\"type\":\"play_tone\",\"frequencyHz\":880,\"durationMs\":500,\"volume\":40}", "2026-05-31T23:50:01Z");

    TEST_ASSERT_TRUE(result.ok);
    TEST_ASSERT_EQUAL(AudioCommandType::PlayTone, result.command.type);
    TEST_ASSERT_EQUAL(880, result.command.frequencyHz);
}

void shouldEnqueueAudioCommand() {
    AudioPlaybackQueue queue;
    AudioCommand command;
    command.commandId = "cmd-1";

    TEST_ASSERT_TRUE(queue.enqueue(command));
    TEST_ASSERT_EQUAL(1, queue.size());
}

void shouldRejectFullAudioQueue() {
    AudioPlaybackQueue queue;
    AudioCommand command;
    command.commandId = "cmd";

    for (int i = 0; i < AUDIO_QUEUE_CAPACITY; ++i) {
        TEST_ASSERT_TRUE(queue.enqueue(command));
    }

    TEST_ASSERT_FALSE(queue.enqueue(command));
}

void shouldRespectHighPriorityAudioCommand() {
    AudioPlaybackQueue queue;
    AudioCommand normal;
    normal.commandId = "normal";
    normal.priority = AudioPriority::Normal;
    AudioCommand high;
    high.commandId = "high";
    high.priority = AudioPriority::High;

    queue.enqueue(normal);
    queue.enqueue(high);

    AudioCommand next;
    TEST_ASSERT_TRUE(queue.dequeue(next));
    TEST_ASSERT_EQUAL_STRING("high", next.commandId.c_str());
}

void shouldClearAudioQueueOnInterruptFlow() {
    AudioPlaybackQueue queue;
    AudioCommand command;
    command.commandId = "cmd";
    command.interruptCurrent = true;

    queue.enqueue(command);
    queue.clear();

    TEST_ASSERT_TRUE(queue.isEmpty());
}

void shouldPreserveFifoForSamePriorityAudioCommands() {
    AudioPlaybackQueue queue;
    AudioCommand first;
    first.commandId = "first";
    first.priority = AudioPriority::Normal;
    AudioCommand second;
    second.commandId = "second";
    second.priority = AudioPriority::Normal;

    TEST_ASSERT_TRUE(queue.enqueue(first));
    TEST_ASSERT_TRUE(queue.enqueue(second));

    AudioCommand next;
    TEST_ASSERT_TRUE(queue.dequeue(next));
    TEST_ASSERT_EQUAL_STRING("first", next.commandId.c_str());
    TEST_ASSERT_TRUE(queue.dequeue(next));
    TEST_ASSERT_EQUAL_STRING("second", next.commandId.c_str());
}

void shouldNotLoseQueueStateAfterRejectedAudioCommand() {
    AudioPlaybackQueue queue;
    AudioCommand command;
    command.commandId = "cmd";

    for (int i = 0; i < AUDIO_QUEUE_CAPACITY; ++i) {
        TEST_ASSERT_TRUE(queue.enqueue(command));
    }

    TEST_ASSERT_FALSE(queue.enqueue(command));
    TEST_ASSERT_EQUAL(AUDIO_QUEUE_CAPACITY, queue.size());
}

void shouldPublishAcceptedAudioStatus() {
    FakeMqttClient client;
    client.connectedValue = true;
    AudioStatusPublisher publisher(client, "bedroom", "esp32-bedroom-01");

    TEST_ASSERT_TRUE(publisher.publishStatus(
        "cmd-1",
        "accepted",
        "Audio command accepted",
        "2026-05-31T23:50:01Z"
    ));
    TEST_ASSERT_EQUAL_STRING("home/bedroom/esp32-bedroom-01/audio/status", client.lastTopic.c_str());
    assertAudioStatusPayload(
        client.lastPayload,
        "accepted",
        "cmd-1",
        "Audio command accepted",
        "2026-05-31T23:50:01Z"
    );
}

void shouldPublishPlayingAudioStatus() {
    FakeMqttClient client;
    client.connectedValue = true;
    AudioStatusPublisher publisher(client, "bedroom", "esp32-bedroom-01");

    TEST_ASSERT_TRUE(publisher.publishStatus(
        "cmd-1",
        "playing",
        "Audio playback started",
        "2026-05-31T23:50:01Z"
    ));
    TEST_ASSERT_EQUAL_STRING("home/bedroom/esp32-bedroom-01/audio/status", client.lastTopic.c_str());
    assertAudioStatusPayload(
        client.lastPayload,
        "playing",
        "cmd-1",
        "Audio playback started",
        "2026-05-31T23:50:01Z"
    );
}

void shouldPublishCompletedAudioStatus() {
    FakeMqttClient client;
    client.connectedValue = true;
    AudioStatusPublisher publisher(client, "bedroom", "esp32-bedroom-01");

    TEST_ASSERT_TRUE(publisher.publishStatus(
        "cmd-1",
        "completed",
        "Audio playback completed",
        "2026-05-31T23:50:02Z"
    ));
    TEST_ASSERT_EQUAL_STRING("home/bedroom/esp32-bedroom-01/audio/status", client.lastTopic.c_str());
    assertAudioStatusPayload(
        client.lastPayload,
        "completed",
        "cmd-1",
        "Audio playback completed",
        "2026-05-31T23:50:02Z"
    );
}

void shouldPublishStoppedAudioStatus() {
    FakeMqttClient client;
    client.connectedValue = true;
    AudioStatusPublisher publisher(client, "bedroom", "esp32-bedroom-01");

    TEST_ASSERT_TRUE(publisher.publishStatus(
        "cmd-1",
        "stopped",
        "Audio playback stopped",
        "2026-05-31T23:50:03Z"
    ));
    TEST_ASSERT_EQUAL_STRING("home/bedroom/esp32-bedroom-01/audio/status", client.lastTopic.c_str());
    assertAudioStatusPayload(
        client.lastPayload,
        "stopped",
        "cmd-1",
        "Audio playback stopped",
        "2026-05-31T23:50:03Z"
    );
}

void shouldPublishRejectedAudioStatus() {
    FakeMqttClient client;
    client.connectedValue = true;
    AudioStatusPublisher publisher(client, "bedroom", "esp32-bedroom-01");

    TEST_ASSERT_TRUE(publisher.publishStatus(
        "cmd-1",
        "rejected",
        "Command expired",
        "2026-05-31T23:50:01Z",
        "expired_command"
    ));
    TEST_ASSERT_EQUAL_STRING("home/bedroom/esp32-bedroom-01/audio/status", client.lastTopic.c_str());
    assertAudioStatusPayload(
        client.lastPayload,
        "rejected",
        "cmd-1",
        "Command expired",
        "2026-05-31T23:50:01Z",
        "expired_command"
    );
}

void shouldPublishFailedAudioStatus() {
    FakeMqttClient client;
    client.connectedValue = true;
    AudioStatusPublisher publisher(client, "bedroom", "esp32-bedroom-01");

    TEST_ASSERT_TRUE(publisher.publishStatus(
        "cmd-1",
        "failed",
        "Could not open local audio stream",
        "2026-05-31T23:50:01Z",
        "audio_url_unreachable"
    ));
    TEST_ASSERT_EQUAL_STRING("home/bedroom/esp32-bedroom-01/audio/status", client.lastTopic.c_str());
    assertAudioStatusPayload(
        client.lastPayload,
        "failed",
        "cmd-1",
        "Could not open local audio stream",
        "2026-05-31T23:50:01Z",
        "audio_url_unreachable"
    );
}

void shouldKeepI2sDriverQuietUntilPlaybackCommand() {
    FakeMqttClient client;
    client.connectedValue = true;
    AudioStatusPublisher publisher(client, "bedroom", "esp32-bedroom-01");
    AudioPlaybackQueue queue;
    MockAudioOutputDriver driver;
    AudioPlaybackService service(driver, publisher, queue);

    TEST_ASSERT_TRUE(service.begin());
    TEST_ASSERT_EQUAL(0, driver.beginCalls);
    TEST_ASSERT_EQUAL(0, driver.toneCalls);
}

void shouldRunAudioPlaybackFlowWithoutHardware() {
    FakeMqttClient client;
    client.connectedValue = true;
    AudioStatusPublisher publisher(client, "bedroom", "esp32-bedroom-01");
    AudioPlaybackQueue queue;
    MockAudioOutputDriver driver;
    driver.completeOnLoop = true;
    AudioPlaybackService service(driver, publisher, queue);

    TEST_ASSERT_TRUE(service.begin());
    TEST_ASSERT_EQUAL(0, driver.beginCalls);

    AudioCommand command;
    command.commandId = "cmd-tone";
    command.type = AudioCommandType::PlayTone;
    command.frequencyHz = 880;
    command.durationMs = 500;
    command.volume = 40;

    service.submit(command, "2026-05-31T23:50:01Z", 1000);

    TEST_ASSERT_EQUAL(1, driver.beginCalls);
    TEST_ASSERT_EQUAL(1, driver.toneCalls);
    TEST_ASSERT_EQUAL(880, driver.lastFrequencyHz);
    TEST_ASSERT_EQUAL(40, driver.lastVolume);
    TEST_ASSERT_TRUE(client.lastPayload.find("\"state\":\"playing\"") != std::string::npos);

    service.loop("2026-05-31T23:50:02Z", 1600);

    TEST_ASSERT_TRUE(client.lastPayload.find("\"state\":\"completed\"") != std::string::npos);
}

void shouldStartAudioUrlPlaybackThroughDriver() {
    FakeMqttClient client;
    client.connectedValue = true;
    AudioStatusPublisher publisher(client, "bedroom", "esp32-bedroom-01");
    AudioPlaybackQueue queue;
    MockAudioOutputDriver driver;
    AudioPlaybackService service(driver, publisher, queue);

    TEST_ASSERT_TRUE(service.begin());

    AudioCommand command;
    command.commandId = "cmd-url";
    command.type = AudioCommandType::PlayAudioUrl;
    command.audioUrl = "http://192.168.0.10:8080/api/audio/messages/test-001.wav";
    command.format = "wav";
    command.volume = 20;

    service.submit(command, "2026-05-31T23:50:01Z", 1000);

    TEST_ASSERT_EQUAL(1, driver.beginCalls);
    TEST_ASSERT_EQUAL(1, driver.urlCalls);
    TEST_ASSERT_EQUAL_STRING("http://192.168.0.10:8080/api/audio/messages/test-001.wav", driver.lastUrl.c_str());
    TEST_ASSERT_EQUAL(20, driver.lastVolume);
    TEST_ASSERT_TRUE(client.lastPayload.find("\"state\":\"playing\"") != std::string::npos);
}

void shouldAllowSecondAudioUrlAfterFirstCompletesBetweenLoops() {
    FakeMqttClient client;
    client.connectedValue = true;
    AudioStatusPublisher publisher(client, "bedroom", "esp32-bedroom-01");
    AudioPlaybackQueue queue;
    MockAudioOutputDriver driver;
    AudioPlaybackService service(driver, publisher, queue);

    TEST_ASSERT_TRUE(service.begin());

    AudioCommand first;
    first.commandId = "cmd-url-1";
    first.type = AudioCommandType::PlayAudioUrl;
    first.audioUrl = "http://192.168.0.10:8080/api/audio/messages/test-001.wav";
    first.format = "wav";
    first.volume = 20;

    service.submit(first, "2026-05-31T23:50:01Z", 1000);
    TEST_ASSERT_EQUAL(1, driver.urlCalls);

    driver.playing = false;
    service.loop("2026-05-31T23:50:02Z", 2000);
    TEST_ASSERT_TRUE(client.lastPayload.find("\"state\":\"completed\"") != std::string::npos);

    AudioCommand second;
    second.commandId = "cmd-url-2";
    second.type = AudioCommandType::PlayAudioUrl;
    second.audioUrl = "http://192.168.0.10:8080/api/audio/messages/test-002.wav";
    second.format = "wav";
    second.volume = 20;

    service.submit(second, "2026-05-31T23:50:03Z", 3000);

    TEST_ASSERT_EQUAL(2, driver.urlCalls);
    TEST_ASSERT_EQUAL_STRING("http://192.168.0.10:8080/api/audio/messages/test-002.wav", driver.lastUrl.c_str());
    TEST_ASSERT_TRUE(client.lastPayload.find("\"state\":\"playing\"") != std::string::npos);
}

void shouldRejectAudioPlaybackWhenDriverCannotStart() {
    FakeMqttClient client;
    client.connectedValue = true;
    AudioStatusPublisher publisher(client, "bedroom", "esp32-bedroom-01");
    AudioPlaybackQueue queue;
    MockAudioOutputDriver driver;
    driver.startResult = false;
    AudioPlaybackService service(driver, publisher, queue);

    TEST_ASSERT_TRUE(service.begin());

    AudioCommand command;
    command.commandId = "cmd-tone";
    command.type = AudioCommandType::PlayTone;
    command.frequencyHz = 880;
    command.durationMs = 500;

    service.submit(command, "2026-05-31T23:50:01Z", 1000);

    TEST_ASSERT_TRUE(client.lastPayload.find("\"state\":\"failed\"") != std::string::npos);
    TEST_ASSERT_TRUE(client.lastPayload.find("\"reason\":\"audio_playback_unavailable\"") != std::string::npos);
}

void shouldBuildPayloadWithIpAddressWhenValidIpIsProvided() {
    EnvironmentReading reading;
    reading.temperatureCelsius = 24.8f;
    reading.humidityPercentage = 61.2f;
    reading.hasTemperature = true;
    reading.hasHumidity = true;

    std::string payload = buildEnvironmentPayloadJson(
        reading,
        "bedroom",
        "esp32-bedroom-01",
        "2026-06-07T18:30:00Z",
        "192.168.15.42"
    );

    TEST_ASSERT_FALSE(payload.empty());

    JsonDocument document;
    DeserializationError error = deserializeJson(document, payload.c_str());
    TEST_ASSERT_EQUAL(DeserializationError::Ok, error.code());

    TEST_ASSERT_EQUAL_STRING("esp32-bedroom-01", document["deviceId"]);
    TEST_ASSERT_EQUAL_STRING("bedroom", document["room"]);
    TEST_ASSERT_EQUAL_FLOAT(24.8f, document["temperatureCelsius"]);
    TEST_ASSERT_EQUAL_FLOAT(61.2f, document["humidityPercentage"]);
    TEST_ASSERT_EQUAL_STRING("2026-06-07T18:30:00Z", document["measuredAt"]);
    TEST_ASSERT_EQUAL_STRING("192.168.15.42", document["ipAddress"]);
}

void shouldOmitIpAddressWhenEmptyIpIsProvided() {
    EnvironmentReading reading;
    reading.temperatureCelsius = 24.8f;
    reading.humidityPercentage = 61.2f;
    reading.hasTemperature = true;
    reading.hasHumidity = true;

    std::string payload = buildEnvironmentPayloadJson(
        reading,
        "bedroom",
        "esp32-bedroom-01",
        "2026-06-07T18:30:00Z",
        ""
    );

    TEST_ASSERT_FALSE(payload.empty());

    JsonDocument document;
    DeserializationError error = deserializeJson(document, payload.c_str());
    TEST_ASSERT_EQUAL(DeserializationError::Ok, error.code());

    TEST_ASSERT_EQUAL_STRING("esp32-bedroom-01", document["deviceId"]);
    TEST_ASSERT_EQUAL_STRING("bedroom", document["room"]);
    TEST_ASSERT_EQUAL_FLOAT(24.8f, document["temperatureCelsius"]);
    TEST_ASSERT_EQUAL_FLOAT(61.2f, document["humidityPercentage"]);
    TEST_ASSERT_EQUAL_STRING("2026-06-07T18:30:00Z", document["measuredAt"]);
    TEST_ASSERT_TRUE(document["ipAddress"].isNull());
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
    RUN_TEST(shouldAcceptReadingWhenLuminosityIsValid);
    RUN_TEST(shouldRejectReadingWhenLuminosityIsInvalid);
    RUN_TEST(shouldAcceptReadingWhenPresenceIsValid);
    RUN_TEST(shouldAcceptReadingWhenPresenceIsFalse);
    RUN_TEST(shouldBuildOfficialMqttTopicWhenRoomAndDeviceAreProvided);
    RUN_TEST(shouldNotUseDeprecatedMqttTopicFormat);
    RUN_TEST(shouldBuildPayloadWhenTemperatureAndHumidityAreValid);
    RUN_TEST(shouldBuildPayloadWithLuminosityWhenLightReadingIsValid);
    RUN_TEST(shouldOmitLuminosityWhenLightReadingIsInvalid);
    RUN_TEST(shouldBuildPayloadWithPresenceTrueWhenPresenceDetectedIsNotNull);
    RUN_TEST(shouldBuildPayloadWithPresenceFalseWhenPresenceDetectedIsNotNull);
    RUN_TEST(shouldOmitPresenceWhenPresenceIsAbsent);
    RUN_TEST(shouldOmitTemperatureWhenTemperatureIsInvalid);
    RUN_TEST(shouldOmitHumidityWhenHumidityIsInvalid);
    RUN_TEST(shouldNotBuildPayloadWhenNoSensorValueIsValid);
    RUN_TEST(shouldRejectPublishWhenMeasuredAtIsEmpty);
    RUN_TEST(shouldIncludeMeasuredAtWhenTimestampIsAvailable);
    RUN_TEST(shouldPublishWhenReadingAndTimestampAreValid);
    RUN_TEST(shouldNotPublishWhenReadingIsInvalid);
    RUN_TEST(shouldNotPublishWhenTimestampIsEmpty);
    RUN_TEST(shouldBuildPayloadWithIpAddressWhenValidIpIsProvided);
    RUN_TEST(shouldOmitIpAddressWhenEmptyIpIsProvided);
    RUN_TEST(shouldPublishToOfficialEnvironmentTopic);
    RUN_TEST(shouldBuildAudioCommandAndStatusTopics);
    RUN_TEST(shouldSubscribeAudioCommandTopicAfterMqttConnect);
    RUN_TEST(shouldParseValidPlayAudioUrlCommand);
    RUN_TEST(shouldParsePlayAudioUrlRequestIdAlias);
    RUN_TEST(shouldRejectUnsupportedPlayAudioUrlFormat);
    RUN_TEST(shouldRejectAudioCommandWithoutCommandId);
    RUN_TEST(shouldRejectUnknownAudioCommandType);
    RUN_TEST(shouldRejectAudioVolumeBelowMinimum);
    RUN_TEST(shouldRejectAudioVolumeAboveMaximum);
    RUN_TEST(shouldRejectInvalidAudioJson);
    RUN_TEST(shouldRejectInvalidToneFrequency);
    RUN_TEST(shouldRejectInvalidToneDuration);
    RUN_TEST(shouldRejectHttpsAudioUrl);
    RUN_TEST(shouldAcceptLocalAudioStreamCommand);
    RUN_TEST(shouldRejectStreamWithoutExpiresAt);
    RUN_TEST(shouldRejectPublicAudioUrl);
    RUN_TEST(shouldRejectUrlWithoutHost);
    RUN_TEST(shouldRejectExpiredAudioCommand);
    RUN_TEST(shouldAcceptSupportedWavPcmHeader);
    RUN_TEST(shouldRejectUnsupportedStereoWavHeader);
    RUN_TEST(shouldAcceptStopAudioCommand);
    RUN_TEST(shouldAcceptSetVolumeCommand);
    RUN_TEST(shouldAcceptPlayToneCommand);
    RUN_TEST(shouldEnqueueAudioCommand);
    RUN_TEST(shouldRejectFullAudioQueue);
    RUN_TEST(shouldRespectHighPriorityAudioCommand);
    RUN_TEST(shouldClearAudioQueueOnInterruptFlow);
    RUN_TEST(shouldPreserveFifoForSamePriorityAudioCommands);
    RUN_TEST(shouldNotLoseQueueStateAfterRejectedAudioCommand);
    RUN_TEST(shouldPublishAcceptedAudioStatus);
    RUN_TEST(shouldPublishPlayingAudioStatus);
    RUN_TEST(shouldPublishCompletedAudioStatus);
    RUN_TEST(shouldPublishStoppedAudioStatus);
    RUN_TEST(shouldPublishRejectedAudioStatus);
    RUN_TEST(shouldPublishFailedAudioStatus);
    RUN_TEST(shouldKeepI2sDriverQuietUntilPlaybackCommand);
    RUN_TEST(shouldRunAudioPlaybackFlowWithoutHardware);
    RUN_TEST(shouldStartAudioUrlPlaybackThroughDriver);
    RUN_TEST(shouldAllowSecondAudioUrlAfterFirstCompletesBetweenLoops);
    RUN_TEST(shouldRejectAudioPlaybackWhenDriverCannotStart);
    RUN_TEST(shouldBeCollisionFreeAndSafe);
    return UNITY_END();
}
