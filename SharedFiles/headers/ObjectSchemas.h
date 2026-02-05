#pragma once
#include <cstdint>
#include <atomic>

#include "DeviceTypes.h"

/* The size of the ObjectType enum */
inline const uint32_t NUM_OBJECT_TYPES = 6U;

/* A constant used interally to ensure packets are aligned in shared memory */
inline const uint32_t ALIGNMENT_CONSTANT = 0x4F424A45U;

/* The name of the Conduit shared memory region, as required by Windows */
inline const char* SHM_NAME = "Local\\ConduitSharedDeviceMemory";

/* The total allocated size of the path table, in bytes */
inline const uint32_t PATH_TABLE_SIZE = 1024U * 5U;		// 5kb

/* The maximum length a path can be before it is deemed to be not-null-terminated, 
and thus invalid */
inline const uint32_t MAX_PATH_LENGTH = 256U;

/* The size in bytes of a single lane, which extends to the driver-client and
client-driver lanes */
inline const uint32_t LANE_SIZE = 1048576U * 5U;		// 5mb

/* The allocated padding at the end of each lane that signals a read header to
reset to offset 0. Should be greater than the maximum possible packet size */
inline const uint32_t LANE_PADDING_SIZE = 1024U * 5U;	// 5kb

/* The rate in Hz that the Conduit driver and lib poll for updates from eachother */
inline const double POLL_RATE = 512.0;

/**
 * @brief Represents the type of input (or pose) of a packet
 */
enum ObjectType {
	Object_DevicePose,
	Object_InputBoolean,
	Object_InputScalar,
	Object_InputSkeleton,
	Object_InputPose,
	Object_InputEyeTracking
};

/**
 * @brief Represents the type of client-driver command of a packet
 */
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

/**
 * @brief Represents the central header in shared memory, containing critical metadata needed by both the Conduit
 * lib and Driver, often simultaneously
 */
struct SharedMemoryHeader {
	/**************************************************
	* @brief General metadata
	**************************************************/

	/**
	 * @brief The current version of the protocol used to ensure agreement on reading and writing rules between the
	 * driver and lib
	 */
	uint32_t protocolVersion;


	/**************************************************
	* @brief Path table metadata
	**************************************************/

	/** @brief The offset in bytes from the start of the shared memory to the start of the path table */
	uint32_t pathTableStart;
	
	/** @brief The total size in bytes allocated for the path table */
	uint32_t pathTableSize;

	/**
	 * @brief An offset in bytes (from <pathTableStart>) for where the driver is currently writing a path, or 0 if the
	 * driver is not currently writing
	 */
	std::atomic<uint32_t> pathTableWritingOffset;


	/**************************************************
	* @brief Driver-Client lane metadata
	**************************************************/

	/** @brief The offset in bytes from the start of the shared memory to the start of the driver-client lane */
	uint32_t driverClientLaneStart;

	/** @brief The total size in bytes allocated for the driver-client lane */
	uint32_t driverClientLaneSize;

	/** 
	 * @brief The total number of writes made by the driver to the driver-client lane, which is compared to a local
	 * read count by the lib to decide when new packets can be read 
	 */
	std::atomic<uint64_t> driverClientWriteCount;

	/** @brief The current offset in bytes that the driver is writing to the driver-client lane at */
	std::atomic<uint32_t> driverClientWriteOffset;

	/** @brief The current offset in bytes that the lib has read at from the driver-client lane */
	std::atomic<uint32_t> driverClientReadOffset;


	/**************************************************
	* @brief Client-Driver lane metadata
	**************************************************/

	/** @brief The offset in bytes from the start of the shared memory to the start of the client-driver lane */
	uint32_t clientDriverLaneStart;

	/** @brief The total size in bytes allocated for the client-driver lane */
	uint32_t clientDriverLaneSize;

	/**
	 * @brief The total number of writes made by the lib to the client-driver lane, which is compared to a local
	 * read count by the driver to decide when new packets can be read 
	 */
	std::atomic<uint64_t> clientDriverWriteCount;

	/** @brief The current offset in bytes that the lib is writing to the client-driver lane at */
	std::atomic<uint32_t> clientDriverWriteOffset;

	/** @brief The current offset in bytes that the driver has read at from the client-driver lane */
	std::atomic<uint32_t> clientDriverReadOffset;
};

