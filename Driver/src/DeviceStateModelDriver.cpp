#include "DeviceStateModelDriver.h"

DeviceStateModel& DeviceStateModel::getInstance() {
	static DeviceStateModel instance;
	return instance;
}

void DeviceStateModel::addDevicePath(const std::string& path) {
	uint32_t nextIndex = this->findNextAvailablePathTableIndex();

	if (nextIndex == static_cast<uint32_t>(-1)) {
		return;
	}

	if (nextIndex != static_cast<uint32_t>(-1)) {
		this->pathTable[nextIndex] = path;
		SharedDeviceMemoryDriver::getInstance().syncPathTableToSharedMemory(nextIndex, path);
	}
}

void DeviceStateModel::removeDevicePath(const std::string& path) {
	uint32_t nextIndex = this->findIndexOfPath(path);
	
	if (this->pathTable.erase(nextIndex) == 1) {
		SharedDeviceMemoryDriver::getInstance().syncPathTableToSharedMemory(nextIndex, "");
	}
}

void DeviceStateModel::addDeviceIndexToContainerMapping(uint32_t deviceIndex, vr::PropertyContainerHandle_t propertyContainer) {
	this->indexTable[deviceIndex] = propertyContainer;
}

void DeviceStateModel::removeDeviceIndexToContainerMapping(uint32_t deviceIndex) {
	this->indexTable.erase(deviceIndex);
}

uint32_t DeviceStateModel::getDeviceIndexFromPropertyContainer(vr::PropertyContainerHandle_t propertyContainer) {
	for (auto it = this->indexTable.begin(); it != this->indexTable.end(); it++) {
		if (it->second == propertyContainer) {
			return it->first;
		}
	}

	return static_cast<uint32_t>(-1);
}

vr::PropertyContainerHandle_t* DeviceStateModel::getPropertyContainerFromDeviceIndex(uint32_t deviceIndex) {
	auto it = this->indexTable.find(deviceIndex);

	if (it != this->indexTable.end()) {
		return &(it->second);
	}

	return nullptr;
}

ModelDevicePoseSerialized* DeviceStateModel::getDevicePose(uint32_t deviceIndex) {
	auto it = this->devicePoses.find(deviceIndex);
	return it == this->devicePoses.end() ? nullptr : &(it->second);
}

void DeviceStateModel::setDevicePoseChanged(uint32_t deviceIndex) {
	ModelDevicePoseSerialized* pose = this->getDevicePose(deviceIndex);
	if (pose != nullptr) {
		SharedDeviceMemoryDriver::getInstance().syncDevicePoseUpdateToSharedMemory(&pose->data, deviceIndex);
	}
}

void DeviceStateModel::addDevicePose(uint32_t deviceIndex) {
	this->devicePoses[deviceIndex];
}

void DeviceStateModel::removeDevicePose(uint32_t deviceIndex) {
	this->devicePoses.erase(deviceIndex);
}

ModelDeviceInputBooleanSerialized* DeviceStateModel::getBooleanInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->booleanInputs.find(deviceIndex);

	if (it1 == this->booleanInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second.second);
}

ModelDeviceInputBooleanSerialized* DeviceStateModel::getBooleanInput(uint32_t deviceIndex, uint32_t pathID) {
	auto it = this->pathTable.find(pathID);
	return it == this->pathTable.end() ? nullptr : this->getBooleanInput(deviceIndex, it->second);
}

ModelDeviceInputBooleanSerialized* DeviceStateModel::getBooleanInput(vr::VRInputComponentHandle_t componentHandle) {
	for (auto& devicePair : this->booleanInputs) {
		for (auto& pathPair : devicePair.second) {
			if (pathPair.second.first == componentHandle) {
				return &(pathPair.second.second);
			}
		}
	}
	return nullptr;
}

void DeviceStateModel::setInputBooleanChanged(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputBooleanSerialized* inputBoolean = this->getBooleanInput(deviceIndex, path);
	if (inputBoolean != nullptr) {
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputBooleanUpdateToSharedMemory(&inputBoolean->data, deviceIndex, this->findIndexOfPath(path));
	}
}

void DeviceStateModel::setInputBooleanChanged(uint32_t deviceIndex, uint32_t pathID) {
	auto it = this->pathTable.find(pathID);
	if (it != this->pathTable.end()) this->setInputBooleanChanged(deviceIndex, it->second);
}

void DeviceStateModel::addBooleanInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle) {
	this->booleanInputs[deviceIndex][path] = std::make_pair(*componentHandle, ModelDeviceInputBooleanSerialized{});
}

void DeviceStateModel::removeBooleanInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->booleanInputs.find(deviceIndex);
	
	if (it != this->booleanInputs.end()) {
		it->second.erase(path);
	}
}

