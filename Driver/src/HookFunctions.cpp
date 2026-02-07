#include "HookFunctions.h"

void* IVRServerDriverHost = nullptr;
void* IVRDriverInput = nullptr;

_TrackedDevicePoseUpdated originalTrackedDevicePoseUpdated = nullptr;
_CreateBooleanComponent originalCreateBooleanComponent = nullptr;
_UpdateBooleanComponent originalUpdateBooleanComponent = nullptr;
_CreateScalarComponent originalCreateScalarComponent = nullptr;
_UpdateScalarComponent originalUpdateScalarComponent = nullptr;
_CreateHapticComponent originalCreateHapticComponent = nullptr;
_CreateSkeletonComponent originalCreateSkeletonComponent = nullptr;
_UpdateSkeletonComponent originalUpdateSkeletonComponent = nullptr;
_CreatePoseComponent originalCreatePoseComponent = nullptr;
_UpdatePoseComponent originalUpdatePoseComponent = nullptr;
_CreateEyeTrackingComponent originalCreateEyeTrackingComponent = nullptr;
_UpdateEyeTrackingComponent originalUpdateEyeTrackingComponent = nullptr;
_TrackedDeviceToPropertyContainer originalTrackedDeviceToPropertyContainer = nullptr;

void callTrackedDevicePoseUpdated(uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize) {
	if (IVRServerDriverHost) {
		originalTrackedDevicePoseUpdated(IVRServerDriverHost, unWhichDevice, newPose, unPoseStructSize);
	}
}

vr::EVRInputError callUpdateBooleanComponent(
	vr::VRInputComponentHandle_t ulComponent,
	bool bNewValue,
	double fTimeOffset
) {
	if (IVRDriverInput) return originalUpdateBooleanComponent(IVRDriverInput, ulComponent, bNewValue, fTimeOffset);
	
	return vr::VRInputError_NoData;
}

vr::EVRInputError callUpdateScalarComponent(
	vr::VRInputComponentHandle_t ulComponent,
	float fNewValue,
	double fTimeOffset
) {
	if (IVRDriverInput) return originalUpdateScalarComponent(IVRDriverInput, ulComponent, fNewValue, fTimeOffset);

	return vr::VRInputError_NoData;
}

vr::EVRInputError callUpdateSkeletonComponent(
	vr::VRInputComponentHandle_t ulComponent,
	vr::EVRSkeletalMotionRange eMotionRange,
	const vr::VRBoneTransform_t* pTransforms,
	uint32_t unTransformCount
) {
	if (IVRDriverInput) return originalUpdateSkeletonComponent(
		IVRDriverInput,
		ulComponent,
		eMotionRange,
		pTransforms,
		unTransformCount
	);

	return vr::VRInputError_NoData;
}

vr::EVRInputError callUpdatePoseComponent(
	vr::VRInputComponentHandle_t ulComponent,
	const vr::HmdMatrix34_t* pMatPoseOffset,
	double fTimeOffset
) {
	if (IVRDriverInput) return originalUpdatePoseComponent(IVRDriverInput, ulComponent, pMatPoseOffset, fTimeOffset);

	return vr::VRInputError_NoData;
}

vr::EVRInputError callUpdateEyeTrackingComponent(
	vr::VRInputComponentHandle_t ulComponent,
	const vr::VREyeTrackingData_t* pEyeTrackingData_t,
	double fTimeOffset
) {
	if (IVRDriverInput) return originalUpdateEyeTrackingComponent(
		IVRDriverInput,
		ulComponent,
		pEyeTrackingData_t,
		fTimeOffset
	);

	return vr::VRInputError_NoData;
}

