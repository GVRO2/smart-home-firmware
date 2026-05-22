#pragma once

#include <Arduino.h>

class TimeProvider {
public:
    TimeProvider();

    void sync();
    void ensureSynced();
    String nowIsoUtc();
    bool isReady() const;

private:
    bool hasValidTime() const;
    String formatUtcIso(time_t utcSeconds) const;

    bool isReady_;
    unsigned long lastSyncAttemptAt_;
};
