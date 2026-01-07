#include "SharedDeviceMemoryDriver.h"
#include "ObjectTypeSchemaBaker.h"

SharedDeviceMemoryDriver SharedDeviceMemoryDriver::getInstance() {
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

	int tempOffset = this->header.schemaStart + this->header.schemaSize; // Guarantee that the header and schema are not accidentally overwritten

	this->header.pathTableStart = tempOffset;
	this->header.driverClientLaneStart = tempOffset;
	this->header.clientDriverLaneStart = tempOffset;

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