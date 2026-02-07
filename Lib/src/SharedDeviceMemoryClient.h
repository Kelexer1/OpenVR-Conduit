#pragma once
#include <Windows.h>
#include <stdint.h>
#include <memory>
#include <thread>
#include <chrono>

#include "ObjectSchemas.h"
#include "DeviceStateModelClient.h"

/**
 * @brief Manages all interactions from the shared memory on the client side, including reading state updates, 
 * writing commands, and notifying relevant classes of updates
 */
class SharedDeviceMemoryClient {
public:
	/**
	 * @brief Returns the singleton SharedDeviceMemoryClient instance
	 * @return The singleton instance
	 */
	static SharedDeviceMemoryClient& getInstance();

	/**
	 * @brief Initializes shared memory related tasks
	 * @return The initialization code, which can be interpreted as follows:
	 * 0 - Success
	 * 1 - Failed to get shared memory handle
	 * 2 - Failed to map shared memory
	 * 3 - Shared memory protocol version mismatch
	 */
	int initialize();

	/**
	 * @brief Handles serializing and writing a command header and command params to the shared memory safely and
	 * with adherence to the protocol
	 * @param type The type of command being issued
	 * @param deviceIndex The index of the device this command is related to
	 * @param paramsStart A pointer to the params struct corresponding to <type>
	 * @param paramsSize The size in bytes of the params being supplied
	 */
	void issueCommandToSharedMemory(ClientCommandType type, uint32_t deviceIndex, void* paramsStart, uint32_t paramsSize);

	/**
	 * @brief Returns the path found at a given offset in the path table
	 * @param offset The offset in bytes into the path table
	 * @return The path if successfull, false if the path is invalid (over the maximum path length or not null terminated)
	 */
	std::string getPathFromPathOffset(uint32_t offset);

	/**
	 * @brief Returns the byte offset into the path table where a path can be found, appending it safely to the path table
	 * if it doesn't yet exist
	 * @param path The input path
	 */
	uint32_t getOffsetOfPath(const std::string& path);

private:
	/** @brief True if the shared memory has been successfully initialized, false otherwise */
	bool initialized;

	/** @brief The handle for the shared memory */
	HANDLE sharedMemoryHandle;

	/** @brief A pointer to the start of the shared memory */
	void* sharedMemory;

	/** @brief The offset in bytes from the start of the shared memory to the start of the path table */
	uint32_t pathTableStart;

	/** @brief A map of paths to their offsets in the path table */
	std::unordered_map<std::string, uint32_t> pathTableOffsets;

	/** @brief The offset in bytes of the driver-client lane from the start of the shared memory */
	uint32_t driverClientLaneStart;

	/** @brief The offset in bytes of where the lib is currently reading in the driver-client lane */
	uint32_t driverClientLaneReadOffset;

	/** @brief The version of the last successfully read packet in the driver-client lane */
	uint64_t driverClientLaneReadCount;

	/** @brief The offset in bytes of the client-driver lane from the start of the shared memory */
	uint32_t clientDriverLaneStart;

	/** @brief The offset in bytes of where the client is currently writing in the client-driver lane */
	uint32_t clientDriverLaneWriteOffset;

	/** @brief The version of the last written packet in the client-driver lane */
	uint64_t clientDriverLaneWriteCount;

	/** @brief Private empty contructor for the singleton pattern */
	SharedDeviceMemoryClient() = default;

	/**
	 * @brief Writes a packet into the client-driver lane
	 * @param packet A pointer to the start of the packet
	 * @param packetSize The size of the packet in bytes
	 */
	void writePacketToClientDriverLane(void* packet, uint32_t packetSize);

	/**
	 * @brief Reads a packet from the driver-client lane
	 * @return Data formatted as follows: (Object entry, (Object type, Data buffer))
	 * The entry contains a successful field that is true if the read was successful, along with any other metadata
	 * The object type dictates how the data buffer should be interpreted
	 * The databuffer has the serialized state snapshot stored as raw bytes, and needs casting
	 */
	std::pair<ObjectEntryData, std::pair<ObjectType, std::unique_ptr<uint8_t[]>>> readPacketFromDriverClientLane();

	/**
	 * @brief Returns true if an only if the given object entry is valid by various criteria
	 * @param entry The entry to test
	 * @param header A pointer to the shared memory header, used to cross-check metadata
	 * @return True if the header is valid, false otherwise
	 */
	bool isValidObjectPacket(const ObjectEntry* entry, const SharedMemoryHeader* header);

	/**
	 * @brief Checks for updates from the driver a single time, and modifies the model according the the data recieved
	 */
	void pollForDriverUpdates();

	/**
	 * @brief Infinitely checks for updates from the driver at the standard poll rate, should be started in a detatched
	 * thread
	 */
	void pollLoop();
};