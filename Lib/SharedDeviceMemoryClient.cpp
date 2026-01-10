#include "SharedDeviceMemoryClient.h"

const char* SHM_NAME = "Local\\ConduitSharedDeviceMemory";
const uint32_t PATH_TABLE_ENTRIES = 256;
const size_t LANE_SIZE = 1 * 1048576; // 1mb
const size_t SHARED_MEMORY_SIZE = sizeof(SharedMemoryHeader) + PATH_TABLE_ENTRIES * sizeof(PathTableEntry) + 2 * LANE_SIZE;
const uint32_t PROTOCOL_VERSION = 0;

SharedDeviceMemoryClient& SharedDeviceMemoryClient::getInstance() {
	static SharedDeviceMemoryClient instance;
	return instance;
}

bool SharedDeviceMemoryClient::initialize() {
	this->sharedMemoryHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);

	if (!this->sharedMemoryHandle) {
		return false;
	}

	this->sharedMemory = MapViewOfFile(this->sharedMemoryHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

	if (!this->sharedMemory) {
		return false;
	}
}

void SharedDeviceMemoryClient::pollForDriverUpdates() {
	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);

	// todo
}

void SharedDeviceMemoryClient::writePacketToClientDriverLane(void* packet, uint32_t packetSize) {
	// todo
}