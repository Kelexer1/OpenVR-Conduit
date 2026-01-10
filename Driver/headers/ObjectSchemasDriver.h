#pragma once
#include "ObjectSchemas.h"

struct DriverObjectState {
	bool useOverridenState;
};

struct DriverDevicePoseSerialized : public DriverObjectState {
	DevicePoseSerialized data;
};

struct DriverDeviceInputBooleanSerialized : public DriverObjectState {
	DeviceInputBooleanSerialized data;
};

struct DriverDeviceInputScalarSerialized : public DriverObjectState {
	DeviceInputScalarSerialized data;
};

struct DriverDeviceInputSkeletonSerialized : public DriverObjectState {
	DeviceInputSkeletonSerialized data;
};

struct DriverDeviceInputPoseSerialized : public DriverObjectState {
	DeviceInputPoseSerialized data;
};

struct DriverDeviceInputEyeTrackingSerialized : public DriverObjectState {
	DeviceInputEyeTrackingSerialized data;
};