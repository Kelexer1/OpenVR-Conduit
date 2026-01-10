#pragma once
#include <Windows.h>
#include <stdint.h>

#include "ObjectSchemas.h"

extern const char* SHM_NAME;
extern const uint32_t PATH_TABLE_ENTRIES;
extern const size_t LANE_SIZE;
extern const size_t SHARED_MEMORY_SIZE;
extern const uint32_t PROTOCOL_VERSION;

class SharedDeviceMemoryClient {
public:
	static SharedDeviceMemoryClient& getInstance();

	bool initialize();
	void pollForDriverUpdates();

private:
	HANDLE sharedMemoryHandle;
	void* sharedMemory;

	uint32_t pathTableStart;
	uint32_t pathTableSize;
	uint64_t pathTableReadCount;

	uint32_t driverClientLaneStart;
	uint32_t driverClientLaneReadOffset;
	uint64_t driverClientLaneReadCount;

	uint32_t clientDriverLaneStart;
	uint32_t clientDriverLaneWriteOffset;
	uint64_t clientDriverWriteCount;

	SharedDeviceMemoryClient() = default;

	void writePacketToClientDriverLane(void* packet, uint32_t packetSize);
};