void overrideTrackedDevicePoseUpdated(
	void* _this,
	uint32_t unWhichDevice,
	const vr::DriverPose_t& newPose,
	uint32_t unPoseStructSize
) {
	ModelDevicePoseSerialized* posePointer = DeviceStateModel::getInstance().getDevicePose(unWhichDevice);
	if (posePointer != nullptr) {
		posePointer->data.pose = FromDriverPose(newPose);
		DeviceStateModel::getInstance().setDevicePoseChanged(unWhichDevice);
	} else {
		DeviceStateModel::getInstance().addDevicePose(unWhichDevice); // Register a new device pose on first sighting
	}

	vr::DriverPose_t poseToSend = newPose;
	if (posePointer && posePointer->useOverriddenState) {
		poseToSend = ToDriverPose(posePointer->data.overwrittenPose);
	}

	// Call the original TrackedDevicePoseUpdated()
	if (originalTrackedDevicePoseUpdated) (originalTrackedDevicePoseUpdated)(
		_this,
		unWhichDevice,
		poseToSend,
		unPoseStructSize
	);
}

vr::EVRInputError overrideCreateBooleanComponent(
	void* _this,
	vr::PropertyContainerHandle_t ulContainer,
	const char* pchName,
	vr::VRInputComponentHandle_t* pHandle
) {
	// Call the original CreateBooleanComponent()
	vr::EVRInputError result = vr::VRInputError_None;
	if (originalCreateBooleanComponent) result = (originalCreateBooleanComponent)(
		_this,
		ulContainer,
		pchName,
		pHandle
	);

	// Register component to model
	uint32_t deviceIndex = DeviceStateModel::getInstance().getDeviceIndexFromPropertyContainer(ulContainer);
	if (deviceIndex != UINT32_MAX) DeviceStateModel::getInstance().addBooleanInput(deviceIndex, pchName, pHandle);

	return result;
}

vr::EVRInputError overrideUpdateBooleanComponent(
	void* _this,
	vr::VRInputComponentHandle_t ulComponent,
	bool bNewValue,
	double fTimeOffset
) {
	ModelDeviceInputBooleanSerialized* input = DeviceStateModel::getInstance().getBooleanInput(ulComponent);

	if (input != nullptr) {
		input->data.value.value = bNewValue;
		input->data.value.timeOffset = fTimeOffset;
		DeviceStateModel::getInstance().setInputBooleanChanged(ulComponent);
	}

	if (input && input->useOverriddenState) {
		bNewValue = input->data.overwrittenValue.value;
		fTimeOffset = input->data.overwrittenValue.timeOffset;
	}

	// Call the original UpdateBooleanComponent()
	if (originalUpdateBooleanComponent) return (originalUpdateBooleanComponent)(
		_this,
		ulComponent,
		bNewValue,
		fTimeOffset
	);

	return vr::VRInputError_NoData;
}

vr::EVRInputError overrideCreateScalarComponent(
	void* _this,
	vr::PropertyContainerHandle_t ulContainer,
	const char* pchName,
	vr::VRInputComponentHandle_t* pHandle,
	vr::EVRScalarType eType,
	vr::EVRScalarUnits eUnits
) {
	// Call the original CreateScalarComponent()
	vr::EVRInputError result = vr::VRInputError_None;
	if (originalCreateScalarComponent) result = (originalCreateScalarComponent)(
		_this,
		ulContainer,
		pchName,
		pHandle,
		eType,
		eUnits
	);

	// Register component to model
	uint32_t deviceIndex = DeviceStateModel::getInstance().getDeviceIndexFromPropertyContainer(ulContainer);
	if (deviceIndex != UINT32_MAX) DeviceStateModel::getInstance().addScalarInput(deviceIndex, pchName, pHandle);

	return result;
}

vr::EVRInputError overrideUpdateScalarComponent(
	void* _this,
	vr::VRInputComponentHandle_t ulComponent,
	float fNewValue,
	double fTimeOffset
) {
	ModelDeviceInputScalarSerialized* input = DeviceStateModel::getInstance().getScalarInput(ulComponent);

	if (input != nullptr) {
		input->data.value.value = fNewValue;
		input->data.value.timeOffset = fTimeOffset;
		DeviceStateModel::getInstance().setInputScalarChanged(ulComponent);
	}

	if (input && input->useOverriddenState) {
		fNewValue = input->data.overwrittenValue.value;
		fTimeOffset = input->data.overwrittenValue.timeOffset;
	}

	// Call the original UpdateScalarComponent()
	if (originalUpdateScalarComponent) return (originalUpdateScalarComponent)(
		_this,
		ulComponent,
		fNewValue,
		fTimeOffset
	);

	return vr::VRInputError_NoData;
}

