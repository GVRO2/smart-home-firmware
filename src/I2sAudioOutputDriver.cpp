#include "I2sAudioOutputDriver.h"

#include "AppConfig.h"
#include "WavPcmAudio.h"

#ifdef ARDUINO
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <cmath>
#include <driver/i2s.h>
#endif

namespace {
constexpr int SAMPLE_RATE = 22050;
constexpr int I2S_PORT = 0;
constexpr uint32_t WAV_MAX_DATA_BYTES = 512UL * 1024UL;
constexpr uint32_t HTTP_TIMEOUT_MS = 10000;

#ifdef ARDUINO
bool readExact(WiFiClient& stream, uint8_t* buffer, size_t length, volatile bool& stopRequested) {
    size_t offset = 0;
    unsigned long lastProgressMs = millis();
    while (offset < length && !stopRequested) {
        int available = stream.available();
        if (available <= 0) {
            if (!stream.connected()) {
                return false;
            }
            if (millis() - lastProgressMs > HTTP_TIMEOUT_MS) {
                return false;
            }
            delay(1);
            continue;
        }
        int toRead = static_cast<int>(length - offset);
        if (toRead > available) {
            toRead = available;
        }
        int read = stream.read(buffer + offset, toRead);
        if (read <= 0) {
            continue;
        }
        offset += static_cast<size_t>(read);
        lastProgressMs = millis();
    }
    return offset == length;
}

bool readWavHeaderFromStream(WiFiClient& stream, WavPcmInfo& info, volatile bool& stopRequested) {
    uint8_t riff[12];
    if (!readExact(stream, riff, sizeof(riff), stopRequested)) {
        return false;
    }
    if (!wavTagEquals(riff, "RIFF") || !wavTagEquals(riff + 8, "WAVE")) {
        return false;
    }

    bool foundFmt = false;
    while (!stopRequested) {
        uint8_t chunkHeader[8];
        if (!readExact(stream, chunkHeader, sizeof(chunkHeader), stopRequested)) {
            return false;
        }
        uint32_t chunkSize = readLe32(chunkHeader + 4);
        if (wavTagEquals(chunkHeader, "fmt ")) {
            if (chunkSize < 16 || chunkSize > 128) {
                return false;
            }
            uint8_t fmt[128] = {};
            if (!readExact(stream, fmt, chunkSize, stopRequested)) {
                return false;
            }
            info.audioFormat = readLe16(fmt);
            info.channels = readLe16(fmt + 2);
            info.sampleRateHz = readLe32(fmt + 4);
            info.bitsPerSample = readLe16(fmt + 14);
            foundFmt = true;
        } else if (wavTagEquals(chunkHeader, "data")) {
            if (!foundFmt) {
                return false;
            }
            info.dataBytes = chunkSize;
            info.dataOffset = 0;
            return info.audioFormat == 1
                && info.channels == 1
                && info.bitsPerSample == 16
                && info.sampleRateHz > 0
                && info.dataBytes > 0
                && info.dataBytes <= WAV_MAX_DATA_BYTES;
        } else {
            uint8_t discard[64];
            uint32_t remaining = chunkSize;
            while (remaining > 0 && !stopRequested) {
                size_t step = remaining > sizeof(discard) ? sizeof(discard) : remaining;
                if (!readExact(stream, discard, step, stopRequested)) {
                    return false;
                }
                remaining -= static_cast<uint32_t>(step);
            }
        }

        if (chunkSize % 2 == 1) {
            uint8_t pad = 0;
            if (!readExact(stream, &pad, 1, stopRequested)) {
                return false;
            }
        }
    }
    return false;
}
#endif
}

bool I2sAudioOutputDriver::begin() {
#ifdef ARDUINO
    i2s_config_t config = {};
    config.mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_TX);
    config.sample_rate = SAMPLE_RATE;
    config.bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT;
    config.channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT;
    config.communication_format = I2S_COMM_FORMAT_STAND_I2S;
    config.intr_alloc_flags = 0;
    config.dma_buf_count = 4;
    config.dma_buf_len = 128;
    config.use_apll = false;
    config.tx_desc_auto_clear = true;
    config.fixed_mclk = 0;

    i2s_pin_config_t pins = {};
    pins.bck_io_num = AUDIO_I2S_BCLK_PIN;
    pins.ws_io_num = AUDIO_I2S_LRC_PIN;
    pins.data_out_num = AUDIO_I2S_DIN_PIN;
    pins.data_in_num = I2S_PIN_NO_CHANGE;

    esp_err_t installResult = i2s_driver_install(static_cast<i2s_port_t>(I2S_PORT), &config, 0, nullptr);
    esp_err_t pinResult = i2s_set_pin(static_cast<i2s_port_t>(I2S_PORT), &pins);
    initialized_ = installResult == ESP_OK && pinResult == ESP_OK;
    Serial.print("[AUDIO] i2s_driver_install=");
    Serial.print(installResult == ESP_OK ? "success" : "failed");
    Serial.print(" i2s_set_pin=");
    Serial.println(pinResult == ESP_OK ? "success" : "failed");
    return initialized_;
