#include "DeviceStateCommandSender.h"

#include "SharedDeviceMemoryClient.h"
#include "DeviceStateModelClient.h"

void DeviceStateCommandSender::addEventListener(IDeviceStateEventReceiver& listener) {
	DeviceStateModelClient::getInstance().addEventListener(listener);
}

void DeviceStateCommandSender::removeEventListener(IDeviceStateEventReceiver& listener) {
	DeviceStateModelClient::getInstance().removeEventListener(listener);
}

int DeviceStateCommandSender::initialize() {
	return SharedDeviceMemoryClient::getInstance().initialize();
}

void DeviceStateCommandSender::notifyClientDisconnect() {
	// Unused, may be implemented in future update
}

void DeviceStateCommandSender::setOverriddenDevicePose(uint32_t deviceIndex, const DevicePose newPose) {
	ModelDevicePoseSerialized* pose = DeviceStateModelClient::getInstance().getDevicePose(deviceIndex);
	if (pose != nullptr) pose->data.overwrittenPose = newPose;

	CommandParams_SetOverriddenStateDevicePose params{};
	params.overriddenPose = newPose;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetOverriddenStateDevicePose, deviceIndex, &params, sizeof(CommandParams_SetOverriddenStateDevicePose)
	);
}

std::optional<DevicePose> DeviceStateCommandSender::getNaturalDevicePose(uint32_t deviceIndex) {
	ModelDevicePoseSerialized* pose = DeviceStateModelClient::getInstance().getDevicePose(deviceIndex);
	if (pose != nullptr) return pose->data.pose;
	return std::nullopt;
}

std::optional<DevicePose> DeviceStateCommandSender::getOverriddenDevicePose(uint32_t deviceIndex) {
	ModelDevicePoseSerialized* pose = DeviceStateModelClient::getInstance().getDevicePose(deviceIndex);
	if (pose != nullptr) return pose->data.overwrittenPose;
	return std::nullopt;
}

void DeviceStateCommandSender::setUseOverriddenDevicePose(uint32_t deviceIndex, bool useOverriddenState) {
	ModelDevicePoseSerialized* pose = DeviceStateModelClient::getInstance().getDevicePose(deviceIndex);
	if (pose != nullptr) pose->useOverriddenState = useOverriddenState;

	CommandParams_SetUseOverriddenStateDevicePose params = {};
	params.useOverriddenState = useOverriddenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetUseOverriddenStateDevicePose,
		deviceIndex,
		&params,
		sizeof(CommandParams_SetUseOverriddenStateDevicePose)
	);
}

bool DeviceStateCommandSender::getUseOverriddenDevicePose(uint32_t deviceIndex) {
	ModelDevicePoseSerialized* pose = DeviceStateModelClient::getInstance().getDevicePose(deviceIndex);
	if (pose != nullptr) return pose->useOverriddenState;
	return false;
}

void DeviceStateCommandSender::setOverriddenBooleanInputState(uint32_t deviceIndex, const std::string& path, const BooleanInput newInput) {
	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(deviceIndex, path);
	if (input != nullptr) input->data.overwrittenValue = newInput;

	CommandParams_SetOverriddenStateDeviceInputBoolean params = {};
	params.inputPathOffset = SharedDeviceMemoryClient::getInstance().getOffsetOfPath(path);
	params.overriddenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetOverriddenStateDeviceInputBoolean,
		deviceIndex,
		&params,
		sizeof(CommandParams_SetOverriddenStateDeviceInputBoolean)
	);
}

std::optional<BooleanInput> DeviceStateCommandSender::getNaturalBooleanInputState(
	uint32_t deviceIndex,
	const std::string& path
) {
	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(
		deviceIndex,
		path
	);
	if (input != nullptr) return input->data.value;

	return std::nullopt;
}

std::optional<BooleanInput> DeviceStateCommandSender::getOverriddenBooleanInputState(
	uint32_t deviceIndex,
	const std::string& path
) {
	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(
		deviceIndex,
		path
	);
	if (input != nullptr) return input->data.overwrittenValue;
	return std::nullopt;
}

void DeviceStateCommandSender::setUseOverriddenBooleanInputState(
	uint32_t deviceIndex,
	const std::string& path,
	bool useOverriddenState
) {
	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(
		deviceIndex,
		path
	);
	if (input != nullptr) input->useOverriddenState = useOverriddenState;

	CommandParams_SetUseOverriddenStateDeviceInput params = {};
	params.inputPathOffset = SharedDeviceMemoryClient::getInstance().getOffsetOfPath(path);
	params.useOverriddenState = useOverriddenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetUseOverriddenStateDeviceInput,
		deviceIndex,
		&params,
		sizeof(CommandParams_SetUseOverriddenStateDeviceInput)
	);
}

