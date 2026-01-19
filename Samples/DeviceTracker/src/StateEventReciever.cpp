#include "StateEventReciever.h"
#include "Model.h"

void StateEventReciever::DeviceInputBooleanAdded(uint32_t deviceIndex, const std::string& path) {
	BooleanInput input = {};
	Model::getInstance().updateBooleanInput(deviceIndex, path, input);
}

void StateEventReciever::DeviceInputBooleanRemoved(uint32_t deviceIndex, const std::string& path) {
	Model::getInstance().removeBooleanInput(deviceIndex, path);
}

void StateEventReciever::DeviceInputScalarAdded(uint32_t deviceIndex, const std::string& path) {
	ScalarInput input = {};
	Model::getInstance().updateScalarInput(deviceIndex, path, input);
}

void StateEventReciever::DeviceInputScalarRemoved(uint32_t deviceIndex, const std::string& path) {
	Model::getInstance().removeScalarInput(deviceIndex, path);
}

void StateEventReciever::DeviceInputSkeletonAdded(uint32_t deviceIndex, const std::string& path) {
	SkeletonInput input = {};
	Model::getInstance().updateSkeletonInput(deviceIndex, path, input);
}

void StateEventReciever::DeviceInputSkeletonRemoved(uint32_t deviceIndex, const std::string& path) {
	Model::getInstance().removeSkeletonInput(deviceIndex, path);
}

void StateEventReciever::DeviceInputPoseAdded(uint32_t deviceIndex, const std::string& path) {
	PoseInput input = {};
	Model::getInstance().updatePoseInput(deviceIndex, path, input);
}

void StateEventReciever::DeviceInputPoseRemoved(uint32_t deviceIndex, const std::string& path) {
	Model::getInstance().removePoseInput(deviceIndex, path);
}

void StateEventReciever::DeviceInputEyeTrackingAdded(uint32_t deviceIndex, const std::string& path) {
	EyeTrackingInput input = {};
	Model::getInstance().updateEyeTrackingInput(deviceIndex, path, input);
}

void StateEventReciever::DeviceInputEyeTrackingRemoved(uint32_t deviceIndex, const std::string& path) {
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