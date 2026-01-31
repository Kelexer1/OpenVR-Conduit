#pragma once
#include "DeviceTypes.h"
#include <unordered_map>
#include <string>
#include <iostream>

class Model {
public:
	static Model& getInstance();

	void updateDevicePose(uint32_t deviceIndex, DevicePose newPose);
	void removeDevicePose(uint32_t deviceIndex);

	void updateBooleanInput(uint32_t deviceIndex, const std::string& path, BooleanInput newInput);
	void removeBooleanInput(uint32_t deviceIndex, const std::string& path);

	void updateScalarInput(uint32_t deviceIndex, const std::string& path, ScalarInput newInput);
	void removeScalarInput(uint32_t deviceIndex, const std::string& path);

	void updateSkeletonInput(uint32_t deviceIndex, const std::string& path, SkeletonInput newInput);
	void removeSkeletonInput(uint32_t deviceIndex, const std::string& path);

	void updatePoseInput(uint32_t deviceIndex, const std::string& path, PoseInput newInput);
	void removePoseInput(uint32_t deviceIndex, const std::string& path);

	void updateEyeTrackingInput(uint32_t deviceIndex, const std::string& path, EyeTrackingInput newInput);
	void removeEyeTrackingInput(uint32_t deviceIndex, const std::string& path);

	void print();
private:
	Model();

	std::unordered_map<uint32_t, DevicePose> devicePoses;
	std::unordered_map<uint32_t, std::unordered_map<std::string, BooleanInput>> booleanInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, ScalarInput>> scalarInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, SkeletonInput>> skeletonInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, PoseInput>> poseInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, EyeTrackingInput>> eyeTrackingInputs;
};