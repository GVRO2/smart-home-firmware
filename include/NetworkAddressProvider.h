#pragma once

#include <Arduino.h>
#include <WiFi.h>

class NetworkAddressProvider {
public:
    NetworkAddressProvider();

    String getLocalIpAddress();
    bool hasValidLocalIpAddress();

private:
    bool isValidIpAddress(const String& ip);
};
