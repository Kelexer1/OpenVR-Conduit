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

extern const wchar_t* SHM_NAME;
extern const uint32_t PATH_TABLE_ENTRIES;
extern const size_t LANE_SIZE;
extern const size_t SHARED_MEMORY_SIZE;
extern const uint32_t PROTOCOL_VERSION;

class SharedDeviceMemoryDriver {
public:
	static SharedDeviceMemoryDriver& getInstance();
	~SharedDeviceMemoryDriver();

	bool initialize();
	void pollForClientUpdates();

	void syncPathTableToSharedMemory(uint32_t pathID, const std::string& path);
	void syncDevicePoseUpdateToSharedMemory(DevicePoseSerialized* packet, uint32_t deviceIndex);
	void syncDeviceInputBooleanUpdateToSharedMemory(DeviceInputBooleanSerialized* packet, uint32_t deviceIndex, uint32_t pathID);
	void syncDeviceInputScalarUpdateToSharedMemory(DeviceInputScalarSerialized* packet, uint32_t deviceIndex, uint32_t pathID);
	void syncDeviceInputSkeletonUpdateToSharedMemory(DeviceInputSkeletonSerialized* packet, uint32_t deviceIndex, uint32_t pathID);
	void syncDeviceInputPoseUpdateToSharedMemory(DeviceInputPoseSerialized* packet, uint32_t deviceIndex, uint32_t pathID);
	void syncDeviceInputEyeTrackingUpdateToSharedMemory(DeviceInputEyeTrackingSerialized* packet, uint32_t deviceIndex, uint32_t pathID);

private:
	HANDLE sharedMemoryHandle;
	void* sharedMemory;

	uint32_t pathTableStart;
	uint32_t pathTableSize;
	uint32_t pathTableWriteIndex; // By index because all entries are uniform size
	uint64_t pathTableWriteCount;

	uint32_t driverClientLaneStart;
	uint32_t driverClientLaneWriteOffset; // By byte offset since entries vary in size
	uint64_t driverClientLaneWriteCount;

	uint32_t clientDriverLaneStart;
	uint32_t clientDriverLaneReadOffset;
	uint64_t clientDriverReadCount;

	SharedDeviceMemoryDriver() = default;

	bool initializeSharedMemoryData();

	void writePacketToDriverClientLane(void* packet, uint32_t packetSize);
	std::unique_ptr<uint8_t[]> readPacketFromClientDriverLane(uint32_t packetSize);
};