vr::EVRInputError overrideCreateHapticComponent(
	void* _this,
	vr::PropertyContainerHandle_t ulContainer,
	const char* pchName,
	vr::VRInputComponentHandle_t* pHandle
) {
	// We dont actually store haptic inputs in the model since they can be managed from applications
	// the hook is just here if its needed in the future, and for consistency

	// Call the original CreateHapticComponent()
	if (originalCreateHapticComponent) return (originalCreateHapticComponent)(_this, ulContainer, pchName, pHandle);

	return vr::VRInputError_NoData;
}

vr::EVRInputError overrideCreateSkeletonComponent(
	void* _this,
	vr::PropertyContainerHandle_t ulContainer,
	const char* pchName,
	const char* pchSkeletonPath,
	const char* pchBasePosePath,
	vr::EVRSkeletalTrackingLevel eSkeletalTrackingLevel,
	const vr::VRBoneTransform_t* pGripLimitTransforms,
	uint32_t unGripLimitTransformCount,
	vr::VRInputComponentHandle_t* pHandle
) {
	// Call the original CreateSkeletonComponent()
	vr::EVRInputError result = vr::VRInputError_None;
	if (originalCreateSkeletonComponent) result = (originalCreateSkeletonComponent)(
		_this,
		ulContainer,
		pchName,
		pchSkeletonPath,
		pchBasePosePath,
		eSkeletalTrackingLevel,
		pGripLimitTransforms,
		unGripLimitTransformCount,
		pHandle
	);

	// Register component to model
	uint32_t deviceIndex = DeviceStateModel::getInstance().getDeviceIndexFromPropertyContainer(ulContainer);
	if (deviceIndex != UINT32_MAX) DeviceStateModel::getInstance().addSkeletonInput(deviceIndex, pchName, pHandle);

	return result;
}

vr::EVRInputError overrideUpdateSkeletonComponent(
	void* _this,
	vr::VRInputComponentHandle_t ulComponent,
	vr::EVRSkeletalMotionRange eMotionRange,
	const vr::VRBoneTransform_t* pTransforms,
	uint32_t unTransformCount
) {
	ModelDeviceInputSkeletonSerialized* input = DeviceStateModel::getInstance().getSkeletonInput(ulComponent);

	if (input != nullptr) {
		input->data.value.motionRange = static_cast<SkeletalMotionRange>(eMotionRange);
		FromVRBoneTransforms(pTransforms, unTransformCount, input->data.value);
		input->data.value.boneTransformCount = unTransformCount;
		DeviceStateModel::getInstance().setInputSkeletonChanged(ulComponent);
	}

	const vr::VRBoneTransform_t* transforms = pTransforms;
	
	if (input && input->useOverriddenState) {
		eMotionRange = static_cast<vr::EVRSkeletalMotionRange>(input->data.overwrittenValue.motionRange);
		vr::VRBoneTransform_t overwrittenTransforms[31];
		ToVRBoneTransforms(input->data.overwrittenValue, overwrittenTransforms);
		transforms = overwrittenTransforms;
		unTransformCount = input->data.overwrittenValue.boneTransformCount;
	}

	// Call the original UpdateSkeletonComponent()
	if (originalUpdateSkeletonComponent) return (originalUpdateSkeletonComponent)(
		_this,
		ulComponent,
		eMotionRange,
		transforms,
		unTransformCount
	);

	return vr::VRInputError_NoData;
}

vr::EVRInputError overrideCreatePoseComponent(
	void* _this,
	vr::PropertyContainerHandle_t ulContainer,
	const char* pchName,
	vr::VRInputComponentHandle_t* pHandle
) {
	// Call the original CreatePoseComponent()
	vr::EVRInputError result = vr::VRInputError_None;
	if (originalCreatePoseComponent) result = (originalCreatePoseComponent)(_this, ulContainer, pchName, pHandle);

	// Register component to model
	uint32_t deviceIndex = DeviceStateModel::getInstance().getDeviceIndexFromPropertyContainer(ulContainer);
	if (deviceIndex != UINT32_MAX) DeviceStateModel::getInstance().addPoseInput(deviceIndex, pchName, pHandle);

	return result;
}

