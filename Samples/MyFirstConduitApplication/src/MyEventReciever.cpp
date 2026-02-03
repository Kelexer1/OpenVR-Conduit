#include "MyEventReciever.h"

void MyEventReciever::DeviceInputBooleanAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added boolean input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReciever::DeviceInputBooleanRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed boolean input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReciever::DeviceInputScalarAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added scalar input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReciever::DeviceInputScalarRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed scalar input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReciever::DeviceInputSkeletonAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added skeleton input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReciever::DeviceInputSkeletonRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed skeleton input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReciever::DeviceInputPoseAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added pose input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReciever::DeviceInputPoseRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed pose input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReciever::DeviceInputEyeTrackingAdded(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Added eye tracking input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReciever::DeviceInputEyeTrackingRemoved(uint32_t deviceIndex, const std::string& path) {
	std::cout << "Removed eye tracking input \"" << path << "\" for Device " << deviceIndex << "\n";
}

void MyEventReciever::DevicePoseChanged(uint32_t deviceIndex, DevicePose oldPose, DevicePose newPose) {}

void MyEventReciever::DeviceInputBooleanChanged(uint32_t deviceIndex, std::string path, BooleanInput oldInput, BooleanInput newInput) {}

void MyEventReciever::DeviceInputScalarChanged(uint32_t deviceIndex, std::string path, ScalarInput oldInput, ScalarInput newInput) {}

void MyEventReciever::DeviceInputSkeletonChanged(uint32_t deviceIndex, std::string path, SkeletonInput oldInput, SkeletonInput newInput) {}

void MyEventReciever::DeviceInputPoseChanged(uint32_t deviceIndex, std::string path, PoseInput oldInput, PoseInput newInput) {}

void MyEventReciever::DeviceInputEyeTrackingChanged(uint32_t deviceIndex, std::string path, EyeTrackingInput oldInput, EyeTrackingInput newInput) {}
