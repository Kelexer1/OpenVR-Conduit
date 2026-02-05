#pragma once
#include "openvr_driver.h"
#include "ObjectSchemas.h"
#include "DeviceStateModelDriver.h"
#include "Utils.h"

/** @brief A pointer to the internal OpenVR IVRServerDriverHost instance */
extern void* IVRServerDriverHost;

/** @brief A pointer to the internal OpenVR IVRDriverInput instance */
extern void* IVRDriverInput;

/**
 * @brief The following functions are wrappers for their OpenVR equivalents, used to push overriden data to the OpenVR
 * runtime. Since they are wrappers of existing methods, documentation is ommited. Original documentation for the
 * wrapped methods is available in the OpenVR SDK
 */

void overrideTrackedDevicePoseUpdated(void* _this, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize);
vr::EVRInputError overrideCreateBooleanComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);
vr::EVRInputError overrideUpdateBooleanComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset);
vr::EVRInputError overrideCreateScalarComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits);
vr::EVRInputError overrideUpdateScalarComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset);
vr::EVRInputError overrideCreateHapticComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);
vr::EVRInputError overrideCreateSkeletonComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, const char* pchSkeletonPath, const char* pchBasePosePath, vr::EVRSkeletalTrackingLevel eSkeletalTrackingLevel, const vr::VRBoneTransform_t* pGripLimitTransforms, uint32_t unGripLimitTransformCount, vr::VRInputComponentHandle_t* pHandle);
vr::EVRInputError overrideUpdateSkeletonComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount);
vr::EVRInputError overrideCreatePoseComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);
vr::EVRInputError overrideUpdatePoseComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, const vr::HmdMatrix34_t* pMatPoseOffset, double fTimeOffset);
vr::EVRInputError overrideCreateEyeTrackingComponent(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);
vr::EVRInputError overrideUpdateEyeTrackingComponent(void* _this, vr::VRInputComponentHandle_t ulComponent, const vr::VREyeTrackingData_t* pEyeTrackingData_t, double fTimeOffset);
vr::PropertyContainerHandle_t overrideTrackedDeviceToPropertyContainer(void* _this, vr::TrackedDeviceIndex_t nDevice);

/**
 * @brief The following functions are wrappers of the wrapper methods above, shifting the responsibility of supplying
 * the correct class instance pointer away from callers. Documentation is ommited. Source documentation is available
 * in the OpenVR SDK for the original methods.
 */

void callTrackedDevicePoseUpdated(uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize);
vr::EVRInputError callUpdateBooleanComponent(vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset);
vr::EVRInputError callUpdateScalarComponent(vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset);
vr::EVRInputError callUpdateSkeletonComponent(vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount);
vr::EVRInputError callUpdatePoseComponent(vr::VRInputComponentHandle_t ulComponent, const vr::HmdMatrix34_t* pMatPoseOffset, double fTimeOffset);
vr::EVRInputError callUpdateEyeTrackingComponent(vr::VRInputComponentHandle_t ulComponent, const vr::VREyeTrackingData_t* pEyeTrackingData_t, double fTimeOffset);

/**
 * @brief The following functions are function pointers for OpenVR methods used to push overriden state. Since these
 * methods are not implemented by Conduit, documentation is ommited. Source documentation is available in the OpenVR
 * SDK
 */

typedef void(*_TrackedDevicePoseUpdated)(void* _this, uint32_t unWhichDevice, const vr::DriverPose_t& newPose, uint32_t unPoseStructSize);
extern _TrackedDevicePoseUpdated originalTrackedDevicePoseUpdated;

typedef vr::EVRInputError(*_CreateBooleanComponent)(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);
extern _CreateBooleanComponent originalCreateBooleanComponent;

typedef vr::EVRInputError(*_UpdateBooleanComponent)(void* _this, vr::VRInputComponentHandle_t ulComponent, bool bNewValue, double fTimeOffset);
extern _UpdateBooleanComponent originalUpdateBooleanComponent;

typedef vr::EVRInputError(*_CreateScalarComponent)(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle, vr::EVRScalarType eType, vr::EVRScalarUnits eUnits);
extern _CreateScalarComponent originalCreateScalarComponent;

typedef vr::EVRInputError(*_UpdateScalarComponent)(void* _this, vr::VRInputComponentHandle_t ulComponent, float fNewValue, double fTimeOffset);
extern _UpdateScalarComponent originalUpdateScalarComponent;

typedef vr::EVRInputError(*_CreateHapticComponent)(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);
extern _CreateHapticComponent originalCreateHapticComponent;

typedef vr::EVRInputError(*_CreateSkeletonComponent)(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, const char* pchSkeletonPath, const char* pchBasePosePath, vr::EVRSkeletalTrackingLevel eSkeletalTrackingLevel, const vr::VRBoneTransform_t* pGripLimitTransforms, uint32_t unGripLimitTransformCount, vr::VRInputComponentHandle_t* pHandle);
extern _CreateSkeletonComponent originalCreateSkeletonComponent;

typedef vr::EVRInputError(*_UpdateSkeletonComponent)(void* _this, vr::VRInputComponentHandle_t ulComponent, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount);
extern _UpdateSkeletonComponent originalUpdateSkeletonComponent;

typedef vr::EVRInputError(*_CreatePoseComponent)(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);
extern _CreatePoseComponent originalCreatePoseComponent;

typedef vr::EVRInputError(*_UpdatePoseComponent)(void* _this, vr::VRInputComponentHandle_t ulComponent, const vr::HmdMatrix34_t* pMatPoseOffset, double fTimeOffset);
extern _UpdatePoseComponent originalUpdatePoseComponent;

typedef vr::EVRInputError(*_CreateEyeTrackingComponent)(void* _this, vr::PropertyContainerHandle_t ulContainer, const char* pchName, vr::VRInputComponentHandle_t* pHandle);
extern _CreateEyeTrackingComponent originalCreateEyeTrackingComponent;

typedef vr::EVRInputError(*_UpdateEyeTrackingComponent)(void* _this, vr::VRInputComponentHandle_t ulComponent, const vr::VREyeTrackingData_t* pEyeTrackingData_t, double fTimeOffset);
extern _UpdateEyeTrackingComponent originalUpdateEyeTrackingComponent;

typedef vr::PropertyContainerHandle_t(*_TrackedDeviceToPropertyContainer)(void* _this, vr::TrackedDeviceIndex_t nDevice);
extern _TrackedDeviceToPropertyContainer originalTrackedDeviceToPropertyContainer;