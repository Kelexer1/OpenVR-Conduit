#include "DeviceStateModelClient.h"

DeviceStateModelClient& DeviceStateModelClient::getInstance() {
	static DeviceStateModelClient instance;
	return instance;
}

void DeviceStateModelClient::addEventListener(const IDeviceStateEventReciever& listener) {
	this->eventListeners.push_back(const_cast<IDeviceStateEventReciever*>(&listener));
}

void DeviceStateModelClient::removeEventListener(const IDeviceStateEventReciever& listener) {
	auto it = std::remove(this->eventListeners.begin(), this->eventListeners.end(), &listener);
	this->eventListeners.erase(it, this->eventListeners.end());
}

void DeviceStateModelClient::addDevicePath(uint32_t pathID, const std::string& path) {
	this->pathTable[pathID] = path;
}

void DeviceStateModelClient::removeDevicePath(uint32_t pathID) {
	this->pathTable.erase(pathID);
}

void DeviceStateModelClient::notifyListenersInputAdded(uint32_t deviceIndex, const std::string& path, ObjectType type) {
	switch (type) {
	case Object_InputBoolean:
		for (IDeviceStateEventReciever* listener : this->eventListeners) {
			listener->DeviceInputBooleanAdded(deviceIndex, path);
		}
		break;
	case Object_InputScalar:
		for (IDeviceStateEventReciever* listener : this->eventListeners) {
			listener->DeviceInputScalarAdded(deviceIndex, path);
		}
		break;
	case Object_InputSkeleton:
		for (IDeviceStateEventReciever* listener : this->eventListeners) {
			listener->DeviceInputSkeletonAdded(deviceIndex, path);
		}
		break;
	case Object_InputPose:
		for (IDeviceStateEventReciever* listener : this->eventListeners) {
			listener->DeviceInputPoseAdded(deviceIndex, path);
		}
		break;
	case Object_InputEyeTracking:
		for (IDeviceStateEventReciever* listener : this->eventListeners) {
			listener->DeviceInputEyeTrackingAdded(deviceIndex, path);
		}
		break;
	}
}

void DeviceStateModelClient::notifyListenersInputRemoved(uint32_t deviceIndex, const std::string& path, ObjectType type) {
	switch (type) {
	case Object_InputBoolean:
		for (IDeviceStateEventReciever* listener : this->eventListeners) {
			listener->DeviceInputBooleanRemoved(deviceIndex, path);
		}
		break;
	case Object_InputScalar:
		for (IDeviceStateEventReciever* listener : this->eventListeners) {
			listener->DeviceInputScalarRemoved(deviceIndex, path);
		}
		break;
	case Object_InputSkeleton:
		for (IDeviceStateEventReciever* listener : this->eventListeners) {
			listener->DeviceInputSkeletonRemoved(deviceIndex, path);
		}
		break;
	case Object_InputPose:
		for (IDeviceStateEventReciever* listener : this->eventListeners) {
			listener->DeviceInputPoseRemoved(deviceIndex, path);
		}
		break;
	case Object_InputEyeTracking:
		for (IDeviceStateEventReciever* listener : this->eventListeners) {
			listener->DeviceInputEyeTrackingRemoved(deviceIndex, path);
		}
		break;
	}
}

ModelDevicePoseSerialized* DeviceStateModelClient::getDevicePose(uint32_t deviceIndex) {
	auto it = this->devicePoses.find(deviceIndex);
	return it == this->devicePoses.end() ? nullptr : &it->second;
}

void DeviceStateModelClient::addDevicePose(uint32_t deviceIndex) {
	this->devicePoses[deviceIndex];
}

void DeviceStateModelClient::removeDevicePose(uint32_t deviceIndex) {
	this->devicePoses.erase(deviceIndex);
}

void DeviceStateModelClient::notifyListenersDevicePoseUpdated(uint32_t deviceIndex, const DevicePoseSerialized oldPose, const DevicePoseSerialized newPose) {
	for (IDeviceStateEventReciever* listener : this->eventListeners) {
		listener->DevicePoseChanged(deviceIndex, oldPose.pose, newPose.pose);
	}
}

ModelDeviceInputBooleanSerialized* DeviceStateModelClient::getBooleanInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return nullptr;
	}

	auto it1 = this->booleanInputs.find(deviceIndex);
	if (it1 == this->booleanInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModelClient::addBooleanInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}
	
	auto it = this->booleanInputs.find(deviceIndex);
	if (it == this->booleanInputs.end()) {
		this->booleanInputs[deviceIndex][path];
		this->notifyListenersInputAdded(deviceIndex, path, Object_InputBoolean);
	}
}

void DeviceStateModelClient::removeBooleanInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	auto it1 = this->booleanInputs.find(deviceIndex);
	if (it1 != this->booleanInputs.end()) {
		it1->second.erase(path);
		this->notifyListenersInputRemoved(deviceIndex, path, Object_InputBoolean);
	}
}

void DeviceStateModelClient::notifyListenersBooleanInputUpdated(uint32_t deviceIndex, uint32_t pathID, const DeviceInputBooleanSerialized oldInput, const DeviceInputBooleanSerialized newInput) {
	const std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	for (IDeviceStateEventReciever* listener : this->eventListeners) {
		listener->DeviceInputBooleanChanged(deviceIndex, path, oldInput.value, newInput.value);
	}
}

