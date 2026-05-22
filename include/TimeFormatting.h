#pragma once

#include <ctime>
#include <string>

inline std::string formatUtcIsoUtc(time_t utcSeconds) {
    struct tm utcTime {};
#if defined(_WIN32)
    gmtime_s(&utcTime, &utcSeconds);
#else
    gmtime_r(&utcSeconds, &utcTime);
#endif

    char buffer[25];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", &utcTime);
    return std::string(buffer);
}