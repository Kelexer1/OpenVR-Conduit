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
	if (offset == UINT32_MAX) return std::string();

	uint8_t* pathTableStart = static_cast<uint8_t*>(this->sharedMemory) + this->pathTableStart;

	SharedMemoryHeader* headerPtr = reinterpret_cast<SharedMemoryHeader*>(this->sharedMemory);

	// Wait for offset to finish writing if required
	while (headerPtr->pathTableWritingOffset.load(std::memory_order_acquire) == offset) {}

	const char* path = reinterpret_cast<const char*>(pathTableStart + offset);

	uint32_t len;
	for (len = 0; len < MAX_PATH_LENGTH; len++) {
		if (path[len] == '\0') break;
	}

	if (len == MAX_PATH_LENGTH) return std::string();

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
				case Command_SetUseOverriddenStateDevicePose: {
						CommandParams_SetUseOverriddenStateDevicePose * params =
							reinterpret_cast<CommandParams_SetUseOverriddenStateDevicePose*>(paramsBuf.get());
						ModelDevicePoseSerialized* pose = model.getDevicePose(deviceIndex);
						if (pose) pose->useOverriddenState = params->useOverriddenState;

						break;
				}
				case Command_SetOverriddenStateDevicePose: {
					CommandParams_SetOverriddenStateDevicePose* params =
						reinterpret_cast<CommandParams_SetOverriddenStateDevicePose*>(paramsBuf.get());
					ModelDevicePoseSerialized* pose = model.getDevicePose(deviceIndex);
					if (pose) {
						pose->data.overwrittenPose = params->overriddenPose;
						model.setDevicePoseChanged(deviceIndex);
					}

					break;
				}
				case Command_SetUseOverriddenStateDeviceInput: {
					CommandParams_SetUseOverriddenStateDeviceInput* params = 
						reinterpret_cast<CommandParams_SetUseOverriddenStateDeviceInput*>(paramsBuf.get());
					std::string inputPath = this->getPathFromPathOffset(params->inputPathOffset);

					ModelDeviceInputBooleanSerialized* inputBoolean = model.getBooleanInput(deviceIndex, inputPath);
					if (inputBoolean) {
						inputBoolean->useOverriddenState = params->useOverriddenState;
						model.setInputBooleanChanged(deviceIndex, inputPath);
						break;
					}

					ModelDeviceInputScalarSerialized* inputScalar = model.getScalarInput(deviceIndex, inputPath);
					if (inputScalar) {
						inputScalar->useOverriddenState = params->useOverriddenState;
						model.setInputScalarChanged(deviceIndex, inputPath);
						break;
					}

					ModelDeviceInputSkeletonSerialized* inputSkeleton = model.getSkeletonInput(deviceIndex, inputPath);
					if (inputSkeleton) {
						inputSkeleton->useOverriddenState = params->useOverriddenState;
						model.setInputSkeletonChanged(deviceIndex, inputPath);
						break;
					}

					ModelDeviceInputPoseSerialized* inputPose = model.getPoseInput(deviceIndex, inputPath);
					if (inputPose) {
						inputPose->useOverriddenState = params->useOverriddenState;
						model.setInputPoseChanged(deviceIndex, inputPath);
						break;
					}

					ModelDeviceInputEyeTrackingSerialized* inputEyeTracking = model.getEyeTrackingInput(
						deviceIndex, 
						inputPath
					);
					if (inputEyeTracking) {
						inputEyeTracking->useOverriddenState = params->useOverriddenState;
						model.setInputEyeTrackingChanged(deviceIndex, inputPath);
						break;
					}

					break;
				}
				case Command_SetOverriddenStateDeviceInputBoolean: {
					CommandParams_SetOverriddenStateDeviceInputBoolean* params =
						reinterpret_cast<CommandParams_SetOverriddenStateDeviceInputBoolean*>(paramsBuf.get());
					std::string inputPath = this->getPathFromPathOffset(params->inputPathOffset);

					ModelDeviceInputBooleanSerialized* input = model.getBooleanInput(deviceIndex, inputPath);
					if (input) {
						input->data.overwrittenValue = params->overriddenValue;
						model.setInputBooleanChanged(deviceIndex, inputPath);
					}

					break;
				}
				case Command_SetOverriddenStateDeviceInputScalar: {
					CommandParams_SetOverriddenStateDeviceInputScalar* params =
						reinterpret_cast<CommandParams_SetOverriddenStateDeviceInputScalar*>(paramsBuf.get());
					std::string inputPath = this->getPathFromPathOffset(params->inputPathOffset);

					ModelDeviceInputScalarSerialized* input = model.getScalarInput(deviceIndex, inputPath);
					if (input) { 
						input->data.overwrittenValue = params->overriddenValue;
						model.setInputScalarChanged(deviceIndex, inputPath);
					}

					break;
				}
				case Command_SetOverriddenStateDeviceInputSkeleton: {
					CommandParams_SetOverriddenStateDeviceInputSkeleton* params = reinterpret_cast<CommandParams_SetOverriddenStateDeviceInputSkeleton*>(paramsBuf.get());
					std::string inputPath = this->getPathFromPathOffset(params->inputPathOffset);

					ModelDeviceInputSkeletonSerialized* input = model.getSkeletonInput(deviceIndex, inputPath);
					if (input) { 
						input->data.overwrittenValue = params->overriddenValue;
						model.setInputSkeletonChanged(deviceIndex, inputPath);
					}

					break;
				}
				case Command_SetOverriddenStateDeviceInputPose: {
					CommandParams_SetOverriddenStateDeviceInputPose* params = reinterpret_cast<CommandParams_SetOverriddenStateDeviceInputPose*>(paramsBuf.get());
					std::string inputPath = this->getPathFromPathOffset(params->inputPathOffset);

					ModelDeviceInputPoseSerialized* input = model.getPoseInput(deviceIndex, inputPath);
					if (input) { 
						input->data.overwrittenValue = params->overriddenValue;
						model.setInputPoseChanged(deviceIndex, inputPath);
					}

					break;
				}
				case Command_SetOverriddenStateDeviceInputEyeTracking: {
					CommandParams_SetOverriddenStateDeviceInputEyeTracking* params = reinterpret_cast<CommandParams_SetOverriddenStateDeviceInputEyeTracking*>(paramsBuf.get());
					std::string inputPath = this->getPathFromPathOffset(params->inputPathOffset);

					ModelDeviceInputEyeTrackingSerialized* input = model.getEyeTrackingInput(deviceIndex, inputPath);
					if (input) {
						input->data.overwrittenValue = params->overriddenValue;
						model.setInputEyeTrackingChanged(deviceIndex, inputPath);
					}

					break;
				}
			}

			this->clientDriverLaneReadCount = commandHeader.version;
		} while (this->clientDriverLaneReadCount < currentWriteCount);

		this->clientDriverLaneReadCount = currentWriteCount;
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
 
