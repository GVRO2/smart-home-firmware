#pragma once

#include "NetworkConnection.h"

class MockNetworkConnection final : public NetworkConnection {
public:
    bool connected = true;
    int connectCalls = 0;
    int ensureConnectedCalls = 0;

    void connect() override {
        ++connectCalls;
    }

    void ensureConnected() override {
        ++ensureConnectedCalls;
    }

    bool isConnected() const override {
        return connected;
    }
};
