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

Napi::Object getAllWiFiInfoWrapped(const Napi::CallbackInfo& info) {
    uint32_t index = 0;
    Napi::Env env = info.Env();
    Napi::Object returnArray = Napi::Array::New(env);
    
    std::vector<nodewifiinfo::WiFiInfo> wifiInfo = nodewifiinfo::getAllWiFiInfo();
    
    for(std::vector<nodewifiinfo::WiFiInfo>::iterator it = wifiInfo.begin(); it != wifiInfo.end(); ++it) {
        Napi::Object infoObj = Napi::Object::New(env);
        
        std::string bssid = (*it).bssid;
        infoObj.Set("SSID", bssid.empty() ? env.Null() : Napi::String::New(env, (*it).ssid));
        infoObj.Set("BSSID", bssid.empty() ? env.Null() : Napi::String::New(env, (*it).bssid));
        infoObj.Set("secure", (*it).secure);
        infoObj.Set("signalStrength", (*it).strength);

        returnArray.Set(index++, infoObj);
    }

    return returnArray;
}

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
    exports.Set("getWifiInfo", Napi::Function::New(env, getWiFiInfoWrapped));
    
    exports.Set("getAllWifiInfo", Napi::Function::New(env, getAllWiFiInfoWrapped));
    
    return exports;
}

NODE_API_MODULE(node_wifi_info, InitAll);
