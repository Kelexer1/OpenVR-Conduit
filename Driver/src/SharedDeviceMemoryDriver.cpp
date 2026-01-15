#include "SharedDeviceMemoryDriver.h"

const wchar_t* SHM_NAME = L"Local\\ConduitSharedDeviceMemory";
const uint32_t PATH_TABLE_ENTRIES = 256;
const size_t LANE_SIZE = 1 * 1048576; // 1mb
const size_t SHARED_MEMORY_SIZE = sizeof(SharedMemoryHeader) + PATH_TABLE_ENTRIES * sizeof(PathTableEntry) + 2 * LANE_SIZE;
const uint32_t PROTOCOL_VERSION = 0;

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

	if (!this->initializeSharedMemoryData()) {
		LogManager::log(LOG_ERROR, "Failed to initialize shared memory data: {}", GetLastError());
		return false;
	}

	return true;
}

bool SharedDeviceMemoryDriver::initializeSharedMemoryData() {
	// Init header
	SharedMemoryHeader header;

	header.protocolVersion = PROTOCOL_VERSION;

	int currentOffset = sizeof(SharedMemoryHeader);

	header.pathTableStart = this->pathTableStart = currentOffset;
	header.pathTableSize = this->pathTableSize = PATH_TABLE_ENTRIES * sizeof(PathTableEntry);
	header.pathTableEntries = PATH_TABLE_ENTRIES;
	header.pathTableWriteOffset = 0;

	currentOffset += header.pathTableSize;

	header.driverClientLaneStart = this->driverClientLaneStart = currentOffset;
	header.driverClientLaneSize = LANE_SIZE;
	header.driverClientWriteOffset = 0;
	
	currentOffset += LANE_SIZE;

	header.clientDriverLaneStart = currentOffset;
	header.clientDriverLaneSize = LANE_SIZE;
	header.clientDriverWriteOffset = 0;

	memcpy(this->sharedMemory, &header, sizeof(SharedMemoryHeader));

	return true;
}

