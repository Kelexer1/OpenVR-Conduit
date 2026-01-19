#pragma once
#include <stdint.h>
#include <string>

#include "DeviceTypes.h"

class IDeviceStateEventReciever {
public:
	virtual void DeviceInputBooleanAdded(uint32_t deviceIndex, const std::string& path) = 0;
	virtual void DeviceInputBooleanRemoved(uint32_t deviceIndex, const std::string& path) = 0;
	virtual void DeviceInputScalarAdded(uint32_t deviceIndex, const std::string& path) = 0;
	virtual void DeviceInputScalarRemoved(uint32_t deviceIndex, const std::string& path) = 0;
	virtual void DeviceInputSkeletonAdded(uint32_t deviceIndex, const std::string& path) = 0;
	virtual void DeviceInputSkeletonRemoved(uint32_t deviceIndex, const std::string& path) = 0;
	virtual void DeviceInputPoseAdded(uint32_t deviceIndex, const std::string& path) = 0;
	virtual void DeviceInputPoseRemoved(uint32_t deviceIndex, const std::string& path) = 0;
	virtual void DeviceInputEyeTrackingAdded(uint32_t deviceIndex, const std::string& path) = 0;
	virtual void DeviceInputEyeTrackingRemoved(uint32_t deviceIndex, const std::string& path) = 0;

	virtual void DevicePoseChanged(uint32_t deviceIndex, DevicePose oldPose, DevicePose newPose) = 0;
	virtual void DeviceInputBooleanChanged(uint32_t deviceIndex, std::string path, BooleanInput oldInput, BooleanInput newInput) = 0;
	virtual void DeviceInputScalarChanged(uint32_t deviceIndex, std::string path, ScalarInput oldInput, ScalarInput newInput) = 0;
	virtual void DeviceInputSkeletonChanged(uint32_t deviceIndex, std::string path, SkeletonInput oldInput, SkeletonInput newInput) = 0;
	virtual void DeviceInputPoseChanged(uint32_t deviceIndex, std::string path, PoseInput oldInput, PoseInput newInput) = 0;
	virtual void DeviceInputEyeTrackingChanged(uint32_t deviceIndex, std::string path, EyeTrackingInput oldInput, EyeTrackingInput newInput) = 0;
};