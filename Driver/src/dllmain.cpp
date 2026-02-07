#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <MinHook.h>
#include <openvr_driver.h>
#include <cstring>

#include "DeviceProvider.h"
#include "HookManager.h"

DeviceProvider deviceProvider;

extern "C" __declspec(dllexport) void* HmdDriverFactory(const char* pInterfaceName, int* pReturnCode) {
	if (strcmp(vr::IServerTrackedDeviceProvider_Version, pInterfaceName) == 0) return &deviceProvider;

	if (pReturnCode) *pReturnCode = vr::VRInitError_Init_InterfaceNotFound;

	return NULL;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
	switch (ul_reason_for_call) {
	case DLL_PROCESS_ATTACH:
		HookManager::setHModule(hModule);
		break;
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}