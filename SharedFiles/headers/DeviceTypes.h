#pragma once

/// These struct declarations are taken directly from the OpenVR SDK, for proper documentation on each structs members, 
/// consult the OpenVR SDK documentation for the respective type

/// struct HmdVector3_t
struct DeviceVector3 {
	double v[3] = { 0.0, 0.0, 0.0 };
};

/// struct HmdVector4_t
struct DeviceVector4 {
	double v[4] = { 0.0, 0.0, 0.0, 0.0 };
};

/// struct HmdMatrix34_t
struct DeviceMatrix34 {
	double m[3][4];
};

/// struct HmdQuaternion_t
struct DeviceQuaternion {
	double w = 1.0;
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
};

/// enum ETrackingResult
enum DeviceTrackingResult {
	TrackingResult_Uninitialized = 1,

	TrackingResult_Calibrating_InProgress = 100,
	TrackingResult_Calibrating_OutOfRange = 101,

	TrackingResult_Running_OK = 200,
	TrackingResult_Running_OutOfRange = 201,

	TrackingResult_Fallback_RotationOnly = 300,
};

/// struct DriverPose_t
struct DevicePose {
	double poseTimeOffset = 0.0;

	DeviceQuaternion qWorldFromDriverRotation;
	double vecWorldFromDriverTranslation[3] = { 0.0, 0.0, 0.0 };

	DeviceQuaternion qDriverFromHeadRotation;
	double vecDriverFromHeadTranslation[3] = { 0.0, 0.0, 0.0 };

	double vecPosition[3] = { 0.0, 0.0, 0.0 };

	double vecVelocity[3] = { 0.0, 0.0, 0.0 };

	double vecAcceleration[3] = { 0.0, 0.0, 0.0 };

	DeviceQuaternion qRotation;

	double vecAngularVelocity[3] = { 0.0, 0.0, 0.0 };

	double vecAngularAcceleration[3] = { 0.0, 0.0, 0.0 };

	DeviceTrackingResult result = TrackingResult_Uninitialized;

	bool poseIsValid = false;
	bool willDriftInYaw = false;
	bool shouldApplyHeadModel = false;
	bool deviceIsConnected = false;
};

/// enum EVRSkeletalMotionRange
enum SkeletalMotionRange {
	VRSkeletalMotionRange_WithController = 0,
	VRSkeletalMotionRange_WithoutController = 1,
};

/// struct VRBoneTransform_t
struct BoneTransform {
	DeviceVector4 position;
	DeviceQuaternion orientation;
};

struct EyeTrackingData {
	bool active;
	bool valid;
	bool tracked;

	DeviceVector3 gazeOrigin;
	DeviceVector3 gazeTarget;
};

struct BooleanInput {
	bool value;
	double timeOffset;
};

struct ScalarInput {
	double value;
	double timeOffset;
};

struct SkeletonInput {
	SkeletalMotionRange motionRange;
	BoneTransform boneTransforms[31];
	uint32_t boneTransformCount;
};

struct PoseInput {
	DeviceMatrix34 poseOffset;
	double timeOffset;
};

struct EyeTrackingInput {
	EyeTrackingData eyeTrackingData;
	double timeOffset;
};