#include "AudioCommandParser.h"

#include <ArduinoJson.h>
#include <cstdlib>

#include "AppConfig.h"

AudioCommandParseResult AudioCommandParser::parse(const std::string& payload, const std::string& nowUtc) const {
    JsonDocument document;
    DeserializationError error = deserializeJson(document, payload.c_str());
    if (error) {
        return reject("invalid_json", "Audio command payload is not valid JSON");
    }

    const char* commandId = document["commandId"] | document["requestId"] | "";
    if (commandId[0] == '\0') {
        return reject("missing_command_id", "Audio command must include commandId");
    }

    const char* typeText = document["type"] | "";
    AudioCommandType type = parseType(typeText);
    if (type == AudioCommandType::Unknown) {
        return reject("unknown_command_type", "Audio command type is not supported");
    }

    AudioCommand command;
    command.commandId = commandId;
    command.type = type;
    command.source = std::string(document["source"] | "");
    command.format = std::string(document["format"] | "");
    command.priority = parsePriority(document["priority"] | "normal");
    command.interruptCurrent = document["interruptCurrent"] | false;
    command.requestedAt = std::string(document["requestedAt"] | "");
    command.expiresAt = std::string(document["expiresAt"] | "");
    bool volumeProvided = !document["volume"].isNull();
    command.volume = volumeProvided ? document["volume"].as<int>() : -1;
    command.frequencyHz = document["frequencyHz"] | 0;
    command.durationMs = document["durationMs"] | 0UL;

    if (volumeProvided && (command.volume < 0 || command.volume > AUDIO_MAX_VOLUME)) {
        return reject("invalid_volume", "Audio volume must be between 0 and 100");
    }

    if (!command.expiresAt.empty() && isExpired(command.expiresAt, nowUtc)) {
        return reject("expired_command", "Command expired before playback");
    }

    if (type == AudioCommandType::PlayAudioUrl) {
        command.audioUrl = std::string(document["audioUrl"] | "");
        if (command.audioUrl.empty()) {
            return reject("missing_audio_url", "play_audio_url requires audioUrl");
        }
        if (!command.format.empty() && command.format != "wav") {
            return reject("unsupported_audio_format", "play_audio_url supports wav in this firmware version");
        }
        if (command.expiresAt.empty()) {
            return reject("missing_expires_at", "play_audio_url requires expiresAt");
        }
        if (!isLocalHttpUrl(command.audioUrl)) {
            return reject("public_or_invalid_audio_url", "Audio URL must be local/private http");
        }
    }

    if (type == AudioCommandType::PlayAudioStream) {
        command.streamUrl = std::string(document["streamUrl"] | "");
        if (command.streamUrl.empty()) {
            return reject("missing_stream_url", "play_audio_stream requires streamUrl");
        }
        if (command.expiresAt.empty()) {
            return reject("missing_expires_at", "play_audio_stream requires expiresAt");
        }
        if (!isLocalHttpUrl(command.streamUrl)) {
            return reject("public_or_invalid_stream_url", "Audio stream URL must be local/private http");
        }
    }

    if (type == AudioCommandType::PlayTone) {
        if (command.frequencyHz < 20 || command.frequencyHz > 20000) {
            return reject("invalid_frequency", "play_tone requires frequencyHz between 20 and 20000");
        }
        if (command.durationMs == 0 || command.durationMs > AUDIO_MAX_DURATION_MS) {
            return reject("invalid_duration", "play_tone duration must be within max duration");
        }
    }

    AudioCommandParseResult result;
    result.ok = true;
    result.command = command;
    result.message = "Audio command accepted";
    return result;
}

AudioCommandType AudioCommandParser::parseType(const char* type) const {
    std::string value = type == nullptr ? "" : type;
    if (value == "play_audio_url") {
        return AudioCommandType::PlayAudioUrl;
    }
    if (value == "play_audio_stream") {
        return AudioCommandType::PlayAudioStream;
    }
    if (value == "play_tone") {
        return AudioCommandType::PlayTone;
    }
    if (value == "stop_audio") {
        return AudioCommandType::StopAudio;
    }
    if (value == "set_volume") {
        return AudioCommandType::SetVolume;
    }
    return AudioCommandType::Unknown;
}

AudioPriority AudioCommandParser::parsePriority(const char* priority) const {
    std::string value = priority == nullptr ? "" : priority;
    if (value == "high") {
        return AudioPriority::High;
    }
    if (value == "low") {
        return AudioPriority::Low;
    }
    return AudioPriority::Normal;
}

bool AudioCommandParser::isLocalHttpUrl(const std::string& url) const {
    const std::string prefix = "http://";
    if (url.rfind(prefix, 0) != 0) {
        return false;
    }

    size_t hostStart = prefix.size();
    size_t hostEnd = url.find('/', hostStart);
    std::string hostPort = url.substr(hostStart, hostEnd == std::string::npos ? std::string::npos : hostEnd - hostStart);
    if (hostPort.empty() || hostPort.find('@') != std::string::npos) {
        return false;
    }

    size_t colon = hostPort.find(':');
    std::string host = colon == std::string::npos ? hostPort : hostPort.substr(0, colon);
    if (host == "localhost" || host == "127.0.0.1") {
        return true;
    }
    if (host.size() > 6 && host.substr(host.size() - 6) == ".local") {
        return true;
    }
    if (host.rfind("10.", 0) == 0 || host.rfind("192.168.", 0) == 0) {
        return true;
    }
    if (host.rfind("172.", 0) == 0) {
        size_t secondDot = host.find('.', 4);
        if (secondDot != std::string::npos) {
            int secondOctet = std::atoi(host.substr(4, secondDot - 4).c_str());
            return secondOctet >= 16 && secondOctet <= 31;
        }
    }
    return false;
}

bool AudioCommandParser::isExpired(const std::string& expiresAt, const std::string& nowUtc) const {
    if (expiresAt.empty() || nowUtc.empty()) {
        return false;
    }
    return expiresAt <= nowUtc;
}

AudioCommandParseResult AudioCommandParser::reject(const char* reason, const char* message) const {
    AudioCommandParseResult result;
    result.ok = false;
    result.reason = reason;
    result.message = message;
    return result;
}
