#pragma once
#include <openvr_driver.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <variant>

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
	DevicePoseSerialized* getDevicePose(uint32_t deviceIndex);
	void setDevicePoseChanged(uint32_t deviceIndex);
	void addDevicePose(uint32_t deviceIndex);
	void removeDevicePose(uint32_t deviceIndex);

	// Boolean input management
	DeviceInputBooleanSerialized* getBooleanInput(uint32_t deviceIndex, const std::string& path);
	void setInputBooleanChanged(uint32_t index, const std::string& path);
	void addBooleanInput(uint32_t deviceIndex, const std::string& path);
	void removeBooleanInput(uint32_t deviceIndex, const std::string& path);

	// Scalar input management
	DeviceInputScalarSerialized* getScalarInput(uint32_t deviceIndex, const std::string& path);
	void setInputScalarChanged(uint32_t index, const std::string& path);
	void addScalarInput(uint32_t deviceIndex, const std::string& path);
	void removeScalarInput(uint32_t deviceIndex, const std::string& path);

	// Skeleton input management
	DeviceInputSkeletonSerialized* getSkeletonInput(uint32_t deviceIndex, const std::string& path);
	void setInputSkeletonChanged(uint32_t index, const std::string& path);
	void addSkeletonInput(uint32_t deviceIndex, const std::string& path);
	void removeSkeletonInput(uint32_t deviceIndex, const std::string& path);

	// Pose input management
	DeviceInputPoseSerialized* getPoseInput(uint32_t deviceIndex, const std::string& path);
	void setInputPoseChanged(uint32_t index, const std::string& path);
	void addPoseInput(uint32_t deviceIndex, const std::string& path);
	void removePoseInput(uint32_t deviceIndex, const std::string& path);

	// Eye tracking input management
	DeviceInputEyeTrackingSerialized* getEyeTrackingInput(uint32_t deviceIndex, const std::string& path);
	void setInputEyeTrackingChanged(uint32_t index, const std::string& path);
	void addEyeTrackingInput(uint32_t deviceIndex, const std::string& path);
	void removeEyeTrackingInput(uint32_t deviceIndex, const std::string& path);

private:
	std::unordered_map<uint32_t, std::string> pathTable;

	// Device Index -> Device Property Container
	std::unordered_map<uint32_t, vr::PropertyContainerHandle_t> indexTable;

	// Device Index -> Device Pose
	std::unordered_map<uint32_t, DevicePoseSerialized> devicePoses;

	// Device Index -> (Input path -> Input data)
	std::unordered_map<uint32_t, std::unordered_map<std::string, DeviceInputBooleanSerialized>> booleanInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, DeviceInputScalarSerialized>> scalarInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, DeviceInputSkeletonSerialized>> skeletonInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, DeviceInputPoseSerialized>> poseInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, DeviceInputEyeTrackingSerialized>> eyeTrackingInputs;

	DeviceStateModel() = default;

	uint32_t findIndexOfPath(const std::string& path);
	uint32_t findNextAvailablePathTableIndex();
};