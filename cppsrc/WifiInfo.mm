#include "WifiInfo.h"
#include <Foundation/Foundation.h>
#include <CoreWLAN/CoreWLAN.h>

nodewifiinfo::WiFiInfo nodewifiinfo::getWiFiInfo() {
    nodewifiinfo::WiFiInfo info = {};
    @autoreleasepool {
        CWWiFiClient* client = [CWWiFiClient sharedWiFiClient];
        if (client) {
            CWInterface *interface = [[CWWiFiClient sharedWiFiClient] interface];
            if (interface.ssid) {
                info.ssid = std::string([interface.ssid UTF8String]);
                info.bssid = std::string([interface.bssid UTF8String]);
                info.secure = (interface.security != kCWSecurityUnknown && interface.security != kCWSecurityNone);
                info.strength = (int32_t) interface.rssiValue;
            }
        }
    }
    return info;
}
