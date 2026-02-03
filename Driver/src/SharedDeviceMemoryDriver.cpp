#include "SharedDeviceMemoryDriver.h"

const uint32_t PROTOCOL_VERSION = 5;
const uint32_t SHARED_MEMORY_SIZE = sizeof(SharedMemoryHeader) + PATH_TABLE_SIZE + 2 * LANE_SIZE;

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
	this->sharedMemoryHandle = CreateFileMappingA(
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

	if (!this->initializeSharedMemoryData()) {
		LogManager::log(LOG_ERROR, "Failed to initialize shared memory data: {}", GetLastError());
		return false;
	}

	return true;
}

bool SharedDeviceMemoryDriver::initializeSharedMemoryData() {
	// Init header
	SharedMemoryHeader header = {};

	header.protocolVersion = PROTOCOL_VERSION;

	int currentOffset = sizeof(SharedMemoryHeader);

	header.pathTableStart = this->pathTableStart = currentOffset;
	header.pathTableSize = PATH_TABLE_SIZE;
	header.pathTableWritingOffset = UINT32_MAX;

	currentOffset += PATH_TABLE_SIZE;

	header.driverClientLaneStart = this->driverClientLaneStart = currentOffset;
	header.driverClientLaneSize = LANE_SIZE;
	header.driverClientWriteCount = 0;
	header.driverClientWriteOffset = 0;
	header.driverClientReadOffset = 0;
	
	currentOffset += LANE_SIZE;

	header.clientDriverLaneStart = this->clientDriverLaneStart = currentOffset;
	header.clientDriverLaneSize = LANE_SIZE;
	header.clientDriverWriteCount = 0;
	header.clientDriverWriteOffset = 0;
	header.clientDriverReadOffset = 0;

	memcpy(this->sharedMemory, &header, sizeof(SharedMemoryHeader));

	return true;
}

std::string SharedDeviceMemoryDriver::getPathFromPathOffset(uint32_t offset) {
	uint8_t* pathTableStart = static_cast<uint8_t*>(this->sharedMemory) + this->pathTableStart;

	SharedMemoryHeader* headerPtr = reinterpret_cast<SharedMemoryHeader*>(this->sharedMemory);
	while (headerPtr->pathTableWritingOffset.load(std::memory_order_acquire) == offset) {} // Wait for offset to finish writing if required

	const char* path = reinterpret_cast<const char*>(pathTableStart + offset);

	uint32_t len;
	for (len = 0; len < MAX_PATH_LENGTH; len++) {
		if (path[len] == '\0') break;
	}

	if (len == MAX_PATH_LENGTH) {
		return std::string();
	}

	return std::string(path);
}

uint32_t SharedDeviceMemoryDriver::getOffsetOfPath(const std::string& path) {
	SharedMemoryHeader* headerPtr = reinterpret_cast<SharedMemoryHeader*>(this->sharedMemory);

	auto foundOffet = this->pathTableOffsets.find(path);
	if (foundOffet != this->pathTableOffsets.end()) {
		headerPtr->pathTableWritingOffset.store(UINT32_MAX, std::memory_order_release);
		return foundOffet->second;
	}

	uint32_t offset = this->currentPathTableWriteOffset;
	uint32_t writeSize = (uint32_t) path.length() + 1;

	if (offset + writeSize > PATH_TABLE_SIZE) {
		LogManager::log(LOG_ERROR, "Path table overflow when adding path {}", path);
		return UINT32_MAX;
	}

	headerPtr->pathTableWritingOffset.store(offset, std::memory_order_release);
	this->pathTableOffsets[path] = offset;

	uint8_t* writeLocation = static_cast<uint8_t*>(this->sharedMemory) + this->pathTableStart + offset;
	
	memcpy(writeLocation, path.c_str(), writeSize);
	this->currentPathTableWriteOffset += writeSize;

	headerPtr->pathTableWritingOffset.store(UINT32_MAX, std::memory_order_release);

	return offset;
}

