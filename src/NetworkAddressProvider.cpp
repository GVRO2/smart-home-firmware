#include "NetworkAddressProvider.h"

NetworkAddressProvider::NetworkAddressProvider() {
}

String NetworkAddressProvider::getLocalIpAddress() {
    if (WiFi.status() != WL_CONNECTED) {
        return "";
    }

    IPAddress ip = WiFi.localIP();
    String ipString = ip.toString();

    if (!isValidIpAddress(ipString)) {
        return "";
    }

    return ipString;
}

bool NetworkAddressProvider::hasValidLocalIpAddress() {
    return isValidIpAddress(getLocalIpAddress());
}

bool NetworkAddressProvider::isValidIpAddress(const String& ip) {
    if (ip.isEmpty()) {
        return false;
    }

    if (ip == "0.0.0.0") {
        return false;
    }

    if (ip == "127.0.0.1") {
        return false;
    }

    return true;
}
