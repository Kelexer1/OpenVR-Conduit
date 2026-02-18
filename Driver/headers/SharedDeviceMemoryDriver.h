#pragma once
#include <windows.h>
#include <string>
#include <atomic>
#include <cstdint>
#include <cstring>

#include "ObjectSchemas.h"
#include "DeviceTypes.h"
#include "LogManager.h"
#include "DeviceStateModelDriver.h"

/**
 * @brief Manages all interactions from shared memory on the driver side, including reading, client commands, writing
 * state update packets, and notifying relevant classes of updates
 */
class SharedDeviceMemoryDriver {
public:
	/**
	 * @brief Returns the singleton SharedDeviceMemoryDriver instance
	 * @return The singleton instance
	 */
	static SharedDeviceMemoryDriver& getInstance();

	/** 
	 * @brief Cleans up shared memory related tasks gracefully 
	 */
	~SharedDeviceMemoryDriver();

	/**
	 * @brief Initializes shared memory related tasks, should be called exactly once
	 * @return True if initialization was successful, false otherwise
	 */
	bool initialize();

	/**
	 * @brief Checks for updates from the client in the client-driver lane, and notifies relavent objects
	 */
	void pollForClientUpdates();

	/**
	 * @brief Writes a packet encoding the state of a device pose to the driver-client lane
	 * @param packet The device pose to be written
	 * @param deviceIndex The device index of the device
	 */
	void syncDevicePoseUpdateToSharedMemory(DevicePoseSerialized* packet, uint32_t deviceIndex);

	/**
	 * @brief Writes a packet encoding the state of a boolean input to the driver-client lane
	 * @param packet The device pose to be written
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void syncDeviceInputBooleanUpdateToSharedMemory(
		DeviceInputBooleanSerialized* packet, 
		uint32_t deviceIndex, 
		const std::string&
	);

	/**
	 * @brief Writes a packet encoding the state of a scalar input to the driver-client lane
	 * @param packet The device pose to be written
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void syncDeviceInputScalarUpdateToSharedMemory(
		DeviceInputScalarSerialized* packet, 
		uint32_t deviceIndex, 
		const std::string&
	);

	/**
	 * @brief Writes a packet encoding the state of a skeleton input to the driver-client lane
	 * @param packet The device pose to be written
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void syncDeviceInputSkeletonUpdateToSharedMemory(
		DeviceInputSkeletonSerialized* packet, 
		uint32_t deviceIndex, 
		const std::string&
	);

	/**
	 * @brief Writes a packet encoding the state of a pose input to the driver-client lane
	 * @param packet The device pose to be written
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void syncDeviceInputPoseUpdateToSharedMemory(
		DeviceInputPoseSerialized* packet, 
		uint32_t deviceIndex, 
		const std::string&
	);

	/**
	 * @brief Writes a packet encoding the state of an eye tracking input to the driver-client lane
	 * @param packet The device pose to be written
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void syncDeviceInputEyeTrackingUpdateToSharedMemory(
		DeviceInputEyeTrackingSerialized* packet, 
		uint32_t deviceIndex, 
		const std::string&
	);

private:
	/** @brief The handle for the shared memory */
	HANDLE sharedMemoryHandle;

	/** @brief A pointer to the start of the shared memory */
	void* sharedMemory;

	/** @brief The offset in bytes from the start of the shared memory to the start of the path table */
	uint32_t pathTableStart;

	/** @brief A map of paths to their offsets in the path table */
	std::unordered_map<std::string, uint32_t> pathTableOffsets;

	/** @brief The offset in bytes in the path table where the driver is currently appending new paths */
	uint32_t currentPathTableWriteOffset;

	/** @brief The offset in bytes of the driver-client lane from the start of the shared memory */
	uint32_t driverClientLaneStart;

	/** @brief The offset in bytes of where the driver is currently writing in the driver-client lane */
	uint32_t driverClientLaneWriteOffset;

	/** @brief The version of the last written packet in the driver-client lane */
	uint64_t driverClientLaneWriteCount;

	/** @brief The offset in bytes of the client-driver lane from the start of the shared memory */
	uint32_t clientDriverLaneStart;

	/** @brief The offset in bytes of where the driver is currently reading in the client-driver lane */
	uint32_t clientDriverLaneReadOffset;

	/** @brief The version of the last successfully read packet in the client-driver lane */
	uint64_t clientDriverLaneReadCount;

	/** @brief Empty constructor for the SharedDeviceMemoryDriver class to prevent direct instantiaton */
	SharedDeviceMemoryDriver() = default;

	/** 
	 * @brief Initializes important metadata in shared memory, such as creating the shared memory header
	 * @return True if initialization was successful, false otherwise
	 */
	bool initializeSharedMemoryData();

	/**
	 * @brief Returns the path at an offset in the path table
	 * @param offset The byte offset in the path table
	 * @return The path, or an empty string if the offset doesn't correspond to a valid path
	 */
	std::string getPathFromPathOffset(uint32_t offset);

	/**
	 * @brief Returns the byte offset into the path table where the given path is located, adding it if required
	 * @param inputPath The input path
	 * @return The byte offset in the path table
	 */
	uint32_t getOffsetOfPath(const std::string& inputPath);

	/**
	 * @brief Writes a packet into the driver-client lane
	 * @param packet A pointer to the packet, where the ObjectEntry and relevant data are already aligned
	 * and filled with required data
	 * @param packetSize The total size of the packet
	 */
	void writePacketToDriverClientLane(void* packet, uint32_t packetSize);

	/**
	 * @brief Reads a packet from the client-driver lane
	 * @return Data formatted as follows: (Command header, (Command type, Param buffer))
	 * The header contains a successful field that is true if the read was successful, along with any other metadata
	 * The command type dictates how the param buffer should be interpreted
	 * The param buffer has the command parameters stored as raw bytes, and needs casting
	 */
	std::pair<ClientCommandHeaderData, std::pair<ClientCommandType, std::unique_ptr<uint8_t[]>>> readPacketFromClientDriverLane();
	
	/**
	 * @brief Realigns the read header to a valid packet by forward searching for valid packets. If none are found
	 * after a specified amount of bytes, the read header is advanced to the write header, dropping all packets in
	 * between
	 * @param headerPtr The shared memory header
	 * @param laneStart The start of the shared memory lane
	 * @param readStart A pointer to where to write the new read offset in the event that forward searching succeeded
	 * (ie. the method returns true)
	 * @param writeOffset The current write offset of the writer
	 * @param output A pointer to where to write the found valid packet from forward searching, only written if forward
	 * searching was successful (ie. the method returns true)
	 * @return True if forward searching was successful , false if the read header was forcibly advanced (ie. there
	 * are no new packets to read)
	 */
	bool realignReadHeader(
		SharedMemoryHeader* headerPtr,
		uint8_t* laneStart,
		uint8_t** readStart,
		uint32_t writeOffset,
		ClientCommandHeader** output
	);

	/**
	 * @brief Returns true if and only if the given command header is valid by various criteria
	 * @param header The header to test
	 * @param sharedMemoryHeader A pointer to the shared memory header, used to cross-check metadata
	 * @return True if the header is valid, false otherwise
	 */
	bool isValidCommandHeader(const ClientCommandHeader* header, const SharedMemoryHeader* sharedMemoryHeader);
};