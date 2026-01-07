#pragma once
#include <windows.h>
#include <vector>
#include <string>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <thread>

#include "ObjectTypeSchemaBaker.h"
#include "DeviceTypes.h"
#include "LogManager.h"

const wchar_t* SHM_NAME = L"Local\\ConduitSharedDeviceMemory";
const size_t SHARED_MEMORY_SIZE = 1048576; // 5mb of space, probably overkill
const uint32_t PROTOCOL_VERSION = 0;

struct SharedMemoryHeader {
	uint32_t version;
	
	uint32_t schemaStart;
	uint32_t schemaSize;

	uint32_t pathTableStart;
	uint32_t pathTableSize;
	uint32_t pathTableEntries;
	uint32_t pathTableEntrySize;

	uint32_t driverClientLaneStart;
	uint32_t driverClientLaneSize;
	uint32_t driverClientWriteCount;
	
	uint32_t clientDriverLaneStart;
	uint32_t clientDriverLaneSize;
	uint32_t clientDriverWriteCount;
};

struct PathTableEntry {
	uint32_t ID;
	char path[128];
	bool valid;
};

class SharedDeviceMemoryDriver {
public:
	static SharedDeviceMemoryDriver getInstance();
	~SharedDeviceMemoryDriver();

	bool initialize();

private:
	static SharedDeviceMemoryDriver singleton;
	SharedDeviceMemoryDriver() = default;

	HANDLE sharedMemoryHandle;
	void* sharedMemory;

	SharedMemoryHeader header;

	bool initializeSharedMemoryData();
	bool initializeSchemaTable();
};