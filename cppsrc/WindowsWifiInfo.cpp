#include "WifiInfo.h"
#include <Windows.h>
#include <wlanapi.h>
#include <algorithm>
#include <iostream>

using std::wcout;
using std::endl;

#pragma comment(lib, "wlanapi.lib")

int test();

HANDLE hClient = NULL;
PWLAN_INTERFACE_INFO_LIST pIfList = NULL;


//Context to pass along with callbacks
typedef struct _WLAN_CALLBACK_INFO {
	GUID interfaceGUID;
	HANDLE scanEvent;
	DWORD callbackReason;
} WLAN_CALLBACK_INFO;


void wlanCallback(WLAN_NOTIFICATION_DATA* scanNotificationData, PVOID myContext)
{
	//Get the data from my struct. If it's null, nothing to do
	WLAN_CALLBACK_INFO* callbackInfo = (WLAN_CALLBACK_INFO*)myContext;
	if (callbackInfo == nullptr)
	{
		return;
	}

	//Check the GUID in the struct against the GUID in the notification data, return if they don't match
	if (memcmp(&callbackInfo->interfaceGUID, &scanNotificationData->InterfaceGuid, sizeof(GUID)) != 0)
	{
		return;
	}

	//If the notification was for a scan complete or failure then we need to set the event
	if ((scanNotificationData->NotificationCode == wlan_notification_acm_scan_complete) || (scanNotificationData->NotificationCode == wlan_notification_acm_scan_fail))
	{
		//Set the notification code as the callbackReason
		callbackInfo->callbackReason = scanNotificationData->NotificationCode;

		//Set the event
		SetEvent(callbackInfo->scanEvent);
	}

	return;
}

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

