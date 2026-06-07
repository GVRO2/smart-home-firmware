#include "AudioPlaybackQueue.h"

bool AudioPlaybackQueue::enqueue(const AudioCommand& command) {
    if (isFull()) {
        return false;
    }

    int insertAt = count_;
    if (command.priority == AudioPriority::High) {
        insertAt = 0;
        while (insertAt < count_ && items_[insertAt].priority == AudioPriority::High) {
            ++insertAt;
        }
    }

    for (int i = count_; i > insertAt; --i) {
        items_[i] = items_[i - 1];
    }
    items_[insertAt] = command;
    ++count_;
    return true;
}

bool AudioPlaybackQueue::dequeue(AudioCommand& command) {
    if (isEmpty()) {
        return false;
    }

    command = items_[0];
    for (int i = 1; i < count_; ++i) {
        items_[i - 1] = items_[i];
    }
    --count_;
    return true;
}

void AudioPlaybackQueue::clear() {
    count_ = 0;
}

bool AudioPlaybackQueue::isFull() const {
    return count_ >= AUDIO_QUEUE_CAPACITY;
}

bool AudioPlaybackQueue::isEmpty() const {
    return count_ == 0;
}

int AudioPlaybackQueue::size() const {
    return count_;
}
