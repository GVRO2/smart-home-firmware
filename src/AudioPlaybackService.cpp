#include "AudioPlaybackService.h"

#include "AppConfig.h"

AudioPlaybackService::AudioPlaybackService(
    AudioOutputDriver& driver,
    AudioStatusPublisher& statusPublisher,
    AudioPlaybackQueue& queue
)
    : driver_(driver), statusPublisher_(statusPublisher), queue_(queue) {
}

bool AudioPlaybackService::begin() {
    acceptingCommands_ = AUDIO_PLAYBACK_ENABLED;
    return acceptingCommands_;
}

void AudioPlaybackService::submit(const AudioCommand& command, const std::string& nowUtc, unsigned long nowMs) {
    if (!AUDIO_PLAYBACK_ENABLED || !acceptingCommands_) {
        publishFailed(command, "audio_disabled", "Audio playback is not initialized", nowUtc);
        return;
    }

    if (command.type == AudioCommandType::StopAudio) {
        driver_.stop();
        queue_.clear();
        hasCurrent_ = false;
        statusPublisher_.publishStatus(command.commandId, "accepted", "Audio stop command accepted", nowUtc);
        statusPublisher_.publishStatus(command.commandId, "completed", "Audio playback stopped", nowUtc);
        return;
    }

    if (command.type == AudioCommandType::SetVolume) {
        driver_.setVolume(command.volume < 0 ? AUDIO_DEFAULT_VOLUME : command.volume);
        statusPublisher_.publishStatus(command.commandId, "accepted", "Volume command accepted", nowUtc);
        statusPublisher_.publishStatus(command.commandId, "completed", "Volume updated", nowUtc);
        return;
    }

    if (command.interruptCurrent) {
        driver_.stop();
        queue_.clear();
        hasCurrent_ = false;
    }

    if (!queue_.enqueue(command)) {
        publishFailed(command, "queue_full", "Audio playback queue is full", nowUtc);
        return;
    }

    publishAccepted(command, nowUtc);
    startNext(nowUtc, nowMs);
}

bool AudioPlaybackService::ensureDriverReady() {
    if (driverReady_) {
        return true;
    }

    driverReady_ = driver_.begin();
    return driverReady_;
}

void AudioPlaybackService::reject(const std::string& commandId, const char* reason, const char* message, const std::string& nowUtc) {
    statusPublisher_.publishStatus(commandId.empty() ? "unknown" : commandId, "rejected", message, nowUtc, reason);
}

void AudioPlaybackService::loop(const std::string& nowUtc, unsigned long nowMs) {
    driver_.loop(nowMs);
    if (hasCurrent_ && !driver_.isPlaying()) {
        statusPublisher_.publishStatus(current_.commandId, "completed", "Audio playback completed", nowUtc);
        hasCurrent_ = false;
    }
    startNext(nowUtc, nowMs);
}

void AudioPlaybackService::startNext(const std::string& nowUtc, unsigned long nowMs) {
    if (driver_.isPlaying() || hasCurrent_) {
        return;
    }

    AudioCommand next;
    if (!queue_.dequeue(next)) {
        return;
    }

    bool started = false;
    if (next.type == AudioCommandType::PlayTone) {
        started = ensureDriverReady()
            && driver_.startTone(next.frequencyHz, next.durationMs, next.volume < 0 ? AUDIO_DEFAULT_VOLUME : next.volume, nowMs);
    } else if (next.type == AudioCommandType::PlayAudioUrl) {
        started = ensureDriverReady()
            && driver_.startUrl(next.audioUrl, next.format, next.volume < 0 ? AUDIO_DEFAULT_VOLUME : next.volume, AUDIO_MAX_DURATION_MS, nowMs);
    } else if (next.type == AudioCommandType::PlayAudioStream) {
        started = ensureDriverReady()
            && driver_.startStream(next.streamUrl, next.format, next.volume < 0 ? AUDIO_DEFAULT_VOLUME : next.volume, AUDIO_MAX_DURATION_MS, nowMs);
    }

    if (!started) {
        publishFailed(next, "audio_playback_unavailable", "Audio output driver could not start this command", nowUtc);
        return;
    }

    current_ = next;
    hasCurrent_ = true;
    statusPublisher_.publishStatus(next.commandId, "playing", "Audio playback started", nowUtc);
}

void AudioPlaybackService::publishAccepted(const AudioCommand& command, const std::string& nowUtc) {
    statusPublisher_.publishStatus(command.commandId, "accepted", "Audio command accepted", nowUtc);
}

void AudioPlaybackService::publishFailed(const AudioCommand& command, const char* reason, const char* message, const std::string& nowUtc) {
    statusPublisher_.publishStatus(command.commandId, "failed", message, nowUtc, reason);
}
