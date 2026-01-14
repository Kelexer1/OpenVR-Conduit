#pragma once
#include <Windows.h>
#include <stdint.h>
#include <memory>

#include "ObjectSchemas.h"
#include "DeviceStateModelClient.h"

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

	void issueCommandToSharedMemory(ClientCommandType type, uint32_t deviceIndex, void* paramsStart, uint32_t paramsSize);

private:
	HANDLE sharedMemoryHandle;
	void* sharedMemory;

	uint32_t pathTableStart;
	uint32_t pathTableReadOffset;
	uint64_t pathTableReadCount;

	uint32_t driverClientLaneStart;
	uint32_t driverClientLaneReadOffset;
	uint64_t driverClientLaneReadCount;

	uint32_t clientDriverLaneStart;
	uint32_t clientDriverLaneWriteOffset;
	uint64_t clientDriverWriteCount;

	SharedDeviceMemoryClient() = default;

	void writePacketToClientDriverLane(void* packet, uint32_t packetSize);
	std::unique_ptr<uint8_t[]> readPacketFromDriverClientLane(uint32_t packetSize);
	std::unique_ptr<uint8_t[]> readPacketFromPathTable(uint32_t packetSize);
};