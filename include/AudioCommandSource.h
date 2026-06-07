#pragma once

#include <string>

class AudioCommandSource {
public:
    virtual ~AudioCommandSource() = default;
    virtual bool pollAudioCommand(std::string& payload) = 0;
};
