#include "Model.h"
#include "openvr.h"

Model& Model::getInstance() {
	static Model instance;
	return instance;
}

void Model::updateDevicePose(uint32_t deviceIndex, DevicePose newPose) {
	this->devicePoses[deviceIndex] = newPose;
}

void Model::removeDevicePose(uint32_t deviceIndex) {
	this->devicePoses.erase(deviceIndex);
}

void Model::updateBooleanInput(uint32_t deviceIndex, const std::string& path, BooleanInput newInput) {
	this->booleanInputs[deviceIndex][path] = newInput;
}

void Model::removeBooleanInput(uint32_t deviceIndex, const std::string& path) {
	this->booleanInputs[deviceIndex].erase(path);
}

void Model::updateScalarInput(uint32_t deviceIndex, const std::string& path, ScalarInput newInput) {
	this->scalarInputs[deviceIndex][path] = newInput;
}

void Model::removeScalarInput(uint32_t deviceIndex, const std::string& path) {
	this->scalarInputs[deviceIndex].erase(path);
}

void Model::updateSkeletonInput(uint32_t deviceIndex, const std::string& path, SkeletonInput newInput) {
	this->skeletonInputs[deviceIndex][path] = newInput;
}

void Model::removeSkeletonInput(uint32_t deviceIndex, const std::string& path) {
	this->skeletonInputs[deviceIndex].erase(path);
}

void Model::updatePoseInput(uint32_t deviceIndex, const std::string& path, PoseInput newInput) {
	this->poseInputs[deviceIndex][path] = newInput;
}

void Model::removePoseInput(uint32_t deviceIndex, const std::string& path) {
	this->poseInputs[deviceIndex].erase(path);
}

void Model::updateEyeTrackingInput(uint32_t deviceIndex, const std::string& path, EyeTrackingInput newInput) {
	this->eyeTrackingInputs[deviceIndex][path] = newInput;
}

void Model::removeEyeTrackingInput(uint32_t deviceIndex, const std::string& path) {
	this->eyeTrackingInputs[deviceIndex].erase(path);
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

Model::Model() {}