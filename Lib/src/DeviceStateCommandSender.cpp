#include "DeviceStateCommandSender.h"

#include "SharedDeviceMemoryClient.h"
#include "DeviceStateModelClient.h"

void DeviceStateCommandSender::addEventListener(IDeviceStateEventReciever& listener) {
	DeviceStateModelClient::getInstance().addEventListener(listener);
}

void DeviceStateCommandSender::removeEventListener(IDeviceStateEventReciever& listener) {
	DeviceStateModelClient::getInstance().removeEventListener(listener);
}

int DeviceStateCommandSender::initialize() {
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

	CommandParams_SetOverridenStateDevicePose params{};
	params.overridenPose = newPose;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetOverridenStateDevicePose, deviceIndex, &params, sizeof(CommandParams_SetOverridenStateDevicePose)
	);
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

	CommandParams_SetUseOverridenStateDevicePose params = {};
	params.useOverridenState = useOverridenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetUseOverridenStateDevicePose, deviceIndex, &params, sizeof(CommandParams_SetUseOverridenStateDevicePose)
	);
}

bool DeviceStateCommandSender::getUseOverridenDevicePose(uint32_t deviceIndex) {
	ModelDevicePoseSerialized* pose = DeviceStateModelClient::getInstance().getDevicePose(deviceIndex);
	if (pose != nullptr) {
		return pose->useOverridenState;
	}

	return false;
}

void DeviceStateCommandSender::setOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path, const BooleanInput newInput) {
	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(deviceIndex, path);
	if (input != nullptr) {
		input->data.overwrittenValue = newInput;
	}

	CommandParams_SetOverridenStateDeviceInputBoolean params = {};
	strncpy_s(params.inputPath, path.c_str(), sizeof(params.inputPath));
	params.inputPath[sizeof(params.inputPath) - 1] = '\0';
	params.overridenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetOverridenStateDeviceInputBoolean, deviceIndex, &params, sizeof(CommandParams_SetOverridenStateDeviceInputBoolean)
	);
}

const BooleanInput* DeviceStateCommandSender::getNaturalBooleanInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(deviceIndex, path);
	if (input != nullptr) {
		return &input->data.value;
	}

	return nullptr;
}

const BooleanInput* DeviceStateCommandSender::getOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(deviceIndex, path);
	if (input != nullptr) {
		return &input->data.overwrittenValue;
	}

	return nullptr;
}

void DeviceStateCommandSender::setUseOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState) {
	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(deviceIndex, path);
	if (input != nullptr) {
		input->useOverridenState = useOverridenState;
	}

	CommandParams_SetUseOverridenStateDeviceInput params = {};
	strncpy_s(params.inputPath, path.c_str(), sizeof(params.inputPath));
	params.inputPath[sizeof(params.inputPath) - 1] = '\0';
	params.useOverridenState = useOverridenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetUseOverridenStateDeviceInput, deviceIndex, &params, sizeof(CommandParams_SetUseOverridenStateDeviceInput)
	);
}

bool DeviceStateCommandSender::getUseOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(deviceIndex, path);
	if (input != nullptr) {
		return input->useOverridenState;
	}

	return false;
}

void DeviceStateCommandSender::setOverridenScalarInputState(uint32_t deviceIndex, const std::string& path, const ScalarInput newInput) {
	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, path);
	if (input != nullptr) {
		input->data.overwrittenValue = newInput;
	}

	CommandParams_SetOverridenStateDeviceInputScalar params = {};
	strncpy_s(params.inputPath, path.c_str(), sizeof(params.inputPath));
	params.inputPath[sizeof(params.inputPath) - 1] = '\0';
	params.overridenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetOverridenStateDeviceInputScalar, deviceIndex, &params, sizeof(CommandParams_SetOverridenStateDeviceInputScalar)
	);
}

const ScalarInput* DeviceStateCommandSender::getNaturalScalarInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, path);
	if (input != nullptr) {
		return &input->data.value;
	}

	return nullptr;
}

const ScalarInput* DeviceStateCommandSender::getOverridenScalarInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, path);
	if (input != nullptr) {
		return &input->data.overwrittenValue;
	}

	return nullptr;
}

void DeviceStateCommandSender::setUseOverridenScalarInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState) {
	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, path);
	if (input != nullptr) {
		input->useOverridenState = useOverridenState;
	}

	CommandParams_SetUseOverridenStateDeviceInput params = {};
	strncpy_s(params.inputPath, path.c_str(), sizeof(params.inputPath));
	params.inputPath[sizeof(params.inputPath) - 1] = '\0';
	params.useOverridenState = useOverridenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetUseOverridenStateDeviceInput, deviceIndex, &params, sizeof(CommandParams_SetUseOverridenStateDeviceInput)
	);
}

bool DeviceStateCommandSender::getUseOverridenScalarInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, path);
	if (input != nullptr) {
		return input->useOverridenState;
	}

	return false;
}

