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
	DeviceStateModelClient model = DeviceStateModelClient::getInstance();

	// Path table updates
	if (this->pathTableReadCount < headerPtr->pathTableWriteCount) {
		PathTableEntry* entry;

		do {
			auto entryBuf = this->readPacketFromPathTable(sizeof(PathTableEntry));
			entry = reinterpret_cast<PathTableEntry*>(entryBuf.get());

			uint32_t pathID = entry->ID;
			std::string path(entry->path);

			if (!path.empty()) {
				model.addDevicePath(pathID, path);
			} else {
				model.removeDevicePath(pathID);
			}
		} while (entry->version < this->pathTableReadCount);
	}

	// Driver -> Client lane updates
	if (this->driverClientLaneReadCount < headerPtr->driverClientWriteCount) {
		ObjectEntry* entry;

		do {
			auto entryBuf = this->readPacketFromDriverClientLane(sizeof(ObjectEntry));
			entry = reinterpret_cast<ObjectEntry*>(entryBuf.get());

			uint32_t deviceIndex = entry->deviceIndex;
			uint32_t pathID = entry->inputPathID;

			switch (entry->type) {
				case Object_DevicePose: {
					auto dataBuf = this->readPacketFromDriverClientLane(sizeof(DevicePoseSerialized));
					DevicePoseSerialized* data = reinterpret_cast<DevicePoseSerialized*>(dataBuf.get());

					if (entry->valid) {
						ModelDevicePoseSerialized* pose = model.getDevicePose(deviceIndex);
						ModelDevicePoseSerialized oldPose = *pose;

						if (pose) {
							pose->data = *data;
							model.notifyListenersDevicePoseUpdated(deviceIndex, oldPose.data, *data);
						}
					} else {
						model.removeDevicePose(deviceIndex);
					}

					break;
				}
				case Object_InputBoolean: {
					auto dataBuf = this->readPacketFromDriverClientLane(sizeof(DeviceInputBooleanSerialized));
					DeviceInputBooleanSerialized* data = reinterpret_cast<DeviceInputBooleanSerialized*>(dataBuf.get());

					if (entry->valid) {
						ModelDeviceInputBooleanSerialized* input = model.getBooleanInput(deviceIndex, pathID);
						ModelDeviceInputBooleanSerialized oldInput = *input;

						if (input) {
							input->data = *data;
							model.notifyListenersBooleanInputUpdated(deviceIndex, pathID, oldInput.data, *data);
						}
					} else {
						model.removeBooleanInput(deviceIndex, pathID);
					}

					break;
				}
				case Object_InputScalar: {
					auto dataBuf = this->readPacketFromDriverClientLane(sizeof(DeviceInputScalarSerialized));
					DeviceInputScalarSerialized* data = reinterpret_cast<DeviceInputScalarSerialized*>(dataBuf.get());

					if (entry->valid) {
						ModelDeviceInputScalarSerialized* input = model.getScalarInput(deviceIndex, pathID);
						ModelDeviceInputScalarSerialized oldInput = *input;

						if (input) {
							input->data = *data;
							model.notifyListenersScalarInputUpdated(deviceIndex, pathID, oldInput.data, *data);
						}
					}
					else {
						model.removeScalarInput(deviceIndex, pathID);
					}

					break;
				}
				case Object_InputSkeleton: {
					auto dataBuf = this->readPacketFromDriverClientLane(sizeof(DeviceInputSkeletonSerialized));
					DeviceInputSkeletonSerialized* data = reinterpret_cast<DeviceInputSkeletonSerialized*>(dataBuf.get());

					if (entry->valid) {
						ModelDeviceInputSkeletonSerialized* input = model.getSkeletonInput(deviceIndex, pathID);
						ModelDeviceInputSkeletonSerialized oldInput = *input;

						if (input) {
							input->data = *data;
							model.notifyListenersSkeletonInputUpdated(deviceIndex, pathID, oldInput.data, *data);
						}
					}
					else {
						model.removeBooleanInput(deviceIndex, pathID);
					}

					break;
				}
				case Object_InputPose: {
					auto dataBuf = this->readPacketFromDriverClientLane(sizeof(DeviceInputPoseSerialized));
					DeviceInputPoseSerialized* data = reinterpret_cast<DeviceInputPoseSerialized*>(dataBuf.get());

					if (entry->valid) {
						ModelDeviceInputPoseSerialized* input = model.getPoseInput(deviceIndex, pathID);
						ModelDeviceInputPoseSerialized oldInput = *input;

						if (input) {
							input->data = *data;
							model.notifyListenersPoseInputUpdated(deviceIndex, pathID, oldInput.data, *data);
						}
					}
					else {
						model.removeBooleanInput(deviceIndex, pathID);
					}

					break;
				}
				case Object_InputEyeTracking: {
					auto dataBuf = this->readPacketFromDriverClientLane(sizeof(DeviceInputEyeTrackingSerialized));
					DeviceInputEyeTrackingSerialized* data = reinterpret_cast<DeviceInputEyeTrackingSerialized*>(dataBuf.get());

					if (entry->valid) {
						ModelDeviceInputEyeTrackingSerialized* input = model.getEyeTrackingInput(deviceIndex, pathID);
						ModelDeviceInputEyeTrackingSerialized oldInput = *input;

						if (input) {
							input->data = *data;
							model.notifyListenersEyeTrackingInputUpdated(deviceIndex, pathID, oldInput.data, *data);
						}
					}
					else {
						model.removeBooleanInput(deviceIndex, pathID);
					}

					break;
				}
			}
		} while (entry->version < this->driverClientLaneReadCount);

		this->driverClientLaneReadCount = headerPtr->driverClientWriteCount;
	}
}

