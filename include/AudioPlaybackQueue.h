#pragma once

#include <array>

#include "AppConfig.h"
#include "AudioCommand.h"

class AudioPlaybackQueue {
public:
    bool enqueue(const AudioCommand& command);
    bool dequeue(AudioCommand& command);
    void clear();
    bool isFull() const;
    bool isEmpty() const;
    int size() const;

private:
    std::array<AudioCommand, AUDIO_QUEUE_CAPACITY> items_;
    int count_ = 0;
};
