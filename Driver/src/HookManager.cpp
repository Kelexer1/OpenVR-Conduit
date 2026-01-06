#include "HookManager.h"

#include "vrmath.h"

#include "LogManager.h"
#include "globals.h"
#include "SharedDeviceMemory.h"
#include "Utils.h"

// Default initializations, will be overriden later
HINSTANCE HookManager::hModule = nullptr;

// Hook Declarations
typedef void(*_TrackedDevicePoseUpdated)(void* _this, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize);
static _TrackedDevicePoseUpdated originalTrackedDevicePoseUpdated = nullptr;

typedef void(*_CreateBooleanComponent)(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);
static _CreateBooleanComponent originalCreateBooleanComponent = nullptr;

typedef void(*_UpdateBooleanComponent)(void* _this, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset);
static _UpdateBooleanComponent originalUpdateBooleanComponent = nullptr;

typedef void(*_CreateScalarComponent)(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits);
static _CreateScalarComponent originalCreateScalarComponent = nullptr;

typedef void(*_UpdateScalarComponent)(void* _this, vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset);
static _UpdateScalarComponent originalUpdateScalarComponent = nullptr;

typedef void(*_CreateHapticComponent)(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);
static _CreateHapticComponent originalCreateHapticComponent = nullptr;

typedef void(*_CreateSkeletonComponent)(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, const char* pchSkeletonPath, const char* pchBasePosePath, vr::EVRSkeletalTrackingLevel eSkeletalTrackingLevel, const vr::VRBoneTransform_t* pGripLimitTransforms, uint32_t unGripLimitTransformCount, vr::VRInputComponentHandle_t* pHandle);
static _CreateSkeletonComponent originalCreateSkeletonComponent = nullptr;

typedef void(*_UpdateSkeletonComponent)(void* _this, vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount);
static _UpdateSkeletonComponent originalUpdateSkeletonComponent = nullptr;

typedef void(*_CreatePoseComponent)(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);
static _CreatePoseComponent originalCreatePoseComponent = nullptr;

typedef void(*_UpdatePoseComponent)(void* _this, vr::VRInputComponentHandle_t ulComponent, const vr::HmdMatrix34_t* pMatPoseOffset, double fTimeOffset);
static _UpdatePoseComponent originalUpdatePoseComponent = nullptr;

typedef void(*_CreateEyeTrackingComponent)(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);
static _CreateEyeTrackingComponent originalCreateEyeTrackingComponent = nullptr;

typedef void(*_UpdateEyeTrackingComponent)(void* _this, vr::VRInputComponentHandle_t ulComponent, const vr::VREyeTrackingData_t* pEyeTrackingData_t, double fTimeOffset);
static _UpdateEyeTrackingComponent originalUpdateEyeTrackingComponent = nullptr;

void overrideTrackedDevicePoseUpdated(void* _this, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize) {
	vr::DriverPose_t overridePose = newPose;

	// Only work with controllers
	if (getDeviceClass(unWhichDevice) == vr::TrackedDeviceClass_Controller) {
		// Update natural pose
		bool active;
		if (sharedMemoryIO.isControllerSlotActive(unWhichDevice, active) && active) {
			sharedMemoryIO.writeNaturalPose(unWhichDevice, FromDriverPose(newPose));
		}

		// Check if using ping-pong communication
		bool usePingPong;
		if (sharedMemoryIO.readUsePingPong(unWhichDevice, usePingPong) && usePingPong) {
			overridePose = ToDriverPose(sharedMemoryIO.pingPong(unWhichDevice));
		} else {
			// Check if using a custom pose
			bool useOverridePose;
			if (sharedMemoryIO.readUseCustomPose(unWhichDevice, useOverridePose) && useOverridePose) {
				// Static read
				DevicePose customPose;
				if (sharedMemoryIO.readOverridenPose(unWhichDevice, customPose)) {
					overridePose = ToDriverPose(customPose);
				}
			}
		}
	}

	// Call the original TrackedDevicePoseUpdated()
	if (originalTrackedDevicePoseUpdated) {
		(originalTrackedDevicePoseUpdated)(_this, unWhichDevice, overridePose, unPoseStructSize);
	}
}

void overrideCreateBooleanComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle) {
	// Call the original CreateBooleanComponent()
	if (originalCreateBooleanComponent) {
		(originalCreateBooleanComponent)(_this, ulContainer, pchName, pHandle);
	}

	LogManager::log(LOG_DEBUG, "CreateBooleanComponent called for component: {}", pchName);
}

void overrideUpdateBooleanComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset) {
	// Call the original UpdateBooleanComponent()
	if (originalUpdateBooleanComponent) {
		(originalUpdateBooleanComponent)(_this, ulComponent, bNewValue, fTimeOffset);
	}

	LogManager::log(LOG_DEBUG, "UpdateBooleanComponent called for component handle: {}", ulComponent);
}

void overrideCreateScalarComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits) {
	// Call the original CreateScalarComponent()
	if (originalCreateScalarComponent) {
		(originalCreateScalarComponent)(_this, ulContainer, pchName, pHandle, eType, eUnits);
	}
}

void overrideUpdateScalarComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset) {
	// Call the original UpdateScalarComponent()
	if (originalUpdateScalarComponent) {
		(originalUpdateScalarComponent)(_this, ulComponent, fNewValue, fTimeOffset);
	}
}

void overrideCreateHapticComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle) {
	// Call the original CreateHapticComponent()
	if (originalCreateHapticComponent) {
		(originalCreateHapticComponent)(_this, ulContainer, pchName, pHandle);
	}
}

void overrideCreateSkeletonComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, const char* pchSkeletonPath, const char* pchBasePosePath, vr::EVRSkeletalTrackingLevel eSkeletalTrackingLevel, const vr::VRBoneTransform_t* pGripLimitTransforms, uint32_t unGripLimitTransformCount, vr::VRInputComponentHandle_t* pHandle) {
	// Call the original CreateSkeletonComponent()
	if (originalCreateSkeletonComponent) {
		(originalCreateSkeletonComponent)(_this, ulContainer, pchName, pchSkeletonPath, pchBasePosePath, eSkeletalTrackingLevel, pGripLimitTransforms, unGripLimitTransformCount, pHandle);
	}
}

void overrideUpdateSkeletonComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount) {
	// Call the original UpdateSkeletonComponent()
	if (originalUpdateSkeletonComponent) {
		(originalUpdateSkeletonComponent)(_this, ulComponent, eMotionRange, pTransforms, unTransformCount);
	}
}

void overrideCreatePoseComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle) {
	// Call the original CreatePoseComponent()
	if (originalCreatePoseComponent) {
		(originalCreatePoseComponent)(_this, ulContainer, pchName, pHandle);
	}
}

void overrideUpdatePoseComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, const vr::HmdMatrix34_t* pMatPoseOffset, double fTimeOffset) {
	// Call the original UpdatePoseComponent()
	if (originalUpdatePoseComponent) {
		(originalUpdatePoseComponent)(_this, ulComponent, pMatPoseOffset, fTimeOffset);
	}
}

void overrideCreateEyeTrackingComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle) {
	// Call the original CreateEyeTrackingComponent()
	if (originalCreateEyeTrackingComponent) {
		(originalCreateEyeTrackingComponent)(_this, ulContainer, pchName, pHandle);
	}
}

void overrideUpdateEyeTrackingComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, const vr::VREyeTrackingData_t* pEyeTrackingData_t, double fTimeOffset) {
	// Call the original UpdateEyeTrackingComponent()
	if (originalUpdateEyeTrackingComponent) {
		(originalUpdateEyeTrackingComponent)(_this, ulComponent, pEyeTrackingData_t, fTimeOffset);
	}
}

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

	LogManager::log(LOG_INFO, "All IVRDriverInput methods hooked successfully");
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
	if (MH_CreateHook(targetFunction, detourFunction, originalFunction)
		!= MH_OK) {
		LogManager::log(LOG_ERROR, "MinHook hook creation failed for {}", name);
		return;
	}

	// Enable hook
	if (MH_EnableHook(targetFunction) != MH_OK) {
		LogManager::log(LOG_ERROR, "MinHook hook enable failed ({})", name);
		return;
	}
}