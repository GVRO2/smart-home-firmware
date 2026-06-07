#pragma once

#include "AudioOutputDriver.h"

class I2sAudioOutputDriver final : public AudioOutputDriver {
public:
    bool begin() override;
    bool startTone(int frequencyHz, unsigned long durationMs, int volume, unsigned long nowMs) override;
    bool startUrl(const std::string& url, const std::string& format, int volume, unsigned long maxDurationMs, unsigned long nowMs) override;
    bool startStream(const std::string& url, const std::string& format, int volume, unsigned long maxDurationMs, unsigned long nowMs) override;
    void stop() override;
    void setVolume(int volume) override;
    void loop(unsigned long nowMs) override;
    bool isPlaying() const override;

private:
    static void playbackTaskEntry(void* context);

    bool initialized_ = false;
    bool playingTone_ = false;
    volatile bool playingUrl_ = false;
    volatile bool stopRequested_ = false;
    void* playbackTaskHandle_ = nullptr;
    int volume_ = 60;
    int frequencyHz_ = 0;
    unsigned long toneEndsAtMs_ = 0;
    unsigned long sampleIndex_ = 0;
    std::string playbackUrl_;
    std::string playbackFormat_;
    unsigned long playbackMaxDurationMs_ = 0;
    unsigned long playbackStartedAtMs_ = 0;
};
