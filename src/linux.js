const WifiInfo = require('./wifiInfo');

const { execSync } = require('child_process');

const NMWiFiStringParseState = {
    INITIAL: 0,
    SSID: 1,
    BSSID: 2,
    SECURITY: 3,
    STRENGTH: 4
};

const StringParseEscapeState = {
    ESCAPE_NEXT: 0,
    NORMAL: 1
};

function getAndParseWifiInfo() {
    try {
        const rawData = execSync('nmcli -t -f active,ssid,bssid,security,signal device wifi list --rescan no');
        const wifis = rawData
            .toString('UTF-8')
            .split('\n')
            .filter(wifiName => (wifiName.startsWith('yes')))
            .map((wifiInfo) => {
                const info = new WifiInfo();
                let parseState = NMWiFiStringParseState.INITIAL;
                let escape = StringParseEscapeState.NORMAL;
                let currentSectionStartIndex = 0;

                for (let i = 0; i < wifiInfo.length; i++) {
                    if ((wifiInfo.charAt(i) == ':' && escape == StringParseEscapeState.NORMAL) || i == wifiInfo.length - 1) {
                        const subStr = wifiInfo.substring(currentSectionStartIndex, i).replace(/\\/g, '');
                        currentSectionStartIndex = i + 1;
                        switch (parseState) {
                            case NMWiFiStringParseState.INITIAL:
                                break;
                            case NMWiFiStringParseState.SSID:
                                info.SSID = subStr;
                                break;
                            case NMWiFiStringParseState.BSSID:
                                info.BSSID = subStr;
                                break;
                            case NMWiFiStringParseState.SECURITY:
                                info.secure = subStr.length > 0;
                                break;
                            case NMWiFiStringParseState.STRENGTH:
                                console.log("strength", subStr);
                                info.signalStrength = Math.round((parseInt(subStr) / 2) - 100);
                                break;
                        }
                        parseState++;
                    } else {
                        if (escape == StringParseEscapeState.NORMAL && wifiInfo.charAt(i) == '\\') {
                            escape = StringParseEscapeState.ESCAPE_NEXT;
                        } else {
                            escape = StringParseEscapeState.NORMAL;
                        }
                    }
                }

                if (!info.BSSID || info.BSSID.length == 0) {
                    info.BSSID = null;
                    info.SSID = null;
                    info.secure = false;
                    info.signalStrength = 0;
                } 

                return info;
            });

        if (wifis.length > 0) {
            return wifis[0];
        }
    } catch (e) {
        console.error("Error whild getting wifi info. Do you have nmcli installed?", e);
    }

    return new WifiInfo();
}

module.exports = {
    getWiFiInfoLinux() {
        return getAndParseWifiInfo();
    },
    getAllWiFiInfoLinux() {
        // TODO: implement: iterate over all the wifi network and add them here
        return [getAndParseWifiInfo()];
    }
};