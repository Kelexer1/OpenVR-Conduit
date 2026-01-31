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

	void notifyListenersInputAdded(uint32_t deviceIndex, const std::string& path, ObjectType type);
	void notifyListenersInputRemoved(uint32_t deviceIndex, const std::string& path, ObjectType type);

	ModelDevicePoseSerialized* getDevicePose(uint32_t deviceIndex);
	void addDevicePose(uint32_t deviceIndex);
	void removeDevicePose(uint32_t deviceIndex);
	void notifyListenersDevicePoseUpdated(uint32_t deviceIndex, const DevicePoseSerialized oldPose, const DevicePoseSerialized newPose);

	ModelDeviceInputBooleanSerialized* getBooleanInput(uint32_t deviceIndex, const std::string& path);
	void addBooleanInput(uint32_t deviceIndex, const std::string& path);
	void removeBooleanInput(uint32_t deviceIndex, const std::string& path);
	void notifyListenersBooleanInputUpdated(uint32_t deviceIndex, const std::string& path, const DeviceInputBooleanSerialized oldInput, const DeviceInputBooleanSerialized newInput);

	ModelDeviceInputScalarSerialized* getScalarInput(uint32_t deviceIndex, const std::string& path);
	void addScalarInput(uint32_t deviceIndex, const std::string& path);
	void removeScalarInput(uint32_t deviceIndex, const std::string& path);
	void notifyListenersScalarInputUpdated(uint32_t deviceIndex, const std::string& path, const DeviceInputScalarSerialized oldInput, const DeviceInputScalarSerialized newInput);

	ModelDeviceInputSkeletonSerialized* getSkeletonInput(uint32_t deviceIndex, const std::string& path);
	void addSkeletonInput(uint32_t deviceIndex, const std::string& path);
	void removeSkeletonInput(uint32_t deviceIndex, const std::string& path);
	void notifyListenersSkeletonInputUpdated(uint32_t deviceIndex, const std::string& path, const DeviceInputSkeletonSerialized oldInput, const DeviceInputSkeletonSerialized newInput);

	ModelDeviceInputPoseSerialized* getPoseInput(uint32_t deviceIndex, const std::string& path);
	void addPoseInput(uint32_t deviceIndex, const std::string& path);
	void removePoseInput(uint32_t deviceIndex, const std::string& path);
	void notifyListenersPoseInputUpdated(uint32_t deviceIndex, const std::string& path, const DeviceInputPoseSerialized oldInput, const DeviceInputPoseSerialized newInput);

	ModelDeviceInputEyeTrackingSerialized* getEyeTrackingInput(uint32_t deviceIndex, const std::string& path);
	void addEyeTrackingInput(uint32_t deviceIndex, const std::string& path);
	void removeEyeTrackingInput(uint32_t deviceIndex, const std::string& path);
	void notifyListenersEyeTrackingInputUpdated(uint32_t deviceIndex, const std::string& path, const DeviceInputEyeTrackingSerialized oldInput, const DeviceInputEyeTrackingSerialized newInput);
private:
	std::vector<IDeviceStateEventReciever*> eventListeners;

	std::unordered_map<uint32_t, ModelDevicePoseSerialized> devicePoses;

	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputBooleanSerialized>> booleanInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputScalarSerialized>> scalarInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputSkeletonSerialized>> skeletonInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputPoseSerialized>> poseInputs;
	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputEyeTrackingSerialized>> eyeTrackingInputs;

	DeviceStateModelClient() = default;
};