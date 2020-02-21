#include <napi.h>

namespace nodewifiinfo {
    typedef struct {
        std::string ssid;
        std::string bssid;
        bool secure;
        int32_t strength;
    } WiFiInfo;

    WiFiInfo getWiFiInfo();
    std::vector<WiFiInfo> getAllWiFiInfo();
}