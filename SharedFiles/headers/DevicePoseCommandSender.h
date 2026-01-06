#pragma once
#include "SharedDeviceMemory.h"

/**
 * @brief Sends commands to the Conduit driver by updating shared memory
 */
class DevicePoseCommandSender {
public:
	/**
	 * @brief Constructs a new DevicePoseCommandSender associated with a shared memory IO
	 * @param sharedMemory The associated shared memory IO
	 */
	DevicePoseCommandSender(SharedDeviceMemoryIO* sharedMemory);

	///=========================
	/// Pose Manipulations
	///=========================

	/**
	 * @brief Fully overrides the stored pose for the device with deviceIndex
	 *
	 * Note that this will override all stored positions, rotations, etc.
	 *
	 * @param deviceIndex The index of the device
	 * @param newPose the pose that should override the current pose
	 */
	void setPose(const uint32_t deviceIndex, DevicePose& pose);

	/**
	 * @brief Fills the vecPosition of the stored pose for the device with deviceIndex
	 *
	 * Leaves the rest of the pose struct untouched
	 *
	 * @param deviceIndex The index of the device
	 * @param newPosition The new position
	 */
	void setPosition(const uint32_t deviceIndex, DeviceVector3& newPosition);

	/**
	 * @brief Fills the qRotation of the stored pose for the device with deviceIndex
	 *
	 * Leaves the rest of the pose struct untouched
	 *
	 * @param deviceIndex The index of the device
	 * @param newRotation The new rotation
	 */
	void setRotation(const uint32_t deviceIndex, DeviceQuaternion& newRotation);

	/**
	 * @brief Fills the vecVelocity of the stored pose for the device with deviceIndex
	 *
	 * Leaves the rest of the pose struct untouched
	 *
	 * @param deviceIndex The index of the device
	 * @param newVelocity The new velocity
	 */
	void setVelocity(const uint32_t deviceIndex, DeviceVector3& newVelocity);

	/**
	 * @brief Fills the vecAngularVelocity of the stored pose for the device with deviceIndex
	 *
	 * Leaves the rest of the pose struct untouched
	 *
	 * @param deviceIndex The index of the device
	 * @param newAngularVelocity The new angular velocity
	 */
	void setAngularVelocity(const uint32_t deviceIndex, DeviceVector3& newAngularVelocity);

	/**
	 * @brief Fills the vecAcceleration of the stored pose for the device with deviceIndex
	 *
	 * Leaves the rest of the pose struct untouched
	 *
	 * @param deviceIndex The index of the device
	 * @param newAcceleration The new acceleration
	 */
	void setAcceleration(const uint32_t deviceIndex, DeviceVector3& newAcceleration);

	/**
	 * @brief Fills the vecAngularAcceleration of the stored pose for the device with deviceIndex
	 *
	 * Leaves the rest of the pose struct untouched
	 *
	 * @param deviceIndex The index of the device
	 * @param newAngularAcceleration The new angular acceleration
	 */
	void setAngularAcceleration(const uint32_t deviceIndex, DeviceVector3& newAngularAcceleration);

	/**
	 * @brief Returns the stored pose of this device
	 *
	 * Note that this pose is NOT nesessarily the actual pose of the device,
	 * it is essentially a queued pose that can be applied to the device
	 *
	 * @param deviceIndex The index of the device
	 * @return The overriden pose of the device, nullptr if something went wrong
	 */
	DevicePose* getPose(const uint32_t deviceIndex) const;

	/**
	 * @brief Returns the *actual* position of this device
	 *
	 * Note that this pose may not equal the stored pose of the device
	 *
	 * @param deviceIndex The index of the device
	 * @return The natural pose of the device, nullptr if something went wrong
	 */
	DevicePose* getNaturalPose(const uint32_t deviceIndex) const;

	///=========================
	/// Override control
	///=========================

	/**
	 * @brief Returns the stored pose of the device with deviceIndex
	 *
	 * Note that this pose is NOT nesessarily the actual pose of the device,
	 * it is essentially a queued pose that can be applied to the device
	 *
	 * @param deviceIndex The index of the device
	 * @param overridePose A boolean representing whether the device should use it's overriden pose (true) or it's natural pose (false)
	 */
	void setUseCustomPose(const uint32_t deviceIndex, const bool shouldOverridePose);

	/**
	 * @brief Returns whether the device is using a custom pose
	 *
	 * Note that this pose may not equal the stored pose of the device
	 *
	 * @param deviceIndex The index of the device
	 * @return True if the device is using its overriden pose, false if it's using its natural pose, nullptr if something went wrong
	 */
	bool* getUseCustomPose(const uint32_t deviceIndex) const;

	/**
	 * @brief Sets whether the device should use ping-pong communication for pose manipulation
	 * @param deviceIndex The index of the device
	 * @param usePingPong True if the device should use ping-pong communication, false if it should not
	 */
	void setUsePingPong(const uint32_t deviceIndex, const bool usePingPong);

	/**
	 * @brief Returns whether the device is currently using ping-pong communication for pose manipulation
	 * @param deviceIndex The index of the device
	 * @return True if the device is using ping-pong, false if it isn't, nullptr if something went wrong
	 */
	bool* getUsePingPong(const uint32_t deviceIndex);

private:
	/// The associated shared memory
	SharedDeviceMemoryIO* sharedMemory;
};