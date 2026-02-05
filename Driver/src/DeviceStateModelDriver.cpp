#include "DeviceStateModelDriver.h"

DeviceStateModel& DeviceStateModel::getInstance() {
	static DeviceStateModel instance;
	return instance;
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

	return UINT32_MAX;
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

		if (pose->useOverridenState) {
			callTrackedDevicePoseUpdated(deviceIndex, ToDriverPose(pose->data.overwrittenPose), sizeof(vr::DriverPose_t));
		}
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
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputBooleanUpdateToSharedMemory(&inputBoolean->data, deviceIndex, path);

		if (inputBoolean->useOverridenState) {
			callUpdateBooleanComponent(this->booleanInputs[deviceIndex][path].first, inputBoolean->data.overwrittenValue.value, inputBoolean->data.overwrittenValue.timeOffset);
		}
	}
}

void DeviceStateModel::setInputBooleanChanged(vr::VRInputComponentHandle_t componentHandle) {
	for (auto& devicePair : this->booleanInputs) {
		for (auto& pathPair : devicePair.second) {
			if (pathPair.second.first == componentHandle) {
				uint32_t deviceIndex = devicePair.first;
				const std::string& path = pathPair.first;
				SharedDeviceMemoryDriver::getInstance().syncDeviceInputBooleanUpdateToSharedMemory(
					&pathPair.second.second.data, 
					deviceIndex, 
					path
				);
				return;
			}
		}
	}
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
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputScalarUpdateToSharedMemory(&inputScalar->data, deviceIndex, path);

		if (inputScalar->useOverridenState) {
			callUpdateScalarComponent(this->scalarInputs[deviceIndex][path].first, inputScalar->data.overwrittenValue.value, inputScalar->data.overwrittenValue.timeOffset);
		}
	}
}

void DeviceStateModel::setInputScalarChanged(vr::VRInputComponentHandle_t componentHandle) {
	for (auto& devicePair : this->scalarInputs) {
		for (auto& pathPair : devicePair.second) {
			if (pathPair.second.first == componentHandle) {
				uint32_t deviceIndex = devicePair.first;
				const std::string& path = pathPair.first;
				SharedDeviceMemoryDriver::getInstance().syncDeviceInputScalarUpdateToSharedMemory(
					&pathPair.second.second.data,
					deviceIndex,
					path
				);
				return;
			}
		}
	}
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
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputSkeletonUpdateToSharedMemory(&inputSkeleton->data, deviceIndex, path);

		if (inputSkeleton->useOverridenState) { 
			vr::VRBoneTransform_t transforms[31];
			ToVRBoneTransforms(inputSkeleton->data.overwrittenValue, transforms);
			callUpdateSkeletonComponent(this->skeletonInputs[deviceIndex][path].first, static_cast<vr::EVRSkeletalMotionRange>(inputSkeleton->data.overwrittenValue.motionRange), transforms, inputSkeleton->data.overwrittenValue.boneTransformCount);
		}
	}
}

void DeviceStateModel::setInputSkeletonChanged(vr::VRInputComponentHandle_t componentHandle) {
	for (auto& devicePair : this->skeletonInputs) {
		for (auto& pathPair : devicePair.second) {
			if (pathPair.second.first == componentHandle) {
				uint32_t deviceIndex = devicePair.first;
				const std::string& path = pathPair.first;
				SharedDeviceMemoryDriver::getInstance().syncDeviceInputSkeletonUpdateToSharedMemory(
					&pathPair.second.second.data,
					deviceIndex,
					path
				);
				return;
			}
		}
	}
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
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputPoseUpdateToSharedMemory(&inputPose->data, deviceIndex, path);

		if (inputPose->useOverridenState) {
			vr::HmdMatrix34_t poseOffset = ToHmdMatrix34(inputPose->data.overwrittenValue.poseOffset);
			callUpdatePoseComponent(this->poseInputs[deviceIndex][path].first, &poseOffset, inputPose->data.overwrittenValue.timeOffset);
		}
	}
}

void DeviceStateModel::setInputPoseChanged(vr::VRInputComponentHandle_t componentHandle) {
	for (auto& devicePair : this->poseInputs) {
		for (auto& pathPair : devicePair.second) {
			if (pathPair.second.first == componentHandle) {
				uint32_t deviceIndex = devicePair.first;
				const std::string& path = pathPair.first;
				SharedDeviceMemoryDriver::getInstance().syncDeviceInputPoseUpdateToSharedMemory(
					&pathPair.second.second.data,
					deviceIndex,
					path
				);
				return;
			}
		}
	}
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
		SharedDeviceMemoryDriver::getInstance().syncDeviceInputEyeTrackingUpdateToSharedMemory(&inputEyeTracking->data, deviceIndex, path);

		if (inputEyeTracking->useOverridenState) {
			vr::VREyeTrackingData_t eyeData = ToVREyeTrackingData(inputEyeTracking->data.overwrittenValue.eyeTrackingData);
			callUpdateEyeTrackingComponent(this->eyeTrackingInputs[deviceIndex][path].first, &eyeData, inputEyeTracking->data.overwrittenValue.timeOffset);
		}
	}
}

void DeviceStateModel::setInputEyeTrackingChanged(vr::VRInputComponentHandle_t componentHandle) {
	for (auto& devicePair : this->eyeTrackingInputs) {
		for (auto& pathPair : devicePair.second) {
			if (pathPair.second.first == componentHandle) {
				uint32_t deviceIndex = devicePair.first;
				const std::string& path = pathPair.first;
				SharedDeviceMemoryDriver::getInstance().syncDeviceInputEyeTrackingUpdateToSharedMemory(
					&pathPair.second.second.data,
					deviceIndex,
					path
				);
				return;
			}
		}
	}
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