ModelDeviceInputScalarSerialized* DeviceStateModel::getScalarInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->scalarInputs.find(deviceIndex);

	if (it1 == this->scalarInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second.second);
}

ModelDeviceInputScalarSerialized* DeviceStateModel::getScalarInput(uint32_t deviceIndex, uint32_t pathID) {
	auto it = this->pathTable.find(pathID);
	return it == this->pathTable.end() ? nullptr : this->getScalarInput(deviceIndex, it->second);
}

ModelDeviceInputScalarSerialized* DeviceStateModel::getScalarInput(vr::VRInputComponentHandle_t componentHandle) {
	for (auto& devicePair : this->scalarInputs) {
		for (auto& pathPair : devicePair.second) {
			if (pathPair.second.first == componentHandle) {
				return &(pathPair.second.second);
			}
		}
	}
	return nullptr;
}

void DeviceStateModel::setInputScalarChanged(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputScalarSerialized* inputScalar = this->getScalarInput(deviceIndex, path);
	if (inputScalar != nullptr) {
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputScalarUpdateToSharedMemory(&inputScalar->data, deviceIndex, this->findIndexOfPath(path));
	}
}

void DeviceStateModel::setInputScalarChanged(uint32_t deviceIndex, uint32_t pathID) {
	auto it = this->pathTable.find(pathID);
	if (it != this->pathTable.end()) this->setInputScalarChanged(deviceIndex, it->second);
}

void DeviceStateModel::addScalarInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle) {
	this->scalarInputs[deviceIndex][path] = std::make_pair(*componentHandle, ModelDeviceInputScalarSerialized{});
}

void DeviceStateModel::removeScalarInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->scalarInputs.find(deviceIndex);

	if (it != this->scalarInputs.end()) {
		it->second.erase(path);
	}
}

ModelDeviceInputSkeletonSerialized* DeviceStateModel::getSkeletonInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->skeletonInputs.find(deviceIndex);

	if (it1 == this->skeletonInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second.second);
}

ModelDeviceInputSkeletonSerialized* DeviceStateModel::getSkeletonInput(uint32_t deviceIndex, uint32_t pathID) {
	auto it = this->pathTable.find(pathID);
	return it == this->pathTable.end() ? nullptr : this->getSkeletonInput(deviceIndex, it->second);
}

ModelDeviceInputSkeletonSerialized* DeviceStateModel::getSkeletonInput(vr::VRInputComponentHandle_t componentHandle) {
	for (auto& devicePair : this->skeletonInputs) {
		for (auto& pathPair : devicePair.second) {
			if (pathPair.second.first == componentHandle) {
				return &(pathPair.second.second);
			}
		}
	}
	return nullptr;
}

void DeviceStateModel::setInputSkeletonChanged(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputSkeletonSerialized* inputSkeleton = this->getSkeletonInput(deviceIndex, path);
	if (inputSkeleton != nullptr) {
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputSkeletonUpdateToSharedMemory(&inputSkeleton->data, deviceIndex, this->findIndexOfPath(path));
	}
}

void DeviceStateModel::setInputSkeletonChanged(uint32_t deviceIndex, uint32_t pathID) {
	auto it = this->pathTable.find(pathID);
	if (it != this->pathTable.end()) this->setInputSkeletonChanged(deviceIndex, it->second);
}

void DeviceStateModel::addSkeletonInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle) {
	this->skeletonInputs[deviceIndex][path] = std::make_pair(*componentHandle, ModelDeviceInputSkeletonSerialized{});
}

void DeviceStateModel::removeSkeletonInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->skeletonInputs.find(deviceIndex);

	if (it != this->skeletonInputs.end()) {
		it->second.erase(path);
	}
}

ModelDeviceInputPoseSerialized* DeviceStateModel::getPoseInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->poseInputs.find(deviceIndex);

	if (it1 == this->poseInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second.second);
}

ModelDeviceInputPoseSerialized* DeviceStateModel::getPoseInput(uint32_t deviceIndex, uint32_t pathID) {
	auto it = this->pathTable.find(pathID);
	return it == this->pathTable.end() ? nullptr : this->getPoseInput(deviceIndex, it->second);
}

ModelDeviceInputPoseSerialized* DeviceStateModel::getPoseInput(vr::VRInputComponentHandle_t componentHandle) {
	for (auto& devicePair : this->poseInputs) {
		for (auto& pathPair : devicePair.second) {
			if (pathPair.second.first == componentHandle) {
				return &(pathPair.second.second);
			}
		}
	}
	return nullptr;
}

void DeviceStateModel::setInputPoseChanged(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputPoseSerialized* inputPose = this->getPoseInput(deviceIndex, path);
	if (inputPose != nullptr) {
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputPoseUpdateToSharedMemory(&inputPose->data, deviceIndex, this->findIndexOfPath(path));
	}
}

