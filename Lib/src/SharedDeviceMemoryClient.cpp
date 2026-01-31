#include "SharedDeviceMemoryClient.h"
#include <iostream>

const uint32_t PROTOCOL_VERSION = 4;

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

		this->driverClientLaneStart = header->driverClientLaneStart;
		//this->driverClientLaneReadOffset = header->driverClientWriteOffset;
		//this->driverClientLaneReadCount = header->driverClientWriteCount;

		this->clientDriverLaneStart = header->clientDriverLaneStart;
		//this->clientDriverLaneWriteOffset = header->clientDriverWriteOffset;
		//this->clientDriverLaneWriteCount = header->clientDriverWriteCount;

		std::thread(&SharedDeviceMemoryClient::pollLoop, this).detach();

		this->initialized = true;
	}

	return 0;
}

void SharedDeviceMemoryClient::pollForDriverUpdates() {
	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
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
			std::string path(entry.inputPath);

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
    ClientCommandHeader header = {};
    header.type = type;
    header.deviceIndex = deviceIndex;
    header.version = this->clientDriverLaneWriteCount;

    this->writePacketToClientDriverLane(&header, sizeof(ClientCommandHeader));
    this->writePacketToClientDriverLane(paramsStart, paramsSize);

	ClientCommandHeader zero = {};
	this->writePacketToClientDriverLane(&zero, sizeof(ClientCommandHeader));

    SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
    this->clientDriverLaneWriteCount++;
    headerPtr->clientDriverWriteCount++;
    headerPtr->clientDriverWriteOffset = this->clientDriverLaneWriteOffset;
}

void SharedDeviceMemoryClient::writePacketToClientDriverLane(void* packet, uint32_t packetSize) {
	if (packet != nullptr && packetSize > 0) {
		SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);

		uint32_t readOffset = headerPtr->clientDriverReadOffset;
		uint32_t writeOffset = this->clientDriverLaneWriteOffset;

		uint32_t freeSpace;
		if (writeOffset >= readOffset) {
			freeSpace = (LANE_SIZE - writeOffset) + readOffset;
		}
		else {
			freeSpace = readOffset - writeOffset;
		}

		if (packetSize > freeSpace) {
			std::cout << "Client would lap reader - dropping packet of size" << packetSize << " (free space :" << freeSpace << ")\n";
			return;
		}

		uint8_t* laneStart = static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart;
		uint32_t currentOffset = this->clientDriverLaneWriteOffset;

		// Handle wrap around
		if (currentOffset + packetSize > LANE_SIZE) {
			uint32_t firstPartitionSize = LANE_SIZE - currentOffset;
			memcpy(laneStart + currentOffset, packet, firstPartitionSize);

			uint32_t secondPartitionSize = packetSize - firstPartitionSize;
			memcpy(laneStart, reinterpret_cast<uint8_t*>(packet) + firstPartitionSize, secondPartitionSize);

			this->clientDriverLaneWriteOffset = secondPartitionSize;
		}
		else {
			memcpy(laneStart + currentOffset, packet, packetSize);
			this->clientDriverLaneWriteOffset = (currentOffset + packetSize) % LANE_SIZE;
		}
	}
}

std::pair<ObjectEntryData, std::pair<ObjectType, std::unique_ptr<uint8_t[]>>> SharedDeviceMemoryClient::readPacketFromDriverClientLane() {
    SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);    
	
	if (this->driverClientLaneReadOffset >= LANE_SIZE - LANE_PADDING_SIZE) this->driverClientLaneReadOffset = 0;

	uint32_t writeOffset = headerPtr->driverClientWriteOffset.load(std::memory_order_acquire);
	if (this->driverClientLaneReadOffset == writeOffset) return { ObjectEntryData{}, {Object_DevicePose, nullptr} };

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
			return { ObjectEntryData{}, {Object_DevicePose, nullptr} };
		}
	}

	uint32_t spinCount = 0;
	while (!rawEntry->committed.load(std::memory_order_acquire)) {
		spinCount++;
		if (spinCount >= 1000000) {
			std::cout << "Waiting on packet commitment: " << spinCount << "\n";
		}
	} // Wait for packet to be valid

	ObjectEntryData entry;
	entry.successful = true;
	entry.type = rawEntry->type;
	entry.deviceIndex = rawEntry->deviceIndex;
	memcpy(entry.inputPath, rawEntry->inputPath, sizeof(entry.inputPath));
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

	if (!(0 <= entry->deviceIndex && entry->deviceIndex < 128)) {
		return false;
	}

	// Valid characters are [a-z]+[A-Z]+[/]
	if (entry->type != Object_DevicePose) {
		const size_t MAX_PATH_LENGTH = sizeof(entry->inputPath);
		bool nullTerminated = false;
		
		for (size_t i = 0; i < MAX_PATH_LENGTH; i++) {
			char c = entry->inputPath[i];
			
			if (c == '\0') {
				nullTerminated = true;
				break;
			}
			
			if (!((c >= 'a' && c <= 'z') || 
			      (c >= 'A' && c <= 'Z') || 
			      (c >= '0' && c <= '9') || 
			      c == '/')) {
				return false;
			}
		}
		
		if (!nullTerminated) {
			return false;
		}
	}

	//const int MAX_VERSION_DELTA = 1000;
	//const int difference = entry->version - header->driverClientWriteCount;
	//if (abs(difference) > MAX_VERSION_DELTA) {
	//	std::cout << "Version delta\n";
	//	return false;
	//}

	return true;
}