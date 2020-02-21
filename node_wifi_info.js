const os = require('os');
const linuxModule = require('./src/linux');
const nodeWifiModule = require('bindings')('node_wifi_info.node');

module.exports = {
  getWifiInfo() {
    const platform = os.platform();

    switch (platform) {
      case 'win32':
      case 'darwin':
        return nodeWifiModule.getWifiInfo();
      case 'linux':
        return linuxModule.getWiFiInfoLinux();
      default:
        throw new Error(`platform '${platform}' is currently unsupported by node-wifi-info.`);
    }
  },
  getAllWifiInfo() {
    const platform = os.platform();
    
    switch (platform) {
      case 'win32':
      case 'darwin':
        return nodeWifiModule.getAllWifiInfo();
      case 'linux':
        return linuxModule.getAllWiFiInfoLinux();
      default:
        throw new Error(`platform '${platform}' is currently unsupported by node-wifi-info.`);
    }
  }
};
