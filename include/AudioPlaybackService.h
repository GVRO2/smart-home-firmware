#pragma once

#include "AudioOutputDriver.h"
#include "AudioPlaybackQueue.h"
#include "AudioStatusPublisher.h"

class AudioPlaybackService {
public:
    AudioPlaybackService(AudioOutputDriver& driver, AudioStatusPublisher& statusPublisher, AudioPlaybackQueue& queue);

    bool begin();
    void submit(const AudioCommand& command, const std::string& nowUtc, unsigned long nowMs);
    void reject(const std::string& commandId, const char* reason, const char* message, const std::string& nowUtc);
    void loop(const std::string& nowUtc, unsigned long nowMs);

private:
    bool ensureDriverReady();
    void startNext(const std::string& nowUtc, unsigned long nowMs);
    void publishAccepted(const AudioCommand& command, const std::string& nowUtc);
    void publishFailed(const AudioCommand& command, const char* reason, const char* message, const std::string& nowUtc);

    AudioOutputDriver& driver_;
    AudioStatusPublisher& statusPublisher_;
    AudioPlaybackQueue& queue_;
    AudioCommand current_;
    bool acceptingCommands_ = false;
    bool driverReady_ = false;
    bool hasCurrent_ = false;
};