#else
    initialized_ = true;
    return true;
#endif
}

bool I2sAudioOutputDriver::startTone(int frequencyHz, unsigned long durationMs, int volume, unsigned long nowMs) {
    if (!initialized_) {
        return false;
    }
    stopRequested_ = false;
#ifdef ARDUINO
    i2s_set_sample_rates(static_cast<i2s_port_t>(I2S_PORT), SAMPLE_RATE);
#endif
    setVolume(volume);
    frequencyHz_ = frequencyHz;
    toneEndsAtMs_ = nowMs + durationMs;
    sampleIndex_ = 0;
    playingTone_ = true;
    Serial.print("[AUDIO] tone_start frequencyHz=");
    Serial.print(frequencyHz_);
    Serial.print(" durationMs=");
    Serial.print(durationMs);
    Serial.print(" volume=");
    Serial.println(volume_);
    return true;
}

bool I2sAudioOutputDriver::startUrl(const std::string& url, const std::string& format, int volume, unsigned long maxDurationMs, unsigned long nowMs) {
    if (!initialized_ || playingTone_ || playingUrl_) {
        return false;
    }
    if (!format.empty() && format != "wav") {
#ifdef ARDUINO
        Serial.println("[AUDIO] playback=failed reason=UNSUPPORTED_AUDIO_FORMAT");
#endif
        return false;
    }

    setVolume(volume);
    playbackUrl_ = url;
    playbackFormat_ = format.empty() ? "wav" : format;
    playbackMaxDurationMs_ = maxDurationMs;
    playbackStartedAtMs_ = nowMs;
    stopRequested_ = false;
    playingUrl_ = true;

#ifdef ARDUINO
    BaseType_t result = xTaskCreatePinnedToCore(
        playbackTaskEntry,
        "audio-url-playback",
        8192,
        this,
        1,
        reinterpret_cast<TaskHandle_t*>(&playbackTaskHandle_),
        1
    );
    if (result != pdPASS) {
        playingUrl_ = false;
        playbackTaskHandle_ = nullptr;
        Serial.println("[AUDIO] playback=failed reason=PLAYBACK_TASK_CREATE_FAILED");
        return false;
    }
    return true;
#else
    playingUrl_ = false;
    return false;
#endif
}

bool I2sAudioOutputDriver::startStream(const std::string&, const std::string&, int, unsigned long, unsigned long) {
    return false;
}

void I2sAudioOutputDriver::stop() {
    playingTone_ = false;
    stopRequested_ = true;
#ifdef ARDUINO
    if (initialized_) {
        i2s_zero_dma_buffer(static_cast<i2s_port_t>(I2S_PORT));
    }
#endif
}

void I2sAudioOutputDriver::setVolume(int volume) {
    if (volume < 0) {
        return;
    }
    if (volume > AUDIO_MAX_VOLUME) {
        volume_ = AUDIO_MAX_VOLUME;
        return;
    }
    volume_ = volume;
}

void I2sAudioOutputDriver::loop(unsigned long nowMs) {
    if (!playingTone_) {
        return;
    }
    if (nowMs >= toneEndsAtMs_) {
        stop();
        return;
    }

#ifdef ARDUINO
    int16_t samples[256];
    float amplitude = 3000.0f * (static_cast<float>(volume_) / 100.0f);
    for (int i = 0; i < 128; ++i) {
        float angle = 2.0f * PI * static_cast<float>(frequencyHz_) * static_cast<float>(sampleIndex_++) / static_cast<float>(SAMPLE_RATE);
        int16_t sample = static_cast<int16_t>(std::sin(angle) * amplitude);
        samples[i * 2] = sample;
        samples[i * 2 + 1] = sample;
    }
    size_t written = 0;
    i2s_write(static_cast<i2s_port_t>(I2S_PORT), samples, sizeof(samples), &written, pdMS_TO_TICKS(20));
#endif
}

bool I2sAudioOutputDriver::isPlaying() const {
    return playingTone_ || playingUrl_;
}

