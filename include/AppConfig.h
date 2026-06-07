#pragma once

#include <stdint.h>

constexpr const char *ROOM_SLUG = "bedroom";
constexpr const char *DEVICE_EXTERNAL_ID = "esp32-bedroom-01";

constexpr unsigned long PUBLISH_INTERVAL_MS = 5000;
constexpr uint16_t MQTT_PACKET_BUFFER_SIZE = 1024;
// Para uso real, prefira 30000 ms para reduzir tráfego e leituras
// desnecessárias.

constexpr int HEARTBEAT_LED_PIN = 2;
constexpr unsigned long HEARTBEAT_INTERVAL_MS = 1000;

constexpr int DHT_PIN = 4;
constexpr int DHT_TYPE = 22;

constexpr int I2C_SDA_PIN = 21;
constexpr int I2C_SCL_PIN = 22;
constexpr uint8_t BH1750_I2C_ADDRESS = 0x23;

constexpr bool AUDIO_PLAYBACK_ENABLED = true;
constexpr int AUDIO_I2S_BCLK_PIN = 26;
constexpr int AUDIO_I2S_LRC_PIN = 25;
constexpr int AUDIO_I2S_DIN_PIN = 33;
constexpr int PRESENCE_SENSOR_PIN = 27;
constexpr int SOUND_SENSOR_PIN = 34;
constexpr int AUDIO_DEFAULT_VOLUME = 60;
constexpr int AUDIO_MAX_VOLUME = 100;
constexpr unsigned long AUDIO_MAX_DURATION_MS = 30000;
constexpr int AUDIO_QUEUE_CAPACITY = 4;
