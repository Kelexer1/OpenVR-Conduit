#include "SharedDeviceMemoryClient.h"
#include <iostream>

const uint32_t PROTOCOL_VERSION = 5;

SharedDeviceMemoryClient& SharedDeviceMemoryClient::getInstance() {
	static SharedDeviceMemoryClient instance;
	return instance;
}

int SharedDeviceMemoryClient::initialize() {
	if (!this->initialized) {
		this->sharedMemoryHandle = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, SHM_NAME);

		if (!this->sharedMemoryHandle) {
			return 1;
		}

		this->sharedMemory = MapViewOfFile(this->sharedMemoryHandle, FILE_MAP_ALL_ACCESS, 0, 0, 0);

		if (!this->sharedMemory) {
			return 2;
		}

		SharedMemoryHeader* header = static_cast<SharedMemoryHeader*>(this->sharedMemory);

		if (header->protocolVersion != PROTOCOL_VERSION) {
			return 3;
		}

		this->pathTableStart = header->pathTableStart;

		this->driverClientLaneStart = header->driverClientLaneStart;

		this->clientDriverLaneStart = header->clientDriverLaneStart;

		std::thread(&SharedDeviceMemoryClient::pollLoop, this).detach();

		this->initialized = true;
	}

	return 0;
}

std::string SharedDeviceMemoryClient::getPathFromPathOffset(uint32_t offset) {
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

uint32_t SharedDeviceMemoryClient::getOffsetOfPath(const std::string& path) {
	SharedMemoryHeader* headerPtr = reinterpret_cast<SharedMemoryHeader*>(this->sharedMemory);

	auto tablePath = this->pathTableOffsets.find(path);
	if (tablePath != this->pathTableOffsets.end()) return tablePath->second;

	uint8_t* pathTableStart = static_cast<uint8_t*>(this->sharedMemory) + this->pathTableStart;

	uint32_t searchOffset = 0;
	while (searchOffset < PATH_TABLE_SIZE) {
		// Wait for driver to finish writing
		while (headerPtr->pathTableWritingOffset.load(std::memory_order_acquire) == searchOffset) {}

		const char* currentPath = reinterpret_cast<const char*>(pathTableStart + searchOffset);
		size_t len = strnlen(currentPath, MAX_PATH_LENGTH);

		if (len == 0 || len == MAX_PATH_LENGTH) break;

		if (path == std::string(currentPath)) {
			this->pathTableOffsets[path] = searchOffset;
			return searchOffset;
		}

		searchOffset += static_cast<uint32_t>(len) + 1;
	}

	return UINT32_MAX;
}

void SharedDeviceMemoryClient::pollForDriverUpdates() {
	SharedMemoryHeader* headerPtr = reinterpret_cast<SharedMemoryHeader*>(this->sharedMemory);
	DeviceStateModelClient& model = DeviceStateModelClient::getInstance();

	// Driver -> Client lane updates
	uint64_t currentWriteCount = headerPtr->driverClientWriteCount.load(std::memory_order_acquire);
	if (this->driverClientLaneReadCount < currentWriteCount) {
		std::atomic_thread_fence(std::memory_order_acquire);

		ObjectEntryData entry;

		do {
			auto packet = this->readPacketFromDriverClientLane();
			entry = packet.first;

			if (!entry.successful) break;

			std::unique_ptr<uint8_t[]>& dataBuf = packet.second.second;

			uint32_t deviceIndex = entry.deviceIndex;
			std::string path(this->getPathFromPathOffset(entry.inputPathOffset));

			switch (entry.type) {
				case Object_DevicePose: {
					DevicePoseSerialized* data = reinterpret_cast<DevicePoseSerialized*>(dataBuf.get());

					if (entry.valid) {
						ModelDevicePoseSerialized* pose = model.getDevicePose(deviceIndex);

						if (!pose) {
							model.addDevicePose(deviceIndex);
							pose = model.getDevicePose(deviceIndex);
						}

						ModelDevicePoseSerialized oldPose = *pose;
						pose->data = *data;
						model.notifyListenersDevicePoseUpdated(deviceIndex, oldPose.data, *data);
					} else {
						model.removeDevicePose(deviceIndex);
					}

					break;
				}
				case Object_InputBoolean: {
					DeviceInputBooleanSerialized* data = reinterpret_cast<DeviceInputBooleanSerialized*>(dataBuf.get());

					if (entry.valid) {
						ModelDeviceInputBooleanSerialized* input = model.getBooleanInput(deviceIndex, path);
						
						if (!input) {
							model.addBooleanInput(deviceIndex, path);
							input = model.getBooleanInput(deviceIndex, path);
						}

						if (input) {
							ModelDeviceInputBooleanSerialized oldInput = *input;
							input->data = *data;
							model.notifyListenersBooleanInputUpdated(deviceIndex, path, oldInput.data, *data);
						}
					} else {
						model.removeBooleanInput(deviceIndex, path);
					}

					break;
				}
				case Object_InputScalar: {
					DeviceInputScalarSerialized* data = reinterpret_cast<DeviceInputScalarSerialized*>(dataBuf.get());

					if (entry.valid) {
						ModelDeviceInputScalarSerialized* input = model.getScalarInput(deviceIndex, path);

						if (!input) {
							model.addScalarInput(deviceIndex, path);
							input = model.getScalarInput(deviceIndex, path);
						}

						if (input) {
							ModelDeviceInputScalarSerialized oldInput = *input;
							input->data = *data;
							model.notifyListenersScalarInputUpdated(deviceIndex, path, oldInput.data, *data);
						}
					}
					else {
						model.removeScalarInput(deviceIndex, path);
					}

					break;
				}
				case Object_InputSkeleton: {
					DeviceInputSkeletonSerialized* data = reinterpret_cast<DeviceInputSkeletonSerialized*>(dataBuf.get());

					if (entry.valid) {
						ModelDeviceInputSkeletonSerialized* input = model.getSkeletonInput(deviceIndex, path);

						if (!input) {
							model.addSkeletonInput(deviceIndex, path);
							input = model.getSkeletonInput(deviceIndex, path);
						}

						if (input) {
							ModelDeviceInputSkeletonSerialized oldInput = *input;
							input->data = *data;
							model.notifyListenersSkeletonInputUpdated(deviceIndex, path, oldInput.data, *data);
						}
					}
					else {
						model.removeSkeletonInput(deviceIndex, path);
					}

					break;
				}
				case Object_InputPose: {
					DeviceInputPoseSerialized* data = reinterpret_cast<DeviceInputPoseSerialized*>(dataBuf.get());

					if (entry.valid) {
						ModelDeviceInputPoseSerialized* input = model.getPoseInput(deviceIndex, path);

						if (!input) {
							model.addPoseInput(deviceIndex, path);
							input = model.getPoseInput(deviceIndex, path);
						}

						if (input) {
							ModelDeviceInputPoseSerialized oldInput = *input;
							input->data = *data;
							model.notifyListenersPoseInputUpdated(deviceIndex, path, oldInput.data, *data);
						}
					}
					else {
						model.removePoseInput(deviceIndex, path);
					}

					break;
				}
				case Object_InputEyeTracking: {
					DeviceInputEyeTrackingSerialized* data = reinterpret_cast<DeviceInputEyeTrackingSerialized*>(dataBuf.get());

					if (entry.valid) {
						ModelDeviceInputEyeTrackingSerialized* input = model.getEyeTrackingInput(deviceIndex, path);

						if (!input) {
							model.addEyeTrackingInput(deviceIndex, path);
							input = model.getEyeTrackingInput(deviceIndex, path);
						}

						if (input) {
							ModelDeviceInputEyeTrackingSerialized oldInput = *input;
							input->data = *data;
							model.notifyListenersEyeTrackingInputUpdated(deviceIndex, path, oldInput.data, *data);
						}
					}
					else {
						model.removeEyeTrackingInput(deviceIndex, path);
					}

					break;
				}
			}

			this->driverClientLaneReadCount = entry.version;
		} while (this->driverClientLaneReadCount < currentWriteCount);

		this->driverClientLaneReadCount = currentWriteCount;
	}
}

void SharedDeviceMemoryClient::pollLoop() {
	double POLL_PERIOD_MICROSECONDS = 1000000.0 / POLL_RATE;

	while (true) {
		this->pollForDriverUpdates();
		std::this_thread::sleep_for(std::chrono::microseconds((int)POLL_PERIOD_MICROSECONDS));
	}
}

void SharedDeviceMemoryClient::issueCommandToSharedMemory(ClientCommandType type, uint32_t deviceIndex, void* paramsStart, uint32_t paramsSize) {
	uint32_t totalSize = 0;
	switch (type) {
	case Command_SetUseOverridenStateDevicePose:
		totalSize = sizeof(CommandParams_SetUseOverridenStateDevicePose); break;
	case Command_SetOverridenStateDevicePose:
		totalSize = sizeof(CommandParams_SetOverridenStateDevicePose); break;
	case Command_SetUseOverridenStateDeviceInput:
		totalSize = sizeof(CommandParams_SetUseOverridenStateDeviceInput); break;
	case Command_SetOverridenStateDeviceInputBoolean:
		totalSize = sizeof(CommandParams_SetOverridenStateDeviceInputBoolean); break;
	case Command_SetOverridenStateDeviceInputScalar:
		totalSize = sizeof(CommandParams_SetOverridenStateDeviceInputScalar); break;
	case Command_SetOverridenStateDeviceInputSkeleton:
		totalSize = sizeof(CommandParams_SetOverridenStateDeviceInputSkeleton); break;
	case Command_SetOverridenStateDeviceInputPose:
		totalSize = sizeof(CommandParams_SetOverridenStateDeviceInputPose); break;
	case Command_SetOverridenStateDeviceInputEyeTracking:
		totalSize = sizeof(CommandParams_SetOverridenStateDeviceInputEyeTracking); break;
	}
	totalSize += sizeof(ClientCommandHeader);
	std::vector<uint8_t> buffer(totalSize);

	ClientCommandHeader* header = reinterpret_cast<ClientCommandHeader*>(buffer.data());
	header->alignmentCheck = ALIGNMENT_CONSTANT;
    header->type = type;
    header->deviceIndex = deviceIndex;
    header->version = this->clientDriverLaneWriteCount;
	header->committed.store(false, std::memory_order_relaxed);

	memcpy(buffer.data() + sizeof(ClientCommandHeader), paramsStart, paramsSize);

	this->writePacketToClientDriverLane(buffer.data(), totalSize);
}

void SharedDeviceMemoryClient::writePacketToClientDriverLane(void* packet, uint32_t packetSize) {
	if (!packet || packetSize <= 0) return;

	SharedMemoryHeader* headerPtr = reinterpret_cast<SharedMemoryHeader*>(this->sharedMemory);

	uint32_t readOffset = headerPtr->clientDriverReadOffset.load(std::memory_order_acquire);
	uint32_t writeOffset = this->clientDriverLaneWriteOffset;

	uint32_t freeSpace = writeOffset >= readOffset
		? (LANE_SIZE - writeOffset) + readOffset
		: readOffset - writeOffset;

	if (packetSize > freeSpace) return;

	uint8_t* laneStart = static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart;

	uint8_t* currentWriteStart;
	uint32_t newWriteOffset;
	if (this->clientDriverLaneWriteOffset >= LANE_SIZE - LANE_PADDING_SIZE) {
		currentWriteStart = laneStart;
		newWriteOffset = packetSize;
	} else {
		currentWriteStart = laneStart + this->clientDriverLaneWriteOffset;
		newWriteOffset = this->clientDriverLaneWriteOffset + packetSize;
	}

	memcpy(currentWriteStart, packet, packetSize);

	ClientCommandHeader* header = reinterpret_cast<ClientCommandHeader*>(currentWriteStart);
	header->committed.store(true, std::memory_order_release);

	this->clientDriverLaneWriteOffset = newWriteOffset;
	headerPtr->clientDriverWriteOffset.store(this->clientDriverLaneWriteOffset, std::memory_order_release);

	this->clientDriverLaneWriteCount++;
	headerPtr->clientDriverWriteCount.store(this->clientDriverLaneWriteCount, std::memory_order_release);
}

std::pair<ObjectEntryData, std::pair<ObjectType, std::unique_ptr<uint8_t[]>>> SharedDeviceMemoryClient::readPacketFromDriverClientLane() {
    SharedMemoryHeader* headerPtr = reinterpret_cast<SharedMemoryHeader*>(this->sharedMemory);    
	
	if (this->driverClientLaneReadOffset >= LANE_SIZE - LANE_PADDING_SIZE) this->driverClientLaneReadOffset = 0;

	uint32_t writeOffset = headerPtr->driverClientWriteOffset.load(std::memory_order_acquire);
	if (this->driverClientLaneReadOffset == writeOffset) return { ObjectEntryData{}, { Object_DevicePose, nullptr } };

    // Read ObjectEntry
	uint8_t* laneStart = static_cast<uint8_t*>(this->sharedMemory) + this->driverClientLaneStart;
    uint8_t* readStart = laneStart + this->driverClientLaneReadOffset;
    ObjectEntry* rawEntry = reinterpret_cast<ObjectEntry*>(readStart);

	// Misalignment correction
	if (!this->isValidObjectPacket(rawEntry, headerPtr)) {
		bool recovered = false;
		uint32_t searchOffset = this->driverClientLaneReadOffset;

		const uint32_t FORWARD_SEARCH_BYTES = 4096;

		// Strategy 1: Forward Check
		for (uint32_t i = 0; i < FORWARD_SEARCH_BYTES; i++) {
			searchOffset = (searchOffset + 1) % LANE_SIZE;

			if (searchOffset == writeOffset) break;

			ObjectEntry* testEntry = reinterpret_cast<ObjectEntry*>(laneStart + searchOffset);

			if (this->isValidObjectPacket(testEntry, headerPtr)) {
				recovered = true;
				this->driverClientLaneReadOffset = searchOffset;
				readStart = laneStart + searchOffset;
				rawEntry = testEntry;
				break;
			}
		}

		// Strategy 2: Jump To Write Offset
		if (!recovered) {
			this->driverClientLaneReadOffset = headerPtr->driverClientWriteOffset.load(std::memory_order_acquire);
			this->driverClientLaneReadCount = headerPtr->driverClientWriteCount.load(std::memory_order_acquire);
			return { ObjectEntryData{}, { Object_DevicePose, nullptr } };
		}
	}

	while (!rawEntry->committed.load(std::memory_order_acquire)) {} // Wait for packet to be valid

	ObjectEntryData entry = {};
	entry.successful = true;
	entry.type = rawEntry->type;
	entry.deviceIndex = rawEntry->deviceIndex;
	entry.inputPathOffset = rawEntry->inputPathOffset;
	entry.version = rawEntry->version;
	entry.valid = rawEntry->valid;

    // Read object data
    uint32_t dataSize = 0;
	ObjectType type = entry.type;
    switch (type) {
    case Object_DevicePose:
        dataSize = sizeof(DevicePoseSerialized); break;
    case Object_InputBoolean:
        dataSize = sizeof(DeviceInputBooleanSerialized); break;
    case Object_InputScalar:
        dataSize = sizeof(DeviceInputScalarSerialized); break;
    case Object_InputSkeleton:
        dataSize = sizeof(DeviceInputSkeletonSerialized); break;
    case Object_InputPose:
        dataSize = sizeof(DeviceInputPoseSerialized); break;
    case Object_InputEyeTracking:
        dataSize = sizeof(DeviceInputEyeTrackingSerialized); break;
    }

    auto dataBuffer = std::make_unique<uint8_t[]>(dataSize);
    memcpy(dataBuffer.get(), readStart + sizeof(ObjectEntry), dataSize);

    this->driverClientLaneReadOffset += sizeof(ObjectEntry) + dataSize;
    headerPtr->driverClientReadOffset.store(this->driverClientLaneReadOffset, std::memory_order_release);

    return std::make_pair(entry, std::make_pair(type, std::move(dataBuffer)));
}

bool SharedDeviceMemoryClient::isValidObjectPacket(const ObjectEntry* entry, const SharedMemoryHeader* header) {
	if (entry->alignmentCheck != ALIGNMENT_CONSTANT) {
		return false;
	}

	if (!(Object_DevicePose <= entry->type && entry->type <= Object_InputEyeTracking)) {
		return false;
	}

	if (!(0 <= entry->deviceIndex && entry->deviceIndex < 64)) {
		return false;
	}

	// Valid characters are [a-z]+[A-Z]+[/]
	if (entry->type != Object_DevicePose) {
		std::string inputPath = this->getPathFromPathOffset(entry->inputPathOffset);

		for (size_t i = 0; i < MAX_PATH_LENGTH; i++) {
			char c = inputPath[i];
			
			if (c == '\0') break;

			if (!((c >= 'a' && c <= 'z') || 
			      (c >= 'A' && c <= 'Z') || 
			      (c >= '0' && c <= '9') || 
			      c == '/')) {
				return false;
			}
		}
	}

	//const int MAX_VERSION_DELTA = 1000;
	//const int difference = entry->version - header->driverClientWriteCount;
	//if (abs(difference) > MAX_VERSION_DELTA) {
	//	return false;
	//}

	return true;
}