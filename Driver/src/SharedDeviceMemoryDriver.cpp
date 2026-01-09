#include "SharedDeviceMemoryDriver.h"
#include "ObjectTypeSchemaBaker.h"

SharedDeviceMemoryDriver& SharedDeviceMemoryDriver::getInstance() {
	static SharedDeviceMemoryDriver instance;
	return instance;
}

SharedDeviceMemoryDriver::~SharedDeviceMemoryDriver() {
	UnmapViewOfFile(this->sharedMemory);
	CloseHandle(this->sharedMemoryHandle);
}

bool SharedDeviceMemoryDriver::initialize() {
	// Create shared memory
	this->sharedMemoryHandle = CreateFileMappingW(
		INVALID_HANDLE_VALUE,
		nullptr,
		PAGE_READWRITE,
		0,
		SHARED_MEMORY_SIZE,
		SHM_NAME
	);

	if (!this->sharedMemoryHandle) {
		LogManager::log(LOG_ERROR, "Failed to create shared memory handle: {}", GetLastError());
		return false;
	}

	this->sharedMemory = MapViewOfFile(this->sharedMemoryHandle, FILE_MAP_ALL_ACCESS, 0, 0, SHARED_MEMORY_SIZE);

	if (this->initializeSharedMemoryData()) {
		LogManager::log(LOG_ERROR, "Failed to initialize shared memory data");
		return false;
	}

	return true;
}

bool SharedDeviceMemoryDriver::initializeSharedMemoryData() {
	// Init header
	this->header.version = PROTOCOL_VERSION;
	
	this->header.schemaStart = sizeof(SharedMemoryHeader);

	if (!this->initializeSchemaTable()) {
		LogManager::log(LOG_ERROR, "Failed to initialize schema table");
		return false;
	}

	int currentOffset = this->header.schemaStart + this->header.schemaSize;

	this->header.pathTableStart = this->pathTableStart = currentOffset;
	this->header.pathTableSize = this->pathTableSize = PATH_TABLE_ENTRIES * sizeof(PathTableEntry);
	this->header.pathTableEntries = PATH_TABLE_ENTRIES;
	this->header.pathTableEntrySize = sizeof(PathTableEntry);

	currentOffset += this->header.pathTableSize;

	this->header.driverClientLaneStart = this->driverClientLaneStart = currentOffset;
	this->header.driverClientLaneSize = LANE_SIZE;
	
	currentOffset += LANE_SIZE;

	this->header.clientDriverLaneStart = currentOffset;
	this->header.clientDriverLaneSize = LANE_SIZE;

	memcpy(this->sharedMemory, &this->header, sizeof(this->header));

	return true;
}

bool SharedDeviceMemoryDriver::initializeSchemaTable() {
	int totalSize = 0;

	uint8_t* schemaTableOffset = static_cast<uint8_t*>(this->sharedMemory) + this->header.schemaStart;
	uint8_t* currentOffset = schemaTableOffset;

	// Write number of object types
	*reinterpret_cast<uint32_t*>(currentOffset) = NUM_OBJECT_TYPES;
	currentOffset += 4;
	totalSize += 4;

	std::vector<ObjectSchemaEntry> schemas = ObjectTypeSchemaBaker::getSchemas();
	std::vector<int> schemaSizes = ObjectTypeSchemaBaker::getSchemaSizes();

	// Calculate and write schema offsets
	int schemaDataStart = 4 + (4 * NUM_OBJECT_TYPES); // After count and all offsets
	for (int i = 0; i < NUM_OBJECT_TYPES; i++) {
		*reinterpret_cast<uint32_t*>(currentOffset) = schemaDataStart;
		currentOffset += 4;
		totalSize += 4;
		schemaDataStart += schemaSizes[i];
	}

	// Write actual schema data
	for (int i = 0; i < NUM_OBJECT_TYPES; i++) {
		memcpy(currentOffset, &schemas[i], schemaSizes[i]);
		currentOffset += schemaSizes[i];
		totalSize += schemaSizes[i];
	}

	this->header.schemaSize = totalSize;

	return true;
}

void SharedDeviceMemoryDriver::writePacketToDriverClientLane(void* packet, uint32_t packetSize) {
	if (packet != nullptr && packetSize > 0) {
		uint8_t* laneStart = static_cast<uint8_t*>(this->sharedMemory) + this->driverClientLaneStart;
		SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);

		size_t currentOffset = this->driverClientLaneWriteOffset;

		// Handle wrap around
		if (currentOffset + packetSize > LANE_SIZE) {
			size_t firstPartitionSize = LANE_SIZE - currentOffset;
			memcpy(laneStart + currentOffset, packet, firstPartitionSize);

			size_t secondPartitionSize = packetSize - firstPartitionSize;
			memcpy(laneStart + currentOffset, reinterpret_cast<uint8_t*>(packet) + firstPartitionSize, secondPartitionSize);

			this->driverClientLaneWriteOffset = secondPartitionSize;
		} else {
			memcpy(laneStart + currentOffset, packet, packetSize);
			this->driverClientLaneWriteOffset = (currentOffset + packetSize) % LANE_SIZE;
		}

		headerPtr->driverClientWriteCount++;
	}
}

void SharedDeviceMemoryDriver::syncPathTableToSharedMemory(PathTableEntry* packet) {
	// Write data starting at current index, should never have to loop around mid write
	memcpy(static_cast<uint8_t*>(this->sharedMemory) + this->pathTableStart + this->pathTableWriteIndex * sizeof(PathTableEntry), packet, sizeof(PathTableEntry));
	
	// Update write count in shared memory header
	this->pathTableWriteIndex = (this->pathTableWriteIndex + 1) % PATH_TABLE_ENTRIES;
	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	headerPtr->pathTableWriteCount++;
}

void SharedDeviceMemoryDriver::syncDevicePoseUpdateToSharedMemory(DevicePoseSerialized* packet) {
	this->writePacketToDriverClientLane((void*)packet, sizeof(DevicePoseSerialized));
}

void SharedDeviceMemoryDriver::syncDeviceInputBooleanUpdateToSharedMemory(DeviceInputBooleanSerialized* packet) {
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputBooleanSerialized));
}

void SharedDeviceMemoryDriver::syncDeviceInputScalarUpdateToSharedMemory(DeviceInputScalarSerialized* packet) {
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputScalarSerialized));
}

void SharedDeviceMemoryDriver::syncDeviceInputSkeletonUpdateToSharedMemory(DeviceInputSkeletonSerialized* packet) {
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputSkeletonSerialized));
}

void SharedDeviceMemoryDriver::syncDeviceInputPoseUpdateToSharedMemory(DeviceInputPoseSerialized* packet) {
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputPoseSerialized));
}

void SharedDeviceMemoryDriver::syncDeviceInputEyeTrackingUpdateToSharedMemory(DeviceInputEyeTrackingSerialized* packet) {
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputEyeTrackingSerialized));
}