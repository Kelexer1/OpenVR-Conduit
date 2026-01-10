#include "HookManager.h"

#include "vrmath.h"

#include "LogManager.h"
#include "DeviceStateModelDriver.h"
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

typedef vr::PropertyContainerHandle_t(*_TrackedDeviceToPropertyContainer)(void* _this, vr::TrackedDeviceIndex_t nDevice);
static _TrackedDeviceToPropertyContainer originalTrackedDeviceToPropertyContainer = nullptr;

void overrideTrackedDevicePoseUpdated(void* _this, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize) {
	DriverDevicePoseSerialized* posePointer = DeviceStateModel::getInstance().getDevicePose(unWhichDevice);
	if (posePointer != nullptr) {
		posePointer->data.pose = FromDriverPose(newPose);
		DeviceStateModel::getInstance().setDevicePoseChanged(unWhichDevice);
	} else {
		DeviceStateModel::getInstance().addDevicePose(unWhichDevice); // Register a new device pose on first sighting
	}

	vr::DriverPose_t poseToSend = newPose;
	if (posePointer && posePointer->useOverridenState) {
		poseToSend = ToDriverPose(posePointer->data.overwrittenPose);
	}

	// Call the original TrackedDevicePoseUpdated()
	if (originalTrackedDevicePoseUpdated) {
		(originalTrackedDevicePoseUpdated)(_this, unWhichDevice, poseToSend, unPoseStructSize);
	}
}

void overrideCreateBooleanComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle) {
	// Register component to model
	DeviceStateModel::getInstance().addDevicePath(pchName);
	uint32_t deviceIndex = DeviceStateModel::getInstance().getDeviceIndexFromPropertyContainer(ulContainer);
	
	if (deviceIndex != static_cast<uint32_t>(-1)) {
		DeviceStateModel::getInstance().addBooleanInput(deviceIndex, pchName, pHandle);
	}

	// Call the original CreateBooleanComponent()
	if (originalCreateBooleanComponent) {
		(originalCreateBooleanComponent)(_this, ulContainer, pchName, pHandle);
	}
}

void overrideUpdateBooleanComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset) {
	DriverDeviceInputBooleanSerialized* input = DeviceStateModel::getInstance().getBooleanInput(ulComponent);

	if (input != nullptr) {
		input->data.value.value = bNewValue;
		input->data.value.timeOffset = fTimeOffset;
	}
	
	if (input && input->useOverridenState) {
		bNewValue = input->data.overwrittenValue.value;
		fTimeOffset = input->data.overwrittenValue.timeOffset;
	}

	// Call the original UpdateBooleanComponent()
	if (originalUpdateBooleanComponent) {
		(originalUpdateBooleanComponent)(_this, ulComponent, bNewValue, fTimeOffset);
	}
}

void overrideCreateScalarComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits) {
	// Register component to model
	DeviceStateModel::getInstance().addDevicePath(pchName);
	uint32_t deviceIndex = DeviceStateModel::getInstance().getDeviceIndexFromPropertyContainer(ulContainer);

	if (deviceIndex != static_cast<uint32_t>(-1)) {
		DeviceStateModel::getInstance().addScalarInput(deviceIndex, pchName, pHandle);
	}
	
	// Call the original CreateScalarComponent()
	if (originalCreateScalarComponent) {
		(originalCreateScalarComponent)(_this, ulContainer, pchName, pHandle, eType, eUnits);
	}
}

void overrideUpdateScalarComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset) {
	DriverDeviceInputScalarSerialized* input = DeviceStateModel::getInstance().getScalarInput(ulComponent);

	if (input != nullptr) {
		input->data.value.value = fNewValue;
		input->data.value.timeOffset = fTimeOffset;
	}

	if (input && input->useOverridenState) {
		fNewValue = input->data.overwrittenValue.value;
		fTimeOffset = input->data.overwrittenValue.timeOffset;
	}

	// Call the original UpdateScalarComponent()
	if (originalUpdateScalarComponent) {
		(originalUpdateScalarComponent)(_this, ulComponent, fNewValue, fTimeOffset);
	}
}

void overrideCreateHapticComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle) {
	// We dont actually store haptic inputs in the model since they can be managed from applications
	// the hook is just here if its needed in the future, and for consistency
	
	// Call the original CreateHapticComponent()
	if (originalCreateHapticComponent) {
		(originalCreateHapticComponent)(_this, ulContainer, pchName, pHandle);
	}
}

void overrideCreateSkeletonComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, const char* pchSkeletonPath, const char* pchBasePosePath, vr::EVRSkeletalTrackingLevel eSkeletalTrackingLevel, const vr::VRBoneTransform_t* pGripLimitTransforms, uint32_t unGripLimitTransformCount, vr::VRInputComponentHandle_t* pHandle) {
	// Register component to model
	DeviceStateModel::getInstance().addDevicePath(pchName);
	uint32_t deviceIndex = DeviceStateModel::getInstance().getDeviceIndexFromPropertyContainer(ulContainer);

	if (deviceIndex != static_cast<uint32_t>(-1)) {
		DeviceStateModel::getInstance().addSkeletonInput(deviceIndex, pchName, pHandle);
	}
	
	// Call the original CreateSkeletonComponent()
	if (originalCreateSkeletonComponent) {
		(originalCreateSkeletonComponent)(_this, ulContainer, pchName, pchSkeletonPath, pchBasePosePath, eSkeletalTrackingLevel, pGripLimitTransforms, unGripLimitTransformCount, pHandle);
	}
}

void overrideUpdateSkeletonComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount) {
	DriverDeviceInputSkeletonSerialized* input = DeviceStateModel::getInstance().getSkeletonInput(ulComponent);

	if (input != nullptr) {
		input->data.value.motionRange = static_cast<SkeletalMotionRange>(eMotionRange);
		for (uint32_t i = 0; i < unTransformCount && i < 31; ++i) {
			input->data.value.boneTransforms[i].position.v[0] = pTransforms[i].position.v[0];
			input->data.value.boneTransforms[i].position.v[1] = pTransforms[i].position.v[1];
			input->data.value.boneTransforms[i].position.v[2] = pTransforms[i].position.v[2];
			input->data.value.boneTransforms[i].position.v[3] = pTransforms[i].position.v[3];
			input->data.value.boneTransforms[i].orientation.w = pTransforms[i].orientation.w;
			input->data.value.boneTransforms[i].orientation.x = pTransforms[i].orientation.x;
			input->data.value.boneTransforms[i].orientation.y = pTransforms[i].orientation.y;
			input->data.value.boneTransforms[i].orientation.z = pTransforms[i].orientation.z;
		}
		input->data.value.boneTransformCount = unTransformCount;
	}
	
	if (input && input->useOverridenState) {
		eMotionRange = static_cast<vr::EVRSkeletalMotionRange>(input->data.overwrittenValue.motionRange);
		pTransforms = reinterpret_cast<const vr::VRBoneTransform_t*>(input->data.overwrittenValue.boneTransforms);
		unTransformCount = input->data.overwrittenValue.boneTransformCount;
	}

	// Call the original UpdateSkeletonComponent()
	if (originalUpdateSkeletonComponent) {
		(originalUpdateSkeletonComponent)(_this, ulComponent, eMotionRange, pTransforms, unTransformCount);
	}
}

void overrideCreatePoseComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle) {
	// Register component to model
	DeviceStateModel::getInstance().addDevicePath(pchName);
	uint32_t deviceIndex = DeviceStateModel::getInstance().getDeviceIndexFromPropertyContainer(ulContainer);

	if (deviceIndex != static_cast<uint32_t>(-1)) {
		DeviceStateModel::getInstance().addPoseInput(deviceIndex, pchName, pHandle);
	}
	
	// Call the original CreatePoseComponent()
	if (originalCreatePoseComponent) {
		(originalCreatePoseComponent)(_this, ulContainer, pchName, pHandle);
	}
}

void overrideUpdatePoseComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, const vr::HmdMatrix34_t* pMatPoseOffset, double fTimeOffset) {
	DriverDeviceInputPoseSerialized* input = DeviceStateModel::getInstance().getPoseInput(ulComponent);

	if (input != nullptr) {
		if (pMatPoseOffset != nullptr) {
			for (int i = 0; i < 3; ++i) {
				for (int j = 0; j < 4; ++j) {
					input->data.value.poseOffset.m[i][j] = static_cast<double>(pMatPoseOffset->m[i][j]);
				}
			}
		}
		input->data.value.timeOffset = fTimeOffset;
	}

	if (input && input->useOverridenState) {
		vr::HmdMatrix34_t overwrittenMatrix;
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 4; ++j) {
				overwrittenMatrix.m[i][j] = static_cast<float>(input->data.overwrittenValue.poseOffset.m[i][j]);
			}
		}
		pMatPoseOffset = &overwrittenMatrix;
		fTimeOffset = input->data.overwrittenValue.timeOffset;
	}

	// Call the original UpdatePoseComponent()
	if (originalUpdatePoseComponent) {
		(originalUpdatePoseComponent)(_this, ulComponent, pMatPoseOffset, fTimeOffset);
	}
}

void overrideCreateEyeTrackingComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle) {
	// Register component to model
	DeviceStateModel::getInstance().addDevicePath(pchName);
	uint32_t deviceIndex = DeviceStateModel::getInstance().getDeviceIndexFromPropertyContainer(ulContainer);

	if (deviceIndex != static_cast<uint32_t>(-1)) {
		DeviceStateModel::getInstance().addEyeTrackingInput(deviceIndex, pchName, pHandle);
	}
	
	// Call the original CreateEyeTrackingComponent()
	if (originalCreateEyeTrackingComponent) {
		(originalCreateEyeTrackingComponent)(_this, ulContainer, pchName, pHandle);
	}
}

void overrideUpdateEyeTrackingComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, const vr::VREyeTrackingData_t* pEyeTrackingData_t, double fTimeOffset) {
	DriverDeviceInputEyeTrackingSerialized* input = DeviceStateModel::getInstance().getEyeTrackingInput(ulComponent);

	if (input != nullptr) {
		input->data.value.eyeTrackingData.active = pEyeTrackingData_t->bActive;
		input->data.value.eyeTrackingData.valid = pEyeTrackingData_t->bValid;
		input->data.value.eyeTrackingData.tracked = pEyeTrackingData_t->bTracked;
		for (int i = 0; i < 3; ++i) {
			input->data.value.eyeTrackingData.gazeOrigin.v[i] = static_cast<double>(pEyeTrackingData_t->vGazeOrigin.v[i]);
			input->data.value.eyeTrackingData.gazeTarget.v[i] = static_cast<double>(pEyeTrackingData_t->vGazeTarget.v[i]);
		}
		input->data.value.timeOffset = fTimeOffset;
	}
	
	if (input && input->useOverridenState) {
		vr::VREyeTrackingData_t overwrittenData;
		overwrittenData.bActive = input->data.overwrittenValue.eyeTrackingData.active;
		overwrittenData.bValid = input->data.overwrittenValue.eyeTrackingData.valid;
		overwrittenData.bTracked = input->data.overwrittenValue.eyeTrackingData.tracked;
		for (int i = 0; i < 3; ++i) {
			overwrittenData.vGazeOrigin.v[i] = static_cast<float>(input->data.overwrittenValue.eyeTrackingData.gazeOrigin.v[i]);
			overwrittenData.vGazeTarget.v[i] = static_cast<float>(input->data.overwrittenValue.eyeTrackingData.gazeTarget.v[i]);
		}
		pEyeTrackingData_t = &overwrittenData;
		fTimeOffset = input->data.overwrittenValue.timeOffset;
	}

	// Call the original UpdateEyeTrackingComponent()
	if (originalUpdateEyeTrackingComponent) {
		(originalUpdateEyeTrackingComponent)(_this, ulComponent, pEyeTrackingData_t, fTimeOffset);
	}
}

vr::PropertyContainerHandle_t overrideTrackedDeviceToPropertyContainer(void* _this, vr::TrackedDeviceIndex_t nDevice) {
	// Call the original TrackedDeviceToPropertyContainer()
	if (originalTrackedDeviceToPropertyContainer) {
		vr::PropertyContainerHandle_t result = (originalTrackedDeviceToPropertyContainer)(_this, nDevice);

		// Save mapping
		DeviceStateModel::getInstance().addDeviceIndexToContainerMapping(nDevice, result);

		return result;
	}

	return vr::k_ulInvalidPropertyContainer;
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