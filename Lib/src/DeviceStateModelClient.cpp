#include "DeviceStateModelClient.h"

DeviceStateModelClient& DeviceStateModelClient::getInstance() {
	static DeviceStateModelClient instance;
	return instance;
}

void DeviceStateModelClient::addEventListener(const IDeviceStateEventReceiver& listener) {
	this->eventListeners.push_back(const_cast<IDeviceStateEventReceiver*>(&listener));
}

void DeviceStateModelClient::removeEventListener(const IDeviceStateEventReceiver& listener) {
	auto it = std::remove(this->eventListeners.begin(), this->eventListeners.end(), &listener);
	this->eventListeners.erase(it, this->eventListeners.end());
}

void DeviceStateModelClient::notifyListenersInputAdded(
	uint32_t deviceIndex,
	const std::string& path,
	ObjectType type
) {
	switch (type) {
	case Object_InputBoolean:
		for (IDeviceStateEventReceiver* listener : this->eventListeners) {
			listener->DeviceInputBooleanAdded(deviceIndex, path);
		}
		break;
	case Object_InputScalar:
		for (IDeviceStateEventReceiver* listener : this->eventListeners) {
			listener->DeviceInputScalarAdded(deviceIndex, path);
		}
		break;
	case Object_InputSkeleton:
		for (IDeviceStateEventReceiver* listener : this->eventListeners) {
			listener->DeviceInputSkeletonAdded(deviceIndex, path);
		}
		break;
	case Object_InputPose:
		for (IDeviceStateEventReceiver* listener : this->eventListeners) {
			listener->DeviceInputPoseAdded(deviceIndex, path);
		}
		break;
	case Object_InputEyeTracking:
		for (IDeviceStateEventReceiver* listener : this->eventListeners) {
			listener->DeviceInputEyeTrackingAdded(deviceIndex, path);
		}
		break;
	}
}

void DeviceStateModelClient::notifyListenersInputRemoved(
	uint32_t deviceIndex,
	const std::string& path,
	ObjectType type
) {
	switch (type) {
	case Object_InputBoolean:
		for (IDeviceStateEventReceiver* listener : this->eventListeners) {
			listener->DeviceInputBooleanRemoved(deviceIndex, path);
		}
		break;
	case Object_InputScalar:
		for (IDeviceStateEventReceiver* listener : this->eventListeners) {
			listener->DeviceInputScalarRemoved(deviceIndex, path);
		}
		break;
	case Object_InputSkeleton:
		for (IDeviceStateEventReceiver* listener : this->eventListeners) {
			listener->DeviceInputSkeletonRemoved(deviceIndex, path);
		}
		break;
	case Object_InputPose:
		for (IDeviceStateEventReceiver* listener : this->eventListeners) {
			listener->DeviceInputPoseRemoved(deviceIndex, path);
		}
		break;
	case Object_InputEyeTracking:
		for (IDeviceStateEventReceiver* listener : this->eventListeners) {
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

void DeviceStateModelClient::notifyListenersDevicePoseUpdated(
	uint32_t deviceIndex,
	const DevicePoseSerialized oldPose,
	const DevicePoseSerialized newPose
) {
	for (IDeviceStateEventReceiver* listener : this->eventListeners) {
		listener->DevicePoseChanged(deviceIndex, oldPose.pose, newPose.pose);
	}
}

ModelDeviceInputBooleanSerialized* DeviceStateModelClient::getBooleanInput(
	uint32_t deviceIndex,
	const std::string& path
) {
	auto it1 = this->booleanInputs.find(deviceIndex);
	if (it1 == this->booleanInputs.end()) return nullptr;

	auto it2 = it1->second.find(path);
	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModelClient::addBooleanInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->booleanInputs[deviceIndex].find(path);
	if (it == this->booleanInputs[deviceIndex].end()) {
		this->booleanInputs[deviceIndex][path];
		this->notifyListenersInputAdded(deviceIndex, path, Object_InputBoolean);
	}
}

void DeviceStateModelClient::removeBooleanInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->booleanInputs.find(deviceIndex);
	if (it1 != this->booleanInputs.end()) {
		it1->second.erase(path);
		this->notifyListenersInputRemoved(deviceIndex, path, Object_InputBoolean);
	}
}

void DeviceStateModelClient::notifyListenersBooleanInputUpdated(
	uint32_t deviceIndex,
	const std::string& path,
	const DeviceInputBooleanSerialized oldInput,
	const DeviceInputBooleanSerialized newInput
) {
	for (IDeviceStateEventReceiver* listener : this->eventListeners) {
		listener->DeviceInputBooleanChanged(deviceIndex, path, oldInput.value, newInput.value);
	}
}

ModelDeviceInputScalarSerialized* DeviceStateModelClient::getScalarInput(
	uint32_t deviceIndex,
	const std::string& path
) {
	auto it1 = this->scalarInputs.find(deviceIndex);
	if (it1 == this->scalarInputs.end()) return nullptr;

	auto it2 = it1->second.find(path);
	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModelClient::addScalarInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->scalarInputs[deviceIndex].find(path);
	if (it == this->scalarInputs[deviceIndex].end()) {
		this->scalarInputs[deviceIndex][path];
		this->notifyListenersInputAdded(deviceIndex, path, Object_InputScalar);
	}
}

void DeviceStateModelClient::removeScalarInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->scalarInputs.find(deviceIndex);
	if (it1 != this->scalarInputs.end()) {
		it1->second.erase(path);
		this->notifyListenersInputRemoved(deviceIndex, path, Object_InputScalar);
	}
}

void DeviceStateModelClient::notifyListenersScalarInputUpdated(
	uint32_t deviceIndex,
	const std::string& path,
	const DeviceInputScalarSerialized oldInput,
	const DeviceInputScalarSerialized newInput
) {
	for (IDeviceStateEventReceiver* listener : this->eventListeners) {
		listener->DeviceInputScalarChanged(deviceIndex, path, oldInput.value, newInput.value);
	}
}

ModelDeviceInputSkeletonSerialized* DeviceStateModelClient::getSkeletonInput(
	uint32_t deviceIndex,
	const std::string& path
) {
	auto it1 = this->skeletonInputs.find(deviceIndex);
	if (it1 == this->skeletonInputs.end()) return nullptr;

	auto it2 = it1->second.find(path);
	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModelClient::addSkeletonInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->skeletonInputs[deviceIndex].find(path);
	if (it == this->skeletonInputs[deviceIndex].end()) {
		this->skeletonInputs[deviceIndex][path];
		this->notifyListenersInputAdded(deviceIndex, path, Object_InputSkeleton);
	}
}

void DeviceStateModelClient::removeSkeletonInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->skeletonInputs.find(deviceIndex);
	if (it1 != this->skeletonInputs.end()) {
		it1->second.erase(path);
		this->notifyListenersInputRemoved(deviceIndex, path, Object_InputSkeleton);
	}
}