/**
 * @brief Represents the metadata for a single state snapshot in the driver-client lane. Each ObjectEntry is
 * immediately followed a <X>Serialized struct matching the type, containing type specific information
 */
struct ObjectEntry {
	/** @brief Used to check if the packet is aligned as expected in shared memory to the read header */
	uint32_t alignmentCheck = ALIGNMENT_CONSTANT;

	/** @brief The type of Object that is being serialized */
	ObjectType type;

	/** @brief The device index of the device */
	uint32_t deviceIndex;

	/** @brief Offset into the path table identifying the target input */
	uint32_t inputPathOffset;

	/** @brief Version number for ordering and packet age */
	uint64_t version;

	/** @brief True if this object is currently active/valid, false if it should be removed */
	bool valid;

	/** @brief True if this object has successfully and fully finish writing to shared memory */
	std::atomic<bool> committed;
};

/**
 * @brief Serialized representation of a device pose, containing both the current and overwritten pose values
 */
struct DevicePoseSerialized {
	/** @brief The actual value as reported by OpenVR */
	DevicePose pose;

	/** @brief The overwritten pose as managed by Conduit */
	DevicePose overwrittenPose;
};

/**
 * @brief Serialized representation of a boolean input, containing both the current and overwritten values
 */
struct DeviceInputBooleanSerialized {
	/** @brief The actual value as reported by OpenVR */
	BooleanInput value;

	/** @brief The overwritten pose as managed by Conduit */
	BooleanInput overwrittenValue;
};

/**
 * @brief Serialized representation of a scalar input, containing both the current and overwritten values
 */
struct DeviceInputScalarSerialized {
	/** @brief The actual value as reported by OpenVR */
	ScalarInput value;

	/** @brief The overwritten pose as managed by Conduit */
	ScalarInput overwrittenValue;
};

/**
 * @brief Serialized representation of a skeleton input, containing both the current and overwritten values
 */
struct DeviceInputSkeletonSerialized {
	/** @brief The actual value as reported by OpenVR */
	SkeletonInput value;

	/** @brief The overwritten pose as managed by Conduit */
	SkeletonInput overwrittenValue;
};

/**
 * @brief Serialized representation of a pose input, containing both the current and overwritten values
 */
struct DeviceInputPoseSerialized {
	/** @brief The actual value as reported by OpenVR */
	PoseInput value;

	/** @brief The overwritten pose as managed by Conduit */
	PoseInput overwrittenValue;
};

/**
 * @brief Serialized representation of an eye tracking input, containing both the current and overwritten values
 */
struct DeviceInputEyeTrackingSerialized {
	/** @brief The actual value as reported by OpenVR */
	EyeTrackingInput value;

	/** @brief The overwritten pose as managed by Conduit */
	EyeTrackingInput overwrittenValue;
};

/**
 * @brief Header for client-to-driver command packets, containing metadata about the command
 */
struct ClientCommandHeader {
	/** @brief Used to verify proper alignment when reading from shared memory */
	uint32_t alignmentCheck = ALIGNMENT_CONSTANT;

	/** @brief The type of command being sent */
	ClientCommandType type;

	/** @brief The index of the target device */
	uint32_t deviceIndex;

	/** @brief Version number for ordering and packet age */
	uint64_t version;

	/** @brief Indicates whether the command has been fully written and is ready to be read */
	std::atomic<bool> committed;
};

/**
 * @brief Parameters for the SetUseOverridenStateDevicePose command
 */
struct CommandParams_SetUseOverridenStateDevicePose {
	/** @brief Whether to use the overridden pose state instead of the actual device pose */
	bool useOverridenState;
};

/**
 * @brief Parameters for the SetOverridenStateDevicePose command
 */
struct CommandParams_SetOverridenStateDevicePose {
	/** @brief The pose value to use when override is enabled */
	DevicePose overridenPose;
};

/**
 * @brief Parameters for the SetUseOverridenStateDeviceInput command
 */
struct CommandParams_SetUseOverridenStateDeviceInput {
	/** @brief Whether to use the overridden input state instead of the actual device input */
	bool useOverridenState;
	/** @brief Offset into the path table identifying the target input */
	uint32_t inputPathOffset;
};

/**
 * @brief Parameters for the SetOverridenStateDeviceInputBoolean command
 */
