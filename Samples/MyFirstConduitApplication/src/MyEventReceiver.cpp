#include "MyEventReceiver.h"

void MyEventReceiver::DeviceInputBooleanAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added boolean input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReceiver::DeviceInputBooleanRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed boolean input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReceiver::DeviceInputScalarAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added scalar input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReceiver::DeviceInputScalarRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed scalar input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReceiver::DeviceInputSkeletonAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added skeleton input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReceiver::DeviceInputSkeletonRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed skeleton input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReceiver::DeviceInputPoseAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added pose input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReceiver::DeviceInputPoseRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed pose input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReceiver::DeviceInputEyeTrackingAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added eye tracking input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReceiver::DeviceInputEyeTrackingRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed eye tracking input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReceiver::DevicePoseChanged(uint32_t deviceIndex, DevicePose oldPose, DevicePose newPose) {}

void MyEventReceiver::DeviceInputBooleanChanged(uint32_t deviceIndex, std::string path, BooleanInput oldInput, BooleanInput newInput) {}

void MyEventReceiver::DeviceInputScalarChanged(uint32_t deviceIndex, std::string path, ScalarInput oldInput, ScalarInput newInput) {}

void MyEventReceiver::DeviceInputSkeletonChanged(uint32_t deviceIndex, std::string path, SkeletonInput oldInput, SkeletonInput newInput) {}

void MyEventReceiver::DeviceInputPoseChanged(uint32_t deviceIndex, std::string path, PoseInput oldInput, PoseInput newInput) {}

void MyEventReceiver::DeviceInputEyeTrackingChanged(uint32_t deviceIndex, std::string path, EyeTrackingInput oldInput, EyeTrackingInput newInput) {}
