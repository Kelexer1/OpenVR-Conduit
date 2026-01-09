#pragma once
#include <cstdint>

#include "DeviceTypes.h"

const uint32_t NUM_OBJECT_TYPES = 6;

enum ObjectType : uint8_t {
	Object_DevicePose,
	Object_InputBoolean,
	Object_InputScalar,
	Object_InputSkeleton,
	Object_InputPose,
	Object_InputEyeTracking
};

enum FieldType : uint8_t {
	Field_DevicePose,
	Field_Bool,
	Field_Double,
	Field_SkeletalMotionRange,
	Field_BoneTransformArray,
	Field_DeviceMatrix34,
	Field_EyeTrackingData
};

#pragma pack(push, 1)

struct ObjectSchemaEntry {
	ObjectType typeID;
	uint32_t size;
	uint8_t fieldCount;
	FieldSchemaEntry fields[];
};

struct FieldSchemaEntry {
	FieldType type;

	uint32_t fieldID;
	uint32_t offset;
	uint32_t size;
};

struct DevicePoseSerialized {
	DevicePose pose;
	DevicePose overwrittenPose;
};

struct DeviceInputBooleanSerialized {
	bool value;
	bool overwrittenValue;
	double timeOffset;
	double overwrittenTimeOffset;
};

struct DeviceInputScalarSerialized {
	double value;
	double overwrittenValue;
	double timeOffset;
	double overwrittenTimeOffset;
};

struct DeviceInputSkeletonSerialized {
	SkeletalMotionRange motionRange;
	BoneTransform bones[31];
	SkeletalMotionRange overwrittenMotionRange;
	BoneTransform overwrittenBones[31];
	double timeOffset;
	double overwrittenTimeOffset;
};

struct DeviceInputPoseSerialized {
	DeviceMatrix34 poseOffset;
	DeviceMatrix34 overwrittenPoseOffset;
	double timeOffset;
	double overwrittenTimeOffset;
};

struct DeviceInputEyeTrackingSerialized {
	EyeTrackingData eyeTrackingData;
	EyeTrackingData overwrittenEyeTrackingData;
	double timeOffset;
	double overwrittenTimeOffset;
};

#pragma pack(pop)