ModelDeviceInputScalarSerialized* DeviceStateModelClient::getScalarInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return nullptr;
	}

	auto it1 = this->scalarInputs.find(deviceIndex);
	if (it1 == this->scalarInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModelClient::addScalarInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	auto it = this->scalarInputs.find(deviceIndex);
	if (it == this->scalarInputs.end()) {
		this->scalarInputs[deviceIndex][path];
		this->notifyListenersInputAdded(deviceIndex, path, Object_InputScalar);
	}
}

void DeviceStateModelClient::removeScalarInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	auto it1 = this->scalarInputs.find(deviceIndex);
	if (it1 != this->scalarInputs.end()) {
		it1->second.erase(path);
		this->notifyListenersInputRemoved(deviceIndex, path, Object_InputScalar);
	}
}

void DeviceStateModelClient::notifyListenersScalarInputUpdated(uint32_t deviceIndex, uint32_t pathID, const DeviceInputScalarSerialized oldInput, const DeviceInputScalarSerialized newInput) {
	const std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	for (IDeviceStateEventReciever* listener : this->eventListeners) {
		listener->DeviceInputScalarChanged(deviceIndex, path, oldInput.value, newInput.value);
	}
}

ModelDeviceInputSkeletonSerialized* DeviceStateModelClient::getSkeletonInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return nullptr;
	}

	auto it1 = this->skeletonInputs.find(deviceIndex);
	if (it1 == this->skeletonInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModelClient::addSkeletonInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	auto it = this->skeletonInputs.find(deviceIndex);
	if (it == this->skeletonInputs.end()) {
		this->skeletonInputs[deviceIndex][path];
		this->notifyListenersInputAdded(deviceIndex, path, Object_InputSkeleton);
	}
}

void DeviceStateModelClient::removeSkeletonInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	auto it1 = this->skeletonInputs.find(deviceIndex);
	if (it1 != this->skeletonInputs.end()) {
		it1->second.erase(path);
		this->notifyListenersInputRemoved(deviceIndex, path, Object_InputSkeleton);
	}
}

void DeviceStateModelClient::notifyListenersSkeletonInputUpdated(uint32_t deviceIndex, uint32_t pathID, const DeviceInputSkeletonSerialized oldInput, const DeviceInputSkeletonSerialized newInput) {
	const std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	for (IDeviceStateEventReciever* listener : this->eventListeners) {
		listener->DeviceInputSkeletonChanged(deviceIndex, path, oldInput.value, newInput.value);
	}
}

ModelDeviceInputPoseSerialized* DeviceStateModelClient::getPoseInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return nullptr;
	}

	auto it1 = this->poseInputs.find(deviceIndex);
	if (it1 == this->poseInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModelClient::addPoseInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	auto it = this->poseInputs.find(deviceIndex);
	if (it == this->poseInputs.end()) {
		this->poseInputs[deviceIndex][path];
		this->notifyListenersInputAdded(deviceIndex, path, Object_InputPose);
	}
}

void DeviceStateModelClient::removePoseInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	auto it1 = this->poseInputs.find(deviceIndex);
	if (it1 != this->poseInputs.end()) {
		it1->second.erase(path);
		this->notifyListenersInputRemoved(deviceIndex, path, Object_InputPose);
	}
}

void DeviceStateModelClient::notifyListenersPoseInputUpdated(uint32_t deviceIndex, uint32_t pathID, const DeviceInputPoseSerialized oldInput, const DeviceInputPoseSerialized newInput) {
	const std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	for (IDeviceStateEventReciever* listener : this->eventListeners) {
		listener->DeviceInputPoseChanged(deviceIndex, path, oldInput.value, newInput.value);
	}
}

ModelDeviceInputEyeTrackingSerialized* DeviceStateModelClient::getEyeTrackingInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return nullptr;
	}

	auto it1 = this->eyeTrackingInputs.find(deviceIndex);
	if (it1 == this->eyeTrackingInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModelClient::addEyeTrackingInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	auto it = this->eyeTrackingInputs.find(deviceIndex);
	if (it == this->eyeTrackingInputs.end()) {
		this->eyeTrackingInputs[deviceIndex][path];
		this->notifyListenersInputAdded(deviceIndex, path, Object_InputEyeTracking);
	}
}

void DeviceStateModelClient::removeEyeTrackingInput(uint32_t deviceIndex, uint32_t pathID) {
	std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	auto it1 = this->eyeTrackingInputs.find(deviceIndex);
	if (it1 != this->eyeTrackingInputs.end()) {
		it1->second.erase(path);
		this->notifyListenersInputRemoved(deviceIndex, path, Object_InputEyeTracking);
	}
}

void DeviceStateModelClient::notifyListenersEyeTrackingInputUpdated(uint32_t deviceIndex, uint32_t pathID, const DeviceInputEyeTrackingSerialized oldInput, const DeviceInputEyeTrackingSerialized newInput) {
	const std::string path = this->getPathFromIndex(pathID);
	if (path == "") {
		return;
	}

	for (IDeviceStateEventReciever* listener : this->eventListeners) {
		listener->DeviceInputEyeTrackingChanged(deviceIndex, path, oldInput.value, newInput.value);
	}
}

const std::string DeviceStateModelClient::getPathFromIndex(uint32_t pathID) {
	auto it = this->pathTable.find(pathID);
	return it == this->pathTable.end() ? "" : it->second;
}

uint32_t DeviceStateModelClient::getIndexFromPath(const std::string& path) {
	for (const auto& entry : this->pathTable) {
		if (entry.second == path) {
			return entry.first;
		}
	}
	return static_cast<uint32_t>(-1);
}