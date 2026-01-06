#include "Utils.h"

vr::ETrackedDeviceClass getDeviceClass(uint32_t deviceIndex) {
	vr::PropertyContainerHandle_t container = vr::VRProperties()->TrackedDeviceToPropertyContainer(deviceIndex);
	vr::ETrackedPropertyError error;
	int32_t result = vr::VRProperties()->GetInt32Property(container, vr::Prop_DeviceClass_Int32, &error);
	if (error == vr::TrackedProp_Success) {
		return static_cast<vr::ETrackedDeviceClass>(result);
	}
	else {
		return vr::TrackedDeviceClass_Invalid;
	}
}

DevicePose FromDriverPose(const vr::DriverPose_t& pose) {
    DevicePose cp{};
    cp.poseTimeOffset = pose.poseTimeOffset;

    cp.qWorldFromDriverRotation = { pose.qWorldFromDriverRotation.w, pose.qWorldFromDriverRotation.x, pose.qWorldFromDriverRotation.y, pose.qWorldFromDriverRotation.z };
    std::copy(std::begin(pose.vecWorldFromDriverTranslation), std::end(pose.vecWorldFromDriverTranslation), cp.vecWorldFromDriverTranslation);

    cp.qDriverFromHeadRotation = { pose.qDriverFromHeadRotation.w, pose.qDriverFromHeadRotation.x, pose.qDriverFromHeadRotation.y, pose.qDriverFromHeadRotation.z };
    std::copy(std::begin(pose.vecDriverFromHeadTranslation), std::end(pose.vecDriverFromHeadTranslation), cp.vecDriverFromHeadTranslation);

    std::copy(std::begin(pose.vecPosition), std::end(pose.vecPosition), cp.vecPosition);
    std::copy(std::begin(pose.vecVelocity), std::end(pose.vecVelocity), cp.vecVelocity);
    std::copy(std::begin(pose.vecAcceleration), std::end(pose.vecAcceleration), cp.vecAcceleration);

    cp.qRotation = { pose.qRotation.w, pose.qRotation.x, pose.qRotation.y, pose.qRotation.z };

    std::copy(std::begin(pose.vecAngularVelocity), std::end(pose.vecAngularVelocity), cp.vecAngularVelocity);
    std::copy(std::begin(pose.vecAngularAcceleration), std::end(pose.vecAngularAcceleration), cp.vecAngularAcceleration);

    cp.result = static_cast<DeviceTrackingResult>(pose.result);

    cp.poseIsValid = pose.poseIsValid;
    cp.willDriftInYaw = pose.willDriftInYaw;
    cp.shouldApplyHeadModel = pose.shouldApplyHeadModel;
    cp.deviceIsConnected = pose.deviceIsConnected;

    return cp;
}

vr::DriverPose_t ToDriverPose(const DevicePose& cp) {
    vr::DriverPose_t pose = {};
    pose.poseTimeOffset = cp.poseTimeOffset;

    pose.qWorldFromDriverRotation = { cp.qWorldFromDriverRotation.w, cp.qWorldFromDriverRotation.x,
                                     cp.qWorldFromDriverRotation.y, cp.qWorldFromDriverRotation.z };
    std::copy(std::begin(cp.vecWorldFromDriverTranslation), std::end(cp.vecWorldFromDriverTranslation), pose.vecWorldFromDriverTranslation);

    pose.qDriverFromHeadRotation = { cp.qDriverFromHeadRotation.w, cp.qDriverFromHeadRotation.x,
                                    cp.qDriverFromHeadRotation.y, cp.qDriverFromHeadRotation.z };
    std::copy(std::begin(cp.vecDriverFromHeadTranslation), std::end(cp.vecDriverFromHeadTranslation), pose.vecDriverFromHeadTranslation);

    std::copy(std::begin(cp.vecPosition), std::end(cp.vecPosition), pose.vecPosition);
    std::copy(std::begin(cp.vecVelocity), std::end(cp.vecVelocity), pose.vecVelocity);
    std::copy(std::begin(cp.vecAcceleration), std::end(cp.vecAcceleration), pose.vecAcceleration);

    pose.qRotation = { cp.qRotation.w, cp.qRotation.x, cp.qRotation.y, cp.qRotation.z };

    std::copy(std::begin(cp.vecAngularVelocity), std::end(cp.vecAngularVelocity), pose.vecAngularVelocity);
    std::copy(std::begin(cp.vecAngularAcceleration), std::end(cp.vecAngularAcceleration), pose.vecAngularAcceleration);

    pose.result = static_cast<vr::ETrackingResult>(cp.result);

    pose.poseIsValid = cp.poseIsValid;
    pose.willDriftInYaw = cp.willDriftInYaw;
    pose.shouldApplyHeadModel = cp.shouldApplyHeadModel;
    pose.deviceIsConnected = cp.deviceIsConnected;

    return pose;
}
