#include "ObjectTypeSchemaBaker.h"

void ObjectTypeSchemaBaker::bakeSchemas() {
	ObjectSchemaEntry* devicePose = (ObjectSchemaEntry*)malloc(entrySize(2));
	devicePose->typeID = Object_DevicePose;
	devicePose->size = sizeof(DevicePoseSerialized);
	devicePose->fieldCount = 2;
	devicePose->fields[0] = { Field_DevicePose, 0, offsetof(DevicePoseSerialized, pose), sizeof(DevicePose) };
	devicePose->fields[1] = { Field_DevicePose, 1, offsetof(DevicePoseSerialized, overwrittenPose), sizeof(DevicePose) };
	ObjectTypeSchemaBaker::schemas.push_back(*devicePose);
	ObjectTypeSchemaBaker::schemaSizes.push_back(entrySize(2));

	ObjectSchemaEntry* inputBoolean = (ObjectSchemaEntry*)malloc(entrySize(4));
	inputBoolean->typeID = Object_InputBoolean;
	inputBoolean->size = sizeof(DeviceInputBooleanSerialized);
	inputBoolean->fieldCount = 4;
	inputBoolean->fields[0] = { Field_Bool, 0, offsetof(DeviceInputBooleanSerialized, value), sizeof(bool) };
	inputBoolean->fields[1] = { Field_Bool, 1, offsetof(DeviceInputBooleanSerialized, overwrittenValue), sizeof(bool) };
	inputBoolean->fields[2] = { Field_Double, 2, offsetof(DeviceInputBooleanSerialized, timeOffset), sizeof(double) };
	inputBoolean->fields[3] = { Field_Double, 3, offsetof(DeviceInputBooleanSerialized, overwrittenTimeOffset), sizeof(double) };
	ObjectTypeSchemaBaker::schemas.push_back(*inputBoolean);
	ObjectTypeSchemaBaker::schemaSizes.push_back(entrySize(4));

	ObjectSchemaEntry* inputScalar = (ObjectSchemaEntry*)malloc(entrySize(4));
	inputScalar->typeID = Object_InputScalar;
	inputScalar->size = sizeof(DeviceInputScalarSerialized);
	inputScalar->fieldCount = 4;
	inputScalar->fields[0] = { Field_Double, 0, offsetof(DeviceInputScalarSerialized, value), sizeof(double) };
	inputScalar->fields[1] = { Field_Double, 1, offsetof(DeviceInputScalarSerialized, overwrittenValue), sizeof(double) };
	inputScalar->fields[2] = { Field_Double, 2, offsetof(DeviceInputScalarSerialized, timeOffset), sizeof(double) };
	inputScalar->fields[3] = { Field_Double, 3, offsetof(DeviceInputScalarSerialized, overwrittenTimeOffset), sizeof(double) };
	ObjectTypeSchemaBaker::schemas.push_back(*inputScalar);
	ObjectTypeSchemaBaker::schemaSizes.push_back(entrySize(4));

	ObjectSchemaEntry* inputSkeleton = (ObjectSchemaEntry*)malloc(entrySize(6));
	inputSkeleton->typeID = Object_InputSkeleton;
	inputSkeleton->size = sizeof(DeviceInputSkeletonSerialized);
	inputSkeleton->fieldCount = 6;
	inputSkeleton->fields[0] = { Field_SkeletalMotionRange, 0, offsetof(DeviceInputSkeletonSerialized, motionRange), sizeof(SkeletalMotionRange) };
	inputSkeleton->fields[1] = { Field_BoneTransformArray, 1, offsetof(DeviceInputSkeletonSerialized, bones), sizeof(BoneTransform) * 31 };
	inputSkeleton->fields[2] = { Field_SkeletalMotionRange, 2, offsetof(DeviceInputSkeletonSerialized, overwrittenMotionRange), sizeof(SkeletalMotionRange) };
	inputSkeleton->fields[3] = { Field_BoneTransformArray, 3, offsetof(DeviceInputSkeletonSerialized, overwrittenBones), sizeof(BoneTransform) * 31 };
	inputSkeleton->fields[4] = { Field_Double, 4, offsetof(DeviceInputSkeletonSerialized, timeOffset), sizeof(double) };
	inputSkeleton->fields[5] = { Field_Double, 5, offsetof(DeviceInputSkeletonSerialized, overwrittenTimeOffset), sizeof(double) };
	ObjectTypeSchemaBaker::schemas.push_back(*inputSkeleton);
	ObjectTypeSchemaBaker::schemaSizes.push_back(entrySize(6));

	ObjectSchemaEntry* inputPose = (ObjectSchemaEntry*)malloc(entrySize(4));
	inputPose->typeID = Object_InputPose;
	inputPose->size = sizeof(DeviceInputPoseSerialized);
	inputPose->fieldCount = 4;
	inputPose->fields[0] = { Field_DeviceMatrix34, 0, offsetof(DeviceInputPoseSerialized, poseOffset), sizeof(DeviceMatrix34) };
	inputPose->fields[1] = { Field_DeviceMatrix34, 1, offsetof(DeviceInputPoseSerialized, overwrittenPoseOffset), sizeof(DeviceMatrix34) };
	inputPose->fields[2] = { Field_Double, 2, offsetof(DeviceInputPoseSerialized, timeOffset), sizeof(double) };
	inputPose->fields[3] = { Field_Double, 3, offsetof(DeviceInputPoseSerialized, overwrittenTimeOffset), sizeof(double) };
	ObjectTypeSchemaBaker::schemas.push_back(*inputPose);
	ObjectTypeSchemaBaker::schemaSizes.push_back(entrySize(4));

	ObjectSchemaEntry* inputEyeTracking = (ObjectSchemaEntry*)malloc(entrySize(4));
	inputEyeTracking->typeID = Object_InputEyeTracking;
	inputEyeTracking->size = sizeof(DeviceInputEyeTrackingSerialized);
	inputEyeTracking->fieldCount = 4;
	inputEyeTracking->fields[0] = { Field_EyeTrackingData, 0, offsetof(DeviceInputEyeTrackingSerialized, eyeTrackingData), sizeof(EyeTrackingData) };
	inputEyeTracking->fields[1] = { Field_EyeTrackingData, 1, offsetof(DeviceInputEyeTrackingSerialized, overwrittenEyeTrackingData), sizeof(EyeTrackingData) };
	inputEyeTracking->fields[2] = { Field_Double, 2, offsetof(DeviceInputEyeTrackingSerialized, timeOffset), sizeof(double) };
	inputEyeTracking->fields[3] = { Field_Double, 3, offsetof(DeviceInputEyeTrackingSerialized, overwrittenTimeOffset), sizeof(double) };
	ObjectTypeSchemaBaker::schemas.push_back(*inputEyeTracking);
	ObjectTypeSchemaBaker::schemaSizes.push_back(entrySize(4));
}

std::vector<ObjectSchemaEntry> ObjectTypeSchemaBaker::getSchemas() { return ObjectTypeSchemaBaker::schemas; }

std::vector<int> ObjectTypeSchemaBaker::getSchemaSizes() { return ObjectTypeSchemaBaker::schemaSizes; }

size_t ObjectTypeSchemaBaker::getTotalSchemaSize() {
	size_t result = 0;

	for (size_t size : ObjectTypeSchemaBaker::schemaSizes) {
		result += size;
	}

	return result;
}

int ObjectTypeSchemaBaker::entrySize(int numFields) {
	return sizeof(ObjectSchemaEntry) + numFields * sizeof(FieldSchemaEntry);
}