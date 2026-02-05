#pragma once
#include <stdint.h>

/**
 * @brief Many of these struct declarations are taken directly from the OpenVR SDK. For proper documentation on each
 * structs members, consult the OpenVR SDK documentation
 */

/**
 * Based on struct HmdVector3_t from the OpenVR SDK, documentation available there
 */ 
struct DeviceVector3 {
	double v[3] = { 0.0, 0.0, 0.0 };
};

/**
 * Based on struct HmdVector4_t from the OpenVR SDK, documentation available there
 */ 
struct DeviceVector4 {
	double v[4] = { 0.0, 0.0, 0.0, 0.0 };
};

/**
 * Based on struct HmdMatrix34_t from the OpenVR SDK, documentation available there
 */
struct DeviceMatrix34 {
	double m[3][4];
};

/**
 * Based on struct HmdQuaternion_t from the OpenVR SDK, documentation available there
 */
struct DeviceQuaternion {
	double w = 1.0;
	double x = 0.0;
	double y = 0.0;
	double z = 0.0;
};

/**
 * Based on enum ETrackingResult from the OpenVR SDK, documentation available there
 */
enum DeviceTrackingResult {
	TrackingResult_Uninitialized = 1,

	TrackingResult_Calibrating_InProgress = 100,
	TrackingResult_Calibrating_OutOfRange = 101,

	TrackingResult_Running_OK = 200,
	TrackingResult_Running_OutOfRange = 201,

	TrackingResult_Fallback_RotationOnly = 300,
};

/**
 * Based on struct DriverPose_t from the OpenVR SDK, documentation available there
 */
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

/**
 * Based on enum EVRSkeletalMotionRange from the OpenVR SDK, documentation available there
 */ 
enum SkeletalMotionRange {
	VRSkeletalMotionRange_WithController = 0,
	VRSkeletalMotionRange_WithoutController = 1,
};

/**
 * Based on struct VRBoneTransform_t from the OpenVR SDK, documentation available there
 */ 
struct BoneTransform {
	DeviceVector4 position;
	DeviceQuaternion orientation;
};

/**
 * Based on struct VREyeTrackingData_t from the OpenVR SDK, documentation available there
 */ 
struct EyeTrackingData {
	bool active;
	bool valid;
	bool tracked;

	DeviceVector3 gazeOrigin;
	DeviceVector3 gazeTarget;
};

/**
 * @brief Represents a single boolean input, such as a button
 */
struct BooleanInput {
	bool value;
	double timeOffset;
};

/**
 * @brief Represents a single scalar input, such as the value of a trigger from 0-1, inclusive
 */
struct ScalarInput {
	float value;
	double timeOffset;
};

/**
 * @brief Represents a single skeleton input, such as the representation of a hand
 * through hand tracking on a supported controller
 */
struct SkeletonInput {
	SkeletalMotionRange motionRange;
	BoneTransform boneTransforms[31];
	uint32_t boneTransformCount;
};

/**
 * @brief Represents a single pose input, or transformation of device pose
 */
struct PoseInput {
	DeviceMatrix34 poseOffset;
	double timeOffset;
};

/**
 * @brief Represents a single eye tracking input
 */
struct EyeTrackingInput {
	EyeTrackingData eyeTrackingData;
	double timeOffset;
};