vr::EVRInputError overrideUpdatePoseComponent(
	void* _this,
	vr::VRInputComponentHandle_t ulComponent,
	const vr::HmdMatrix34_t* pMatPoseOffset,
	double fTimeOffset
) {
	ModelDeviceInputPoseSerialized* input = DeviceStateModel::getInstance().getPoseInput(ulComponent);

	if (input != nullptr) {
		if (pMatPoseOffset != nullptr) input->data.value.poseOffset = FromHmdMatrix34(*pMatPoseOffset);
		input->data.value.timeOffset = fTimeOffset;
		DeviceStateModel::getInstance().setInputPoseChanged(ulComponent);
	}

	const vr::HmdMatrix34_t* matrixToSend = pMatPoseOffset;

	if (input && input->useOverriddenState) {
		vr::HmdMatrix34_t overwrittenMatrix = ToHmdMatrix34(input->data.overwrittenValue.poseOffset);
		matrixToSend = &overwrittenMatrix;
		fTimeOffset = input->data.overwrittenValue.timeOffset;
	}

	// Call the original UpdatePoseComponent()
	if (originalUpdatePoseComponent) return (originalUpdatePoseComponent)(
		_this,
		ulComponent,
		matrixToSend,
		fTimeOffset
	);

	return vr::VRInputError_NoData;
}

vr::EVRInputError overrideCreateEyeTrackingComponent(
	void* _this,
	vr::PropertyContainerHandle_t ulContainer,
	const char* pchName,
	vr::VRInputComponentHandle_t* pHandle
) {
	// Call the original CreateEyeTrackingComponent()
	vr::EVRInputError result = vr::VRInputError_None;
	if (originalCreateEyeTrackingComponent) result = (originalCreateEyeTrackingComponent)(
		_this,
		ulContainer,
		pchName,
		pHandle
	);

	// Register component to model
	uint32_t deviceIndex = DeviceStateModel::getInstance().getDeviceIndexFromPropertyContainer(ulContainer);
	if (deviceIndex != UINT32_MAX) DeviceStateModel::getInstance().addEyeTrackingInput(deviceIndex, pchName, pHandle);

	return result;
}

vr::EVRInputError overrideUpdateEyeTrackingComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, const vr::VREyeTrackingData_t* pEyeTrackingData_t, double fTimeOffset) {
	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModel::getInstance().getEyeTrackingInput(ulComponent);

	if (input != nullptr) {
		input->data.value.eyeTrackingData = FromVREyeTrackingData(*pEyeTrackingData_t);
		input->data.value.timeOffset = fTimeOffset;
		DeviceStateModel::getInstance().setInputEyeTrackingChanged(ulComponent);
	}

	const vr::VREyeTrackingData_t* eyeTrackingDataToSend = pEyeTrackingData_t;

	if (input && input->useOverriddenState) {
		vr::VREyeTrackingData_t overwrittenEyeTrackingData = ToVREyeTrackingData(input->data.overwrittenValue.eyeTrackingData);
		eyeTrackingDataToSend = &overwrittenEyeTrackingData;
		fTimeOffset = input->data.overwrittenValue.timeOffset;
	}

	// Call the original UpdateEyeTrackingComponent()
	if (originalUpdateEyeTrackingComponent) return (originalUpdateEyeTrackingComponent)(
		_this,
		ulComponent,
		eyeTrackingDataToSend,
		fTimeOffset
	);

	return vr::VRInputError_NoData;
}

vr::PropertyContainerHandle_t overrideTrackedDeviceToPropertyContainer(void* _this, vr::TrackedDeviceIndex_t nDevice) {
	if (originalTrackedDeviceToPropertyContainer) {
		// Call the original TrackedDeviceToPropertyContainer()
		vr::PropertyContainerHandle_t result = (originalTrackedDeviceToPropertyContainer)(_this, nDevice);

		// Save mapping
		DeviceStateModel::getInstance().addDeviceIndexToContainerMapping(nDevice, result);

		return result;
	}

	return vr::k_ulInvalidPropertyContainer;
}