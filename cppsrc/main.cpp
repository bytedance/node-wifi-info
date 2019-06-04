#include <napi.h>
#include "WifiInfo.h"

Napi::Object getWiFiInfoWrapped(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();
    Napi::Object returnObj = Napi::Object::New(env);
    
    nodewifiinfo::WiFiInfo wifiInfo = nodewifiinfo::getWiFiInfo();
    
    std::string bssid = wifiInfo.bssid;
    returnObj.Set("SSID", bssid.empty() ? env.Null() : Napi::String::New(env, wifiInfo.ssid));
    returnObj.Set("BSSID", bssid.empty() ? env.Null() : Napi::String::New(env, wifiInfo.bssid));
    returnObj.Set("secure", wifiInfo.secure);
    returnObj.Set("signalStrength", wifiInfo.strength);

    return returnObj;
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    exports.Set(
                "getWifiInfo", Napi::Function::New(env, getWiFiInfoWrapped)
                );
    
    return exports;
}

NODE_API_MODULE(node_wifi_info, InitAll);
