#include "DeviceStateModel.h"

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
		SharedDeviceMemoryDriver::getInstance().syncPathTableToSharedMemory(this);
	}
}

void DeviceStateModel::removeDevicePath(const std::string& path) {
	if (this->pathTable.erase(findIndexOfPath(path) == 1)) {
		SharedDeviceMemoryDriver::getInstance().syncPathTableToSharedMemory(this);
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

DevicePoseSerialized* DeviceStateModel::getDevicePose(uint32_t deviceIndex) {
	auto it = this->devicePoses.find(deviceIndex);
	return it == this->devicePoses.end() ? nullptr : &(it->second);
}

void DeviceStateModel::setDevicePoseChanged(uint32_t deviceIndex) {
	DevicePoseSerialized* pose = this->getDevicePose(deviceIndex);
	if (pose != nullptr) {
		SharedDeviceMemoryDriver::getInstance().syncDevicePoseUpdateToSharedMemory(pose);
	}
}

void DeviceStateModel::addDevicePose(uint32_t deviceIndex) {
	this->devicePoses[deviceIndex];
}

void DeviceStateModel::removeDevicePose(uint32_t deviceIndex) {
	this->devicePoses.erase(deviceIndex);
}

DeviceInputBooleanSerialized* DeviceStateModel::getBooleanInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->booleanInputs.find(deviceIndex);

	if (it1 == this->booleanInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModel::setInputBooleanChanged(uint32_t deviceIndex, const std::string& path) {
	DeviceInputBooleanSerialized* inputBoolean = this->getBooleanInput(deviceIndex, path);
	if (inputBoolean != nullptr) {
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputBooleanUpdateToSharedMemory(inputBoolean);
	}
}

void DeviceStateModel::addBooleanInput(uint32_t deviceIndex, const std::string& path) {
	this->booleanInputs[deviceIndex][path];
}

void DeviceStateModel::removeBooleanInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->booleanInputs.find(deviceIndex);
	
	if (it != this->booleanInputs.end()) {
		it->second.erase(path);
	}
}

DeviceInputScalarSerialized* DeviceStateModel::getScalarInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->scalarInputs.find(deviceIndex);

	if (it1 == this->scalarInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModel::setInputScalarChanged(uint32_t deviceIndex, const std::string& path) {
	DeviceInputScalarSerialized* inputScalar = this->getScalarInput(deviceIndex, path);
	if (inputScalar != nullptr) {
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputScalarUpdateToSharedMemory(inputScalar);
	}
}

void DeviceStateModel::addScalarInput(uint32_t deviceIndex, const std::string& path) {
	this->scalarInputs[deviceIndex][path];
}

void DeviceStateModel::removeScalarInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->scalarInputs.find(deviceIndex);

	if (it != this->scalarInputs.end()) {
		it->second.erase(path);
	}
}

DeviceInputSkeletonSerialized* DeviceStateModel::getSkeletonInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->skeletonInputs.find(deviceIndex);

	if (it1 == this->skeletonInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModel::setInputSkeletonChanged(uint32_t deviceIndex, const std::string& path) {
	DeviceInputSkeletonSerialized* inputSkeleton = this->getSkeletonInput(deviceIndex, path);
	if (inputSkeleton != nullptr) {
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputSkeletonUpdateToSharedMemory(inputSkeleton);
	}
}

void DeviceStateModel::addSkeletonInput(uint32_t deviceIndex, const std::string& path) {
	this->skeletonInputs[deviceIndex][path];
}

void DeviceStateModel::removeSkeletonInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->skeletonInputs.find(deviceIndex);

	if (it != this->skeletonInputs.end()) {
		it->second.erase(path);
	}
}

DeviceInputPoseSerialized* DeviceStateModel::getPoseInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->poseInputs.find(deviceIndex);

	if (it1 == this->poseInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModel::setInputPoseChanged(uint32_t deviceIndex, const std::string& path) {
	DeviceInputPoseSerialized* inputPose = this->getPoseInput(deviceIndex, path);
	if (inputPose != nullptr) {
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputPoseUpdateToSharedMemory(inputPose);
	}
}

void DeviceStateModel::addPoseInput(uint32_t deviceIndex, const std::string& path) {
	this->poseInputs[deviceIndex][path];
}

void DeviceStateModel::removePoseInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->poseInputs.find(deviceIndex);

	if (it != this->poseInputs.end()) {
		it->second.erase(path);
	}
}

DeviceInputEyeTrackingSerialized* DeviceStateModel::getEyeTrackingInput(uint32_t deviceIndex, const std::string& path) {
	auto it1 = this->eyeTrackingInputs.find(deviceIndex);

	if (it1 == this->eyeTrackingInputs.end()) {
		return nullptr;
	}

	auto it2 = it1->second.find(path);

	return it2 == it1->second.end() ? nullptr : &(it2->second);
}

void DeviceStateModel::setInputEyeTrackingChanged(uint32_t deviceIndex, const std::string& path) {
	DeviceInputEyeTrackingSerialized* inputEyeTracking = this->getEyeTrackingInput(deviceIndex, path);
	if (inputEyeTracking != nullptr) {
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputEyeTrackingUpdateToSharedMemory(inputEyeTracking);
	}
}

void DeviceStateModel::addEyeTrackingInput(uint32_t deviceIndex, const std::string& path) {
	this->eyeTrackingInputs[deviceIndex][path];
}

void DeviceStateModel::removeEyeTrackingInput(uint32_t deviceIndex, const std::string& path) {
	auto it = this->eyeTrackingInputs.find(deviceIndex);

	if (it != this->eyeTrackingInputs.end()) {
		it->second.erase(path);
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