#include "WifiInfo.h"

// This is just a substitute file. Linux uses nmcli which is directly invoked in nodejs (no native needed)

nodewifiinfo::WiFiInfo nodewifiinfo::getWiFiInfo() {
    return {};
}

std::vector<nodewifiinfo::WiFiInfo> nodewifiinfo::getAllWiFiInfo() {
    std::vector<nodewifiinfo::WiFiInfo> info{ {} };

    return info;
}