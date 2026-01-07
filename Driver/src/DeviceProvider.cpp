#include "DeviceProvider.h"

#include <thread>

#include "LogManager.h"
#include "SharedDeviceMemoryDriver.h"
#include "main.h"

std::thread mainThread;

vr::EVRInitError DeviceProvider::Init(vr::IVRDriverContext* pDriverContext) {
	vr::InitServerDriverContext(pDriverContext);

	// Initialize all essential components
	LogManager::initialize();
	HookManager::initializeHooks();
	HookManager::setupHooks_IVRServerDriverHost((void*)vr::VRServerDriverHost());
	HookManager::setupHooks_IVRDriverInput((void*)vr::VRDriverInput());
	bool sharedMemoryInitializationResult = SharedDeviceMemoryDriver::getInstance().initialize();

	LogManager::log(LOG_INFO, "Shared memory initialization {0}", sharedMemoryInitializationResult ? "succeeded" : "failed");

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