void DeviceStateModel::setInputPoseChanged(uint32_t deviceIndex, uint32_t pathID) {
	auto it = this->pathTable.find(pathID);
	if (it != this->pathTable.end()) this->setInputPoseChanged(deviceIndex, it->second);
}

void DeviceStateModel::addPoseInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle) {
	this->poseInputs[deviceIndex][path] = std::make_pair(*componentHandle, ModelDeviceInputPoseSerialized{});
}

void DeviceStateModel::removePoseInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->poseInputs.find(deviceIndex);

	if (it != this->poseInputs.end()) {
		it->second.erase(path);
	}
}

ModelDeviceInputEyeTrackingSerialized* DeviceStateModel::getEyeTrackingInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->eyeTrackingInputs.find(deviceIndex);

	if (it1 == this->eyeTrackingInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second.second);
}

ModelDeviceInputEyeTrackingSerialized* DeviceStateModel::getEyeTrackingInput(uint32_t deviceIndex, uint32_t pathID) {
	auto it = this->pathTable.find(pathID);
	return it == this->pathTable.end() ? nullptr : this->getEyeTrackingInput(deviceIndex, it->second);
}

ModelDeviceInputEyeTrackingSerialized* DeviceStateModel::getEyeTrackingInput(vr::VRInputComponentHandle_t componentHandle) {
	for (auto& devicePair : this->eyeTrackingInputs) {
		for (auto& pathPair : devicePair.second) {
			if (pathPair.second.first == componentHandle) {
				return &(pathPair.second.second);
			}
		}
	}
	return nullptr;
}

void DeviceStateModel::setInputEyeTrackingChanged(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputEyeTrackingSerialized* inputEyeTracking = this->getEyeTrackingInput(deviceIndex, path);
	if (inputEyeTracking != nullptr) {
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputEyeTrackingUpdateToSharedMemory(&inputEyeTracking->data, deviceIndex, this->findIndexOfPath(path));
	}
}

void DeviceStateModel::setInputEyeTrackingChanged(uint32_t deviceIndex, uint32_t pathID) {
	auto it = this->pathTable.find(pathID);
	if (it != this->pathTable.end()) this->setInputEyeTrackingChanged(deviceIndex, it->second);
}

void DeviceStateModel::addEyeTrackingInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle) {
	this->eyeTrackingInputs[deviceIndex][path] = std::make_pair(*componentHandle, ModelDeviceInputEyeTrackingSerialized{});
}

void DeviceStateModel::removeEyeTrackingInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->eyeTrackingInputs.find(deviceIndex);

	if (it != this->eyeTrackingInputs.end()) {
		it->second.erase(path);
	}
}

void DeviceStateModel::cleanDeviceIndexAssociations(uint32_t deviceIndex) {
	// Free path table slot
	this->pathTable.erase(deviceIndex);

	// Free device pose
	this->removeDevicePose(deviceIndex);

	// Free index table
	this->removeDeviceIndexToContainerMapping(deviceIndex);

	// Collect all paths associated with this device index
	std::vector<std::string> pathsToCheck;
	
	collectPathsAndEraseDevice(this->booleanInputs, deviceIndex, pathsToCheck);
	collectPathsAndEraseDevice(this->scalarInputs, deviceIndex, pathsToCheck);
	collectPathsAndEraseDevice(this->skeletonInputs, deviceIndex, pathsToCheck);
	collectPathsAndEraseDevice(this->poseInputs, deviceIndex, pathsToCheck);
	collectPathsAndEraseDevice(this->eyeTrackingInputs, deviceIndex, pathsToCheck);
	
	// Remove paths that are no longer used by any device
	for (const auto& path : pathsToCheck) {
		if (!isPathUsedByAnyDevice(path)) {
			this->removeDevicePath(path);
		}
	}
}

uint32_t DeviceStateModel::findIndexOfPath(const std::string& path) {
	auto it = std::find_if(this->pathTable.begin(), this->pathTable.end(),
		[&](const std::pair<uint32_t, std::string>& pair) {
			return pair.second == path;
		}
	);

	if (it != this->pathTable.end()) {
		return it->first;
	}

	return static_cast<uint32_t>(-1);
}

uint32_t DeviceStateModel::findNextAvailablePathTableIndex() {
	for (uint32_t i = 0; i < PATH_TABLE_ENTRIES; i++) {
		auto it = this->pathTable.find(i);

		if (it == this->pathTable.end()) {
			return i;
		}
	}

	return static_cast<uint32_t>(-1);
}

bool DeviceStateModel::isPathUsedByAnyDevice(const std::string& path) {
	return isPathInUse(this->booleanInputs, path) ||
	       isPathInUse(this->scalarInputs, path) ||
	       isPathInUse(this->skeletonInputs, path) ||
	       isPathInUse(this->poseInputs, path) ||
	       isPathInUse(this->eyeTrackingInputs, path);
}