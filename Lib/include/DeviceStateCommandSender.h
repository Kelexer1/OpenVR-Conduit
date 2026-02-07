#pragma once

#include "DeviceTypes.h"
#include "IDeviceStateEventReceiver.h"

#include <stdint.h>
#include <string>

/**
 * @brief Handles sending client commands to the Conduit driver, and notifies event listeners of incoming events from
 * driver
 */
class DeviceStateCommandSender {
public:
	/**************************************************
	* @brief Livecycle commands
	**************************************************/

	/**
	 * @brief Subscribes an event receiver for live events notifications from the Conduit driver
	 * @param listener The event receiver
	 */
	void addEventListener(IDeviceStateEventReceiver& listener);

	/**
	 * @brief Stops an existing event Receiver from recieving live event notifications from the Conduit driver
	 * @param listener The event receiver
	 */
	void removeEventListener(IDeviceStateEventReceiver& listener);

	/**
	 * @brief Initializes the client, including shared memory and other required objects for functionality
	 * @return The initialization code, which can be interpreted as follows:
	 * 0 - Success
	 * 1 - Failed to get shared memory handle
	 * 2 - Failed to map shared memory
	 * 3 - Shared memory protocol version mismatch
	 */
	int initialize();

	/**
	 * @brief Used to notify the conduit driver that a client is disconnecting (Unused)
	 */
	void notifyClientDisconnect();

	/**************************************************
	* @brief Device pose commands
	**************************************************/

	/**
	 * @brief Sets the overridden state of a device pose for a device
	 * @param deviceIndex The device index of the device
	 * @param newPose The new overridden pose
	 */
	void setOverriddenDevicePose(uint32_t deviceIndex, const DevicePose newPose);

	/**
	 * @brief Returns the natural (non-overridden) state of a device pose for a device
	 * @param deviceIndex The device index of the device
	 * @return A pointer to the pose if successful, nullptr otherwise
	 */
	const DevicePose* getNaturalDevicePose(uint32_t deviceIndex);

	/**
	 * @brief Returns the overridden state of a device pose for a device
	 * @param deviceIndex The device index of the device
	 * @return A pointer to the pose if successfull, nullptr otherwise
	 */
	const DevicePose* getOverriddenDevicePose(uint32_t deviceIndex);

	/**
	 * @brief Sets whether the OpenVR runtime should use the overridden state, or the natural state of a devices pose
	 * @param deviceIndex The device index of the device
	 * @param useOverriddenState True if the overridden state should be used, false if the natural state should be used
	 */
	void setUseOverriddenDevicePose(uint32_t deviceIndex, bool useOverriddenState);

	/**
	 * @brief Returns whether the OpenVR runtime is using the overridden or natural state of a devices pose
	 * @param deviceIndex The device index of the device
	 * @return True if the device is using its overridden state, false otherwise
	 */
	bool getUseOverriddenDevicePose(uint32_t deviceIndex);

	/**************************************************
	* @brief Boolean input commands
	**************************************************/

	/**
	 * @brief Sets the overridden state of a boolean input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param newInput The new overridden boolean input state
	 */
	void setOverriddenBooleanInputState(uint32_t deviceIndex, const std::string& path, const BooleanInput newInput);

	/**
	 * @brief Returns the natural (non-overridden) state of a boolean input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the boolean input if successful, nullptr otherwise
	 */
	const BooleanInput* getNaturalBooleanInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the overridden state of a boolean input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the boolean input if successful, nullptr otherwise
	 */
	const BooleanInput* getOverriddenBooleanInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Sets whether the OpenVR runtime should use the overridden or natural state of a boolean input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param useOverriddenState True if the overridden state should be used, false if the natural state should be used
	 */
	void setUseOverriddenBooleanInputState(uint32_t deviceIndex, const std::string& path, bool useOverriddenState);

	/**
	 * @brief Returns whether the OpenVR runtime is using the overridden or natural state of a boolean input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return True if the input is using its overridden state, false otherwise
	 */
	bool getUseOverriddenBooleanInputState(uint32_t deviceIndex, const std::string& path);

	/**************************************************
	* @brief Scalar input commands
	**************************************************/

	/**
	 * @brief Sets the overridden state of a scalar input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param newInput The new overridden scalar input state
	 */
	void setOverriddenScalarInputState(uint32_t deviceIndex, const std::string& path, const ScalarInput newInput);