void SharedDeviceMemoryDriver::pollForClientUpdates() {
	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);

	if (this->clientDriverReadCount < headerPtr->clientDriverWriteCount) {
		ClientCommandHeader* commandHeader;
		do {
			// Read command header
			auto headerBuf = this->readPacketFromClientDriverLane(sizeof(ClientCommandHeader));
			commandHeader = reinterpret_cast<ClientCommandHeader*>(headerBuf.get());

			if (!this->isValidCommandHeader(commandHeader, headerPtr)) {
				LogManager::log(LOG_ERROR, "Alignment error detected in client driver lane at offset {}", this->clientDriverLaneReadOffset);

				bool recovered = false;
				size_t searchOffset = this->clientDriverLaneReadOffset;

				const size_t FORWARD_SEARCH_BYTES = 4096;
				const size_t BACKWARD_SEARCH_BYTES = 512;

				// Strategy 1: Forward Check
				for (size_t i = 0; i < FORWARD_SEARCH_BYTES; i++) {
					searchOffset = (searchOffset + 1) % LANE_SIZE;

					size_t tempOffset = this->clientDriverLaneReadOffset;
					this->clientDriverLaneReadOffset = searchOffset;
					auto testBuf = this->readPacketFromClientDriverLane(sizeof(ClientCommandHeader));
					ClientCommandHeader* testHeader = reinterpret_cast<ClientCommandHeader*>(testBuf.get());

					if (this->isValidCommandHeader(testHeader, headerPtr)) {
						LogManager::log(LOG_INFO, "Recovered alignment at offset {} (skipped {} bytes)", searchOffset, i);
						recovered = true;
						break;
					}

					this->clientDriverLaneReadOffset = tempOffset;
				}

				// Strategy 2: Backward Check
				if (!recovered) {
					for (size_t i = 0; i < FORWARD_SEARCH_BYTES; i++) {
						searchOffset = (searchOffset - 1) % LANE_SIZE;

						size_t tempOffset = this->clientDriverLaneReadOffset;
						this->clientDriverLaneReadOffset = searchOffset;
						auto testBuf = this->readPacketFromClientDriverLane(sizeof(ClientCommandHeader));
						ClientCommandHeader* testHeader = reinterpret_cast<ClientCommandHeader*>(testBuf.get());

						if (this->isValidCommandHeader(testHeader, headerPtr)) {
							LogManager::log(LOG_INFO, "Recovered alignment at offset {} (skipped -{} bytes)", searchOffset, i);
							recovered = true;
							break;
						}

						this->clientDriverLaneReadOffset = tempOffset;
					}
				}
				
				// Strategy 3: Jump To Write Offset
				if (!recovered) {
					LogManager::log(LOG_ERROR, "Could not recover alignment after searching +{} / -{} bytes, discarded queued commands", FORWARD_SEARCH_BYTES, BACKWARD_SEARCH_BYTES);
					this->clientDriverLaneReadOffset = headerPtr->clientDriverWriteOffset;
					this->clientDriverReadCount = headerPtr->clientDriverWriteCount;
				}

				headerBuf = this->readPacketFromClientDriverLane(sizeof(ClientCommandHeader));
				commandHeader = reinterpret_cast<ClientCommandHeader*>(headerBuf.get());
			}

			uint32_t deviceIndex = commandHeader->deviceIndex;

			DeviceStateModel model = DeviceStateModel::getInstance();

			switch (commandHeader->type) {
				case Command_SetUseOverridenStateDevicePose: {
					auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetUseOverridenStateDevicePose));
					CommandParams_SetUseOverridenStateDevicePose* params = reinterpret_cast<CommandParams_SetUseOverridenStateDevicePose*>(paramsBuf.get());

					ModelDevicePoseSerialized* pose = model.getDevicePose(deviceIndex);
					if (pose) {
						pose->useOverridenState = params->useOverridenState;
					}

					break;
				}
				case Command_SetOverridenStateDevicePose: {
					auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetOverridenStateDevicePose));
					CommandParams_SetOverridenStateDevicePose* params = reinterpret_cast<CommandParams_SetOverridenStateDevicePose*>(paramsBuf.get());

					ModelDevicePoseSerialized* pose = model.getDevicePose(deviceIndex);
					if (pose) {
						pose->data.overwrittenPose = params->overridenPose;
						model.setDevicePoseChanged(deviceIndex);
					}

					break;
				}
				case Command_SetUseOverridenStateDeviceInput: {
					auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetUseOverridenStateDeviceInput));
					CommandParams_SetUseOverridenStateDeviceInput* params = reinterpret_cast<CommandParams_SetUseOverridenStateDeviceInput*>(paramsBuf.get());

					ModelDeviceInputBooleanSerialized* inputBoolean = model.getBooleanInput(deviceIndex, params->pathID);
					if (inputBoolean) {
						inputBoolean->useOverridenState = params->useOverridenState;
						model.setInputBooleanChanged(deviceIndex, params->pathID);
					}

					ModelDeviceInputScalarSerialized* inputScalar = model.getScalarInput(deviceIndex, params->pathID);
					if (inputScalar) {
						inputScalar->useOverridenState = params->useOverridenState;
						model.setInputScalarChanged(deviceIndex, params->pathID);
					}

					ModelDeviceInputSkeletonSerialized* inputSkeleton = model.getSkeletonInput(deviceIndex, params->pathID);
					if (inputSkeleton) {
						inputSkeleton->useOverridenState = params->useOverridenState;
						model.setInputSkeletonChanged(deviceIndex, params->pathID);
					}

					ModelDeviceInputPoseSerialized* inputPose = model.getPoseInput(deviceIndex, params->pathID);
					if (inputPose) {
						inputPose->useOverridenState = params->useOverridenState;
						model.setInputPoseChanged(deviceIndex, params->pathID);
					}

					ModelDeviceInputEyeTrackingSerialized* inputEyeTracking = model.getEyeTrackingInput(deviceIndex, params->pathID);
					if (inputEyeTracking) {
						inputEyeTracking->useOverridenState = params->useOverridenState;
						model.setInputEyeTrackingChanged(deviceIndex, params->pathID);
					}
						
					break;
				}
				case Command_SetOverridenStateDeviceInputBoolean: {
					auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetOverridenStateDeviceInputBoolean));
					CommandParams_SetOverridenStateDeviceInputBoolean* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputBoolean*>(paramsBuf.get());

					ModelDeviceInputBooleanSerialized* input = model.getBooleanInput(deviceIndex, params->pathID);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
				case Command_SetOverridenStateDeviceInputScalar: {
					auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetOverridenStateDeviceInputScalar));
					CommandParams_SetOverridenStateDeviceInputScalar* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputScalar*>(paramsBuf.get());

					ModelDeviceInputScalarSerialized* input = model.getScalarInput(deviceIndex, params->pathID);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
				case Command_SetOverridenStateDeviceInputSkeleton: {
					auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetOverridenStateDeviceInputSkeleton));
					CommandParams_SetOverridenStateDeviceInputSkeleton* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputSkeleton*>(paramsBuf.get());

					ModelDeviceInputSkeletonSerialized* input = model.getSkeletonInput(deviceIndex, params->pathID);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
				case Command_SetOverridenStateDeviceInputPose: {
					auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetOverridenStateDeviceInputPose));
					CommandParams_SetOverridenStateDeviceInputPose* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputPose*>(paramsBuf.get());

					ModelDeviceInputPoseSerialized* input = model.getPoseInput(deviceIndex, params->pathID);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
				case Command_SetOverridenStateDeviceInputEyeTracking: {
					auto paramsBuf = this->readPacketFromClientDriverLane(sizeof(CommandParams_SetOverridenStateDeviceInputEyeTracking));
					CommandParams_SetOverridenStateDeviceInputEyeTracking* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputEyeTracking*>(paramsBuf.get());

					ModelDeviceInputEyeTrackingSerialized* input = model.getEyeTrackingInput(deviceIndex, params->pathID);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
			}
		} while (commandHeader->version < this->clientDriverReadCount);

		this->clientDriverReadCount = headerPtr->clientDriverWriteCount;
	}
}

