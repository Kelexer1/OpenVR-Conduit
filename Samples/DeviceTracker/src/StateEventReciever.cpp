#include "StateEventReciever.h"
#include "Model.h"

void StateEventReciever::DeviceInputBooleanAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added boolean input " << deviceIndex << " : " << path << "\n";
	BooleanInput input = {};
	Model::getInstance().updateBooleanInput(deviceIndex, path, input);
}

void StateEventReciever::DeviceInputBooleanRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed boolean input " << deviceIndex << " : " << path << "\n";
	Model::getInstance().removeBooleanInput(deviceIndex, path);
}

void StateEventReciever::DeviceInputScalarAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added scalar input " << deviceIndex << " : " << path << "\n";
	ScalarInput input = {};
	Model::getInstance().updateScalarInput(deviceIndex, path, input);
}

void StateEventReciever::DeviceInputScalarRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed scalar input " << deviceIndex << " : " << path << "\n";
	Model::getInstance().removeScalarInput(deviceIndex, path);
}

void StateEventReciever::DeviceInputSkeletonAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added skeleton input " << deviceIndex << " : " << path << "\n";
	SkeletonInput input = {};
	Model::getInstance().updateSkeletonInput(deviceIndex, path, input);
}

void StateEventReciever::DeviceInputSkeletonRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed skeleton input " << deviceIndex << " : " << path << "\n";
	Model::getInstance().removeSkeletonInput(deviceIndex, path);
}

void StateEventReciever::DeviceInputPoseAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added pose input " << deviceIndex << " : " << path << "\n";
	PoseInput input = {};
	Model::getInstance().updatePoseInput(deviceIndex, path, input);
}

void StateEventReciever::DeviceInputPoseRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed pose input " << deviceIndex << " : " << path << "\n";
	Model::getInstance().removePoseInput(deviceIndex, path);
}

void StateEventReciever::DeviceInputEyeTrackingAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added eye tracking input " << deviceIndex << " : " << path << "\n";
	EyeTrackingInput input = {};
	Model::getInstance().updateEyeTrackingInput(deviceIndex, path, input);
}

void StateEventReciever::DeviceInputEyeTrackingRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed eye tracking input " << deviceIndex << " : " << path << "\n";
	Model::getInstance().removeEyeTrackingInput(deviceIndex, path);
}

void StateEventReciever::DevicePoseChanged(uint32_t deviceIndex, DevicePose oldPose, DevicePose newPose) {
	Model::getInstance().updateDevicePose(deviceIndex, newPose);
}

void StateEventReciever::DeviceInputBooleanChanged(uint32_t deviceIndex, std::string path, BooleanInput oldInput, BooleanInput newInput) {
	Model::getInstance().updateBooleanInput(deviceIndex, path, newInput);
}

void StateEventReciever::DeviceInputScalarChanged(uint32_t deviceIndex, std::string path, ScalarInput oldInput, ScalarInput newInput) {
	Model::getInstance().updateScalarInput(deviceIndex, path, newInput);
}

void StateEventReciever::DeviceInputSkeletonChanged(uint32_t deviceIndex, std::string path, SkeletonInput oldInput, SkeletonInput newInput) {
	Model::getInstance().updateSkeletonInput(deviceIndex, path, newInput);
}

void StateEventReciever::DeviceInputPoseChanged(uint32_t deviceIndex, std::string path, PoseInput oldInput, PoseInput newInput) {
	Model::getInstance().updatePoseInput(deviceIndex, path, newInput);
}

void StateEventReciever::DeviceInputEyeTrackingChanged(uint32_t deviceIndex, std::string path, EyeTrackingInput oldInput, EyeTrackingInput newInput) {
	Model::getInstance().updateEyeTrackingInput(deviceIndex, path, newInput);
}