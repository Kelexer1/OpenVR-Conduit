#include "Utils.h"

vr::ETrackedDeviceClass getDeviceClass(uint32_t deviceIndex) {
	vr::PropertyContainerHandle_t container = vr::VRProperties()->TrackedDeviceToPropertyContainer(deviceIndex);
	vr::ETrackedPropertyError error;
	int32_t result = vr::VRProperties()->GetInt32Property(container, vr::Prop_DeviceClass_Int32, &error);
	if (error == vr::TrackedProp_Success) return static_cast<vr::ETrackedDeviceClass>(result);
	else return vr::TrackedDeviceClass_Invalid;
}

DevicePose FromDriverPose(const vr::DriverPose_t& pose) {
    DevicePose cp{};
    cp.poseTimeOffset = pose.poseTimeOffset;

    cp.qWorldFromDriverRotation = {
        pose.qWorldFromDriverRotation.w,
        pose.qWorldFromDriverRotation.x,
        pose.qWorldFromDriverRotation.y,
        pose.qWorldFromDriverRotation.z
    };
    std::copy(
        std::begin(pose.vecWorldFromDriverTranslation),
        std::end(pose.vecWorldFromDriverTranslation),
        cp.vecWorldFromDriverTranslation
    );

    cp.qDriverFromHeadRotation = {
        pose.qDriverFromHeadRotation.w,
        pose.qDriverFromHeadRotation.x,
        pose.qDriverFromHeadRotation.y,
        pose.qDriverFromHeadRotation.z
    };
    std::copy(
        std::begin(pose.vecDriverFromHeadTranslation),
        std::end(pose.vecDriverFromHeadTranslation),
        cp.vecDriverFromHeadTranslation
    );

    std::copy(std::begin(pose.vecPosition), std::end(pose.vecPosition), cp.vecPosition);
    std::copy(std::begin(pose.vecVelocity), std::end(pose.vecVelocity), cp.vecVelocity);
    std::copy(std::begin(pose.vecAcceleration), std::end(pose.vecAcceleration), cp.vecAcceleration);

    cp.qRotation = { pose.qRotation.w, pose.qRotation.x, pose.qRotation.y, pose.qRotation.z };

    std::copy(std::begin(pose.vecAngularVelocity), std::end(pose.vecAngularVelocity), cp.vecAngularVelocity);
    std::copy(
        std::begin(pose.vecAngularAcceleration),
        std::end(pose.vecAngularAcceleration),
        cp.vecAngularAcceleration
    );

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

    pose.qWorldFromDriverRotation = { 
        cp.qWorldFromDriverRotation.w,
        cp.qWorldFromDriverRotation.x,
        cp.qWorldFromDriverRotation.y,
        cp.qWorldFromDriverRotation.z
    };
    std::copy(
        std::begin(cp.vecWorldFromDriverTranslation),
        std::end(cp.vecWorldFromDriverTranslation),
        pose.vecWorldFromDriverTranslation
    );

    pose.qDriverFromHeadRotation = {
        cp.qDriverFromHeadRotation.w,
        cp.qDriverFromHeadRotation.x,
        cp.qDriverFromHeadRotation.y,
        cp.qDriverFromHeadRotation.z
    };
    std::copy(
        std::begin(cp.vecDriverFromHeadTranslation),
        std::end(cp.vecDriverFromHeadTranslation),
        pose.vecDriverFromHeadTranslation
    );

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

vr::HmdMatrix34_t ToHmdMatrix34(const DeviceMatrix34& mat) {
    vr::HmdMatrix34_t result = {};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = static_cast<float>(mat.m[i][j]);
        }
    }
    return result;
}

DeviceMatrix34 FromHmdMatrix34(const vr::HmdMatrix34_t& mat) {
    DeviceMatrix34 result = {};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = static_cast<double>(mat.m[i][j]);
        }
    }
    return result;
}

void ToVRBoneTransforms(const SkeletonInput& input, vr::VRBoneTransform_t* outTransforms) {
    for (uint32_t i = 0; i < input.boneTransformCount; i++) {
        const auto& bt = input.boneTransforms[i];
        outTransforms[i].position = {
            (float)bt.position.v[0],
            (float)bt.position.v[1],
            (float)bt.position.v[2],
            (float)bt.position.v[3]
        };
        outTransforms[i].orientation = {
            (float)bt.orientation.w,
            (float)bt.orientation.x,
            (float)bt.orientation.y,
            (float)bt.orientation.z
        };
    }
}

void FromVRBoneTransforms(const vr::VRBoneTransform_t* transforms, uint32_t count, SkeletonInput& outInput) {
    outInput.boneTransformCount = count;
    for (uint32_t i = 0; i < count; i++) {
        const auto& bt = transforms[i];
        outInput.boneTransforms[i].position = {
            bt.position.v[0],
            bt.position.v[1],
            bt.position.v[2],
            bt.position.v[3]
        };
        outInput.boneTransforms[i].orientation = {
            bt.orientation.w,
            bt.orientation.x,
            bt.orientation.y,
            bt.orientation.z
        };
    }
}

vr::VREyeTrackingData_t ToVREyeTrackingData(const EyeTrackingData& data) {
    vr::VREyeTrackingData_t result = {};
    result.bActive = data.active;
    result.bValid = data.valid;
    result.bTracked = data.tracked;
    result.vGazeOrigin = { (float)data.gazeOrigin.v[0], (float)data.gazeOrigin.v[1], (float)data.gazeOrigin.v[2] };
    result.vGazeTarget = { (float)data.gazeTarget.v[0], (float)data.gazeTarget.v[1], (float)data.gazeTarget.v[2] };
    return result;
}

EyeTrackingData FromVREyeTrackingData(const vr::VREyeTrackingData_t& data) {
    EyeTrackingData result = {};
    result.active = data.bActive;
    result.valid = data.bValid;
    result.tracked = data.bTracked;
    result.gazeOrigin = { data.vGazeOrigin.v[0], data.vGazeOrigin.v[1], data.vGazeOrigin.v[2] };
    result.gazeTarget = { data.vGazeTarget.v[0], data.vGazeTarget.v[1], data.vGazeTarget.v[2] };
    return result;
}