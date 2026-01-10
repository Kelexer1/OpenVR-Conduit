//#pragma once
//#include "SharedDeviceMemory.h"
//#include "openvr_driver.h"
//
///**
// * @brief Sends commands to the DeviceModifier driver by updating shared memory
// */
//class DeviceInputCommandSender {
//public:
//	/**
//	 * @brief Constructs a new DeviceInputCommandSender associated with a shared memory IO
//	 * @param sharedMemory The associated shared memory IO
//	 */
//	DeviceInputCommandSender(SharedDeviceMemoryIO* sharedMemory);
//
//	// Boolean Components
//	void setBooleanComponentState(uint32_t deviceIndex, const char* pchName, bool newValue, double timeOffset);
//	void getBooleanComponentState(uint32_t deviceIndex, const char* pchName, bool& outValue, double& outTimeOffset);
//	void setUseOverridenBooleanComponentState(uint32_t deviceIndex, const char* pchName, bool shouldOverride);
//	bool getUseOverridenBooleanComponentState(uint32_t deviceIndex, const char* pchName);
//	void setUseBooleanPingPong(uint32_t deviceIndex, const char* pchName, bool usePingPong);
//	bool getUseBooleanPingPong(uint32_t deviceIndex, const char* pchName);
//	void getNaturalBooleanComponentState(uint32_t deviceIndex, const char* pchName, bool& outValue, double& outTimeOffset);
//
//	// Scalar Components
//	void setScalarComponentState(uint32_t deviceIndex, const char* pchName, float newValue, double timeOffset);
//	void getScalarComponentState(uint32_t deviceIndex, const char* pchName, float& outValue, double& outTimeOffset);
//	void setUseOverridenScalarComponentState(uint32_t deviceIndex, const char* pchName, bool shouldOverride);
//	bool getUseOverridenScalarComponentState(uint32_t deviceIndex, const char* pchName);
//	void setUseScalarPingPong(uint32_t deviceIndex, const char* pchName, bool usePingPong);
//	bool getUseScalarPingPong(uint32_t deviceIndex, const char* pchName);
//	void getNaturalScalarComponentState(uint32_t deviceIndex, const char* pchName, float& outValue, double& outTimeOffset);
//
//	// Skeleton Components
//	void setSkeletonComponentState(uint32_t deviceIndex, const char* pchName, vr::EVRSkeletalMotionRange eMotionRange, const vr::VRBoneTransform_t* pTransforms, uint32_t unTransformCount);
//	void getSkeletonComponentState(uint32_t deviceIndex, const char* pchName, vr::EVRSkeletalMotionRange& outMotionRange, vr::VRBoneTransform_t* outTransforms, uint32_t& outTransformCount);
//	void setUseOverridenSkeletonComponentState(uint32_t deviceIndex, const char* pchName, bool shouldOverride);
//	bool getUseOverridenSkeletonComponentState(uint32_t deviceIndex, const char* pchName);
//	void setUseSkeletonPingPong(uint32_t deviceIndex, const char* pchName, bool usePingPong);
//	bool getUseSkeletonPingPong(uint32_t deviceIndex, const char* pchName);
//	void getNaturalSkeletonComponentState(uint32_t deviceIndex, const char* pchName, vr::EVRSkeletalMotionRange& outMotionRange, vr::VRBoneTransform_t* outTransforms, uint32_t& outTransformCount);
//
//	// Pose Components
//	void setPoseComponentState(uint32_t deviceIndex, const char* pchName, const vr::HmdMatrix34_t& matPoseOffset, double timeOffset);
//	void getPoseComponentState(uint32_t deviceIndex, const char* pchName, vr::HmdMatrix34_t& outMatPoseOffset, double& outTimeOffset);
//	void setUseOverridenPoseComponentState(uint32_t deviceIndex, const char* pchName, bool shouldOverride);
//	bool getUseOverridenPoseComponentState(uint32_t deviceIndex, const char* pchName);
//	void setUsePosePingPong(uint32_t deviceIndex, const char* pchName, bool usePingPong);
//	bool getUsePosePingPong(uint32_t deviceIndex, const char* pchName);
//	void getNaturalPoseComponentState(uint32_t deviceIndex, const char* pchName, vr::HmdMatrix34_t& outMatPoseOffset, double& outTimeOffset);
//
//	// Eye Tracking Components
//	void setEyeTrackingComponentState(uint32_t deviceIndex, const char* pchName, const vr::VREyeTrackingData_t& eyeTrackingData, double timeOffset);
//	void getEyeTrackingComponentState(uint32_t deviceIndex, const char* pchName, vr::VREyeTrackingData_t& outEyeTrackingData, double& outTimeOffset);
//	void setUseOverridenEyeTrackingComponentState(uint32_t deviceIndex, const char* pchName, bool shouldOverride);
//	bool getUseOverridenEyeTrackingComponentState(uint32_t deviceIndex, const char* pchName);
//	void setUseEyeTrackingPingPong(uint32_t deviceIndex, const char* pchName, bool usePingPong);
//	bool getUseEyeTrackingPingPong(uint32_t deviceIndex, const char* pchName);
//	void getNaturalEyeTrackingComponentState(uint32_t deviceIndex, const char* pchName, vr::VREyeTrackingData_t& outEyeTrackingData, double& outTimeOffset);
//
//private:
//	/// The associated shared memory
//	SharedDeviceMemoryIO* sharedMemory;
//};