#pragma once
#include "IDeviceStateEventReciever.h"
#include <iostream>

class MyEventReciever : public IDeviceStateEventReciever {
public:
	void DeviceInputBooleanAdded(uint32_t deviceIndex, const std::string& path) override;
	void DeviceInputBooleanRemoved(uint32_t deviceIndex, const std::string& path) override;
	void DeviceInputScalarAdded(uint32_t deviceIndex, const std::string& path) override;
	void DeviceInputScalarRemoved(uint32_t deviceIndex, const std::string& path) override;
	void DeviceInputSkeletonAdded(uint32_t deviceIndex, const std::string& path) override;
	void DeviceInputSkeletonRemoved(uint32_t deviceIndex, const std::string& path) override;
	void DeviceInputPoseAdded(uint32_t deviceIndex, const std::string& path) override;
	void DeviceInputPoseRemoved(uint32_t deviceIndex, const std::string& path) override;
	void DeviceInputEyeTrackingAdded(uint32_t deviceIndex, const std::string& path) override;
	void DeviceInputEyeTrackingRemoved(uint32_t deviceIndex, const std::string& path) override;

	void DevicePoseChanged(uint32_t deviceIndex, DevicePose oldPose, DevicePose newPose) override;
	void DeviceInputBooleanChanged(uint32_t deviceIndex, std::string path, BooleanInput oldInput, BooleanInput newInput) override;
	void DeviceInputScalarChanged(uint32_t deviceIndex, std::string path, ScalarInput oldInput, ScalarInput newInput) override;
	void DeviceInputSkeletonChanged(uint32_t deviceIndex, std::string path, SkeletonInput oldInput, SkeletonInput newInput) override;
	void DeviceInputPoseChanged(uint32_t deviceIndex, std::string path, PoseInput oldInput, PoseInput newInput) override;
	void DeviceInputEyeTrackingChanged(uint32_t deviceIndex, std::string path, EyeTrackingInput oldInput, EyeTrackingInput newInput) override;
};