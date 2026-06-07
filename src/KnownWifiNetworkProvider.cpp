#include "KnownWifiNetworkProvider.h"
#include "Secrets.h"

KnownWifiNetworkProvider::KnownWifiNetworkProvider() : networkCount_(0) {
#if KNOWN_WIFI_NETWORKS_COUNT >= 1
    networks_[networkCount_++] = {KNOWN_WIFI_NETWORK_0_SSID, KNOWN_WIFI_NETWORK_0_PASSWORD, KNOWN_WIFI_NETWORK_0_PRIORITY};
#endif
#if KNOWN_WIFI_NETWORKS_COUNT >= 2
    networks_[networkCount_++] = {KNOWN_WIFI_NETWORK_1_SSID, KNOWN_WIFI_NETWORK_1_PASSWORD, KNOWN_WIFI_NETWORK_1_PRIORITY};
#endif
#if KNOWN_WIFI_NETWORKS_COUNT >= 3
    networks_[networkCount_++] = {KNOWN_WIFI_NETWORK_2_SSID, KNOWN_WIFI_NETWORK_2_PASSWORD, KNOWN_WIFI_NETWORK_2_PRIORITY};
#endif
#if KNOWN_WIFI_NETWORKS_COUNT >= 4
    networks_[networkCount_++] = {KNOWN_WIFI_NETWORK_3_SSID, KNOWN_WIFI_NETWORK_3_PASSWORD, KNOWN_WIFI_NETWORK_3_PRIORITY};
#endif
#if KNOWN_WIFI_NETWORKS_COUNT >= 5
    networks_[networkCount_++] = {KNOWN_WIFI_NETWORK_4_SSID, KNOWN_WIFI_NETWORK_4_PASSWORD, KNOWN_WIFI_NETWORK_4_PRIORITY};
#endif
}

int KnownWifiNetworkProvider::getNetworkCount() const {
    return networkCount_;
}

KnownWifiNetwork KnownWifiNetworkProvider::getNetwork(int index) const {
    if (index < 0 || index >= networkCount_) {
        return {"", "", 0};
    }
    return networks_[index];
}
