#pragma once
#include <openvr_driver.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "SharedDeviceMemoryDriver.h"
#include "ObjectSchemas.h"

class DeviceStateModel {
public:
	static DeviceStateModel& getInstance();

	// Path management
	void addDevicePath(const std::string& path);
	void removeDevicePath(const std::string& path);

	// Index To Property Container management
	void addDeviceIndexToContainerMapping(uint32_t deviceIndex, vr::PropertyContainerHandle_t propertyContainer);
	void removeDeviceIndexToContainerMapping(uint32_t deviceIndex);
	uint32_t getDeviceIndexFromPropertyContainer(vr::PropertyContainerHandle_t propertyContainer);
	vr::PropertyContainerHandle_t* getPropertyContainerFromDeviceIndex(uint32_t deviceIndex);

	// Pose management
	ModelDevicePoseSerialized* getDevicePose(uint32_t deviceIndex);
	void setDevicePoseChanged(uint32_t deviceIndex);
	void addDevicePose(uint32_t deviceIndex);
	void removeDevicePose(uint32_t deviceIndex);

	// Boolean input management
	ModelDeviceInputBooleanSerialized* getBooleanInput(uint32_t deviceIndex, const std::string& path);
	ModelDeviceInputBooleanSerialized* getBooleanInput(uint32_t deviceIndex, uint32_t pathID);
	ModelDeviceInputBooleanSerialized* getBooleanInput(vr::VRInputComponentHandle_t componentHandle);
	void setInputBooleanChanged(uint32_t index, const std::string& path);
	void setInputBooleanChanged(uint32_t index, uint32_t pathID);
	void addBooleanInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle);
	void removeBooleanInput(uint32_t deviceIndex, const std::string& path);

	// Scalar input management
	ModelDeviceInputScalarSerialized* getScalarInput(uint32_t deviceIndex, const std::string& path);
	ModelDeviceInputScalarSerialized* getScalarInput(uint32_t deviceIndex, uint32_t pathID);
	ModelDeviceInputScalarSerialized* getScalarInput(vr::VRInputComponentHandle_t componentHandle);
	void setInputScalarChanged(uint32_t index, const std::string& path);
	void setInputScalarChanged(uint32_t index, uint32_t pathID);
	void addScalarInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle);
	void removeScalarInput(uint32_t deviceIndex, const std::string& path);
	
	// Skeleton input management
	ModelDeviceInputSkeletonSerialized* getSkeletonInput(uint32_t deviceIndex, const std::string& path);
	ModelDeviceInputSkeletonSerialized* getSkeletonInput(uint32_t deviceIndex, uint32_t pathID);
	ModelDeviceInputSkeletonSerialized* getSkeletonInput(vr::VRInputComponentHandle_t componentHandle);
	void setInputSkeletonChanged(uint32_t index, const std::string& path);
	void setInputSkeletonChanged(uint32_t index, uint32_t pathID);
	void addSkeletonInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle);
	void removeSkeletonInput(uint32_t deviceIndex, const std::string& path);

	// Pose input management
	ModelDeviceInputPoseSerialized* getPoseInput(uint32_t deviceIndex, const std::string& path);
	ModelDeviceInputPoseSerialized* getPoseInput(uint32_t deviceIndex, uint32_t pathID);
	ModelDeviceInputPoseSerialized* getPoseInput(vr::VRInputComponentHandle_t componentHandle);
	void setInputPoseChanged(uint32_t index, const std::string& path);
	void setInputPoseChanged(uint32_t index, uint32_t pathID);
	void addPoseInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle);
	void removePoseInput(uint32_t deviceIndex, const std::string& path);

	// Eye tracking input management
	ModelDeviceInputEyeTrackingSerialized* getEyeTrackingInput(uint32_t deviceIndex, const std::string& path);
	ModelDeviceInputEyeTrackingSerialized* getEyeTrackingInput(uint32_t deviceIndex, uint32_t pathID);
	ModelDeviceInputEyeTrackingSerialized* getEyeTrackingInput(vr::VRInputComponentHandle_t componentHandle);
	void setInputEyeTrackingChanged(uint32_t index, const std::string& path);
	void setInputEyeTrackingChanged(uint32_t index, uint32_t pathID);
	void addEyeTrackingInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle);
	void removeEyeTrackingInput(uint32_t deviceIndex, const std::string& path);

	// Cleanup
	void cleanDeviceIndexAssociations(uint32_t deviceIndex);

private:
	std::unordered_map<uint32_t, std::string> pathTable;

	// Device Index -> Device Property Container
	std::unordered_map<uint32_t, vr::PropertyContainerHandle_t> indexTable;

	// Device Index -> Device Pose
	std::unordered_map<uint32_t, ModelDevicePoseSerialized> devicePoses;

	// Device Index -> (Input path -> Input data)
	std::unordered_map<uint32_t, std::unordered_map<std::string, std::pair<vr::VRInputComponentHandle_t, ModelDeviceInputBooleanSerialized>>> booleanInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, std::pair<vr::VRInputComponentHandle_t, ModelDeviceInputScalarSerialized>>> scalarInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, std::pair<vr::VRInputComponentHandle_t, ModelDeviceInputSkeletonSerialized>>> skeletonInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, std::pair<vr::VRInputComponentHandle_t, ModelDeviceInputPoseSerialized>>> poseInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, std::pair<vr::VRInputComponentHandle_t, ModelDeviceInputEyeTrackingSerialized>>> eyeTrackingInputs;

	DeviceStateModel() = default;

	uint32_t findNextAvailablePathTableIndex();
	uint32_t findIndexOfPath(const std::string& path);

	template<typename T>
	void collectPathsAndEraseDevice(
		std::unordered_map<uint32_t, 
		std::unordered_map<std::string, T>>& inputMap,
		uint32_t deviceIndex,
		std::vector<std::string>& pathsToCheck
	) {
		auto it = inputMap.find(deviceIndex);
		if (it != inputMap.end()) {
			for (const auto& pathPair : it->second) {
				pathsToCheck.push_back(pathPair.first);
			}
			inputMap.erase(deviceIndex);
		}
	}

	bool isPathUsedByAnyDevice(const std::string& path);

	template<typename T>
	bool isPathInUse(
		const std::unordered_map<uint32_t, 
		std::unordered_map<std::string, T>>& inputMap,
		const std::string& path
	) {
		for (const auto& devicePair : inputMap) {
			if (devicePair.second.find(path) != devicePair.second.end()) {
				return true;
			}
		}
		return false;
	}
};