void DeviceStateModelClient::notifyListenersSkeletonInputUpdated(
	uint32_t deviceIndex,
	const std::string& path,
	const DeviceInputSkeletonSerialized oldInput,
	const DeviceInputSkeletonSerialized newInput
) {
	for (IDeviceStateEventReceiver* listener : this->eventListeners) {
		listener->DeviceInputSkeletonChanged(deviceIndex, path, oldInput.value, newInput.value);
	}
}

ModelDeviceInputPoseSerialized* DeviceStateModelClient::getPoseInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->poseInputs.find(deviceIndex);
	if (it1 == this->poseInputs.end()) return nullptr;

	auto it2 = it1->second.find(path);
	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModelClient::addPoseInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->poseInputs[deviceIndex].find(path);
	if (it == this->poseInputs[deviceIndex].end()) {
		this->poseInputs[deviceIndex][path];
		this->notifyListenersInputAdded(deviceIndex, path, Object_InputPose);
	}
}

void DeviceStateModelClient::removePoseInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->poseInputs.find(deviceIndex);
	if (it1 != this->poseInputs.end()) {
		it1->second.erase(path);
		this->notifyListenersInputRemoved(deviceIndex, path, Object_InputPose);
	}
}

void DeviceStateModelClient::notifyListenersPoseInputUpdated(
	uint32_t deviceIndex,
	const std::string& path,
	const DeviceInputPoseSerialized oldInput,
	const DeviceInputPoseSerialized newInput
) {
	for (IDeviceStateEventReceiver* listener : this->eventListeners) {
		listener->DeviceInputPoseChanged(deviceIndex, path, oldInput.value, newInput.value);
	}
}

ModelDeviceInputEyeTrackingSerialized* DeviceStateModelClient::getEyeTrackingInput(
	uint32_t deviceIndex,
	const std::string& path
) {
	auto it1 = this->eyeTrackingInputs.find(deviceIndex);
	if (it1 == this->eyeTrackingInputs.end()) return nullptr;

	auto it2 = it1->second.find(path);
	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModelClient::addEyeTrackingInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->eyeTrackingInputs[deviceIndex].find(path);
	if (it == this->eyeTrackingInputs[deviceIndex].end()) {
		this->eyeTrackingInputs[deviceIndex][path];
		this->notifyListenersInputAdded(deviceIndex, path, Object_InputEyeTracking);
	}
}

void DeviceStateModelClient::removeEyeTrackingInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->eyeTrackingInputs.find(deviceIndex);
	if (it1 != this->eyeTrackingInputs.end()) {
		it1->second.erase(path);
		this->notifyListenersInputRemoved(deviceIndex, path, Object_InputEyeTracking);
	}
}

void DeviceStateModelClient::notifyListenersEyeTrackingInputUpdated(
	uint32_t deviceIndex,
	const std::string& path,
	const DeviceInputEyeTrackingSerialized oldInput,
	const DeviceInputEyeTrackingSerialized newInput
) {
	for (IDeviceStateEventReceiver* listener : this->eventListeners) {
		listener->DeviceInputEyeTrackingChanged(deviceIndex, path, oldInput.value, newInput.value);
	}
}