#pragma once
#include "ObjectSchemas.h"

#include <unordered_map>
#include <string>

class DeviceStateModelClient {
public:
	static DeviceStateModelClient& getInstance();

	void addDevicePath(uint32_t pathID, const std::string& path);
	void removeDevicePath(uint32_t pathID);

	DevicePose* getNaturalDevicePose(uint32_t deviceIndex);
	DevicePose* getOverridenDevicePose(uint32_t deviceIndex);
	void addDevicePose(uint32_t deviceIndex);
	void removeDevicePose(uint32_t deviceIndex);

	BooleanInput* getNaturalBooleanInput(uint32_t deviceIndex, const std::string& path);
	BooleanInput* getOverridenBooleanInput(uint32_t deviceIndex, const std::string& path);
	void addBooleanInput(uint32_t deviceIndex, const std::string& path);
	void removeBooleanInput(uint32_t deviceIndex, const std::string& path);

	ScalarInput* getNaturalScalarInput(uint32_t deviceIndex, const std::string& path);
	ScalarInput* getOverridenScalarInput(uint32_t deviceIndex, const std::string& path);
	void addScalarInput(uint32_t deviceIndex, const std::string& path);
	void removeScalarInput(uint32_t deviceIndex, const std::string& path);

	SkeletonInput* getNaturalSkeletonInput(uint32_t deviceIndex, const std::string& path);
	SkeletonInput* getOverridenSkeletonInput(uint32_t deviceIndex, const std::string& path);
	void addSkeletonInput(uint32_t deviceIndex, const std::string& path);
	void removeSkeletonInput(uint32_t deviceIndex, const std::string& path);

	PoseInput* getNaturalPoseInput(uint32_t deviceIndex, const std::string& path);
	PoseInput* getOverridenPoseInput(uint32_t deviceIndex, const std::string& path);
	void addPoseInput(uint32_t deviceIndex, const std::string& path);
	void removePoseInput(uint32_t deviceIndex, const std::string& path);

	EyeTrackingInput* getNaturalEyeTrackingInput(uint32_t deviceIndex, const std::string& path);
	EyeTrackingInput* getOverridenEyeTrackingInput(uint32_t deviceIndex, const std::string& path);
	void addEyeTrackingInput(uint32_t deviceIndex, const std::string& path);
	void removeEyeTrackingInput(uint32_t deviceIndex, const std::string& path);

private:
	std::unordered_map<uint32_t, std::string> pathTable;

	std::unordered_map<uint32_t, DevicePoseSerialized> devicePoses;

	std::unordered_map<uint32_t, std::unordered_map<std::string, DeviceInputBooleanSerialized>> booleanInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, DeviceInputScalarSerialized>> scalarInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, DeviceInputSkeletonSerialized>> skeletonInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, DeviceInputPoseSerialized>> poseInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, DeviceInputEyeTrackingSerialized>> eyeTrackingInputs;

	DeviceStateModelClient() = default;

	uint32_t findIndexOfPath(const std::string& path);
};