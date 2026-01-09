#pragma once
#include <windows.h>
#include <string>
#include <atomic>
#include <cstdint>
#include <cstring>

#include "ObjectTypeSchemaBaker.h"
#include "DeviceTypes.h"
#include "LogManager.h"

const wchar_t* SHM_NAME = L"Local\\ConduitSharedDeviceMemory";
const uint32_t PATH_TABLE_ENTRIES = 256;
const size_t LANE_SIZE = 1 * 1048576; // 1mb
size_t SHARED_MEMORY_SIZE = sizeof(SharedMemoryHeader) + 
							ObjectTypeSchemaBaker::getTotalSchemaSize() + 
							PATH_TABLE_ENTRIES * sizeof(PathTableEntry) +
							2 * LANE_SIZE;
const uint32_t PROTOCOL_VERSION = 0;

struct SharedMemoryHeader {
	uint32_t schemaStart;
	uint32_t schemaSize;

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
	std::atomic<uint32_t> clientDriverWriteCount;	// Client keeps its own last consumed write count to track updates
};

struct PathTableEntry {
	uint32_t ID;	// Used to lookup the input path of inputs
	char path[128];
};

struct ObjectEntry {
	ObjectType type;

	uint32_t deviceIndex;
	uint32_t inputPathID;

	uint64_t version;
};

class DeviceStateModel; // Forward declare DeviceStateModel

class SharedDeviceMemoryDriver {
public:
	static SharedDeviceMemoryDriver& getInstance();
	~SharedDeviceMemoryDriver();

	bool initialize();

	void syncPathTableToSharedMemory(PathTableEntry* packet);
	void syncDevicePoseUpdateToSharedMemory(DevicePoseSerialized* packet);
	void syncDeviceInputBooleanUpdateToSharedMemory(DeviceInputBooleanSerialized* packet);
	void syncDeviceInputScalarUpdateToSharedMemory(DeviceInputScalarSerialized* packet);
	void syncDeviceInputSkeletonUpdateToSharedMemory(DeviceInputSkeletonSerialized* packet);
	void syncDeviceInputPoseUpdateToSharedMemory(DeviceInputPoseSerialized* packet);
	void syncDeviceInputEyeTrackingUpdateToSharedMemory(DeviceInputEyeTrackingSerialized* packet);

private:
	HANDLE sharedMemoryHandle;
	void* sharedMemory;

	SharedMemoryHeader header;

	uint32_t pathTableStart;
	uint32_t pathTableSize;
	uint32_t pathTableWriteIndex;

	uint32_t driverClientLaneStart;
	uint32_t driverClientLaneWriteOffset;

	uint32_t clientDriverLaneStart;
	uint32_t clientDriverLaneReadOffset;
	uint32_t clientDriverReadCount;

	SharedDeviceMemoryDriver() = default;

	bool initializeSharedMemoryData();
	bool initializeSchemaTable();

	void writePacketToDriverClientLane(void* packet, uint32_t packetSize);
};