#pragma once
#include <openvr_driver.h>

#include "DeviceTypes.h"

vr::ETrackedDeviceClass getDeviceClass(uint32_t deviceIndex);

DevicePose FromDriverPose(const vr::DriverPose_t& pose);

vr::DriverPose_t ToDriverPose(const DevicePose& cp);