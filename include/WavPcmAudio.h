#pragma once

#include <cstddef>
#include <cstdint>

struct WavPcmInfo {
    uint16_t audioFormat = 0;
    uint16_t channels = 0;
    uint32_t sampleRateHz = 0;
    uint16_t bitsPerSample = 0;
    uint32_t dataBytes = 0;
    size_t dataOffset = 0;
};

inline uint16_t readLe16(const uint8_t* data) {
    return static_cast<uint16_t>(data[0]) | static_cast<uint16_t>(data[1] << 8);
}

inline uint32_t readLe32(const uint8_t* data) {
    return static_cast<uint32_t>(data[0])
        | (static_cast<uint32_t>(data[1]) << 8)
        | (static_cast<uint32_t>(data[2]) << 16)
        | (static_cast<uint32_t>(data[3]) << 24);
}

inline bool wavTagEquals(const uint8_t* data, const char* tag) {
    return data[0] == static_cast<uint8_t>(tag[0])
        && data[1] == static_cast<uint8_t>(tag[1])
        && data[2] == static_cast<uint8_t>(tag[2])
        && data[3] == static_cast<uint8_t>(tag[3]);
}

inline bool parseWavPcmHeader(const uint8_t* data, size_t size, WavPcmInfo& info) {
    if (data == nullptr || size < 44) {
        return false;
    }
    if (!wavTagEquals(data, "RIFF") || !wavTagEquals(data + 8, "WAVE")) {
        return false;
    }

    bool foundFmt = false;
    bool foundData = false;
    size_t offset = 12;
    WavPcmInfo parsed;

    while (offset + 8 <= size) {
        const uint8_t* chunk = data + offset;
        uint32_t chunkSize = readLe32(chunk + 4);
        size_t chunkDataOffset = offset + 8;
        if (chunkDataOffset + chunkSize > size) {
            return false;
        }

        if (wavTagEquals(chunk, "fmt ")) {
            if (chunkSize < 16) {
                return false;
            }
            parsed.audioFormat = readLe16(data + chunkDataOffset);
            parsed.channels = readLe16(data + chunkDataOffset + 2);
            parsed.sampleRateHz = readLe32(data + chunkDataOffset + 4);
            parsed.bitsPerSample = readLe16(data + chunkDataOffset + 14);
            foundFmt = true;
        } else if (wavTagEquals(chunk, "data")) {
            parsed.dataBytes = chunkSize;
            parsed.dataOffset = chunkDataOffset;
            foundData = true;
        }

        offset = chunkDataOffset + chunkSize + (chunkSize % 2);
        if (foundFmt && foundData) {
            break;
        }
    }

    if (!foundFmt || !foundData) {
        return false;
    }
    if (parsed.audioFormat != 1 || parsed.channels != 1 || parsed.bitsPerSample != 16) {
        return false;
    }
    if (parsed.sampleRateHz == 0 || parsed.dataBytes == 0) {
        return false;
    }

    info = parsed;
    return true;
}
