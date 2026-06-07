#pragma once

#include <string>

class AudioStatusSink {
public:
    virtual ~AudioStatusSink() = default;
    virtual bool publishStatus(
        const std::string& commandId,
        const char* state,
        const char* message,
        const std::string& reportedAtUtc,
        const char* reason = nullptr
    ) = 0;
};
