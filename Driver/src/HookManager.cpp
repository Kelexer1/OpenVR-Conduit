#include "HookManager.h"

// Default initializations, will be overriden later
HINSTANCE HookManager::hModule = nullptr;

void HookManager::initializeHooks() {
	// Initialize MinHook
	if (MH_Initialize() != MH_OK) {
		LogManager::log(LOG_ERROR, "MinHook Initialization failed");
		return;
	}
	LogManager::log(LOG_INFO, "MinHook initialized successfully");
}

void HookManager::setupHooks_IVRServerDriverHost(void* host) {
	if (!host) {
		LogManager::log(LOG_ERROR, "IVRServerDriverHost is not initialized");
		return;
	}

	void** vtable = *(void***)host;
	if (!vtable) {
		LogManager::log(LOG_ERROR, "Failed to get vtable from IVRServerDriverHost");
		return;
	}

	createHook(vtable[Offset_TrackedDevicePoseUpdated], &overrideTrackedDevicePoseUpdated, reinterpret_cast<void**>(&originalTrackedDevicePoseUpdated), "TrackedDevicePoseUpdated");
	
	IVRServerDriverHost = host;

	LogManager::log(LOG_INFO, "All IVRServerDriverHost methods hooked successfully");
}

void HookManager::setupHooks_IVRDriverInput(void* input) {
	if (!input) {
		LogManager::log(LOG_ERROR, "IVRDriverInput is not initialized");
		return;
	}

	void** vtable = *(void***)input;
	if (!vtable) {
		LogManager::log(LOG_ERROR, "Failed to get vtable from IVRDriverInput");
		return;
	}

	createHook(vtable[Offset_CreateBooleanComponent], &overrideCreateBooleanComponent, reinterpret_cast<void**>(&originalCreateBooleanComponent), "CreateBooleanComponent");
	createHook(vtable[Offset_UpdateBooleanComponent], &overrideUpdateBooleanComponent, reinterpret_cast<void**>(&originalUpdateBooleanComponent), "UpdateBooleanComponent");
	createHook(vtable[Offset_CreateScalarComponent], &overrideCreateScalarComponent, reinterpret_cast<void**>(&originalCreateScalarComponent), "CreateScalarComponent");
	createHook(vtable[Offset_UpdateScalarComponent], &overrideUpdateScalarComponent, reinterpret_cast<void**>(&originalUpdateScalarComponent), "UpdateScalarComponent");
	createHook(vtable[Offset_CreateHapticComponent], &overrideCreateHapticComponent, reinterpret_cast<void**>(&originalCreateHapticComponent), "CreateHapticComponent");
	createHook(vtable[Offset_CreateSkeletonComponent], &overrideCreateSkeletonComponent, reinterpret_cast<void**>(&originalCreateSkeletonComponent), "CreateSkeletonComponent");
	createHook(vtable[Offset_UpdateSkeletonComponent], &overrideUpdateSkeletonComponent, reinterpret_cast<void**>(&originalUpdateSkeletonComponent), "UpdateSkeletonComponent");
	createHook(vtable[Offset_CreatePoseComponent], &overrideCreatePoseComponent, reinterpret_cast<void**>(&originalCreatePoseComponent), "CreatePoseComponent");
	createHook(vtable[Offset_UpdatePoseComponent], &overrideUpdatePoseComponent, reinterpret_cast<void**>(&originalUpdatePoseComponent), "UpdatePoseComponent");
	createHook(vtable[Offset_CreateEyeTrackingComponent], &overrideCreateEyeTrackingComponent, reinterpret_cast<void**>(&originalCreateEyeTrackingComponent), "CreateEyeTrackingComponent");
	createHook(vtable[Offset_UpdateEyeTrackingComponent], &overrideUpdateEyeTrackingComponent, reinterpret_cast<void**>(&originalUpdateEyeTrackingComponent), "UpdateEyeTrackingComponent");

	IVRDriverInput = input;

	LogManager::log(LOG_INFO, "All IVRDriverInput methods hooked successfully");
}

void HookManager::setupHooks_IVRProperties(void* properties) {
	if (!properties) {
		LogManager::log(LOG_ERROR, "IVRProperties is not initialized");
		return;
	}

	void** vtable = *(void***)properties;
	if (!vtable) {
		LogManager::log(LOG_ERROR, "Failed to get vtable from IVRProperties");
		return;
	}

	createHook(vtable[Offset_TrackedDeviceToPropertyContainer], &overrideTrackedDeviceToPropertyContainer, reinterpret_cast<void**>(&originalTrackedDeviceToPropertyContainer), "TrackedDeviceToPropertyContainer");

	LogManager::log(LOG_INFO, "All IVRProperties methods hooked successfully");
}

void HookManager::shutdown(std::string reason) {
	MH_Uninitialize();

	LogManager::log(LOG_INFO, "Shutdown called: {}", reason);
	LogManager::shutdown();

	Sleep(1000);

	// Start exiting thread
	CreateThread(nullptr, 0, &HookManager::ejectThread, nullptr, 0, nullptr);
}

DWORD __stdcall HookManager::ejectThread(LPVOID) {
	Sleep(100);
	FreeLibraryAndExitThread(hModule, 0);
	return 0;
}

void HookManager::setHModule(HMODULE hModule) {
	HookManager::hModule = hModule;
}

void HookManager::dumpVTableFromHost(void* host) {
	if (!host) {
		LogManager::log(LOG_ERROR, "Host is not initialized");
		return;
	}

	void** vtable = *(void***)host;
	if (!vtable) {
		LogManager::log(LOG_ERROR, "Failed to get vtable from host");
		return;
	}

	LogManager::log(LOG_DEBUG, "Dumping vtable entries:");
	for (int i = 0; i < 20; i++) {
		LogManager::log(LOG_DEBUG, "vtable[{}] = {}", i, vtable[i]);
	}
	LogManager::log(LOG_DEBUG, "Finished dumping vtable entries");
}

void HookManager::createHook(void* targetFunction, void* detourFunction, void** originalFunction, const std::string& name) {
	// Create hook
	MH_STATUS status = MH_CreateHook(targetFunction, detourFunction, originalFunction);
	if (status != MH_OK) {
		LogManager::log(LOG_ERROR, "MinHook hook creation failed for {} with error code: {}", name, static_cast<int>(status));
		return;
	}

	// Enable hook
	status = MH_EnableHook(targetFunction);
	if (status != MH_OK) {
		LogManager::log(LOG_ERROR, "MinHook hook enable failed for {} with error code: {}", name, static_cast<int>(status));
		return;
	}
}