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
	header.pathTableEntrySize = sizeof(PathTableEntry);

	currentOffset += header.pathTableSize;

	header.driverClientLaneStart = this->driverClientLaneStart = currentOffset;
	header.driverClientLaneSize = LANE_SIZE;
	
	currentOffset += LANE_SIZE;

	header.clientDriverLaneStart = currentOffset;
	header.clientDriverLaneSize = LANE_SIZE;

	memcpy(this->sharedMemory, &header, sizeof(SharedMemoryHeader));

	return true;
}

void SharedDeviceMemoryDriver::pollForClientUpdates() {
	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);

	if (this->clientDriverReadCount < headerPtr->clientDriverWriteCount) {
		ClientCommandHeader* commandHeader;
		do {
			// Read command header
			commandHeader = reinterpret_cast<ClientCommandHeader*>(static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart + this->clientDriverLaneReadOffset);
			this->clientDriverLaneReadOffset += sizeof(ClientCommandHeader);

			uint32_t deviceIndex = commandHeader->deviceIndex;

			DeviceStateModel model = DeviceStateModel::getInstance();

			switch (commandHeader->commandType) {
				case Command_SetUseOverridenStateDevicePose: {
					CommandParams_SetUseOverridenStateDevicePose* params = reinterpret_cast<CommandParams_SetUseOverridenStateDevicePose*>(
						static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart + this->clientDriverLaneReadOffset
						);
					this->clientDriverLaneReadOffset += sizeof(CommandParams_SetUseOverridenStateDevicePose);

					DriverDevicePoseSerialized* pose = model.getDevicePose(deviceIndex);
					if (pose) {
						pose->useOverridenState = params->useOverridenState;
					}

					break;
				}
				case Command_SetOverridenStateDevicePose: {
					CommandParams_SetOverridenStateDevicePose* params = reinterpret_cast<CommandParams_SetOverridenStateDevicePose*>(
						static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart + this->clientDriverLaneReadOffset
						);
					this->clientDriverLaneReadOffset += sizeof(CommandParams_SetOverridenStateDevicePose);

					DriverDevicePoseSerialized* pose = model.getDevicePose(deviceIndex);
					if (pose) {
						pose->data.overwrittenPose = params->overridenPose.overwrittenPose;
						model.setDevicePoseChanged(deviceIndex);
					}

					break;
				}
				case Command_SetUseOverridenStateDeviceInput: {
					CommandParams_SetUseOverridenStateDeviceInput* params = reinterpret_cast<CommandParams_SetUseOverridenStateDeviceInput*>(
						static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart + this->clientDriverLaneReadOffset
						);
					this->clientDriverLaneReadOffset += sizeof(CommandParams_SetUseOverridenStateDeviceInput);

					DriverDeviceInputBooleanSerialized* inputBoolean = model.getBooleanInput(deviceIndex, params->pathID);
					if (inputBoolean) {
						inputBoolean->useOverridenState = params->useOverridenState;
						model.setInputBooleanChanged(deviceIndex, params->pathID);
					}

					DriverDeviceInputScalarSerialized* inputScalar = model.getScalarInput(deviceIndex, params->pathID);
					if (inputScalar) {
						inputScalar->useOverridenState = params->useOverridenState;
						model.setInputScalarChanged(deviceIndex, params->pathID);
					}

					DriverDeviceInputSkeletonSerialized* inputSkeleton = model.getSkeletonInput(deviceIndex, params->pathID);
					if (inputSkeleton) {
						inputSkeleton->useOverridenState = params->useOverridenState;
						model.setInputSkeletonChanged(deviceIndex, params->pathID);
					}

					DriverDeviceInputPoseSerialized* inputPose = model.getPoseInput(deviceIndex, params->pathID);
					if (inputPose) {
						inputPose->useOverridenState = params->useOverridenState;
						model.setInputPoseChanged(deviceIndex, params->pathID);
					}

					DriverDeviceInputEyeTrackingSerialized* inputEyeTracking = model.getEyeTrackingInput(deviceIndex, params->pathID);
					if (inputEyeTracking) {
						inputEyeTracking->useOverridenState = params->useOverridenState;
						model.setInputEyeTrackingChanged(deviceIndex, params->pathID);
					}
						
					break;
				}
				case Command_SetOverridenStateDeviceInputBoolean: {
					CommandParams_SetOverridenStateDeviceInputBoolean* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputBoolean*>(
						static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart + this->clientDriverLaneReadOffset
						);
					this->clientDriverLaneReadOffset += sizeof(CommandParams_SetOverridenStateDeviceInputBoolean);

					DriverDeviceInputBooleanSerialized* input = model.getBooleanInput(deviceIndex, params->pathID);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
				case Command_SetOverridenStateDeviceInputScalar: {
					CommandParams_SetOverridenStateDeviceInputScalar* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputScalar*>(
						static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart + this->clientDriverLaneReadOffset
						);
					this->clientDriverLaneReadOffset += sizeof(CommandParams_SetOverridenStateDeviceInputScalar);

					DriverDeviceInputScalarSerialized* input = model.getScalarInput(deviceIndex, params->pathID);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
				case Command_SetOverridenStateDeviceInputSkeleton: {
					CommandParams_SetOverridenStateDeviceInputSkeleton* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputSkeleton*>(
						static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart + this->clientDriverLaneReadOffset
						);
					this->clientDriverLaneReadOffset += sizeof(CommandParams_SetOverridenStateDeviceInputSkeleton);

					DriverDeviceInputSkeletonSerialized* input = model.getSkeletonInput(deviceIndex, params->pathID);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
				case Command_SetOverridenStateDeviceInputPose: {
					CommandParams_SetOverridenStateDeviceInputPose* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputPose*>(
						static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart + this->clientDriverLaneReadOffset
						);
					this->clientDriverLaneReadOffset += sizeof(CommandParams_SetOverridenStateDeviceInputPose);

					DriverDeviceInputPoseSerialized* input = model.getPoseInput(deviceIndex, params->pathID);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
				case Command_SetOverridenStateDeviceInputEyeTracking: {
					CommandParams_SetOverridenStateDeviceInputEyeTracking* params = reinterpret_cast<CommandParams_SetOverridenStateDeviceInputEyeTracking*>(
						static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart + this->clientDriverLaneReadOffset
						);
					this->clientDriverLaneReadOffset += sizeof(CommandParams_SetOverridenStateDeviceInputEyeTracking);

					DriverDeviceInputEyeTrackingSerialized* input = model.getEyeTrackingInput(deviceIndex, params->pathID);
					if (input) {
						input->data.overwrittenValue = params->overridenValue;
					}

					break;
				}
			}

			// Read command param struct based on command type
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

void SharedDeviceMemoryDriver::syncPathTableToSharedMemory(uint32_t pathID, const std::string& path) {
	PathTableEntry entry;
	entry.ID = pathID;
	entry.version = this->pathTableWriteCount;
	strncpy_s(entry.path, path.c_str(), sizeof(entry.path) - 1);

	// Write data starting at current index, should never have to loop around mid write
	memcpy(static_cast<uint8_t*>(this->sharedMemory) + this->pathTableStart + this->pathTableWriteIndex * sizeof(PathTableEntry), &entry, sizeof(PathTableEntry));
	
	// Update write count in shared memory header
	this->pathTableWriteIndex = (this->pathTableWriteIndex + 1) % PATH_TABLE_ENTRIES;
	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	headerPtr->pathTableWriteCount++;
}

void SharedDeviceMemoryDriver::syncDevicePoseUpdateToSharedMemory(DevicePoseSerialized* packet, uint32_t deviceIndex) {
	ObjectEntry entry;
	entry.type = Object_DevicePose;
	entry.deviceIndex = deviceIndex;
	entry.inputPathID = static_cast<uint32_t>(-1);
	entry.version = this->pathTableWriteCount;
	this->writePacketToDriverClientLane((void*)&entry, sizeof(ObjectEntry));
	this->writePacketToDriverClientLane((void*)packet, sizeof(DevicePoseSerialized));

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	this->driverClientLaneWriteCount++;
	headerPtr->driverClientWriteCount++;
}

void SharedDeviceMemoryDriver::syncDeviceInputBooleanUpdateToSharedMemory(DeviceInputBooleanSerialized* packet, uint32_t deviceIndex, uint32_t pathID) {
	ObjectEntry entry;
	entry.type = Object_InputBoolean;
	entry.deviceIndex = deviceIndex;
	entry.inputPathID = pathID;
	entry.version = this->pathTableWriteCount;
	this->writePacketToDriverClientLane((void*)&entry, sizeof(ObjectEntry));
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputBooleanSerialized));

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	this->driverClientLaneWriteCount++;
	headerPtr->driverClientWriteCount++;
}

void SharedDeviceMemoryDriver::syncDeviceInputScalarUpdateToSharedMemory(DeviceInputScalarSerialized* packet, uint32_t deviceIndex, uint32_t pathID) {
	ObjectEntry entry;
	entry.type = Object_InputScalar;
	entry.deviceIndex = deviceIndex;
	entry.inputPathID = pathID;
	entry.version = this->pathTableWriteCount;
	this->writePacketToDriverClientLane((void*)&entry, sizeof(ObjectEntry));
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputScalarSerialized));

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	this->driverClientLaneWriteCount++;
	headerPtr->driverClientWriteCount++;
}

void SharedDeviceMemoryDriver::syncDeviceInputSkeletonUpdateToSharedMemory(DeviceInputSkeletonSerialized* packet, uint32_t deviceIndex, uint32_t pathID) {
	ObjectEntry entry;
	entry.type = Object_InputSkeleton;
	entry.deviceIndex = deviceIndex;
	entry.inputPathID = pathID;
	entry.version = this->pathTableWriteCount;
	this->writePacketToDriverClientLane((void*)&entry, sizeof(ObjectEntry));
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputSkeletonSerialized));

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	this->driverClientLaneWriteCount++;
	headerPtr->driverClientWriteCount++;
}