std::vector<nodewifiinfo::WiFiInfo> nodewifiinfo::getAllWiFiInfo() {
	// WIN API Vars
	DWORD dwResult = 0;
	DWORD dwCurVersion = 0;
	// Return Value
	std::vector<nodewifiinfo::WiFiInfo> infoList;

	// Get handle if null
	if (hClient == NULL) {
		dwResult = WlanOpenHandle(2, NULL, &dwCurVersion, &hClient);
	}
	if (dwResult != ERROR_SUCCESS) {
		printf("Error: Unable to open wlanHandle\n");
		hClient = NULL;
		return infoList;
	}

	// Enumerate interfaces (this only happens once for performance reason
	// Since hardware change during application runtime is unlikely)
	if (pIfList == NULL) {
		dwResult = WlanEnumInterfaces(hClient, NULL, &pIfList);
	}
	if (dwResult != ERROR_SUCCESS) {
		pIfList = NULL;
		return infoList;
	}

	DWORD connectInfoSize = sizeof(WLAN_CONNECTION_ATTRIBUTES);
	for (size_t i = 0; i < (int)pIfList->dwNumberOfItems; i++)
	{
		PWLAN_INTERFACE_INFO pIfInfo = (WLAN_INTERFACE_INFO*)&pIfList->InterfaceInfo[i];

		/*******/

		//Declare the callback parameter struct
		WLAN_CALLBACK_INFO callbackInfo = { 0 };
		callbackInfo.interfaceGUID = pIfInfo->InterfaceGuid;

		//Create an event to be triggered in the scan case
		callbackInfo.scanEvent = CreateEvent(
			nullptr,
			FALSE,
			FALSE,
			nullptr);


		//Register for wlan scan notifications
		WlanRegisterNotification(hClient,
			WLAN_NOTIFICATION_SOURCE_ALL,
			TRUE,
			(WLAN_NOTIFICATION_CALLBACK)wlanCallback,
			(PVOID)&callbackInfo,
			NULL,
			NULL);


		//Start a scan. If the WlanScan call fails, log the error
		WlanScan(hClient, &pIfInfo->InterfaceGuid, NULL, NULL, NULL);
		if (GetLastError() != ERROR_SUCCESS)
		{
			wcout << "\tError triggering scan on interface " << i + 1 << ". Error: " << GetLastError() << endl;
			continue;
		}
		else
		{
			//Scan request successfully sent
			wcout << "\tScan request sent. Waiting for reply." << endl;
		}


		//Wait for the event to be signaled, or an error to occur. Don't wait longer than 15 seconds.
		DWORD waitResult = WaitForSingleObject(callbackInfo.scanEvent, 15000);

		//Check how we got here, via callback or timeout
		if (waitResult == WAIT_OBJECT_0)
		{
			if (callbackInfo.callbackReason == wlan_notification_acm_scan_complete)
			{
				wcout << "\tReply: The scan for networks has completed." << endl << endl;
			}
			else if (callbackInfo.callbackReason == wlan_notification_acm_scan_fail)
			{
				wcout << "\tReply: The scan for connectable networks failed." << endl << endl;
			}


		}
		else if (waitResult == WAIT_TIMEOUT)
		{
			wcout << "\tError: No response was received after 15 seconds." << endl;
			wcout << "\n\tWindows Logo certified wireless drivers are required to complete scans\n"
				<< "\tin under four seconds, so there may be something wrong." << endl << endl;
		}
		else
		{
			wcout << "\n\tUnknown error waiting for response. Error Code: " << waitResult << endl << endl;
		}


		/******************/


		PWLAN_AVAILABLE_NETWORK_LIST pBssList = NULL;
		PWLAN_AVAILABLE_NETWORK pBssEntry = NULL;

		dwResult = WlanGetAvailableNetworkList(hClient,
			&pIfInfo->InterfaceGuid,
			0,
			NULL,
			&pBssList);

		if (dwResult != ERROR_SUCCESS)
		{
			printf("WlanGetAvailableNetworkList failed with error: %u\n", dwResult);
			return infoList;
		}

		for (DWORD j = 0; j < pBssList->dwNumberOfItems; j++)
		{
			pBssEntry = (WLAN_AVAILABLE_NETWORK*)&pBssList->Network[j];

			nodewifiinfo::WiFiInfo info;
			
			//SSID
			info.ssid = std::string(
				(char*)pBssEntry->dot11Ssid.ucSSID,
				pBssEntry->dot11Ssid.uSSIDLength
			);
	
			// Secure
			info.secure = pBssEntry->bSecurityEnabled;
			// Strength
			info.strength = (pBssEntry->wlanSignalQuality / 2) - 100;

			PWLAN_BSS_LIST pBssList2 = nullptr;
			dwResult = WlanGetNetworkBssList(
				hClient,
				&pIfInfo->InterfaceGuid,
				&pBssEntry->dot11Ssid,
				dot11_BSS_type_infrastructure,
				pBssEntry->bSecurityEnabled,
				nullptr,
				&pBssList2);
			
			if (dwResult != ERROR_SUCCESS)
			{
				printf("WlanGetNetworkBssList failed with error: %u\n", dwResult);
				return infoList;
			}

			if (1 == pBssList2->dwNumberOfItems)
			{
				// BSSID
				char bssidAddr[19];
				for (short i = 0; i < sizeof(pBssList2->wlanBssEntries[0].dot11Bssid); i++) {
					sprintf((bssidAddr + (i * 3)), "%02x:", pBssList2->wlanBssEntries[0].dot11Bssid[i]);
				}
				bssidAddr[17] = '\0';
				info.bssid = std::string(bssidAddr);
			}
			else
			{
				info.bssid = "unknown";
			}

			infoList.push_back(info);
		}
	}

	// remove duplicate networks (due to windows network memory features)
	auto comperator = [] (const WiFiInfo& lhs, const WiFiInfo& rhs) { return lhs.ssid == rhs.ssid; };
	auto predicate = [] (const WiFiInfo& lhs, const WiFiInfo& rhs) { return lhs.strength < rhs.strength; };
	
	std::sort(infoList.begin(), infoList.end(), predicate);
	infoList.erase(std::unique(infoList.begin(), infoList.end(), comperator), infoList.end());

	return infoList;
}
