#pragma once
#include <cstdint>
#include <atomic>

#include "DeviceTypes.h"

inline const uint32_t NUM_OBJECT_TYPES = 6U;
inline const uint32_t ALIGNMENT_CONSTANT = 0x4F424A45U;

inline const char* SHM_NAME = "Local\\ConduitSharedDeviceMemory";
inline const uint32_t PATH_TABLE_SIZE = 1024U * 5U;
inline const uint32_t MAX_PATH_LENGTH = 256U;
inline const uint32_t LANE_SIZE = 1048576U * 5U;		// 5mb
inline const uint32_t LANE_PADDING_SIZE = 1024U * 5U;	// 5kb

inline const double POLL_RATE = 512.0;

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
	std::atomic<uint32_t> pathTableWritingOffset; // 0 if not writing, offset in path table being written otherwise

	uint32_t driverClientLaneStart;
	uint32_t driverClientLaneSize;
	std::atomic<uint64_t> driverClientWriteCount;	// Client keeps its own last consumed write count to track updates
	std::atomic<uint32_t> driverClientWriteOffset;
	std::atomic<uint32_t> driverClientReadOffset;

	uint32_t clientDriverLaneStart;
	uint32_t clientDriverLaneSize;
	std::atomic<uint64_t> clientDriverWriteCount;	// Driver keeps its own last consumed write count to track updates
	std::atomic<uint32_t> clientDriverWriteOffset;
	std::atomic<uint32_t> clientDriverReadOffset;
};

struct ObjectEntry {
	uint32_t alignmentCheck = ALIGNMENT_CONSTANT; // Used to check if the shared memory being read is properly aligned with an object entry

	ObjectType type;

	uint32_t deviceIndex;
	uint32_t inputPathOffset;

	uint64_t version;

	bool valid;

	std::atomic<bool> committed;
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

	std::atomic<bool> committed;
};

struct CommandParams_SetUseOverridenStateDevicePose {
	bool useOverridenState;
};

struct CommandParams_SetOverridenStateDevicePose {
	DevicePose overridenPose;
};

struct CommandParams_SetUseOverridenStateDeviceInput {
	bool useOverridenState;
	uint32_t inputPathOffset;
};

struct CommandParams_SetOverridenStateDeviceInputBoolean {
	BooleanInput overridenValue;
	uint32_t inputPathOffset;
};

struct CommandParams_SetOverridenStateDeviceInputScalar {
	ScalarInput overridenValue;
	uint32_t inputPathOffset;
};

struct CommandParams_SetOverridenStateDeviceInputSkeleton {
	SkeletonInput overridenValue;
	uint32_t inputPathOffset;
};

struct CommandParams_SetOverridenStateDeviceInputPose {
	PoseInput overridenValue;
	uint32_t inputPathOffset;
};

struct CommandParams_SetOverridenStateDeviceInputEyeTracking {
	EyeTrackingInput overridenValue;
	uint32_t inputPathOffset;
};

#pragma pack(pop)

struct ObjectEntryData {
	bool successful;
	bool valid;
	ObjectType type;
	uint32_t deviceIndex;
	uint64_t version;
	uint32_t inputPathOffset;
};

struct ClientCommandHeaderData {
	bool successful;
	ClientCommandType type;
	uint32_t deviceIndex;
	uint64_t version;
};

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
