#include "DeviceProvider.h"

#include <thread>

#include "LogManager.h"
#include "SharedDeviceMemoryDriver.h"
#include "main.h"

std::thread mainThread;

vr::EVRInitError DeviceProvider::Init(vr::IVRDriverContext* pDriverContext) {
	vr::InitServerDriverContext(pDriverContext);

	LogManager::initialize(); // Initialize Log manager
	HookManager::initializeMinHook(); // Initialize MinHook
	HookManager::setupHooks_IVRServerDriverHost((void*)vr::VRServerDriverHost()); // IVRServerDriverHost hooks
	HookManager::setupHooks_IVRDriverInput((void*)vr::VRDriverInput()); // IVRDriverInput hooks

	// IVRProperties hooks
	void* pProperties = nullptr;
	vr::EVRInitError propertiesError = vr::VRInitError_None;
	pProperties = vr::VRDriverContext()->GetGenericInterface(vr::IVRProperties_Version, &propertiesError);
	if (propertiesError != vr::VRInitError_None) {
		LogManager::log(LOG_ERROR, "Failed to get IVRProperties interface: {}", static_cast<int>(propertiesError));
		return propertiesError;
	}
	HookManager::setupHooks_IVRProperties(pProperties);

	// Initialize shared memory
	bool sharedMemoryInitializationResult = SharedDeviceMemoryDriver::getInstance().initialize(); 

	LogManager::log(LOG_INFO, "Shared memory initialization {0}", 
		sharedMemoryInitializationResult ? "succeeded" : "failed"
	);

	// Start main thread asyncronously
	mainThread = std::thread([] { Main::getInstance().main(); });
	mainThread.detach();

	LogManager::log(LOG_INFO, "Initialized Device Provider");

	return vr::VRInitError_None;
}

void DeviceProvider::Cleanup() {
	if (mainThread.joinable()) {
		mainThread.join();
	}

	MH_DisableHook(MH_ALL_HOOKS);
	MH_Uninitialize();
}

const char* const* DeviceProvider::GetInterfaceVersions() {
	return vr::k_InterfaceVersions;
}

void DeviceProvider::RunFrame() {}

bool DeviceProvider::ShouldBlockStandbyMode() { return false; }

void DeviceProvider::EnterStandby() {}

void DeviceProvider::LeaveStandby() {}