void SharedDeviceMemoryDriver::writePacketToDriverClientLane(void* packet, uint32_t packetSize) {
	if (packet != nullptr && packetSize > 0) {
		uint8_t* laneStart = static_cast<uint8_t*>(this->sharedMemory) + this->driverClientLaneStart;
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
	}
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
	size_t currentOffset = this->clientDriverLaneReadOffset;

	// Handle wrap around
	if (currentOffset + packetSize > LANE_SIZE) {
		size_t firstPartitionSize = LANE_SIZE - currentOffset;
		memcpy(buffer.get(), laneStart + currentOffset, firstPartitionSize);

		size_t secondPartitionSize = packetSize - firstPartitionSize;
		memcpy(buffer.get() + firstPartitionSize, laneStart, secondPartitionSize);

		this->clientDriverLaneReadOffset = secondPartitionSize;
	} else {
		memcpy(buffer.get(), laneStart + currentOffset, packetSize);
		this->clientDriverLaneReadOffset = (currentOffset + packetSize) % LANE_SIZE;
	}

	return buffer;
}

void SharedDeviceMemoryDriver::syncPathTableToSharedMemory(uint32_t pathID, const std::string& path) {
	PathTableEntry entry = {};
	entry.ID = pathID;
	entry.version = this->pathTableWriteCount;
	strncpy_s(entry.path, path.c_str(), sizeof(entry.path) - 1);

	// Write data starting at current index, should never have to loop around mid write
	memcpy(static_cast<uint8_t*>(this->sharedMemory) + this->pathTableStart + this->pathTableWriteIndex * sizeof(PathTableEntry), &entry, sizeof(PathTableEntry));
	
	// Update write count in shared memory header
	this->pathTableWriteIndex = (this->pathTableWriteIndex + 1) % PATH_TABLE_ENTRIES;
	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	headerPtr->pathTableWriteCount++;
	headerPtr->pathTableWriteOffset = this->pathTableWriteIndex * sizeof(PathTableEntry);
}

void SharedDeviceMemoryDriver::syncDevicePoseUpdateToSharedMemory(DevicePoseSerialized* packet, uint32_t deviceIndex) {
	ObjectEntry entry = {};
	entry.type = Object_DevicePose;
	entry.deviceIndex = deviceIndex;
	entry.inputPathID = static_cast<uint32_t>(-1);
	entry.version = this->pathTableWriteCount;
	entry.valid = true;
	this->writePacketToDriverClientLane((void*)&entry, sizeof(ObjectEntry));
	this->writePacketToDriverClientLane((void*)packet, sizeof(DevicePoseSerialized));

	ObjectEntry zero = {};
	this->writePacketToDriverClientLane((void*)&zero, sizeof(ObjectEntry));

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	this->driverClientLaneWriteCount++;
	headerPtr->driverClientWriteCount++;
	headerPtr->driverClientWriteOffset = this->driverClientLaneWriteOffset;
}

