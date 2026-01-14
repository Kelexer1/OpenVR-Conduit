#include "DeviceStateCommandSender.h"

bool DeviceStateCommandSender::initialize() {
	return SharedDeviceMemoryClient::getInstance().initialize();
}

void DeviceStateCommandSender::notifyClientDisconnect() {
	// todo send disconnect command
}

void DeviceStateCommandSender::setOverridenDevicePose(uint32_t deviceIndex, const DevicePose newPose) {
	ModelDevicePoseSerialized* pose = DeviceStateModelClient::getInstance().getDevicePose(deviceIndex);
	if (pose != nullptr) {
		pose->data.overwrittenPose = newPose;
	}

	CommandParams_SetOverridenStateDevicePose* params = {};
	params->overridenPose = newPose;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(Command_SetOverridenStateDevicePose, deviceIndex, params, sizeof(CommandParams_SetOverridenStateDevicePose));
}

const DevicePose* DeviceStateCommandSender::getNaturalDevicePose(uint32_t deviceIndex) {
	ModelDevicePoseSerialized* pose = DeviceStateModelClient::getInstance().getDevicePose(deviceIndex);
	if (pose != nullptr) {
		return &pose->data.pose; 
	}
	return nullptr;
}

const DevicePose* DeviceStateCommandSender::getOverridenDevicePose(uint32_t deviceIndex) {
	ModelDevicePoseSerialized* pose = DeviceStateModelClient::getInstance().getDevicePose(deviceIndex);
	if (pose != nullptr) {
		return &pose->data.overwrittenPose;
	}
	return nullptr;
}

void DeviceStateCommandSender::setUseOverridenDevicePose(uint32_t deviceIndex, bool useOverridenState) {
	ModelDevicePoseSerialized* pose = DeviceStateModelClient::getInstance().getDevicePose(deviceIndex);
	if (pose != nullptr) {
		pose->useOverridenState = useOverridenState;
	}

	CommandParams_SetUseOverridenStateDevicePose* params = {};
	params->useOverridenState = useOverridenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(Command_SetUseOverridenStateDevicePose, deviceIndex, params, sizeof(CommandParams_SetUseOverridenStateDevicePose));
}

bool DeviceStateCommandSender::getUseOverridenDevicePose(uint32_t deviceIndex) {
	ModelDevicePoseSerialized* pose = DeviceStateModelClient::getInstance().getDevicePose(deviceIndex);
	if (pose != nullptr) {
		return pose->useOverridenState;
	}

	return false;
}

void DeviceStateCommandSender::setOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path, const BooleanInput newInput) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return;

	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(deviceIndex, pathID);
	if (input != nullptr) {
		input->data.overwrittenValue = newInput;
	}

	CommandParams_SetOverridenStateDeviceInputBoolean* params = {};
	params->pathID = pathID;
	params->overridenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(Command_SetOverridenStateDeviceInputBoolean, deviceIndex, params, sizeof(CommandParams_SetOverridenStateDeviceInputBoolean));
}

const BooleanInput* DeviceStateCommandSender::getNaturalBooleanInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return nullptr;

	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(deviceIndex, pathID);
	if (input != nullptr) {
		return &input->data.value;
	}

	return nullptr;
}

const BooleanInput* DeviceStateCommandSender::getOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return nullptr;

	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(deviceIndex, pathID);
	if (input != nullptr) {
		return &input->data.overwrittenValue;
	}

	return nullptr;
}

void DeviceStateCommandSender::setUseOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return;

	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(deviceIndex, pathID);
	if (input != nullptr) {
		input->useOverridenState = useOverridenState;
	}

	CommandParams_SetUseOverridenStateDeviceInput* params = {};
	params->pathID = pathID;
	params->useOverridenState = useOverridenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(Command_SetUseOverridenStateDeviceInput, deviceIndex, params, sizeof(CommandParams_SetUseOverridenStateDeviceInput));
}

bool DeviceStateCommandSender::getUseOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return false;

	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(deviceIndex, pathID);
	if (input != nullptr) {
		return input->useOverridenState;
	}
}

void DeviceStateCommandSender::setOverridenScalarInputState(uint32_t deviceIndex, const std::string& path, const ScalarInput newInput) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return;

	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, pathID);
	if (input != nullptr) {
		input->data.overwrittenValue = newInput;
	}

	CommandParams_SetOverridenStateDeviceInputScalar* params = {};
	params->pathID = pathID;
	params->overridenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(Command_SetOverridenStateDeviceInputScalar, deviceIndex, params, sizeof(CommandParams_SetOverridenStateDeviceInputScalar));
}

const ScalarInput* DeviceStateCommandSender::getNaturalScalarInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return nullptr;

	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, pathID);
	if (input != nullptr) {
		return &input->data.value;
	}

	return nullptr;
}

const ScalarInput* DeviceStateCommandSender::getOverridenScalarInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return nullptr;

	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, pathID);
	if (input != nullptr) {
		return &input->data.overwrittenValue;
	}

	return nullptr;
}

void DeviceStateCommandSender::setUseOverridenScalarInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return;

	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, pathID);
	if (input != nullptr) {
		input->useOverridenState = useOverridenState;
	}

	CommandParams_SetUseOverridenStateDeviceInput* params = {};
	params->pathID = pathID;
	params->useOverridenState = useOverridenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(Command_SetUseOverridenStateDeviceInput, deviceIndex, params, sizeof(CommandParams_SetUseOverridenStateDeviceInput));
}

