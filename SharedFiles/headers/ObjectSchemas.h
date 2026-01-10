#pragma once
#include <cstdint>
#include <atomic>

#include "DeviceTypes.h"

const uint32_t NUM_OBJECT_TYPES = 6;

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
	uint32_t pathTableEntrySize;
	std::atomic<uint32_t> pathTableWriteCount;		// Client keeps its own last consumed write count to track updates

	uint32_t driverClientLaneStart;
	uint32_t driverClientLaneSize;
	std::atomic<uint32_t> driverClientWriteCount;	// Client keeps its own last consumed write count to track updates

	uint32_t clientDriverLaneStart;
	uint32_t clientDriverLaneSize;
	std::atomic<uint32_t> clientDriverWriteCount;	// Driver keeps its own last consumed write count to track updates
};

struct PathTableEntry {
	uint32_t ID;	// Used to lookup the input path of inputs
	char path[128];
	uint64_t version;
};

struct ObjectEntry {
	ObjectType type;

	uint32_t deviceIndex;
	uint32_t inputPathID;

	uint64_t version;
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
	ClientCommandType commandType;

	uint32_t deviceIndex;

	uint64_t version;
};

struct CommandParams_SetUseOverridenStateDevicePose {
	bool useOverridenState;
};

struct CommandParams_SetOverridenStateDevicePose {
	DevicePoseSerialized overridenPose;
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