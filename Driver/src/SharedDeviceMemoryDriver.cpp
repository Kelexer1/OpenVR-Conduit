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

uint32_t SharedDeviceMemoryDriver::getOffsetOfPath(const std::string& path) {
	SharedMemoryHeader* headerPtr = reinterpret_cast<SharedMemoryHeader*>(this->sharedMemory);

	auto foundOffet = this->pathTableOffsets.find(path);
	if (foundOffet != this->pathTableOffsets.end()) {
		headerPtr->pathTableWritingOffset.store(UINT32_MAX, std::memory_order_release);
		return foundOffet->second;
	}

	uint32_t offset = this->currentPathTableWriteOffset;
	uint32_t writeSize = path.length() + 1;

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
	//SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);

	//if (this->clientDriverReadCount < headerPtr->clientDriverWriteCount) {
	//	ClientCommandHeader* commandHeader;
	//	do {
	//		// Read command header
	//		auto headerBuf = this->readPacketFromClientDriverLane(sizeof(ClientCommandHeader));
	//		commandHeader = reinterpret_cast<ClientCommandHeader*>(headerBuf.get());

	//		if (!this->isValidCommandHeader(commandHeader, headerPtr)) {
	//			LogManager::log(LOG_ERROR, "Alignment error detected in client driver lane at offset {}", this->clientDriverLaneReadOffset);

	//			bool recovered = false;
	//			uint32_t searchOffset = this->clientDriverLaneReadOffset;

	//			const uint32_t FORWARD_SEARCH_BYTES = 4096;

	//			// Strategy 1: Forward Check
	//			for (uint32_t i = 0; i < FORWARD_SEARCH_BYTES; i++) {
	//				searchOffset = (searchOffset + 1) % LANE_SIZE;

	//				uint32_t tempOffset = this->clientDriverLaneReadOffset;
	//				this->clientDriverLaneReadOffset = searchOffset;
	//				auto testBuf = this->readPacketFromClientDriverLane(sizeof(ClientCommandHeader));
	//				ClientCommandHeader* testHeader = reinterpret_cast<ClientCommandHeader*>(testBuf.get());

	//				if (this->isValidCommandHeader(testHeader, headerPtr)) {
	//					LogManager::log(LOG_INFO, "Recovered alignment at offset {} (skipped {} bytes)", searchOffset, i);
	//					recovered = true;
	//					break;
	//				}

	//				this->clientDriverLaneReadOffset = tempOffset;
	//			}
	//			
	//			// Strategy 2: Jump To Write Offset
	//			if (!recovered) {
	//				LogManager::log(LOG_ERROR, "Could not recover alignment after searching +{} bytes, discarded queued commands", FORWARD_SEARCH_BYTES);
	//				this->clientDriverLaneReadOffset = headerPtr->clientDriverWriteOffset;
	//				this->clientDriverReadCount = headerPtr->clientDriverWriteCount;
	//			}

	//			headerBuf = this->readPacketFromClientDriverLane(sizeof(ClientCommandHeader));
	//			commandHeader = reinterpret_cast<ClientCommandHeader*>(headerBuf.get());
	//		}

	//		uint32_t deviceIndex = commandHeader->deviceIndex;

	//		DeviceStateModel& model = DeviceStateModel::getInstance();

	//		switch (commandHeader->type) {
	//			case Command_SetUseOverridenStateDevicePose: {
	//				auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetUseOverridenStateDevicePose));
	//				CommandParams_SetUseOverridenStateDevicePose* params = reinterpret_cast<CommandParams_SetUseOverridenStateDevicePose*>(paramsBuf.get());

	//				ModelDevicePoseSerialized* pose = model.getDevicePose(deviceIndex);
	//				if (pose) {
	//					pose->useOverridenState = params->useOverridenState;
	//				}

	//				break;
	//			}
	//			case Command_SetOverridenStateDevicePose: {
	//				auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetOverridenStateDevicePose));
	//				CommandParams_SetOverridenStateDevicePose* params = reinterpret_cast<CommandParams_SetOverridenStateDevicePose*>(paramsBuf.get());

	//				ModelDevicePoseSerialized* pose = model.getDevicePose(deviceIndex);
	//				if (pose) {
	//					pose->data.overwrittenPose = params->overridenPose;
	//					model.setDevicePoseChanged(deviceIndex);
	//				}

	//				break;
	//			}
	//			case Command_SetUseOverridenStateDeviceInput: {
	//				auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetUseOverridenStateDeviceInput));
	//				CommandParams_SetUseOverridenStateDeviceInput* params = reinterpret_cast<CommandParams_SetUseOverridenStateDeviceInput*>(paramsBuf.get());

	//				ModelDeviceInputBooleanSerialized* inputBoolean = model.getBooleanInput(deviceIndex, params->inputPath);
	//				if (inputBoolean) {
	//					inputBoolean->useOverridenState = params->useOverridenState;
	//					model.setInputBooleanChanged(deviceIndex, params->inputPath);
	//				}

	//				ModelDeviceInputScalarSerialized* inputScalar = model.getScalarInput(deviceIndex, params->inputPath);
	//				if (inputScalar) {
	//					inputScalar->useOverridenState = params->useOverridenState;
	//					model.setInputScalarChanged(deviceIndex, params->inputPath);
	//				}

	//				ModelDeviceInputSkeletonSerialized* inputSkeleton = model.getSkeletonInput(deviceIndex, params->inputPath);
	//				if (inputSkeleton) {
	//					inputSkeleton->useOverridenState = params->useOverridenState;
	//					model.setInputSkeletonChanged(deviceIndex, params->inputPath);
	//				}

	//				ModelDeviceInputPoseSerialized* inputPose = model.getPoseInput(deviceIndex, params->inputPath);
	//				if (inputPose) {
	//					inputPose->useOverridenState = params->useOverridenState;
	//					model.setInputPoseChanged(deviceIndex, params->inputPath);
	//				}

	//				ModelDeviceInputEyeTrackingSerialized* inputEyeTracking = model.getEyeTrackingInput(deviceIndex, params->inputPath);
	//				if (inputEyeTracking) {
	//					inputEyeTracking->useOverridenState = params->useOverridenState;
	//					model.setInputEyeTrackingChanged(deviceIndex, params->inputPath);
	//				}
	//					
	//				break;
	//			}
	//			case Command_SetOverridenStateDeviceInputBoolean: {
	//				auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetOverridenStateDeviceInputBoolean));
	//				CommandParams_SetOverridenStateDeviceInputBoolean* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputBoolean*>(paramsBuf.get());

	//				ModelDeviceInputBooleanSerialized* input = model.getBooleanInput(deviceIndex, params->inputPath);
	//				if (input) {
	//					input->data.overwrittenValue = params->overridenValue;
	//				}

	//				break;
	//			}
	//			case Command_SetOverridenStateDeviceInputScalar: {
	//				auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetOverridenStateDeviceInputScalar));
	//				CommandParams_SetOverridenStateDeviceInputScalar* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputScalar*>(paramsBuf.get());

	//				ModelDeviceInputScalarSerialized* input = model.getScalarInput(deviceIndex, params->inputPath);
	//				if (input) {
	//					input->data.overwrittenValue = params->overridenValue;
	//				}

	//				break;
	//			}
	//			case Command_SetOverridenStateDeviceInputSkeleton: {
	//				auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetOverridenStateDeviceInputSkeleton));
	//				CommandParams_SetOverridenStateDeviceInputSkeleton* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputSkeleton*>(paramsBuf.get());

	//				ModelDeviceInputSkeletonSerialized* input = model.getSkeletonInput(deviceIndex, params->inputPath);
	//				if (input) {
	//					input->data.overwrittenValue = params->overridenValue;
	//				}

	//				break;
	//			}
	//			case Command_SetOverridenStateDeviceInputPose: {
	//				auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetOverridenStateDeviceInputPose));
	//				CommandParams_SetOverridenStateDeviceInputPose* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputPose*>(paramsBuf.get());

	//				ModelDeviceInputPoseSerialized* input = model.getPoseInput(deviceIndex, params->inputPath);
	//				if (input) {
	//					input->data.overwrittenValue = params->overridenValue;
	//				}

	//				break;
	//			}
	//			case Command_SetOverridenStateDeviceInputEyeTracking: {
	//				auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetOverridenStateDeviceInputEyeTracking));
	//				CommandParams_SetOverridenStateDeviceInputEyeTracking* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputEyeTracking*>(paramsBuf.get());

	//				ModelDeviceInputEyeTrackingSerialized* input = model.getEyeTrackingInput(deviceIndex, params->inputPath);
	//				if (input) {
	//					input->data.overwrittenValue = params->overridenValue;
	//				}

	//				break;
	//			}
	//		}
	//	} while (commandHeader->version < this->clientDriverReadCount);

	//	this->clientDriverReadCount = headerPtr->clientDriverWriteCount;
	//}
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
 
std::unique_ptr<uint8_t[]> SharedDeviceMemoryDriver::readPacketFromClientDriverLane(uint32_t packetSize) {
	if (packetSize == 0) {
		return nullptr;
	}

	auto buffer = std::make_unique<uint8_t[]>(packetSize);
	if (!buffer) {
		return nullptr;
	}

	uint8_t* laneStart = static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart;
	uint32_t currentOffset = this->clientDriverLaneReadOffset;

	// Handle wrap around
	if (currentOffset + packetSize > LANE_SIZE) {
		uint32_t firstPartitionSize = LANE_SIZE - currentOffset;
		memcpy(buffer.get(), laneStart + currentOffset, firstPartitionSize);

		uint32_t secondPartitionSize = packetSize - firstPartitionSize;
		memcpy(buffer.get() + firstPartitionSize, laneStart, secondPartitionSize);

		this->clientDriverLaneReadOffset = secondPartitionSize;
	} else {
		memcpy(buffer.get(), laneStart + currentOffset, packetSize);
		this->clientDriverLaneReadOffset = (currentOffset + packetSize) % LANE_SIZE;
	}

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	headerPtr->clientDriverReadOffset = this->clientDriverLaneReadOffset;

	return buffer;
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

	if (!(0 <= header->deviceIndex && header->deviceIndex < 128)) {
		return false;
	}

	//const int MAX_VERSION_DELTA = 1000;
	//const int difference = header->version - sharedMemoryHeader->clientDriverWriteCount;
	//if (abs(difference) > MAX_VERSION_DELTA) {
	//	return false;
	//}

	return true;
}