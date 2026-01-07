#include "ObjectTypeSchemaBaker.h"

void ObjectTypeSchemaBaker::bakeSchemas() {
	ObjectSchemaEntry* devicePose = (ObjectSchemaEntry*)malloc(entrySize(2));
	devicePose->typeID = Object_DevicePose;
	devicePose->size = sizeof(DevicePoseSerialized);
	devicePose->fields[0] = { Field_DevicePose, 0, offsetof(DevicePoseSerialized, pose), sizeof(DevicePose) };
	devicePose->fields[1] = { Field_DevicePose, 1, offsetof(DevicePoseSerialized, overwrittenPose), sizeof(DevicePose) };
	ObjectTypeSchemaBaker::schemas.push_back(*devicePose);
	ObjectTypeSchemaBaker::schemaSizes.push_back(entrySize(2));

	ObjectSchemaEntry* inputBoolean = (ObjectSchemaEntry*)malloc(entrySize(4));
	inputBoolean->typeID = Object_InputBoolean;
	inputBoolean->size = sizeof(InputBooleanSerialized);
	inputBoolean->fields[0] = { Field_Bool, 0, offsetof(InputBooleanSerialized, value), sizeof(bool) };
	inputBoolean->fields[1] = { Field_Bool, 1, offsetof(InputBooleanSerialized, overwrittenValue), sizeof(bool) };
	inputBoolean->fields[2] = { Field_Double, 2, offsetof(InputBooleanSerialized, timeOffset), sizeof(double) };
	inputBoolean->fields[3] = { Field_Double, 3, offsetof(InputBooleanSerialized, overwrittenTimeOffset), sizeof(double) };
	ObjectTypeSchemaBaker::schemas.push_back(*inputBoolean);
	ObjectTypeSchemaBaker::schemaSizes.push_back(entrySize(4));

	ObjectSchemaEntry* inputScalar = (ObjectSchemaEntry*)malloc(entrySize(4));
	inputScalar->typeID = Object_InputScalar;
	inputScalar->size = sizeof(InputScalarSerialized);
	inputScalar->fields[0] = { Field_Double, 0, offsetof(InputScalarSerialized, value), sizeof(double) };
	inputScalar->fields[1] = { Field_Double, 1, offsetof(InputScalarSerialized, overwrittenValue), sizeof(double) };
	inputScalar->fields[2] = { Field_Double, 2, offsetof(InputScalarSerialized, timeOffset), sizeof(double) };
	inputScalar->fields[3] = { Field_Double, 3, offsetof(InputScalarSerialized, overwrittenTimeOffset), sizeof(double) };
	ObjectTypeSchemaBaker::schemas.push_back(*inputScalar);
	ObjectTypeSchemaBaker::schemaSizes.push_back(entrySize(4));

	ObjectSchemaEntry* inputSkeleton = (ObjectSchemaEntry*)malloc(entrySize(6));
	inputSkeleton->typeID = Object_InputSkeleton;
	inputSkeleton->size = sizeof(InputSkeletonSerialized);
	inputSkeleton->fields[0] = { Field_SkeletalMotionRange, 0, offsetof(InputSkeletonSerialized, motionRange), sizeof(SkeletalMotionRange) };
	inputSkeleton->fields[1] = { Field_BoneTransformArray, 1, offsetof(InputSkeletonSerialized, bones), sizeof(BoneTransform) * 31 };
	inputSkeleton->fields[2] = { Field_SkeletalMotionRange, 2, offsetof(InputSkeletonSerialized, overwrittenMotionRange), sizeof(SkeletalMotionRange) };
	inputSkeleton->fields[3] = { Field_BoneTransformArray, 3, offsetof(InputSkeletonSerialized, overwrittenBones), sizeof(BoneTransform) * 31 };
	inputSkeleton->fields[4] = { Field_Double, 4, offsetof(InputSkeletonSerialized, timeOffset), sizeof(double) };
	inputSkeleton->fields[5] = { Field_Double, 5, offsetof(InputSkeletonSerialized, overwrittenTimeOffset), sizeof(double) };
	ObjectTypeSchemaBaker::schemas.push_back(*inputSkeleton);
	ObjectTypeSchemaBaker::schemaSizes.push_back(entrySize(6));

	ObjectSchemaEntry* inputPose = (ObjectSchemaEntry*)malloc(entrySize(4));
	inputPose->typeID = Object_InputPose;
	inputPose->size = sizeof(InputPoseSerialized);
	inputPose->fields[0] = { Field_DeviceMatrix34, 0, offsetof(InputPoseSerialized, poseOffset), sizeof(DeviceMatrix34) };
	inputPose->fields[1] = { Field_DeviceMatrix34, 1, offsetof(InputPoseSerialized, overwrittenPoseOffset), sizeof(DeviceMatrix34) };
	inputPose->fields[2] = { Field_Double, 2, offsetof(InputPoseSerialized, timeOffset), sizeof(double) };
	inputPose->fields[3] = { Field_Double, 3, offsetof(InputPoseSerialized, overwrittenTimeOffset), sizeof(double) };
	ObjectTypeSchemaBaker::schemas.push_back(*inputPose);
	ObjectTypeSchemaBaker::schemaSizes.push_back(entrySize(4));

	ObjectSchemaEntry* inputEyeTracking = (ObjectSchemaEntry*)malloc(entrySize(4));
	inputEyeTracking->typeID = Object_InputEyeTracking;
	inputEyeTracking->size = sizeof(InputEyeTrackingSerialized);
	inputEyeTracking->fields[0] = { Field_EyeTrackingData, 0, offsetof(InputEyeTrackingSerialized, eyeTrackingData), sizeof(EyeTrackingData) };
	inputEyeTracking->fields[1] = { Field_EyeTrackingData, 1, offsetof(InputEyeTrackingSerialized, overwrittenEyeTrackingData), sizeof(EyeTrackingData) };
	inputEyeTracking->fields[2] = { Field_Double, 2, offsetof(InputEyeTrackingSerialized, timeOffset), sizeof(double) };
	inputEyeTracking->fields[3] = { Field_Double, 3, offsetof(InputEyeTrackingSerialized, overwrittenTimeOffset), sizeof(double) };
	ObjectTypeSchemaBaker::schemas.push_back(*inputEyeTracking);
	ObjectTypeSchemaBaker::schemaSizes.push_back(entrySize(4));
}

std::vector<ObjectSchemaEntry> ObjectTypeSchemaBaker::getSchemas() { return ObjectTypeSchemaBaker::schemas; }

std::vector<int> ObjectTypeSchemaBaker::getSchemaSizes() { return ObjectTypeSchemaBaker::schemaSizes; }

int entrySize(int numFields) {
	return sizeof(ObjectSchemaEntry) + numFields * sizeof(FieldSchemaEntry);
}