var WifiInfo = /** @class */ (function () {
    function WifiInfo(SSID, BSSID, secure, signalStrength) {
        if (SSID === void 0) SSID = null;
        if (BSSID === void 0) BSSID = null;
        if (secure === void 0) secure = false;
        if (signalStrength === void 0) signalStrength = 0;

        this.SSID = SSID;
        this.BSSID = BSSID;
        this.secure = secure;
        this.signalStrength = signalStrength;
    }

    return WifiInfo;
}());

module.exports = WifiInfo;