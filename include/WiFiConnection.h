#pragma once

#include <Arduino.h>
#include "KnownWifiNetworkProvider.h"

class WiFiConnection {
public:
    WiFiConnection(KnownWifiNetworkProvider& networkProvider);

    void connect();
    void ensureConnected();
    bool isConnected() const;
    String getLocalIpAddress();

private:
    void connectInternal();
    bool connectToBestKnownWifi();
    bool tryConnectToNetwork(const KnownWifiNetwork& network);

    KnownWifiNetworkProvider& networkProvider_;
    unsigned long lastConnectAttemptAt_;
    unsigned long lastScanAttemptAt_;
};