void SharedDeviceMemoryClient::issueCommandToSharedMemory(ClientCommandType type, uint32_t deviceIndex, void* paramsStart, uint32_t paramsSize) {
	ClientCommandHeader* header = {};
	header->type = type;
	header->deviceIndex = deviceIndex;
	header->version = this->clientDriverWriteCount;

	this->writePacketToClientDriverLane(header, sizeof(ClientCommandHeader));
	this->writePacketToClientDriverLane(paramsStart, paramsSize);

	SharedMemoryHeader* headerPtr = static_cast<SharedMemoryHeader*>(this->sharedMemory);
	this->clientDriverWriteCount++;
	headerPtr->clientDriverWriteCount++;
}

void SharedDeviceMemoryClient::writePacketToClientDriverLane(void* packet, uint32_t packetSize) {
	if (packet != nullptr && packetSize > 0) {
		uint8_t* laneStart = static_cast<uint8_t*>(this->sharedMemory) + this->clientDriverLaneStart;
		size_t currentOffset = this->clientDriverLaneWriteOffset;

		// Handle wrap around
		if (currentOffset + packetSize > LANE_SIZE) {
			size_t firstPartitionSize = LANE_SIZE - currentOffset;
			memcpy(laneStart + currentOffset, packet, firstPartitionSize);

			size_t secondPartitionSize = packetSize - firstPartitionSize;
			memcpy(laneStart + currentOffset, reinterpret_cast<uint8_t*>(packet) + firstPartitionSize, secondPartitionSize);

			this->clientDriverLaneWriteOffset = secondPartitionSize;
		}
		else {
			memcpy(laneStart + currentOffset, packet, packetSize);
			this->clientDriverLaneWriteOffset = (currentOffset + packetSize) % LANE_SIZE;
		}
	}
}

std::unique_ptr<uint8_t[]> SharedDeviceMemoryClient::readPacketFromDriverClientLane(uint32_t packetSize) {
	if (packetSize == 0) {
		return nullptr;
	}

	auto buffer = std::make_unique<uint8_t[]>(packetSize);
	if (!buffer) {
		return nullptr;
	}

	uint8_t* laneStart = static_cast<uint8_t*>(this->sharedMemory) + this->driverClientLaneStart;
	size_t currentOffset = this->driverClientLaneReadOffset;

	// Handle wrap around
	if (currentOffset + packetSize > LANE_SIZE) {
		size_t firstPartitionSize = LANE_SIZE - currentOffset;
		memcpy(buffer.get(), laneStart + currentOffset, firstPartitionSize);

		size_t secondPartitionSize = packetSize - firstPartitionSize;
		memcpy(buffer.get() + firstPartitionSize, laneStart, secondPartitionSize);

		this->driverClientLaneReadOffset = secondPartitionSize;
	}
	else {
		memcpy(buffer.get(), laneStart + currentOffset, packetSize);
		this->driverClientLaneReadOffset = (currentOffset + packetSize) % LANE_SIZE;
	}

	return buffer;
}

std::unique_ptr<uint8_t[]> SharedDeviceMemoryClient::readPacketFromPathTable(uint32_t packetSize) {
	if (packetSize == 0) {
		return nullptr;
	}

	auto buffer = std::make_unique<uint8_t[]>(packetSize);
	if (!buffer) {
		return nullptr;
	}

	uint8_t* laneStart = static_cast<uint8_t*>(this->sharedMemory) + this->pathTableStart;
	size_t currentOffset = this->pathTableReadOffset;

	// Handle wrap around
	if (currentOffset + packetSize > LANE_SIZE) {
		size_t firstPartitionSize = LANE_SIZE - currentOffset;
		memcpy(buffer.get(), laneStart + currentOffset, firstPartitionSize);

		size_t secondPartitionSize = packetSize - firstPartitionSize;
		memcpy(buffer.get() + firstPartitionSize, laneStart, secondPartitionSize);

		this->pathTableReadOffset = secondPartitionSize;
	}
	else {
		memcpy(buffer.get(), laneStart + currentOffset, packetSize);
		this->pathTableReadOffset = (currentOffset + packetSize) % LANE_SIZE;
	}

	return buffer;
}