#pragma once
#include "DeviceTypes.h"

#include <cstdint>

/**
 * @brief An abstract interface for recieving event pings from the Conduit driver
 * 
 * This class should *not* be instanciated directly. Instead, inherit from it and implement its methods
 */
class IDevicePoseEventListener {
public:
	virtual ~IDevicePoseEventListener() = default;

	/**
	 * @brief Called when the Conduit driver is requesting a pose
	 *
	 * This method should be filled with your desired logic and always return a valid driver pose
	 *
	 * @return The new pose
	 */
	virtual DevicePose poseRequested(uint32_t deviceIndex, DevicePose naturalPose) = 0;
};