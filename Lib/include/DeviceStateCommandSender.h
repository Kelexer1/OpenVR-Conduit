#pragma once

#include "DeviceTypes.h"
#include "IDeviceStateEventReciever.h"

#include <stdint.h>
#include <string>

class DeviceStateCommandSender {
public:
	void addEventListener(IDeviceStateEventReciever& listener);
	void removeEventListener(IDeviceStateEventReciever& listener);

	// Lifecycle commands
	bool initialize();
	void notifyClientDisconnect();

	// State commands
	void setOverridenDevicePose(uint32_t deviceIndex, const DevicePose newPose);
	const DevicePose* getNaturalDevicePose(uint32_t deviceIndex);
	const DevicePose* getOverridenDevicePose(uint32_t deviceIndex);
	void setUseOverridenDevicePose(uint32_t deviceIndex, bool useOverridenState);
	bool getUseOverridenDevicePose(uint32_t deviceIndex);

	void setOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path, const BooleanInput newInput);
	const BooleanInput* getNaturalBooleanInputState(uint32_t deviceIndex, const std::string& path);
	const BooleanInput* getOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path);
	void setUseOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState);
	bool getUseOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path);

	void setOverridenScalarInputState(uint32_t deviceIndex, const std::string& path, const ScalarInput newInput);
	const ScalarInput* getNaturalScalarInputState(uint32_t deviceIndex, const std::string& path);
	const ScalarInput* getOverridenScalarInputState(uint32_t deviceIndex, const std::string& path);
	void setUseOverridenScalarInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState);
	bool getUseOverridenScalarInputState(uint32_t deviceIndex, const std::string& path);

	void setOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path, const SkeletonInput newInput);
	const SkeletonInput* getNaturalSkeletonInputState(uint32_t deviceIndex, const std::string& path);
	const SkeletonInput* getOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path);
	void setUseOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState);
	bool getUseOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path);

	void setOverridenPoseInputState(uint32_t deviceIndex, const std::string& path, const PoseInput newInput);
	const PoseInput* getNaturalPoseInputState(uint32_t deviceIndex, const std::string& path);
	const PoseInput* getOverridenPoseInputState(uint32_t deviceIndex, const std::string& path);
	void setUseOverridenPoseInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState);
	bool getUseOverridenPoseInputState(uint32_t deviceIndex, const std::string& path);

	void setOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path, const EyeTrackingInput newInput);
	const EyeTrackingInput* getNaturalEyeTrackingInputState(uint32_t deviceIndex, const std::string& path);
	const EyeTrackingInput* getOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path);
	void setUseOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState);
	bool getUseOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path);
};