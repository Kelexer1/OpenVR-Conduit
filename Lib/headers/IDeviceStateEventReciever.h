#pragma once
#include <stdint.h>
#include <string>

#include "ObjectSchemas.h"

class IDeviceStateEventReciever {
public:
	virtual void DevicePoseChanged(uint32_t deviceIndex, DevicePose oldPose, DevicePose newPose) = 0;
	virtual void DeviceInputBooleanChanged(uint32_t deviceIndex, std::string path, BooleanInput oldInput, BooleanInput newInput) = 0;
	virtual void DeviceInputScalarChanged(uint32_t deviceIndex, std::string path, ScalarInput oldInput, ScalarInput newInput) = 0;
	virtual void DeviceInputSkeletonChanged(uint32_t deviceIndex, std::string path, SkeletonInput oldInput, SkeletonInput newInput) = 0;
	virtual void DeviceInputPoseChanged(uint32_t deviceIndex, std::string path, PoseInput oldInput, PoseInput newInput) = 0;
	virtual void DeviceInputEyeTrackingChanged(uint32_t deviceIndex, std::string path, EyeTrackingInput oldInput, EyeTrackingInput newInput) = 0;
};