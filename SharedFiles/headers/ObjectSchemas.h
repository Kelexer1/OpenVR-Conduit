#pragma once
#include <cstdint>
#include <atomic>

#include "DeviceTypes.h"

const uint32_t NUM_OBJECT_TYPES = 6;
const uint32_t ALIGNMENT_CONSTANT = 0x4F424A45;

enum ObjectType {
	Object_DevicePose,
	Object_InputBoolean,
	Object_InputScalar,
	Object_InputSkeleton,
	Object_InputPose,
	Object_InputEyeTracking
};

enum ClientCommandType {
	Command_SetUseOverridenStateDevicePose,
	Command_SetOverridenStateDevicePose,
	Command_SetUseOverridenStateDeviceInput,
	Command_SetOverridenStateDeviceInputBoolean,
	Command_SetOverridenStateDeviceInputScalar,
	Command_SetOverridenStateDeviceInputSkeleton,
	Command_SetOverridenStateDeviceInputPose,
	Command_SetOverridenStateDeviceInputEyeTracking
};

#pragma pack(push, 1)

struct SharedMemoryHeader {
	uint32_t protocolVersion;

	uint32_t pathTableStart;
	uint32_t pathTableSize;
	uint32_t pathTableEntries;
	std::atomic<uint32_t> pathTableWriteCount;		// Client keeps its own last consumed write count to track updates
	std::atomic<uint32_t> pathTableWriteOffset;

	uint32_t driverClientLaneStart;
	uint32_t driverClientLaneSize;
	std::atomic<uint32_t> driverClientWriteCount;	// Client keeps its own last consumed write count to track updates
	std::atomic<uint32_t> driverClientWriteOffset;

	uint32_t clientDriverLaneStart;
	uint32_t clientDriverLaneSize;
	std::atomic<uint32_t> clientDriverWriteCount;	// Driver keeps its own last consumed write count to track updates
	std::atomic<uint32_t> clientDriverWriteOffset;
};

struct PathTableEntry {
	uint32_t alignmentCheck = ALIGNMENT_CONSTANT;

	uint32_t ID;	// Used to lookup the input path of inputs
	char path[128];
	uint64_t version;
};

struct ObjectEntry {
	uint32_t alignmentCheck = ALIGNMENT_CONSTANT; // Used to check if the shared memory being read is properly aligned with an object entry

	ObjectType type;

	uint32_t deviceIndex;
	uint32_t inputPathID;

	uint64_t version;
	bool valid;
};

struct DevicePoseSerialized {
	DevicePose pose;
	DevicePose overwrittenPose;
};

struct DeviceInputBooleanSerialized {
	BooleanInput value;
	BooleanInput overwrittenValue;
};

struct DeviceInputScalarSerialized {
	ScalarInput value;
	ScalarInput overwrittenValue;
};

struct DeviceInputSkeletonSerialized {
	SkeletonInput value;
	SkeletonInput overwrittenValue;
};

struct DeviceInputPoseSerialized {
	PoseInput value;
	PoseInput overwrittenValue;
};

struct DeviceInputEyeTrackingSerialized {
	EyeTrackingInput value;
	EyeTrackingInput overwrittenValue;
};

struct ClientCommandHeader {
	uint32_t alignmentCheck = ALIGNMENT_CONSTANT;

	ClientCommandType type;

	uint32_t deviceIndex;

	uint64_t version;
};

struct CommandParams_SetUseOverridenStateDevicePose {
	bool useOverridenState;
};

struct CommandParams_SetOverridenStateDevicePose {
	DevicePose overridenPose;
};

struct CommandParams_SetUseOverridenStateDeviceInput {
	bool useOverridenState;
	uint32_t pathID;
};

struct CommandParams_SetOverridenStateDeviceInputBoolean {
	BooleanInput overridenValue;
	uint32_t pathID;
};

struct CommandParams_SetOverridenStateDeviceInputScalar {
	ScalarInput overridenValue;
	uint32_t pathID;
};

struct CommandParams_SetOverridenStateDeviceInputSkeleton {
	SkeletonInput overridenValue;
	uint32_t pathID;
};

struct CommandParams_SetOverridenStateDeviceInputPose {
	PoseInput overridenValue;
	uint32_t pathID;
};

struct CommandParams_SetOverridenStateDeviceInputEyeTracking {
	EyeTrackingInput overridenValue;
	uint32_t pathID;
};

#pragma pack(pop)

struct ModelObjectState {
	bool useOverridenState;
};

struct ModelDevicePoseSerialized : public ModelObjectState {
	DevicePoseSerialized data;
};

struct ModelDeviceInputBooleanSerialized : public ModelObjectState {
	DeviceInputBooleanSerialized data;
};

struct ModelDeviceInputScalarSerialized : public ModelObjectState {
	DeviceInputScalarSerialized data;
};

struct ModelDeviceInputSkeletonSerialized : public ModelObjectState {
	DeviceInputSkeletonSerialized data;
};

struct ModelDeviceInputPoseSerialized : public ModelObjectState {
	DeviceInputPoseSerialized data;
};

struct ModelDeviceInputEyeTrackingSerialized : public ModelObjectState {
	DeviceInputEyeTrackingSerialized data;
};