struct CommandParams_SetOverridenStateDeviceInputBoolean {
	/** @brief The boolean value to use when override is enabled */
	BooleanInput overridenValue;
	/** @brief Offset into the path table identifying the target input */
	uint32_t inputPathOffset;
};

/**
 * @brief Parameters for the SetOverridenStateDeviceInputScalar command
 */
struct CommandParams_SetOverridenStateDeviceInputScalar {
	/** @brief The scalar value to use when override is enabled */
	ScalarInput overridenValue;
	/** @brief Offset into the path table identifying the target input */
	uint32_t inputPathOffset;
};

/**
 * @brief Parameters for the SetOverridenStateDeviceInputSkeleton command
 */
struct CommandParams_SetOverridenStateDeviceInputSkeleton {
	/** @brief The skeleton value to use when override is enabled */
	SkeletonInput overridenValue;
	/** @brief Offset into the path table identifying the target input */
	uint32_t inputPathOffset;
};

/**
 * @brief Parameters for the SetOverridenStateDeviceInputPose command
 */
struct CommandParams_SetOverridenStateDeviceInputPose {
	/** @brief The pose value to use when override is enabled */
	PoseInput overridenValue;
	/** @brief Offset into the path table identifying the target input */
	uint32_t inputPathOffset;
};

/**
 * @brief Parameters for the SetOverridenStateDeviceInputEyeTracking command
 */
struct CommandParams_SetOverridenStateDeviceInputEyeTracking {
	/** @brief The eye tracking value to use when override is enabled */
	EyeTrackingInput overridenValue;
	/** @brief Offset into the path table identifying the target input */
	uint32_t inputPathOffset;
};

#pragma pack(pop)

/**
 * @brief Parsed data from an ObjectEntry, used for processing after reading from shared memory
 */
struct ObjectEntryData {
	/** @brief Whether the read operation was successful */
	bool successful;
	/** @brief Whether the object entry contains valid data */
	bool valid;
	/** @brief The type of input or pose represented */
	ObjectType type;
	/** @brief The index of the associated device */
	uint32_t deviceIndex;
	/** @brief Version number for ordering and packet age */
	uint64_t version;
	/** @brief Offset into the path table for the input path */
	uint32_t inputPathOffset;
};

/**
 * @brief Parsed data from a ClientCommandHeader, used for processing after reading from shared memory
 */
struct ClientCommandHeaderData {
	/** @brief Whether the read operation was successful */
	bool successful;
	/** @brief The type of command being processed */
	ClientCommandType type;
	/** @brief The index of the target device */
	uint32_t deviceIndex;
	/** @brief Version number for ordering and packet age */
	uint64_t version;
};

/**
 * @brief Base struct for model objects that tracks whether an override state is active
 */
struct ModelObjectState {
	/** @brief Whether to use the overridden state instead of the actual device state */
	bool useOverridenState;
};

/**
 * @brief Model representation of a serialized device pose with override state tracking
 */
struct ModelDevicePoseSerialized : public ModelObjectState {
	/** @brief The serialized pose data containing current and overwritten values */
	DevicePoseSerialized data;
};

/**
 * @brief Model representation of a serialized boolean input with override state tracking
 */
struct ModelDeviceInputBooleanSerialized : public ModelObjectState {
	/** @brief The serialized boolean data containing current and overwritten values */
	DeviceInputBooleanSerialized data;
};

/**
 * @brief Model representation of a serialized scalar input with override state tracking
 */
struct ModelDeviceInputScalarSerialized : public ModelObjectState {
	/** @brief The serialized scalar data containing current and overwritten values */
	DeviceInputScalarSerialized data;
};

/**
 * @brief Model representation of a serialized skeleton input with override state tracking
 */
struct ModelDeviceInputSkeletonSerialized : public ModelObjectState {
	/** @brief The serialized skeleton data containing current and overwritten values */
	DeviceInputSkeletonSerialized data;
};

/**
 * @brief Model representation of a serialized pose input with override state tracking
 */
struct ModelDeviceInputPoseSerialized : public ModelObjectState {
	/** @brief The serialized pose data containing current and overwritten values */
	DeviceInputPoseSerialized data;
};

/**
 * @brief Model representation of a serialized eye tracking input with override state tracking
 */
struct ModelDeviceInputEyeTrackingSerialized : public ModelObjectState {
	/** @brief The serialized eye tracking data containing current and overwritten values */
	DeviceInputEyeTrackingSerialized data;
};
