#pragma once

#include <string>

class AudioOutputDriver {
public:
    virtual ~AudioOutputDriver() = default;

    virtual bool begin() = 0;
    virtual bool startTone(int frequencyHz, unsigned long durationMs, int volume, unsigned long nowMs) = 0;
    virtual bool startUrl(const std::string& url, const std::string& format, int volume, unsigned long maxDurationMs, unsigned long nowMs) = 0;
    virtual bool startStream(const std::string& url, const std::string& format, int volume, unsigned long maxDurationMs, unsigned long nowMs) = 0;
    virtual void stop() = 0;
    virtual void setVolume(int volume) = 0;
    virtual void loop(unsigned long nowMs) = 0;
    virtual bool isPlaying() const = 0;
};