void DeviceStateCommandSender::setOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path, const SkeletonInput newInput) {
	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(deviceIndex, path);
	if (input != nullptr) {
		input->data.overwrittenValue = newInput;
	}

	CommandParams_SetOverridenStateDeviceInputSkeleton params = {};
	strncpy_s(params.inputPath, path.c_str(), sizeof(params.inputPath));
	params.inputPath[sizeof(params.inputPath) - 1] = '\0';
	params.overridenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetOverridenStateDeviceInputSkeleton, deviceIndex, &params, sizeof(CommandParams_SetOverridenStateDeviceInputSkeleton)
	);
}

const SkeletonInput* DeviceStateCommandSender::getNaturalSkeletonInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(deviceIndex, path);
	if (input != nullptr) {
		return &input->data.value;
	}

	return nullptr;
}

const SkeletonInput* DeviceStateCommandSender::getOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(deviceIndex, path);
	if (input != nullptr) {
		return &input->data.overwrittenValue;
	}

	return nullptr;
}

void DeviceStateCommandSender::setUseOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState) {
	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(deviceIndex, path);
	if (input != nullptr) {
		input->useOverridenState = useOverridenState;
	}

	CommandParams_SetUseOverridenStateDeviceInput params = {};
	strncpy_s(params.inputPath, path.c_str(), sizeof(params.inputPath));
	params.inputPath[sizeof(params.inputPath) - 1] = '\0';
	params.useOverridenState = useOverridenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetUseOverridenStateDeviceInput, deviceIndex, &params, sizeof(CommandParams_SetUseOverridenStateDeviceInput)
	);
}

bool DeviceStateCommandSender::getUseOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(deviceIndex, path);
	if (input != nullptr) {
		return input->useOverridenState;
	}

	return false;
}

void DeviceStateCommandSender::setOverridenPoseInputState(uint32_t deviceIndex, const std::string& path, const PoseInput newInput) {
	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, path);
	if (input != nullptr) {
		input->data.overwrittenValue = newInput;
	}

	CommandParams_SetOverridenStateDeviceInputPose params = {};
	strncpy_s(params.inputPath, path.c_str(), sizeof(params.inputPath));
	params.inputPath[sizeof(params.inputPath) - 1] = '\0';
	params.overridenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetOverridenStateDeviceInputPose, deviceIndex, &params, sizeof(CommandParams_SetOverridenStateDeviceInputPose)
	);
}

const PoseInput* DeviceStateCommandSender::getNaturalPoseInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, path);
	if (input != nullptr) {
		return &input->data.value;
	}

	return nullptr;
}

const PoseInput* DeviceStateCommandSender::getOverridenPoseInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, path);
	if (input != nullptr) {
		return &input->data.overwrittenValue;
	}

	return nullptr;
}

void DeviceStateCommandSender::setUseOverridenPoseInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState) {
	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, path);
	if (input != nullptr) {
		input->useOverridenState = useOverridenState;
	}

	CommandParams_SetUseOverridenStateDeviceInput params = {};
	strncpy_s(params.inputPath, path.c_str(), sizeof(params.inputPath));
	params.inputPath[sizeof(params.inputPath) - 1] = '\0';
	params.useOverridenState = useOverridenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetUseOverridenStateDeviceInput, deviceIndex, &params, sizeof(CommandParams_SetUseOverridenStateDeviceInput)
	);
}

bool DeviceStateCommandSender::getUseOverridenPoseInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, path);
	if (input != nullptr) {
		return input->useOverridenState;
	}

	return false;
}

void DeviceStateCommandSender::setOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path, const EyeTrackingInput newInput) {
	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(deviceIndex, path);
	if (input != nullptr) {
		input->data.overwrittenValue = newInput;
	}

	CommandParams_SetOverridenStateDeviceInputEyeTracking params = {};
	strncpy_s(params.inputPath, path.c_str(), sizeof(params.inputPath));
	params.inputPath[sizeof(params.inputPath) - 1] = '\0';
	params.overridenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetOverridenStateDeviceInputEyeTracking, deviceIndex, &params, sizeof(CommandParams_SetOverridenStateDeviceInputEyeTracking)
	);
}

const EyeTrackingInput* DeviceStateCommandSender::getNaturalEyeTrackingInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(deviceIndex, path);
	if (input != nullptr) {
		return &input->data.value;
	}

	return nullptr;
}

const EyeTrackingInput* DeviceStateCommandSender::getOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(deviceIndex, path);
	if (input != nullptr) {
		return &input->data.overwrittenValue;
	}

	return nullptr;
}

void DeviceStateCommandSender::setUseOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState) {
	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(deviceIndex, path);
	if (input != nullptr) {
		input->useOverridenState = useOverridenState;
	}

	CommandParams_SetUseOverridenStateDeviceInput params = {};
	strncpy_s(params.inputPath, path.c_str(), sizeof(params.inputPath));
	params.inputPath[sizeof(params.inputPath) - 1] = '\0';
	params.useOverridenState = useOverridenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetUseOverridenStateDeviceInput, deviceIndex, &params, sizeof(CommandParams_SetUseOverridenStateDeviceInput)
	);
}

bool DeviceStateCommandSender::getUseOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(deviceIndex, path);
	if (input != nullptr) {
		return input->useOverridenState;
	}

	return false;
}