void SharedDeviceMemoryDriver::syncDeviceInputBooleanUpdateToSharedMemory(DeviceInputBooleanSerialized* packet, uint32_t deviceIndex, uint32_t pathID) {
	ObjectEntry entry = {};
	entry.type = Object_InputBoolean;
	entry.deviceIndex = deviceIndex;
	entry.inputPathID = pathID;
	entry.version = this->pathTableWriteCount;
	entry.valid = true;
	this->writePacketToDriverClientLane((void*)&entry, sizeof(ObjectEntry));
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputBooleanSerialized));

	ObjectEntry zero = {};
	this->writePacketToDriverClientLane((void*)&zero, sizeof(ObjectEntry));

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	this->driverClientLaneWriteCount++;
	headerPtr->driverClientWriteCount++;
	headerPtr->driverClientWriteOffset = this->driverClientLaneWriteOffset;
}

void SharedDeviceMemoryDriver::syncDeviceInputScalarUpdateToSharedMemory(DeviceInputScalarSerialized* packet, uint32_t deviceIndex, uint32_t pathID) {
	ObjectEntry entry = {};
	entry.type = Object_InputScalar;
	entry.deviceIndex = deviceIndex;
	entry.inputPathID = pathID;
	entry.version = this->pathTableWriteCount;
	entry.valid = true;
	this->writePacketToDriverClientLane((void*)&entry, sizeof(ObjectEntry));
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputScalarSerialized));

	ObjectEntry zero = {};
	this->writePacketToDriverClientLane((void*)&zero, sizeof(ObjectEntry));

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	this->driverClientLaneWriteCount++;
	headerPtr->driverClientWriteCount++;
	headerPtr->driverClientWriteOffset = this->driverClientLaneWriteOffset;
}

void SharedDeviceMemoryDriver::syncDeviceInputSkeletonUpdateToSharedMemory(DeviceInputSkeletonSerialized* packet, uint32_t deviceIndex, uint32_t pathID) {
	ObjectEntry entry = {};
	entry.type = Object_InputSkeleton;
	entry.deviceIndex = deviceIndex;
	entry.inputPathID = pathID;
	entry.version = this->pathTableWriteCount;
	entry.valid = true;
	this->writePacketToDriverClientLane((void*)&entry, sizeof(ObjectEntry));
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputSkeletonSerialized));

	ObjectEntry zero = {};
	this->writePacketToDriverClientLane((void*)&zero, sizeof(ObjectEntry));

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	this->driverClientLaneWriteCount++;
	headerPtr->driverClientWriteCount++;
	headerPtr->driverClientWriteOffset = this->driverClientLaneWriteOffset;
}

void SharedDeviceMemoryDriver::syncDeviceInputPoseUpdateToSharedMemory(DeviceInputPoseSerialized* packet, uint32_t deviceIndex, uint32_t pathID) {
	ObjectEntry entry = {};
	entry.type = Object_InputPose;
	entry.deviceIndex = deviceIndex;
	entry.inputPathID = pathID;
	entry.version = this->pathTableWriteCount;
	entry.valid = true;
	this->writePacketToDriverClientLane((void*)&entry, sizeof(ObjectEntry));
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputPoseSerialized));

	ObjectEntry zero = {};
	this->writePacketToDriverClientLane((void*)&zero, sizeof(ObjectEntry));

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	this->driverClientLaneWriteCount++;
	headerPtr->driverClientWriteCount++;
	headerPtr->driverClientWriteOffset = this->driverClientLaneWriteOffset;
}

void SharedDeviceMemoryDriver::syncDeviceInputEyeTrackingUpdateToSharedMemory(DeviceInputEyeTrackingSerialized* packet, uint32_t deviceIndex, uint32_t pathID) {
	ObjectEntry entry = {};
	entry.type = Object_InputEyeTracking;
	entry.deviceIndex = deviceIndex;
	entry.inputPathID = pathID;
	entry.version = this->pathTableWriteCount;
	entry.valid = true;
	this->writePacketToDriverClientLane((void*)&entry, sizeof(ObjectEntry));
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputEyeTrackingSerialized));

	ObjectEntry zero = {};
	this->writePacketToDriverClientLane((void*)&zero, sizeof(ObjectEntry));

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	this->driverClientLaneWriteCount++;
	headerPtr->driverClientWriteCount++;
	headerPtr->driverClientWriteOffset = this->driverClientLaneWriteOffset;
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

	const int MAX_VERSION_DELTA = 1000;
	const int difference = header->version - sharedMemoryHeader->clientDriverWriteCount;
	if (abs(difference) > MAX_VERSION_DELTA) {
		return false;
	}

	return true;
}