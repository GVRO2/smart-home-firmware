#pragma once

constexpr const char* ROOM_SLUG = "bedroom";
constexpr const char* DEVICE_EXTERNAL_ID = "esp32-bedroom-01";

constexpr unsigned long PUBLISH_INTERVAL_MS = 5000;
// Para uso real, prefira 30000 ms para reduzir tráfego e leituras desnecessárias.

constexpr int DHT_PIN = 4;
constexpr int DHT_TYPE = 22;

constexpr int I2C_SDA_PIN = 21;
constexpr int I2C_SCL_PIN = 22;
constexpr uint8_t BH1750_I2C_ADDRESS = 0x23;
