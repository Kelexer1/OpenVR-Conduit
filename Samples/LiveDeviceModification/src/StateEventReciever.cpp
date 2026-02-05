#include "StateEventReciever.h"

StateEventReciever::StateEventReciever(DeviceStateCommandSender& commandSender) : commandSender(commandSender) {}

void StateEventReciever::DeviceInputBooleanAdded(uint32_t deviceIndex, const std::string& path) {}

void StateEventReciever::DeviceInputBooleanRemoved(uint32_t deviceIndex, const std::string& path) {}

void StateEventReciever::DeviceInputScalarAdded(uint32_t deviceIndex, const std::string& path) {}

void StateEventReciever::DeviceInputScalarRemoved(uint32_t deviceIndex, const std::string& path) {}

void StateEventReciever::DeviceInputSkeletonAdded(uint32_t deviceIndex, const std::string& path) {}

void StateEventReciever::DeviceInputSkeletonRemoved(uint32_t deviceIndex, const std::string& path) {}

void StateEventReciever::DeviceInputPoseAdded(uint32_t deviceIndex, const std::string& path) {}

void StateEventReciever::DeviceInputPoseRemoved(uint32_t deviceIndex, const std::string& path) {}

void StateEventReciever::DeviceInputEyeTrackingAdded(uint32_t deviceIndex, const std::string& path) {}

void StateEventReciever::DeviceInputEyeTrackingRemoved(uint32_t deviceIndex, const std::string& path) {}

void StateEventReciever::DevicePoseChanged(uint32_t deviceIndex, DevicePose oldPose, DevicePose newPose) {
    vr::IVRSystem* vrSystem = vr::VRSystem();
    if (!vrSystem || vrSystem->GetTrackedDeviceClass(deviceIndex) != vr::TrackedDeviceClass_Controller) return;

    newPose.vecPosition[0] = newPose.vecPosition[1] = newPose.vecPosition[2] = 0;
    this->commandSender.setOverridenDevicePose(deviceIndex, newPose);

    auto& seenDeviceIndexes = this->seenDeviceIndexes;
    if (find(seenDeviceIndexes.begin(), seenDeviceIndexes.end(), deviceIndex) == seenDeviceIndexes.end()) {
        seenDeviceIndexes.push_back(deviceIndex);
        this->commandSender.setUseOverridenDevicePose(deviceIndex, true);
    }
}

void StateEventReciever::DeviceInputBooleanChanged(uint32_t deviceIndex, std::string path, BooleanInput oldInput, BooleanInput newInput) {}

void StateEventReciever::DeviceInputScalarChanged(uint32_t deviceIndex, std::string path, ScalarInput oldInput, ScalarInput newInput) {}

void StateEventReciever::DeviceInputSkeletonChanged(uint32_t deviceIndex, std::string path, SkeletonInput oldInput, SkeletonInput newInput) {}

void StateEventReciever::DeviceInputPoseChanged(uint32_t deviceIndex, std::string path, PoseInput oldInput, PoseInput newInput) {}

void StateEventReciever::DeviceInputEyeTrackingChanged(uint32_t deviceIndex, std::string path, EyeTrackingInput oldInput, EyeTrackingInput newInput) {}