void SharedDeviceMemoryDriver::syncDeviceInputPoseUpdateToSharedMemory(DeviceInputPoseSerialized* packet, uint32_t deviceIndex, uint32_t pathID) {
	ObjectEntry entry;
	entry.type = Object_InputPose;
	entry.deviceIndex = deviceIndex;
	entry.inputPathID = pathID;
	entry.version = this->pathTableWriteCount;
	this->writePacketToDriverClientLane((void*)&entry, sizeof(ObjectEntry));
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputPoseSerialized));

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	this->driverClientLaneWriteCount++;
	headerPtr->driverClientWriteCount++;
}

void SharedDeviceMemoryDriver::syncDeviceInputEyeTrackingUpdateToSharedMemory(DeviceInputEyeTrackingSerialized* packet, uint32_t deviceIndex, uint32_t pathID) {
	ObjectEntry entry;
	entry.type = Object_InputEyeTracking;
	entry.deviceIndex = deviceIndex;
	entry.inputPathID = pathID;
	entry.version = this->pathTableWriteCount;
	this->writePacketToDriverClientLane((void*)&entry, sizeof(ObjectEntry));
	this->writePacketToDriverClientLane((void*)packet, sizeof(DeviceInputEyeTrackingSerialized));

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	this->driverClientLaneWriteCount++;
	headerPtr->driverClientWriteCount++;
}