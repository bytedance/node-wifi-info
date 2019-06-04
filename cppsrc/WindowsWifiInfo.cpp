#include "WifiInfo.h"
#include <Windows.h>
#include <wlanapi.h>

#pragma comment(lib, "wlanapi.lib")

int test();

HANDLE hClient = NULL;
PWLAN_INTERFACE_INFO_LIST pIfList = NULL;


nodewifiinfo::WiFiInfo nodewifiinfo::getWiFiInfo() {
	// WIN API Vars
	DWORD dwResult = 0;
	DWORD dwCurVersion = 0;
	// Return Value
	WiFiInfo info = {};

	// Get handle if null
	if (hClient == NULL) {
		dwResult = WlanOpenHandle(2, NULL, &dwCurVersion, &hClient);
	}
	if (dwResult != ERROR_SUCCESS) {
		printf("Error: Unable to open wlanHandle\n");
		hClient = NULL;
		return info;
	}

	// Enumerate interfaces (this only happens once for performance reason
	// Since hardware change during application runtime is unlikely)
	if (pIfList == NULL) {
		dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
	}
	if (dwResult != ERROR_SUCCESS) {
		pIfList = NULL;
		return info;
	}

	DWORD connectInfoSize = sizeof(WLAN_CONNECTION_ATTRIBUTES);
	for (size_t i = 0; i < (int)pIfList->dwNumberOfItems; i++)
	{
		PWLAN_INTERFACE_INFO pIfInfo = (WLAN_INTERFACE_INFO*)& pIfList->InterfaceInfo[i];
		PWLAN_CONNECTION_ATTRIBUTES pConnectInfo = NULL;
		dwResult = WlanQueryInterface(hClient,
			&pIfInfo->InterfaceGuid,
			wlan_intf_opcode_current_connection,
			NULL,
			&connectInfoSize,
			(PVOID*)&pConnectInfo,
			NULL);
		if (dwResult != ERROR_SUCCESS) {
			continue;
		}
		if (pConnectInfo->isState != wlan_interface_state_connected) {
			WlanFreeMemory(pConnectInfo);
			continue;
		}
		//SSID
		info.ssid = std::string(
			(char*)pConnectInfo->wlanAssociationAttributes.dot11Ssid.ucSSID,
			pConnectInfo->wlanAssociationAttributes.dot11Ssid.uSSIDLength
		);
		// BSSID
		char bssidAddr[18];
		for (short i = 0; i < sizeof(pConnectInfo->wlanAssociationAttributes.dot11Bssid); i++) { 
			sprintf((bssidAddr + (i * 3)),"%02x:", pConnectInfo->wlanAssociationAttributes.dot11Bssid[i]);
		}
		bssidAddr[17] = '\0';
		info.bssid = std::string(bssidAddr);
		// Secure
		info.secure = pConnectInfo->wlanSecurityAttributes.bSecurityEnabled;
		// Strength
		info.strength = (pConnectInfo->wlanAssociationAttributes.wlanSignalQuality / 2) - 100;

		//Free
		WlanFreeMemory(pConnectInfo);

		return info;
	}
	return info;
}
