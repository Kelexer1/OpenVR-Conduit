#include "StateEventReceiver.h"

StateEventReceiver::StateEventReceiver(DeviceStateCommandSender& commandSender) : commandSender(commandSender) {}

void StateEventReceiver::DeviceInputBooleanAdded(uint32_t deviceIndex, const std::string& path) {}

void StateEventReceiver::DeviceInputBooleanRemoved(uint32_t deviceIndex, const std::string& path) {}

void StateEventReceiver::DeviceInputScalarAdded(uint32_t deviceIndex, const std::string& path) {}

void StateEventReceiver::DeviceInputScalarRemoved(uint32_t deviceIndex, const std::string& path) {}

void StateEventReceiver::DeviceInputSkeletonAdded(uint32_t deviceIndex, const std::string& path) {}

void StateEventReceiver::DeviceInputSkeletonRemoved(uint32_t deviceIndex, const std::string& path) {}

void StateEventReceiver::DeviceInputPoseAdded(uint32_t deviceIndex, const std::string& path) {}

void StateEventReceiver::DeviceInputPoseRemoved(uint32_t deviceIndex, const std::string& path) {}

void StateEventReceiver::DeviceInputEyeTrackingAdded(uint32_t deviceIndex, const std::string& path) {}

void StateEventReceiver::DeviceInputEyeTrackingRemoved(uint32_t deviceIndex, const std::string& path) {}

void StateEventReceiver::DevicePoseChanged(uint32_t deviceIndex, DevicePose oldPose, DevicePose newPose) {
    vr::IVRSystem* vrSystem = vr::VRSystem();
    if (!vrSystem || vrSystem->GetTrackedDeviceClass(deviceIndex) != vr::TrackedDeviceClass_Controller) return;

    newPose.vecPosition[0] = newPose.vecPosition[1] = newPose.vecPosition[2] = 0;
    this->commandSender.setOverriddenDevicePose(deviceIndex, newPose);

    auto& seenDeviceIndexes = this->seenDeviceIndexes;
    if (find(seenDeviceIndexes.begin(), seenDeviceIndexes.end(), deviceIndex) == seenDeviceIndexes.end()) {
        seenDeviceIndexes.push_back(deviceIndex);
        this->commandSender.setUseOverriddenDevicePose(deviceIndex, true);
    }
}

void StateEventReceiver::DeviceInputBooleanChanged(uint32_t deviceIndex, std::string path, BooleanInput oldInput, BooleanInput newInput) {}

void StateEventReceiver::DeviceInputScalarChanged(uint32_t deviceIndex, std::string path, ScalarInput oldInput, ScalarInput newInput) {}

void StateEventReceiver::DeviceInputSkeletonChanged(uint32_t deviceIndex, std::string path, SkeletonInput oldInput, SkeletonInput newInput) {}

void StateEventReceiver::DeviceInputPoseChanged(uint32_t deviceIndex, std::string path, PoseInput oldInput, PoseInput newInput) {}

void StateEventReceiver::DeviceInputEyeTrackingChanged(uint32_t deviceIndex, std::string path, EyeTrackingInput oldInput, EyeTrackingInput newInput) {}
