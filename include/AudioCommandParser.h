#pragma once

#include <string>

#include "AudioCommand.h"

class AudioCommandParser {
public:
    AudioCommandParseResult parse(const std::string& payload, const std::string& nowUtc) const;

private:
    AudioCommandType parseType(const char* type) const;
    AudioPriority parsePriority(const char* priority) const;
    bool isLocalHttpUrl(const std::string& url) const;
    bool isExpired(const std::string& expiresAt, const std::string& nowUtc) const;
    AudioCommandParseResult reject(const char* reason, const char* message) const;
};
