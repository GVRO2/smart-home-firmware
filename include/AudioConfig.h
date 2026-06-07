#pragma once

#include <string>

#include "AppConfig.h"

constexpr const char* AUDIO_COMMAND_TOPIC_SUFFIX = "audio/command";
constexpr const char* AUDIO_STATUS_TOPIC_SUFFIX = "audio/status";

inline std::string buildAudioCommandTopic(const char* roomSlug, const char* deviceExternalId) {
    return std::string("home/") + roomSlug + "/" + deviceExternalId + "/" + AUDIO_COMMAND_TOPIC_SUFFIX;
}

inline std::string buildAudioStatusTopic(const char* roomSlug, const char* deviceExternalId) {
    return std::string("home/") + roomSlug + "/" + deviceExternalId + "/" + AUDIO_STATUS_TOPIC_SUFFIX;
}
