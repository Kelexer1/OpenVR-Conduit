#pragma once
#include "ObjectSchemas.h"
#include "IDeviceStateEventReciever.h"

#include <unordered_map>
#include <string>

class DeviceStateModelClient {
public:
	static DeviceStateModelClient& getInstance();

	void addEventListener(const IDeviceStateEventReciever& listener);
	void removeEventListener(const IDeviceStateEventReciever& listener);

	void addDevicePath(uint32_t pathID, const std::string& path);
	void removeDevicePath(uint32_t pathID);

	ModelDevicePoseSerialized* getDevicePose(uint32_t deviceIndex);
	void addDevicePose(uint32_t deviceIndex);
	void removeDevicePose(uint32_t deviceIndex);
	void notifyListenersDevicePoseUpdated(uint32_t deviceIndex, const DevicePoseSerialized oldPose, const DevicePoseSerialized newPose);

	ModelDeviceInputBooleanSerialized* getBooleanInput(uint32_t deviceIndex, uint32_t pathID);
	void addBooleanInput(uint32_t deviceIndex, uint32_t pathID);
	void removeBooleanInput(uint32_t deviceIndex, uint32_t pathID);
	void notifyListenersBooleanInputUpdated(uint32_t deviceIndex, uint32_t pathID, const DeviceInputBooleanSerialized oldInput, const DeviceInputBooleanSerialized newInput);

	ModelDeviceInputScalarSerialized* getScalarInput(uint32_t deviceIndex, uint32_t pathID);
	void addScalarInput(uint32_t deviceIndex, uint32_t pathID);
	void removeScalarInput(uint32_t deviceIndex, uint32_t pathID);
	void notifyListenersScalarInputUpdated(uint32_t deviceIndex, uint32_t pathID, const DeviceInputScalarSerialized oldInput, const DeviceInputScalarSerialized newInput);

	ModelDeviceInputSkeletonSerialized* getSkeletonInput(uint32_t deviceIndex, uint32_t pathID);
	void addSkeletonInput(uint32_t deviceIndex, uint32_t pathID);
	void removeSkeletonInput(uint32_t deviceIndex, uint32_t pathID);
	void notifyListenersSkeletonInputUpdated(uint32_t deviceIndex, uint32_t pathID, const DeviceInputSkeletonSerialized oldInput, const DeviceInputSkeletonSerialized newInput);

	ModelDeviceInputPoseSerialized* getPoseInput(uint32_t deviceIndex, uint32_t pathID);
	void addPoseInput(uint32_t deviceIndex, uint32_t pathID);
	void removePoseInput(uint32_t deviceIndex, uint32_t pathID);
	void notifyListenersPoseInputUpdated(uint32_t deviceIndex, uint32_t pathID, const DeviceInputPoseSerialized oldInput, const DeviceInputPoseSerialized newInput);

	ModelDeviceInputEyeTrackingSerialized* getEyeTrackingInput(uint32_t deviceIndex, uint32_t pathID);
	void addEyeTrackingInput(uint32_t deviceIndex, uint32_t pathID);
	void removeEyeTrackingInput(uint32_t deviceIndex, uint32_t pathID);
	void notifyListenersEyeTrackingInputUpdated(uint32_t deviceIndex, uint32_t pathID, const DeviceInputEyeTrackingSerialized oldInput, const DeviceInputEyeTrackingSerialized newInput);

	const std::string getPathFromIndex(uint32_t pathID);
	uint32_t getIndexFromPath(const std::string& path);

private:
	std::vector<IDeviceStateEventReciever*> eventListeners;

	std::unordered_map<uint32_t, std::string> pathTable;

	std::unordered_map<uint32_t, ModelDevicePoseSerialized> devicePoses;

	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputBooleanSerialized>> booleanInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputScalarSerialized>> scalarInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputSkeletonSerialized>> skeletonInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputPoseSerialized>> poseInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputEyeTrackingSerialized>> eyeTrackingInputs;

	DeviceStateModelClient() = default;
};