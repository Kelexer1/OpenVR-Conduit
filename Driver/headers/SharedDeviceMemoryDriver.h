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

class SharedDeviceMemoryDriver {
public:
	static SharedDeviceMemoryDriver& getInstance();
	~SharedDeviceMemoryDriver();

	bool initialize();
	void pollForClientUpdates();

	void syncDevicePoseUpdateToSharedMemory(DevicePoseSerialized* packet, uint32_t deviceIndex);
	void syncDeviceInputBooleanUpdateToSharedMemory(DeviceInputBooleanSerialized* packet, uint32_t deviceIndex, const std::string&);
	void syncDeviceInputScalarUpdateToSharedMemory(DeviceInputScalarSerialized* packet, uint32_t deviceIndex, const std::string&);
	void syncDeviceInputSkeletonUpdateToSharedMemory(DeviceInputSkeletonSerialized* packet, uint32_t deviceIndex, const std::string&);
	void syncDeviceInputPoseUpdateToSharedMemory(DeviceInputPoseSerialized* packet, uint32_t deviceIndex, const std::string&);
	void syncDeviceInputEyeTrackingUpdateToSharedMemory(DeviceInputEyeTrackingSerialized* packet, uint32_t deviceIndex, const std::string&);

private:
	HANDLE sharedMemoryHandle;
	void* sharedMemory;

	uint32_t pathTableStart;

	std::unordered_map<std::string, uint32_t> pathTableOffsets;
	uint32_t currentPathTableWriteOffset;

	uint32_t driverClientLaneStart;
	uint32_t driverClientLaneWriteOffset; // By byte offset since entries vary in size
	uint64_t driverClientLaneWriteCount;

	uint32_t clientDriverLaneStart;
	uint32_t clientDriverLaneReadOffset;
	uint64_t clientDriverLaneReadCount;

	SharedDeviceMemoryDriver() = default;

	bool initializeSharedMemoryData();

	std::string getPathFromPathOffset(uint32_t offset);
	uint32_t getOffsetOfPath(const std::string& inputPath);

	void writePacketToDriverClientLane(void* packet, uint32_t packetSize);
	std::pair<ClientCommandHeaderData, std::pair<ClientCommandType, std::unique_ptr<uint8_t[]>>> readPacketFromClientDriverLane();
	bool isValidCommandHeader(const ClientCommandHeader* header, const SharedMemoryHeader* sharedMemoryHeader);
};