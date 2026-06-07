#pragma once

#include <Arduino.h>

struct KnownWifiNetwork {
    const char* ssid;
    const char* password;
    int priority;
};

class KnownWifiNetworkProvider {
public:
    KnownWifiNetworkProvider();

    int getNetworkCount() const;
    KnownWifiNetwork getNetwork(int index) const;

private:
    KnownWifiNetwork networks_[10];
    int networkCount_;
};
