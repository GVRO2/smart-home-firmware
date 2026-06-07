#pragma once

#include <string>

enum class AudioCommandType {
    PlayAudioUrl,
    PlayAudioStream,
    PlayTone,
    StopAudio,
    SetVolume,
    Unknown
};

enum class AudioPriority {
    Low,
    Normal,
    High
};

struct AudioCommand {
    std::string commandId;
    AudioCommandType type = AudioCommandType::Unknown;
    std::string source;
    std::string audioUrl;
    std::string streamUrl;
    std::string format;
    AudioPriority priority = AudioPriority::Normal;
    int volume = -1;
    bool interruptCurrent = false;
    std::string expiresAt;
    std::string requestedAt;
    int frequencyHz = 0;
    unsigned long durationMs = 0;
};

struct AudioCommandParseResult {
    bool ok = false;
    AudioCommand command;
    std::string reason;
    std::string message;
};

inline const char* audioCommandTypeToString(AudioCommandType type) {
    switch (type) {
        case AudioCommandType::PlayAudioUrl:
            return "play_audio_url";
        case AudioCommandType::PlayAudioStream:
            return "play_audio_stream";
        case AudioCommandType::PlayTone:
            return "play_tone";
        case AudioCommandType::StopAudio:
            return "stop_audio";
        case AudioCommandType::SetVolume:
            return "set_volume";
        default:
            return "unknown";
    }
}