bool DeviceStateCommandSender::getUseOverridenScalarInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return false;

	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, pathID);
	if (input != nullptr) {
		return input->useOverridenState;
	}
}

void DeviceStateCommandSender::setOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path, const SkeletonInput newInput) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return;

	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(deviceIndex, pathID);
	if (input != nullptr) {
		input->data.overwrittenValue = newInput;
	}

	CommandParams_SetOverridenStateDeviceInputSkeleton* params = {};
	params->pathID = pathID;
	params->overridenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(Command_SetOverridenStateDeviceInputSkeleton, deviceIndex, params, sizeof(CommandParams_SetOverridenStateDeviceInputSkeleton));
}

const SkeletonInput* DeviceStateCommandSender::getNaturalSkeletonInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return nullptr;

	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(deviceIndex, pathID);
	if (input != nullptr) {
		return &input->data.value;
	}

	return nullptr;
}

const SkeletonInput* DeviceStateCommandSender::getOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return nullptr;

	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(deviceIndex, pathID);
	if (input != nullptr) {
		return &input->data.overwrittenValue;
	}

	return nullptr;
}

void DeviceStateCommandSender::setUseOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return;

	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(deviceIndex, pathID);
	if (input != nullptr) {
		input->useOverridenState = useOverridenState;
	}

	CommandParams_SetUseOverridenStateDeviceInput* params = {};
	params->pathID = pathID;
	params->useOverridenState = useOverridenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(Command_SetUseOverridenStateDeviceInput, deviceIndex, params, sizeof(CommandParams_SetUseOverridenStateDeviceInput));
}

bool DeviceStateCommandSender::getUseOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return false;

	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(deviceIndex, pathID);
	if (input != nullptr) {
		return input->useOverridenState;
	}
}

void DeviceStateCommandSender::setOverridenPoseInputState(uint32_t deviceIndex, const std::string& path, const PoseInput newInput) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return;

	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, pathID);
	if (input != nullptr) {
		input->data.overwrittenValue = newInput;
	}

	CommandParams_SetOverridenStateDeviceInputPose* params = {};
	params->pathID = pathID;
	params->overridenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(Command_SetOverridenStateDeviceInputPose, deviceIndex, params, sizeof(CommandParams_SetOverridenStateDeviceInputPose));
}

const PoseInput* DeviceStateCommandSender::getNaturalPoseInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return nullptr;

	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, pathID);
	if (input != nullptr) {
		return &input->data.value;
	}

	return nullptr;
}

const PoseInput* DeviceStateCommandSender::getOverridenPoseInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return nullptr;

	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, pathID);
	if (input != nullptr) {
		return &input->data.overwrittenValue;
	}

	return nullptr;
}

void DeviceStateCommandSender::setUseOverridenPoseInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return;

	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, pathID);
	if (input != nullptr) {
		input->useOverridenState = useOverridenState;
	}

	CommandParams_SetUseOverridenStateDeviceInput* params = {};
	params->pathID = pathID;
	params->useOverridenState = useOverridenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(Command_SetUseOverridenStateDeviceInput, deviceIndex, params, sizeof(CommandParams_SetUseOverridenStateDeviceInput));
}

bool DeviceStateCommandSender::getUseOverridenPoseInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return false;

	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, pathID);
	if (input != nullptr) {
		return input->useOverridenState;
	}
}

void DeviceStateCommandSender::setOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path, const EyeTrackingInput newInput) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return;

	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(deviceIndex, pathID);
	if (input != nullptr) {
		input->data.overwrittenValue = newInput;
	}

	CommandParams_SetOverridenStateDeviceInputEyeTracking* params = {};
	params->pathID = pathID;
	params->overridenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(Command_SetOverridenStateDeviceInputEyeTracking, deviceIndex, params, sizeof(CommandParams_SetOverridenStateDeviceInputEyeTracking));
}

const EyeTrackingInput* DeviceStateCommandSender::getNaturalEyeTrackingInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return nullptr;

	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(deviceIndex, pathID);
	if (input != nullptr) {
		return &input->data.value;
	}

	return nullptr;
}

const EyeTrackingInput* DeviceStateCommandSender::getOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return nullptr;

	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(deviceIndex, pathID);
	if (input != nullptr) {
		return &input->data.overwrittenValue;
	}

	return nullptr;
}

void DeviceStateCommandSender::setUseOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return;

	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(deviceIndex, pathID);
	if (input != nullptr) {
		input->useOverridenState = useOverridenState;
	}

	CommandParams_SetUseOverridenStateDeviceInput* params = {};
	params->pathID = pathID;
	params->useOverridenState = useOverridenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(Command_SetUseOverridenStateDeviceInput, deviceIndex, params, sizeof(CommandParams_SetUseOverridenStateDeviceInput));
}

bool DeviceStateCommandSender::getUseOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path) {
	uint32_t pathID = DeviceStateModelClient::getInstance().getIndexFromPath(path);
	if (pathID == static_cast<uint32_t>(-1)) return false;

	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(deviceIndex, pathID);
	if (input != nullptr) {
		return input->useOverridenState;
	}
}