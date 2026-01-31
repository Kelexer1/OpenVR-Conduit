#pragma once
#include <Windows.h>
#include <stdint.h>
#include <memory>
#include <thread>
#include <chrono>

#include "ObjectSchemas.h"
#include "DeviceStateModelClient.h"

class SharedDeviceMemoryClient {
public:
	static SharedDeviceMemoryClient& getInstance();

	int initialize();

	void issueCommandToSharedMemory(ClientCommandType type, uint32_t deviceIndex, void* paramsStart, uint32_t paramsSize);

private:
	bool initialized;

	HANDLE sharedMemoryHandle;
	void* sharedMemory;

	uint32_t driverClientLaneStart;
	uint32_t driverClientLaneReadOffset;
	uint64_t driverClientLaneReadCount;

	uint32_t clientDriverLaneStart;
	uint32_t clientDriverLaneWriteOffset;
	uint64_t clientDriverLaneWriteCount;

	SharedDeviceMemoryClient() = default;

	void writePacketToClientDriverLane(void* packet, uint32_t packetSize);
	std::pair<ObjectEntryData, std::pair<ObjectType, std::unique_ptr<uint8_t[]>>> readPacketFromDriverClientLane();
	bool isValidObjectPacket(const ObjectEntry* entry, const SharedMemoryHeader* header);

	void pollForDriverUpdates();
	void pollLoop();
};