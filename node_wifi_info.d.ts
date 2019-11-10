declare interface WifiInfo {
    SSID: string;
    BSSID: string;
    secure: boolean;
    signalStrength: number;
}

declare const _default: {
    getWifiInfo(): WifiInfo;
};

export = _default;