void I2sAudioOutputDriver::playbackTaskEntry(void* context) {
#ifdef ARDUINO
    I2sAudioOutputDriver* driver = static_cast<I2sAudioOutputDriver*>(context);
    HTTPClient http;
    uint32_t totalBytesWritten = 0;

    Serial.print("[AUDIO] http_get=start url=");
    Serial.println(driver->playbackUrl_.c_str());

    http.setTimeout(HTTP_TIMEOUT_MS);
    if (!http.begin(driver->playbackUrl_.c_str())) {
        Serial.println("[AUDIO] playback=failed reason=HTTP_BEGIN_FAILED");
        driver->playingUrl_ = false;
        driver->playbackTaskHandle_ = nullptr;
        vTaskDelete(nullptr);
        return;
    }

    int statusCode = http.GET();
    Serial.print("[AUDIO] http_get=status code=");
    Serial.println(statusCode);
    if (statusCode != HTTP_CODE_OK) {
        Serial.print("[AUDIO] playback=failed reason=HTTP_STATUS_NOT_200 status=");
        Serial.println(statusCode);
        http.end();
        driver->playingUrl_ = false;
        driver->playbackTaskHandle_ = nullptr;
        vTaskDelete(nullptr);
        return;
    }

    WiFiClient* stream = http.getStreamPtr();
    WavPcmInfo wav;
    if (stream == nullptr || !readWavHeaderFromStream(*stream, wav, driver->stopRequested_)) {
        Serial.println("[AUDIO] playback=failed reason=UNSUPPORTED_WAV_FORMAT");
        http.end();
        driver->playingUrl_ = false;
        driver->playbackTaskHandle_ = nullptr;
        vTaskDelete(nullptr);
        return;
    }

    Serial.print("[AUDIO] wav_header=valid sampleRateHz=");
    Serial.print(wav.sampleRateHz);
    Serial.print(" channels=");
    Serial.print(wav.channels);
    Serial.print(" bitsPerSample=");
    Serial.print(wav.bitsPerSample);
    Serial.print(" dataBytes=");
    Serial.println(wav.dataBytes);

    esp_err_t rateResult = i2s_set_sample_rates(static_cast<i2s_port_t>(I2S_PORT), wav.sampleRateHz);
    if (rateResult != ESP_OK) {
        Serial.println("[AUDIO] playback=failed reason=I2S_SET_SAMPLE_RATE_FAILED");
        http.end();
        driver->playingUrl_ = false;
        driver->playbackTaskHandle_ = nullptr;
        vTaskDelete(nullptr);
        return;
    }

    Serial.println("[AUDIO] playback=start format=wav pcm_s16le");
    uint8_t input[512];
    int16_t output[512];
    uint32_t remaining = wav.dataBytes;
    const unsigned long startedAt = millis();

    while (remaining > 0 && !driver->stopRequested_) {
        if (driver->playbackMaxDurationMs_ > 0 && millis() - startedAt > driver->playbackMaxDurationMs_) {
            Serial.println("[AUDIO] playback=failed reason=MAX_DURATION_EXCEEDED");
            break;
        }

        size_t bytesToRead = remaining > sizeof(input) ? sizeof(input) : remaining;
        if (bytesToRead % 2 == 1) {
            --bytesToRead;
        }
        if (bytesToRead == 0 || !readExact(*stream, input, bytesToRead, driver->stopRequested_)) {
            Serial.println("[AUDIO] playback=failed reason=HTTP_STREAM_READ_FAILED");
            break;
        }

        size_t sampleCount = bytesToRead / 2;
        float gain = static_cast<float>(driver->volume_) / 100.0f;
        for (size_t i = 0; i < sampleCount; ++i) {
            int16_t sample = static_cast<int16_t>(readLe16(input + (i * 2)));
            int32_t scaled = static_cast<int32_t>(static_cast<float>(sample) * gain);
            output[i * 2] = static_cast<int16_t>(scaled);
            output[i * 2 + 1] = static_cast<int16_t>(scaled);
        }

        size_t written = 0;
        esp_err_t writeResult = i2s_write(
            static_cast<i2s_port_t>(I2S_PORT),
            output,
            sampleCount * 2 * sizeof(int16_t),
            &written,
            pdMS_TO_TICKS(100)
        );
        if (writeResult != ESP_OK || written == 0) {
            Serial.println("[AUDIO] playback=failed reason=I2S_WRITE_FAILED");
            break;
        }

        totalBytesWritten += static_cast<uint32_t>(written);
        remaining -= static_cast<uint32_t>(bytesToRead);
    }

    if (driver->stopRequested_) {
        Serial.println("[AUDIO] playback=stopped");
    } else if (remaining == 0) {
        Serial.print("[AUDIO] playback=completed bytesWritten=");
        Serial.println(totalBytesWritten);
    }

    i2s_zero_dma_buffer(static_cast<i2s_port_t>(I2S_PORT));
    http.end();
    driver->playingUrl_ = false;
    driver->playbackTaskHandle_ = nullptr;
    vTaskDelete(nullptr);
#else
    (void)context;
#endif
}
