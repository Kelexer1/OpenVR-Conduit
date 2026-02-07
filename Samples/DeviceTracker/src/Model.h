#pragma once
#include "DeviceTypes.h"
#include "DeviceStateCommandSender.h"
#include "IDeviceStateEventReceiver.h"
#include <unordered_map>
#include <string>
#include <iostream>

class Model : public IDeviceStateEventReceiver {
public:
	Model(DeviceStateCommandSender& commandSender);

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

	void print();
private:
	DeviceStateCommandSender commandSender;

	std::unordered_map<uint32_t, DevicePose> devicePoses;
	std::unordered_map<uint32_t, std::unordered_map<std::string, BooleanInput>> booleanInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, ScalarInput>> scalarInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, SkeletonInput>> skeletonInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, PoseInput>> poseInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, EyeTrackingInput>> eyeTrackingInputs;
};