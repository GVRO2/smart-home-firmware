#pragma once

#include "ClockProvider.h"

class MockClockProvider final : public ClockProvider {
public:
    bool ready = true;
    std::string nowValue = "2026-05-31T23:50:01Z";
    int syncCalls = 0;
    int ensureSyncedCalls = 0;

    void sync() override {
        ++syncCalls;
    }

    void ensureSynced() override {
        ++ensureSyncedCalls;
    }

    std::string nowIsoUtc() override {
        return nowValue;
    }

    bool isReady() const override {
        return ready;
    }
};