	/**
	 * @brief Returns the natural (non-overridden) state of a scalar input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the scalar input if successful, nullptr otherwise
	 */
	const ScalarInput* getNaturalScalarInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the overridden state of a scalar input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the scalar input if successful, nullptr otherwise
	 */
	const ScalarInput* getOverriddenScalarInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Sets whether the OpenVR runtime should use the overridden or natural state of a scalar input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param useOverriddenState True if the overridden state should be used, false if the natural state should be used
	 */
	void setUseOverriddenScalarInputState(uint32_t deviceIndex, const std::string& path, bool useOverriddenState);

	/**
	 * @brief Returns whether the OpenVR runtime is using the overridden or natural state of a scalar input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return True if the input is using its overridden state, false otherwise
	 */
	bool getUseOverriddenScalarInputState(uint32_t deviceIndex, const std::string& path);

	/**************************************************
	* @brief Skeleton input commands
	**************************************************/

	/**
	 * @brief Sets the overridden state of a skeleton input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param newInput The new overridden skeleton input state
	 */
	void setOverriddenSkeletonInputState(uint32_t deviceIndex, const std::string& path, const SkeletonInput newInput);

	/**
	 * @brief Returns the natural (non-overridden) state of a skeleton input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the skeleton input if successful, nullptr otherwise
	 */
	const SkeletonInput* getNaturalSkeletonInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the overridden state of a skeleton input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the skeleton input if successful, nullptr otherwise
	 */
	const SkeletonInput* getOverriddenSkeletonInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Sets whether the OpenVR runtime should use the overridden or natural state of a skeleton input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param useOverriddenState True if the overridden state should be used, false if the natural state should be used
	 */
	void setUseOverriddenSkeletonInputState(uint32_t deviceIndex, const std::string& path, bool useOverriddenState);

	/**
	 * @brief Returns whether the OpenVR runtime is using the overridden or natural state of a skeleton input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return True if the input is using its overridden state, false otherwise
	 */
	bool getUseOverriddenSkeletonInputState(uint32_t deviceIndex, const std::string& path);

	/**************************************************
	* @brief Pose input commands
	**************************************************/

	/**
	 * @brief Sets the overridden state of a pose input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param newInput The new overridden pose input state
	 */
	void setOverriddenPoseInputState(uint32_t deviceIndex, const std::string& path, const PoseInput newInput);

	/**
	 * @brief Returns the natural (non-overridden) state of a pose input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the pose input if successful, nullptr otherwise
	 */
	const PoseInput* getNaturalPoseInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the overridden state of a pose input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the pose input if successful, nullptr otherwise
	 */
	const PoseInput* getOverriddenPoseInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Sets whether the OpenVR runtime should use the overridden or natural state of a pose input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param useOverriddenState True if the overridden state should be used, false if the natural state should be used
	 */
	void setUseOverriddenPoseInputState(uint32_t deviceIndex, const std::string& path, bool useOverriddenState);

	/**
	 * @brief Returns whether the OpenVR runtime is using the overridden or natural state of a pose input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return True if the input is using its overridden state, false otherwise
	 */
	bool getUseOverriddenPoseInputState(uint32_t deviceIndex, const std::string& path);

	/**************************************************
	* @brief Eye tracking input commands
	**************************************************/

	/**
	 * @brief Sets the overridden state of an eye tracking input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param newInput The new overridden eye tracking input state
	 */
	void setOverriddenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path, const EyeTrackingInput newInput);

	/**
	 * @brief Returns the natural (non-overridden) state of an eye tracking input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the eye tracking input if successful, nullptr otherwise
	 */
	const EyeTrackingInput* getNaturalEyeTrackingInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the overridden state of an eye tracking input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the eye tracking input if successful, nullptr otherwise
	 */
	const EyeTrackingInput* getOverriddenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Sets whether the OpenVR runtime should use the overridden or natural state of an eye tracking input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param useOverriddenState True if the overridden state should be used, false if the natural state should be used
	 */
	void setUseOverriddenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path, bool useOverriddenState);

	/**
	 * @brief Returns whether the OpenVR runtime is using the overridden or natural state of an eye tracking input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return True if the input is using its overridden state, false otherwise
	 */
	bool getUseOverriddenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path);
};