bool DeviceStateCommandSender::getUseOverriddenBooleanInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputBooleanSerialized* input = DeviceStateModelClient::getInstance().getBooleanInput(
		deviceIndex,
		path
	);
	if (input != nullptr) return input->useOverriddenState;
	return false;
}

void DeviceStateCommandSender::setOverriddenScalarInputState(
	uint32_t deviceIndex,
	const std::string& path,
	const ScalarInput newInput
) {
	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, path);
	if (input != nullptr) input->data.overwrittenValue = newInput;

	CommandParams_SetOverriddenStateDeviceInputScalar params = {};
	params.inputPathOffset = SharedDeviceMemoryClient::getInstance().getOffsetOfPath(path);
	params.overriddenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetOverriddenStateDeviceInputScalar,
		deviceIndex,
		&params,
		sizeof(CommandParams_SetOverriddenStateDeviceInputScalar)
	);
}

std::optional<ScalarInput> DeviceStateCommandSender::getNaturalScalarInputState(
	uint32_t deviceIndex,
	const std::string& path
) {
	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, path);
	if (input != nullptr) return input->data.value;
	return std::nullopt;
}

std::optional<ScalarInput> DeviceStateCommandSender::getOverriddenScalarInputState(
	uint32_t deviceIndex,
	const std::string& path
) {
	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, path);
	if (input != nullptr) return input->data.overwrittenValue;
	return std::nullopt;
}

void DeviceStateCommandSender::setUseOverriddenScalarInputState(
	uint32_t deviceIndex,
	const std::string& path,
	bool useOverriddenState
) {
	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, path);
	if (input != nullptr) input->useOverriddenState = useOverriddenState;

	CommandParams_SetUseOverriddenStateDeviceInput params = {};
	params.inputPathOffset = SharedDeviceMemoryClient::getInstance().getOffsetOfPath(path);
	params.useOverriddenState = useOverriddenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetUseOverriddenStateDeviceInput,
		deviceIndex,
		&params,
		sizeof(CommandParams_SetUseOverriddenStateDeviceInput)
	);
}

bool DeviceStateCommandSender::getUseOverriddenScalarInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputScalarSerialized* input = DeviceStateModelClient::getInstance().getScalarInput(deviceIndex, path);
	if (input != nullptr) return input->useOverriddenState;
	return false;
}

void DeviceStateCommandSender::setOverriddenSkeletonInputState(
	uint32_t deviceIndex,
	const std::string& path,
	const SkeletonInput newInput
) {
	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(
		deviceIndex,
		path
	);
	if (input != nullptr) input->data.overwrittenValue = newInput;

	CommandParams_SetOverriddenStateDeviceInputSkeleton params = {};
	params.inputPathOffset = SharedDeviceMemoryClient::getInstance().getOffsetOfPath(path);
	params.overriddenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetOverriddenStateDeviceInputSkeleton,
		deviceIndex,
		&params,
		sizeof(CommandParams_SetOverriddenStateDeviceInputSkeleton)
	);
}

std::optional<SkeletonInput> DeviceStateCommandSender::getNaturalSkeletonInputState(
	uint32_t deviceIndex,
	const std::string& path
) {
	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(
		deviceIndex,
		path
	);
	if (input != nullptr) return input->data.value;
	return std::nullopt;
}

std::optional<SkeletonInput> DeviceStateCommandSender::getOverriddenSkeletonInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(
		deviceIndex,
		path
	);
	if (input != nullptr) return input->data.overwrittenValue;
	return std::nullopt;
}

void DeviceStateCommandSender::setUseOverriddenSkeletonInputState(
	uint32_t deviceIndex,
	const std::string& path,
	bool useOverriddenState
) {
	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(
		deviceIndex,
		path
	);
	if (input != nullptr) input->useOverriddenState = useOverriddenState;

	CommandParams_SetUseOverriddenStateDeviceInput params = {};
	params.inputPathOffset = SharedDeviceMemoryClient::getInstance().getOffsetOfPath(path);
	params.useOverriddenState = useOverriddenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetUseOverriddenStateDeviceInput,
		deviceIndex,
		&params,
		sizeof(CommandParams_SetUseOverriddenStateDeviceInput)
	);
}

