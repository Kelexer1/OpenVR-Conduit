#include "Model.h"
#include "openvr.h"

Model::Model(DeviceStateCommandSender& commandSender) : commandSender(commandSender) {}

void Model::DeviceInputBooleanAdded(uint32_t deviceIndex, const std::string& path) {
	this->booleanInputs[deviceIndex][path];
	commandSender.setUseOverriddenBooleanInputState(deviceIndex, path, true);
}

void Model::DeviceInputBooleanRemoved(uint32_t deviceIndex, const std::string& path) {
	auto it = this->booleanInputs.find(deviceIndex);
	if (it != this->booleanInputs.end()) {
		it->second.erase(path);
		if (it->second.empty()) {
			this->booleanInputs.erase(it);
		}
	}
}

void Model::DeviceInputScalarAdded(uint32_t deviceIndex, const std::string& path) {
	this->scalarInputs[deviceIndex][path];
}

void Model::DeviceInputScalarRemoved(uint32_t deviceIndex, const std::string& path) {
	auto it = this->scalarInputs.find(deviceIndex);
	if (it != this->scalarInputs.end()) {
		it->second.erase(path);
		if (it->second.empty()) {
			this->scalarInputs.erase(it);
		}
	}
}

void Model::DeviceInputSkeletonAdded(uint32_t deviceIndex, const std::string& path) {
	this->scalarInputs[deviceIndex][path];
}

void Model::DeviceInputSkeletonRemoved(uint32_t deviceIndex, const std::string& path) {
	auto it = this->skeletonInputs.find(deviceIndex);
	if (it != this->skeletonInputs.end()) {
		it->second.erase(path);
		if (it->second.empty()) {
			this->skeletonInputs.erase(it);
		}
	}
}

void Model::DeviceInputPoseAdded(uint32_t deviceIndex, const std::string& path) {
	this->poseInputs[deviceIndex][path];
}

void Model::DeviceInputPoseRemoved(uint32_t deviceIndex, const std::string& path) {
	auto it = this->poseInputs.find(deviceIndex);
	if (it != this->poseInputs.end()) {
		it->second.erase(path);
		if (it->second.empty()) {
			this->poseInputs.erase(it);
		}
	}
}

void Model::DeviceInputEyeTrackingAdded(uint32_t deviceIndex, const std::string& path) {
	this->eyeTrackingInputs[deviceIndex][path];
}

void Model::DeviceInputEyeTrackingRemoved(uint32_t deviceIndex, const std::string& path) {
	auto it = this->eyeTrackingInputs.find(deviceIndex);
	if (it != this->eyeTrackingInputs.end()) {
		it->second.erase(path);
		if (it->second.empty()) {
			this->eyeTrackingInputs.erase(it);
		}
	}
}

void Model::DevicePoseChanged(uint32_t deviceIndex, DevicePose oldPose, DevicePose newPose) {
	this->devicePoses[deviceIndex] = newPose;
}

void Model::DeviceInputBooleanChanged(uint32_t deviceIndex, std::string path, BooleanInput oldInput, BooleanInput newInput) {
	this->booleanInputs[deviceIndex][path] = newInput;
}

void Model::DeviceInputScalarChanged(uint32_t deviceIndex, std::string path, ScalarInput oldInput, ScalarInput newInput) {
	this->scalarInputs[deviceIndex][path] = newInput;
}

void Model::DeviceInputSkeletonChanged(uint32_t deviceIndex, std::string path, SkeletonInput oldInput, SkeletonInput newInput) {
	this->skeletonInputs[deviceIndex][path] = newInput;
}

void Model::DeviceInputPoseChanged(uint32_t deviceIndex, std::string path, PoseInput oldInput, PoseInput newInput) {
	this->poseInputs[deviceIndex][path] = newInput;
}

void Model::DeviceInputEyeTrackingChanged(uint32_t deviceIndex, std::string path, EyeTrackingInput oldInput, EyeTrackingInput newInput) {
	this->eyeTrackingInputs[deviceIndex][path] = newInput;
}

void Model::print() {
	std::cout << "========== Current State ==========\n";
	for (uint32_t deviceIndex = 0; deviceIndex < vr::k_unMaxTrackedDeviceCount; deviceIndex++) {
		// Check if this device has any data
		bool hasData = devicePoses.find(deviceIndex) != devicePoses.end() ||
					   booleanInputs.find(deviceIndex) != booleanInputs.end() ||
					   scalarInputs.find(deviceIndex) != scalarInputs.end() ||
					   skeletonInputs.find(deviceIndex) != skeletonInputs.end() ||
					   poseInputs.find(deviceIndex) != poseInputs.end() ||
					   eyeTrackingInputs.find(deviceIndex) != eyeTrackingInputs.end();
		
		if (!hasData) continue;
		
		std::cout << "Device " + std::to_string(deviceIndex) + ":\n";
		
		// Device Pose
		if (devicePoses.find(deviceIndex) != devicePoses.end()) {
			const auto& pose = devicePoses.at(deviceIndex);
			std::cout << "  Pose: pos[" + 
				std::to_string(pose.vecPosition[0]) + ", " +
				std::to_string(pose.vecPosition[1]) + ", " +
				std::to_string(pose.vecPosition[2]) + "] valid=" +
				(pose.poseIsValid ? "true" : "false") + "\n";
		}
		
		// Boolean Inputs
		if (booleanInputs.find(deviceIndex) != booleanInputs.end()) {
			for (const auto& [path, input] : booleanInputs.at(deviceIndex)) {
				std::cout << "  Boolean Input [" + path + "]: " + 
					(input.value ? "true" : "false") + "\n";
			}
		}
		
		// Scalar Inputs
		if (scalarInputs.find(deviceIndex) != scalarInputs.end()) {
			for (const auto& [path, input] : scalarInputs.at(deviceIndex)) {
				std::cout << "  Scalar Input [" + path + "]: " + 
					std::to_string(input.value) + "\n";
			}
		}
		
		// Skeleton Inputs
		if (skeletonInputs.find(deviceIndex) != skeletonInputs.end()) {
			for (const auto& [path, input] : skeletonInputs.at(deviceIndex)) {
				std::cout << "  Skeleton Input [" + path + "]: " + 
					std::to_string(input.boneTransformCount) + " bones, range=" +
					std::to_string(input.motionRange) + "\n";
			}
		}
		
		// Pose Inputs
		if (poseInputs.find(deviceIndex) != poseInputs.end()) {
			for (const auto& [path, input] : poseInputs.at(deviceIndex)) {
				std::cout << "  Pose Input [" + path + "]: offset=" + 
					std::to_string(input.timeOffset) + "\n";
			}
		}
		
		// Eye Tracking Inputs
		if (eyeTrackingInputs.find(deviceIndex) != eyeTrackingInputs.end()) {
			for (const auto& [path, input] : eyeTrackingInputs.at(deviceIndex)) {
				const auto& et = input.eyeTrackingData;
				std::cout << "  Eye Tracking Input [" + path + "]: active=" + 
					(et.active ? "true" : "false") + " valid=" +
					(et.valid ? "true" : "false") + " tracked=" +
					(et.tracked ? "true" : "false") + "\n";
			}
		}
		
		std::cout << "\n";
	}
}