std::pair<ClientCommandHeaderData, std::pair<ClientCommandType, std::unique_ptr<uint8_t[]>>> 
SharedDeviceMemoryDriver::readPacketFromClientDriverLane() {
	SharedMemoryHeader* headerPtr = reinterpret_cast<SharedMemoryHeader*>(this->sharedMemory);

	if (this->clientDriverLaneReadOffset >= LANE_SIZE - LANE_PADDING_SIZE) this->clientDriverLaneReadOffset = 0;

	uint32_t writeOffset = headerPtr->clientDriverWriteOffset.load(std::memory_order_acquire);
	if (this->clientDriverLaneReadOffset == writeOffset) 
		return { ClientCommandHeaderData{}, { Command_SetOverriddenStateDevicePose, nullptr } };

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
				LogManager::log(LOG_DEBUG, "Packet misaligned, forward search {} bytes", i);
				break;
			}
		}

		// Strategy 2: Jump To Write Offset
		if (!recovered) {
			this->clientDriverLaneReadOffset = headerPtr->clientDriverWriteOffset.load(std::memory_order_acquire);
			this->clientDriverLaneReadCount = headerPtr->clientDriverWriteCount.load(std::memory_order_acquire);
			LogManager::log(LOG_DEBUG, "Packet misaligned, jumped to write header");
			return { ClientCommandHeaderData{}, { Command_SetUseOverriddenStateDevicePose , nullptr } };
		}
	}

	while (!rawHeader->committed.load(std::memory_order_acquire)) {} // Wait for packet to be valid

	LogManager::log(LOG_DEBUG, "Read a packet for index {}: {}", rawHeader->deviceIndex, (int)rawHeader->type);

	ClientCommandHeaderData header;
	header.successful = true;
	header.type = rawHeader->type;
	header.deviceIndex = rawHeader->deviceIndex;
	header.version = rawHeader->version;

	// Read object data
	uint32_t dataSize = 0;
	ClientCommandType type = header.type;
	switch (type) {
	case Command_SetUseOverriddenStateDevicePose:
		dataSize = sizeof(CommandParams_SetUseOverriddenStateDevicePose); break;
	case Command_SetOverriddenStateDevicePose:
		dataSize = sizeof(CommandParams_SetOverriddenStateDevicePose); break;
	case Command_SetUseOverriddenStateDeviceInput:
		dataSize = sizeof(CommandParams_SetUseOverriddenStateDeviceInput); break;
	case Command_SetOverriddenStateDeviceInputBoolean:
		dataSize = sizeof(CommandParams_SetOverriddenStateDeviceInputBoolean); break;
	case Command_SetOverriddenStateDeviceInputScalar:
		dataSize = sizeof(CommandParams_SetOverriddenStateDeviceInputScalar); break;
	case Command_SetOverriddenStateDeviceInputSkeleton:
		dataSize = sizeof(CommandParams_SetOverriddenStateDeviceInputSkeleton); break;
	case Command_SetOverriddenStateDeviceInputPose:
		dataSize = sizeof(CommandParams_SetOverriddenStateDeviceInputPose); break;
	case Command_SetOverriddenStateDeviceInputEyeTracking:
		dataSize = sizeof(CommandParams_SetOverriddenStateDeviceInputEyeTracking); break;
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

void SharedDeviceMemoryDriver::syncDeviceInputBooleanUpdateToSharedMemory(
	DeviceInputBooleanSerialized* packet,
	uint32_t deviceIndex,
	const std::string& path
) {
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

void SharedDeviceMemoryDriver::syncDeviceInputScalarUpdateToSharedMemory(
	DeviceInputScalarSerialized* packet,
	uint32_t deviceIndex,
	const std::string& path
) {
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

void SharedDeviceMemoryDriver::syncDeviceInputSkeletonUpdateToSharedMemory(
	DeviceInputSkeletonSerialized* packet,
	uint32_t deviceIndex,
	const std::string& path
) {
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

void SharedDeviceMemoryDriver::syncDeviceInputPoseUpdateToSharedMemory(
	DeviceInputPoseSerialized* packet,
	uint32_t deviceIndex,
	const std::string& path
) {
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

void SharedDeviceMemoryDriver::syncDeviceInputEyeTrackingUpdateToSharedMemory(
	DeviceInputEyeTrackingSerialized* packet,
	uint32_t deviceIndex,
	const std::string& path
) {
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

bool SharedDeviceMemoryDriver::isValidCommandHeader(
	const ClientCommandHeader* header,
	const SharedMemoryHeader* sharedMemoryHeader
) {
	if (header->alignmentCheck != ALIGNMENT_CONSTANT) return false;

	if (!(Command_SetUseOverriddenStateDevicePose <= header->type && 
		header->type <= Command_SetOverriddenStateDeviceInputEyeTracking
	)) return false;

	if (!(0 <= header->deviceIndex && header->deviceIndex < vr::k_unMaxTrackedDeviceCount)) return false;

	return true;
}