bool DeviceStateCommandSender::getUseOverriddenSkeletonInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputSkeletonSerialized* input = DeviceStateModelClient::getInstance().getSkeletonInput(
		deviceIndex,
		path
	);
	if (input != nullptr) return input->useOverriddenState;
	return false;
}

void DeviceStateCommandSender::setOverriddenPoseInputState(
	uint32_t deviceIndex,
	const std::string& path,
	const PoseInput newInput
) {
	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, path);
	if (input != nullptr) input->data.overwrittenValue = newInput;

	CommandParams_SetOverriddenStateDeviceInputPose params = {};
	params.inputPathOffset = SharedDeviceMemoryClient::getInstance().getOffsetOfPath(path);
	params.overriddenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetOverriddenStateDeviceInputPose,
		deviceIndex,
		&params,
		sizeof(CommandParams_SetOverriddenStateDeviceInputPose)
	);
}

std::optional<PoseInput> DeviceStateCommandSender::getNaturalPoseInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, path);
	if (input != nullptr) return input->data.value;
	return std::nullopt;
}

std::optional<PoseInput> DeviceStateCommandSender::getOverriddenPoseInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, path);
	if (input != nullptr) return input->data.overwrittenValue;
	return std::nullopt;
}

void DeviceStateCommandSender::setUseOverriddenPoseInputState(
	uint32_t deviceIndex,
	const std::string& path,
	bool useOverriddenState
) {
	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, path);
	if (input != nullptr) input->useOverriddenState = useOverriddenState;

	CommandParams_SetUseOverriddenStateDeviceInput params = {};
	params.inputPathOffset = SharedDeviceMemoryClient::getInstance().getOffsetOfPath(path);
	params.useOverriddenState = useOverriddenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetUseOverriddenStateDeviceInput,
		deviceIndex,
		&params,
		sizeof(CommandParams_SetUseOverriddenStateDeviceInput)
	);
}

bool DeviceStateCommandSender::getUseOverriddenPoseInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputPoseSerialized* input = DeviceStateModelClient::getInstance().getPoseInput(deviceIndex, path);
	if (input != nullptr) {
		return input->useOverriddenState;
	}

	return false;
}

void DeviceStateCommandSender::setOverriddenEyeTrackingInputState(
	uint32_t deviceIndex,
	const std::string& path,
	const EyeTrackingInput newInput
) {
	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(
		deviceIndex,
		path
	);
	if (input != nullptr) input->data.overwrittenValue = newInput;

	CommandParams_SetOverriddenStateDeviceInputEyeTracking params = {};
	params.inputPathOffset = SharedDeviceMemoryClient::getInstance().getOffsetOfPath(path);
	params.overriddenValue = newInput;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetOverriddenStateDeviceInputEyeTracking,
		deviceIndex,
		&params,
		sizeof(CommandParams_SetOverriddenStateDeviceInputEyeTracking)
	);
}

std::optional<EyeTrackingInput> DeviceStateCommandSender::getNaturalEyeTrackingInputState(
	uint32_t deviceIndex,
	const std::string& path
) {
	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(
		deviceIndex,
		path
	);
	if (input != nullptr) return input->data.value;
	return std::nullopt;
}

std::optional<EyeTrackingInput> DeviceStateCommandSender::getOverriddenEyeTrackingInputState(
	uint32_t deviceIndex,
	const std::string& path
) {
	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(
		deviceIndex,
		path
	);
	if (input != nullptr) return input->data.overwrittenValue;
	return std::nullopt;
}

void DeviceStateCommandSender::setUseOverriddenEyeTrackingInputState(
	uint32_t deviceIndex,
	const std::string& path,
	bool useOverriddenState
) {
	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(
		deviceIndex,
		path
	);
	if (input != nullptr) input->useOverriddenState = useOverriddenState;

	CommandParams_SetUseOverriddenStateDeviceInput params = {};
	params.inputPathOffset = SharedDeviceMemoryClient::getInstance().getOffsetOfPath(path);
	params.useOverriddenState = useOverriddenState;
	SharedDeviceMemoryClient::getInstance().issueCommandToSharedMemory(
		Command_SetUseOverriddenStateDeviceInput,
		deviceIndex,
		&params,
		sizeof(CommandParams_SetUseOverriddenStateDeviceInput)
	);
}

bool DeviceStateCommandSender::getUseOverriddenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path) {
	ModelDeviceInputEyeTrackingSerialized* input = DeviceStateModelClient::getInstance().getEyeTrackingInput(
		deviceIndex,
		path
	);
	if (input != nullptr) return input->useOverriddenState;
	return false;
}