void SharedDeviceMemoryDriver::pollForClientUpdates() {
	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	DeviceStateModel& model = DeviceStateModel::getInstance();

	uint64_t currentWriteCount = headerPtr->clientDriverWriteCount.load(std::memory_order_acquire);
	if (this->clientDriverLaneReadCount < currentWriteCount) {
		std::atomic_thread_fence(std::memory_order_acquire);

		ClientCommandHeaderData commandHeader;

		do {
			auto packet = this->readPacketFromClientDriverLane();
			commandHeader = packet.first;

			if (!commandHeader.successful) break;

			std::unique_ptr<uint8_t[]>& paramsBuf = packet.second.second;

			uint32_t deviceIndex = commandHeader.deviceIndex;
			std::string path;

			switch (commandHeader.type) {
				case Command_SetUseOverridenStateDevicePose: {
						CommandParams_SetUseOverridenStateDevicePose * params = reinterpret_cast<CommandParams_SetUseOverridenStateDevicePose*>(paramsBuf.get());

						ModelDevicePoseSerialized* pose = model.getDevicePose(deviceIndex);
						if (pose) {
							pose->useOverridenState = params->useOverridenState;
						}

						break;
				}
				case Command_SetOverridenStateDevicePose: {
					CommandParams_SetOverridenStateDevicePose* params = reinterpret_cast<CommandParams_SetOverridenStateDevicePose*>(paramsBuf.get());

					ModelDevicePoseSerialized* pose = model.getDevicePose(deviceIndex);
					if (pose) {
						pose->data.overwrittenPose = params->overridenPose;
						model.setDevicePoseChanged(deviceIndex);
					}

					break;
				}
				case Command_SetUseOverridenStateDeviceInput: {
					CommandParams_SetUseOverridenStateDeviceInput* params = reinterpret_cast<CommandParams_SetUseOverridenStateDeviceInput*>(paramsBuf.get());
					std::string inputPath = this->getPathFromPathOffset(params->inputPathOffset);

					ModelDeviceInputBooleanSerialized* inputBoolean = model.getBooleanInput(deviceIndex, inputPath);
					if (inputBoolean) {
						inputBoolean->useOverridenState = params->useOverridenState;
						model.setInputBooleanChanged(deviceIndex, inputPath);
					}

					ModelDeviceInputScalarSerialized* inputScalar = model.getScalarInput(deviceIndex, inputPath);
					if (inputScalar) {
						inputScalar->useOverridenState = params->useOverridenState;
						model.setInputScalarChanged(deviceIndex, inputPath);
					}

					ModelDeviceInputSkeletonSerialized* inputSkeleton = model.getSkeletonInput(deviceIndex, inputPath);
					if (inputSkeleton) {
						inputSkeleton->useOverridenState = params->useOverridenState;
						model.setInputSkeletonChanged(deviceIndex, inputPath);
					}

					ModelDeviceInputPoseSerialized* inputPose = model.getPoseInput(deviceIndex, inputPath);
					if (inputPose) {
						inputPose->useOverridenState = params->useOverridenState;
						model.setInputPoseChanged(deviceIndex, inputPath);
					}

					ModelDeviceInputEyeTrackingSerialized* inputEyeTracking = model.getEyeTrackingInput(deviceIndex, inputPath);
					if (inputEyeTracking) {
						inputEyeTracking->useOverridenState = params->useOverridenState;
						model.setInputEyeTrackingChanged(deviceIndex, inputPath);
					}

					break;
				}
				case Command_SetOverridenStateDeviceInputBoolean: {
					CommandParams_SetOverridenStateDeviceInputBoolean* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputBoolean*>(paramsBuf.get());
					std::string inputPath = this->getPathFromPathOffset(params->inputPathOffset);

					ModelDeviceInputBooleanSerialized* input = model.getBooleanInput(deviceIndex, inputPath);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
				case Command_SetOverridenStateDeviceInputScalar: {
					CommandParams_SetOverridenStateDeviceInputScalar* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputScalar*>(paramsBuf.get());
					std::string inputPath = this->getPathFromPathOffset(params->inputPathOffset);

					ModelDeviceInputScalarSerialized* input = model.getScalarInput(deviceIndex, inputPath);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
				case Command_SetOverridenStateDeviceInputSkeleton: {
					CommandParams_SetOverridenStateDeviceInputSkeleton* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputSkeleton*>(paramsBuf.get());
					std::string inputPath = this->getPathFromPathOffset(params->inputPathOffset);

					ModelDeviceInputSkeletonSerialized* input = model.getSkeletonInput(deviceIndex, inputPath);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
				case Command_SetOverridenStateDeviceInputPose: {
					CommandParams_SetOverridenStateDeviceInputPose* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputPose*>(paramsBuf.get());
					std::string inputPath = this->getPathFromPathOffset(params->inputPathOffset);

					ModelDeviceInputPoseSerialized* input = model.getPoseInput(deviceIndex, inputPath);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
				case Command_SetOverridenStateDeviceInputEyeTracking: {
					CommandParams_SetOverridenStateDeviceInputEyeTracking* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputEyeTracking*>(paramsBuf.get());
					std::string inputPath = this->getPathFromPathOffset(params->inputPathOffset);

					ModelDeviceInputEyeTrackingSerialized* input = model.getEyeTrackingInput(deviceIndex, inputPath);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
			}

			this->clientDriverLaneReadCount = commandHeader.version;
		} while (commandHeader.version < this->clientDriverLaneReadCount);

		this->clientDriverLaneReadCount = headerPtr->clientDriverWriteCount;
	} 
}

void SharedDeviceMemoryDriver::writePacketToDriverClientLane(void* packet, uint32_t packetSize) {
	if (!packet || packetSize <= 0) return;

	SharedMemoryHeader* headerPtr = reinterpret_cast<SharedMemoryHeader*>(this->sharedMemory);

	uint32_t readOffset = headerPtr->driverClientReadOffset.load(std::memory_order_acquire);
	uint32_t writeOffset = this->driverClientLaneWriteOffset;
		
	uint32_t freeSpace = writeOffset >= readOffset 
		? (LANE_SIZE - writeOffset) + readOffset
		: readOffset - writeOffset;
		
	if (packetSize > freeSpace) return;

	uint8_t* laneStart = static_cast<uint8_t*>(this->sharedMemory) + this->driverClientLaneStart;

	// The max packet size is the skeleton serialized data (4kb), which is unable to go out of bounds
	// since the padding is 5kb, therefore, no bound checks are performed
	uint8_t* currentWriteStart;
	uint32_t newWriteOffset;
	if (this->driverClientLaneWriteOffset >= LANE_SIZE - LANE_PADDING_SIZE) {
		currentWriteStart = laneStart;
		newWriteOffset = packetSize;
	} else {
		currentWriteStart = laneStart + this->driverClientLaneWriteOffset;
		newWriteOffset = this->driverClientLaneWriteOffset + packetSize;
	}

	memcpy(currentWriteStart, packet, packetSize);

	ObjectEntry* entry = reinterpret_cast<ObjectEntry*>(currentWriteStart);
	entry->committed.store(true, std::memory_order_release);

	this->driverClientLaneWriteOffset = newWriteOffset;
	headerPtr->driverClientWriteOffset.store(this->driverClientLaneWriteOffset, std::memory_order_release);

	this->driverClientLaneWriteCount++;
	headerPtr->driverClientWriteCount.store(this->driverClientLaneWriteCount, std::memory_order_release);
}
 
std::pair<ClientCommandHeaderData, std::pair<ClientCommandType, std::unique_ptr<uint8_t[]>>> SharedDeviceMemoryDriver::readPacketFromClientDriverLane() {
	SharedMemoryHeader* headerPtr = reinterpret_cast<SharedMemoryHeader*>(this->sharedMemory);

	if (this->clientDriverLaneReadOffset >= LANE_SIZE - LANE_PADDING_SIZE) this->clientDriverLaneReadOffset = 0;

	uint32_t writeOffset = headerPtr->clientDriverWriteOffset.load(std::memory_order_acquire);
	if (this->clientDriverLaneReadOffset == writeOffset) return { ClientCommandHeaderData{}, { Command_SetOverridenStateDevicePose, nullptr } };

	// Read ClientCommandHeader
	uint8_t* laneStart = static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart;
	uint8_t* readStart = laneStart + this->clientDriverLaneReadOffset;
	ClientCommandHeader* rawHeader = reinterpret_cast<ClientCommandHeader*>(readStart);

	// Misalignment correction
	if (!this->isValidCommandHeader(rawHeader, headerPtr)) {
		bool recovered = false;
		uint32_t searchOffset = this->clientDriverLaneReadOffset;

		const uint32_t FORWARD_SEARCH_BYTES = 4096;

		// Strategy 1: Forward Check
		for (uint32_t i = 0; i < FORWARD_SEARCH_BYTES; i++) {
			searchOffset = (searchOffset + 1) % LANE_SIZE;

			if (searchOffset == writeOffset) break;

			ClientCommandHeader* testHeader = reinterpret_cast<ClientCommandHeader*>(laneStart + searchOffset);

			if (this->isValidCommandHeader(testHeader, headerPtr)) {
				recovered = true;
				this->clientDriverLaneReadOffset = searchOffset;
				readStart = laneStart + searchOffset;
				rawHeader = testHeader;
				break;
			}
		}

		// Strategy 2: Jump To Write Offset
		if (!recovered) {
			this->clientDriverLaneReadOffset = headerPtr->clientDriverWriteOffset.load(std::memory_order_acquire);
			this->clientDriverLaneReadCount = headerPtr->clientDriverWriteCount.load(std::memory_order_acquire);
			return { ClientCommandHeaderData{}, { Command_SetUseOverridenStateDevicePose , nullptr} };
		}
	}

	while (!rawHeader->committed.load(std::memory_order_acquire)) {} // Wait for packet to be valid

	ClientCommandHeaderData header;
	header.successful = true;
	header.type = rawHeader->type;
	header.deviceIndex = rawHeader->deviceIndex;
	header.version = rawHeader->version;

	// Read object data
	uint32_t dataSize = 0;
	ClientCommandType type = header.type;
	switch (type) {
	case Command_SetUseOverridenStateDevicePose:
		dataSize = sizeof(CommandParams_SetUseOverridenStateDevicePose); break;
	case Command_SetOverridenStateDevicePose:
		dataSize = sizeof(CommandParams_SetOverridenStateDevicePose); break;
	case Command_SetUseOverridenStateDeviceInput:
		dataSize = sizeof(CommandParams_SetUseOverridenStateDeviceInput); break;
	case Command_SetOverridenStateDeviceInputBoolean:
		dataSize = sizeof(CommandParams_SetOverridenStateDeviceInputBoolean); break;
	case Command_SetOverridenStateDeviceInputScalar:
		dataSize = sizeof(CommandParams_SetOverridenStateDeviceInputScalar); break;
	case Command_SetOverridenStateDeviceInputSkeleton:
		dataSize = sizeof(CommandParams_SetOverridenStateDeviceInputSkeleton); break;
	case Command_SetOverridenStateDeviceInputPose:
		dataSize = sizeof(CommandParams_SetOverridenStateDeviceInputPose); break;
	case Command_SetOverridenStateDeviceInputEyeTracking:
		dataSize = sizeof(CommandParams_SetOverridenStateDeviceInputEyeTracking); break;
	}

	auto dataBuffer = std::make_unique<uint8_t[]>(dataSize);
	memcpy(dataBuffer.get(), readStart + sizeof(ClientCommandHeader), dataSize);

	this->clientDriverLaneReadOffset += sizeof(ClientCommandHeader) + dataSize;
	headerPtr->clientDriverReadOffset.store(this->clientDriverLaneReadOffset, std::memory_order_release);

	return std::make_pair(header, std::make_pair(type, std::move(dataBuffer)));
}

void SharedDeviceMemoryDriver::syncDevicePoseUpdateToSharedMemory(DevicePoseSerialized* packet, uint32_t deviceIndex) {
	constexpr uint32_t totalSize = sizeof(ObjectEntry) + sizeof(DevicePoseSerialized);
	uint8_t buffer[totalSize];

	ObjectEntry* entry = reinterpret_cast<ObjectEntry*>(buffer);
	entry->alignmentCheck = ALIGNMENT_CONSTANT;
	entry->type = Object_DevicePose;
	entry->deviceIndex = deviceIndex;
	entry->inputPathOffset = 0;
	entry->version = this->driverClientLaneWriteCount;
	entry->valid = true;
	entry->committed.store(false, std::memory_order_relaxed);

	memcpy(buffer + sizeof(ObjectEntry), packet, sizeof(DevicePoseSerialized));

	this->writePacketToDriverClientLane(buffer, totalSize);
}

void SharedDeviceMemoryDriver::syncDeviceInputBooleanUpdateToSharedMemory(DeviceInputBooleanSerialized* packet, uint32_t deviceIndex, const std::string& path) {
	uint32_t offset = this->getOffsetOfPath(path);
	if (offset == UINT32_MAX) return;
	
	constexpr uint32_t totalSize = sizeof(ObjectEntry) + sizeof(DeviceInputBooleanSerialized);
	uint8_t buffer[totalSize];

	ObjectEntry* entry = reinterpret_cast<ObjectEntry*>(buffer);
	entry->alignmentCheck = ALIGNMENT_CONSTANT;
	entry->type = Object_InputBoolean;
	entry->deviceIndex = deviceIndex;
	entry->inputPathOffset = offset;
	entry->version = this->driverClientLaneWriteCount;
	entry->valid = true;
	entry->committed.store(false, std::memory_order_relaxed);

	memcpy(buffer + sizeof(ObjectEntry), packet, sizeof(DeviceInputBooleanSerialized));
	
	this->writePacketToDriverClientLane(buffer, totalSize);
}

void SharedDeviceMemoryDriver::syncDeviceInputScalarUpdateToSharedMemory(DeviceInputScalarSerialized* packet, uint32_t deviceIndex, const std::string& path) {
	uint32_t offset = this->getOffsetOfPath(path);
	if (offset == UINT32_MAX) return;
	
	constexpr uint32_t totalSize = sizeof(ObjectEntry) + sizeof(DeviceInputScalarSerialized);
	uint8_t buffer[totalSize];

	ObjectEntry* entry = reinterpret_cast<ObjectEntry*>(buffer);
	entry->alignmentCheck = ALIGNMENT_CONSTANT;
	entry->type = Object_InputScalar;
	entry->deviceIndex = deviceIndex;
	entry->inputPathOffset = offset;
	entry->version = this->driverClientLaneWriteCount;
	entry->valid = true;
	entry->committed.store(false, std::memory_order_relaxed);

	memcpy(buffer + sizeof(ObjectEntry), packet, sizeof(DeviceInputScalarSerialized));

	this->writePacketToDriverClientLane(buffer, totalSize);
}

void SharedDeviceMemoryDriver::syncDeviceInputSkeletonUpdateToSharedMemory(DeviceInputSkeletonSerialized* packet, uint32_t deviceIndex, const std::string& path) {
	uint32_t offset = this->getOffsetOfPath(path);
	if (offset == UINT32_MAX) return;
	
	constexpr uint32_t totalSize = sizeof(ObjectEntry) + sizeof(DeviceInputSkeletonSerialized);
	uint8_t buffer[totalSize];

	ObjectEntry* entry = reinterpret_cast<ObjectEntry*>(buffer);
	entry->alignmentCheck = ALIGNMENT_CONSTANT;
	entry->type = Object_InputSkeleton;
	entry->deviceIndex = deviceIndex;
	entry->inputPathOffset = offset;
	entry->version = this->driverClientLaneWriteCount;
	entry->valid = true;
	entry->committed.store(false, std::memory_order_relaxed);

	memcpy(buffer + sizeof(ObjectEntry), packet, sizeof(DeviceInputSkeletonSerialized));

	this->writePacketToDriverClientLane(buffer, totalSize);
}

void SharedDeviceMemoryDriver::syncDeviceInputPoseUpdateToSharedMemory(DeviceInputPoseSerialized* packet, uint32_t deviceIndex, const std::string& path) {
	uint32_t offset = this->getOffsetOfPath(path);
	if (offset == UINT32_MAX) return;
	
	constexpr uint32_t totalSize = sizeof(ObjectEntry) + sizeof(DeviceInputPoseSerialized);
	uint8_t buffer[totalSize];

	ObjectEntry* entry = reinterpret_cast<ObjectEntry*>(buffer);
	entry->alignmentCheck = ALIGNMENT_CONSTANT;
	entry->type = Object_InputPose;
	entry->deviceIndex = deviceIndex;
	entry->inputPathOffset = offset;
	entry->version = this->driverClientLaneWriteCount;
	entry->valid = true;
	entry->committed.store(false, std::memory_order_relaxed);

	memcpy(buffer + sizeof(ObjectEntry), packet, sizeof(DeviceInputPoseSerialized));

	this->writePacketToDriverClientLane(buffer, totalSize);
}

void SharedDeviceMemoryDriver::syncDeviceInputEyeTrackingUpdateToSharedMemory(DeviceInputEyeTrackingSerialized* packet, uint32_t deviceIndex, const std::string& path) {
	uint32_t offset = this->getOffsetOfPath(path);
	if (offset == UINT32_MAX) return;
	
	constexpr uint32_t totalSize = sizeof(ObjectEntry) + sizeof(DeviceInputEyeTrackingSerialized);
	uint8_t buffer[totalSize];

	ObjectEntry* entry = reinterpret_cast<ObjectEntry*>(buffer);
	entry->alignmentCheck = ALIGNMENT_CONSTANT;
	entry->type = Object_InputEyeTracking;
	entry->deviceIndex = deviceIndex;
	entry->inputPathOffset = offset;
	entry->version = this->driverClientLaneWriteCount;
	entry->valid = true;
	entry->committed.store(false, std::memory_order_relaxed);

	memcpy(buffer + sizeof(ObjectEntry), packet, sizeof(DeviceInputEyeTrackingSerialized));

	this->writePacketToDriverClientLane(buffer, totalSize);
}

bool SharedDeviceMemoryDriver::isValidCommandHeader(const ClientCommandHeader* header, const SharedMemoryHeader* sharedMemoryHeader) {
	if (header->alignmentCheck != ALIGNMENT_CONSTANT) {
		return false;
	}

	if (!(Command_SetUseOverridenStateDevicePose <= header->type && header->type <= Command_SetOverridenStateDeviceInputEyeTracking)) {
		return false;
	}

	if (!(0 <= header->deviceIndex && header->deviceIndex < vr::k_unMaxTrackedDeviceCount)) {
		return false;
	}

	//const int MAX_VERSION_DELTA = 1000;
	//const int difference = header->version - sharedMemoryHeader->clientDriverWriteCount;
	//if (abs(difference) > MAX_VERSION_DELTA) {
	//	return false;
	//}

	return true;
}