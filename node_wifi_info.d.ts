declare interface WifiInfo {
    SSID: string;
    BSSID: string;
    secure: boolean;
    signalStrength: number;
}

declare const _default: {
    getWifiInfo(): WifiInfo;
    getAllWifiInfo(): Array<WifiInfo>;
};

export = _default;
