#pragma once

#include <string>

#include "AudioOutputDriver.h"

class MockAudioOutputDriver final : public AudioOutputDriver {
public:
    bool beginResult = true;
    bool startResult = true;
    bool playing = false;
    bool completeOnLoop = false;
    int beginCalls = 0;
    int toneCalls = 0;
    int urlCalls = 0;
    int streamCalls = 0;
    int stopCalls = 0;
    int setVolumeCalls = 0;
    int lastFrequencyHz = 0;
    unsigned long lastDurationMs = 0;
    int lastVolume = -1;
    std::string lastUrl;

    bool begin() override {
        ++beginCalls;
        return beginResult;
    }

    bool startTone(int frequencyHz, unsigned long durationMs, int volume, unsigned long) override {
        ++toneCalls;
        lastFrequencyHz = frequencyHz;
        lastDurationMs = durationMs;
        lastVolume = volume;
        playing = startResult;
        return startResult;
    }

    bool startUrl(const std::string& url, const std::string&, int volume, unsigned long, unsigned long) override {
        ++urlCalls;
        lastUrl = url;
        lastVolume = volume;
        playing = startResult;
        return startResult;
    }

    bool startStream(const std::string& url, const std::string&, int volume, unsigned long, unsigned long) override {
        ++streamCalls;
        lastUrl = url;
        lastVolume = volume;
        playing = startResult;
        return startResult;
    }

    void stop() override {
        ++stopCalls;
        playing = false;
    }

    void setVolume(int volume) override {
        ++setVolumeCalls;
        lastVolume = volume;
    }

    void loop(unsigned long) override {
        if (completeOnLoop) {
            playing = false;
        }
    }

    bool isPlaying() const override {
        return playing;
    }
};
