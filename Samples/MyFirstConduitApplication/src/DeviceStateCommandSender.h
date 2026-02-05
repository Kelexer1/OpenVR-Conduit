#pragma once

#include "DeviceTypes.h"
#include "IDeviceStateEventReciever.h"

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
	 * @brief Subscribes an event reciever for live events notifications from the Conduit driver
	 * @param listener The event reciever
	 */
	void addEventListener(IDeviceStateEventReciever& listener);

	/**
	 * @brief Stops an existing event reciever from recieving live event notifications from the Conduit driver
	 * @param listener The event reciever
	 */
	void removeEventListener(IDeviceStateEventReciever& listener);

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
	 * @brief Sets the overriden state of a device pose for a device
	 * @param deviceIndex The device index of the device
	 * @param newPose The new overriden pose
	 */
	void setOverridenDevicePose(uint32_t deviceIndex, const DevicePose newPose);

	/**
	 * @brief Returns the natural (non-overriden) state of a device pose for a device
	 * @param deviceIndex The device index of the device
	 * @return A pointer to the pose if successful, nullptr otherwise
	 */
	const DevicePose* getNaturalDevicePose(uint32_t deviceIndex);

	/**
	 * @brief Returns the overriden state of a device pose for a device
	 * @param deviceIndex The device index of the device
	 * @return A pointer to the pose if successfull, nullptr otherwise
	 */
	const DevicePose* getOverridenDevicePose(uint32_t deviceIndex);

	/**
	 * @brief Sets whether the OpenVR runtime should use the overriden state, or the natural state of a devices pose
	 * @param deviceIndex The device index of the device
	 * @param useOverridenState True if the overriden state should be used, false if the natural state should be used
	 */
	void setUseOverridenDevicePose(uint32_t deviceIndex, bool useOverridenState);

	/**
	 * @brief Returns whether the OpenVR runtime is using the overriden or natural state of a devices pose
	 * @param deviceIndex The device index of the device
	 * @return True if the device is using its overriden state, false otherwise
	 */
	bool getUseOverridenDevicePose(uint32_t deviceIndex);

	/**************************************************
	* @brief Boolean input commands
	**************************************************/

	/**
	 * @brief Sets the overriden state of a boolean input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param newInput The new overriden boolean input state
	 */
	void setOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path, const BooleanInput newInput);

	/**
	 * @brief Returns the natural (non-overriden) state of a boolean input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the boolean input if successful, nullptr otherwise
	 */
	const BooleanInput* getNaturalBooleanInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the overriden state of a boolean input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the boolean input if successful, nullptr otherwise
	 */
	const BooleanInput* getOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Sets whether the OpenVR runtime should use the overriden or natural state of a boolean input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param useOverridenState True if the overriden state should be used, false if the natural state should be used
	 */
	void setUseOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState);

	/**
	 * @brief Returns whether the OpenVR runtime is using the overriden or natural state of a boolean input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return True if the input is using its overriden state, false otherwise
	 */
	bool getUseOverridenBooleanInputState(uint32_t deviceIndex, const std::string& path);

	/**************************************************
	* @brief Scalar input commands
	**************************************************/

	/**
	 * @brief Sets the overriden state of a scalar input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param newInput The new overriden scalar input state
	 */
	void setOverridenScalarInputState(uint32_t deviceIndex, const std::string& path, const ScalarInput newInput);

	/**
	 * @brief Returns the natural (non-overriden) state of a scalar input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the scalar input if successful, nullptr otherwise
	 */
	const ScalarInput* getNaturalScalarInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the overriden state of a scalar input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the scalar input if successful, nullptr otherwise
	 */
	const ScalarInput* getOverridenScalarInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Sets whether the OpenVR runtime should use the overriden or natural state of a scalar input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param useOverridenState True if the overriden state should be used, false if the natural state should be used
	 */
	void setUseOverridenScalarInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState);

	/**
	 * @brief Returns whether the OpenVR runtime is using the overriden or natural state of a scalar input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return True if the input is using its overriden state, false otherwise
	 */
	bool getUseOverridenScalarInputState(uint32_t deviceIndex, const std::string& path);

	/**************************************************
	* @brief Skeleton input commands
	**************************************************/

	/**
	 * @brief Sets the overriden state of a skeleton input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param newInput The new overriden skeleton input state
	 */
	void setOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path, const SkeletonInput newInput);

	/**
	 * @brief Returns the natural (non-overriden) state of a skeleton input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the skeleton input if successful, nullptr otherwise
	 */
	const SkeletonInput* getNaturalSkeletonInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the overriden state of a skeleton input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the skeleton input if successful, nullptr otherwise
	 */
	const SkeletonInput* getOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Sets whether the OpenVR runtime should use the overriden or natural state of a skeleton input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param useOverridenState True if the overriden state should be used, false if the natural state should be used
	 */
	void setUseOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState);

	/**
	 * @brief Returns whether the OpenVR runtime is using the overriden or natural state of a skeleton input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return True if the input is using its overriden state, false otherwise
	 */
	bool getUseOverridenSkeletonInputState(uint32_t deviceIndex, const std::string& path);

	/**************************************************
	* @brief Pose input commands
	**************************************************/

	/**
	 * @brief Sets the overriden state of a pose input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param newInput The new overriden pose input state
	 */
	void setOverridenPoseInputState(uint32_t deviceIndex, const std::string& path, const PoseInput newInput);

	/**
	 * @brief Returns the natural (non-overriden) state of a pose input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the pose input if successful, nullptr otherwise
	 */
	const PoseInput* getNaturalPoseInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the overriden state of a pose input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the pose input if successful, nullptr otherwise
	 */
	const PoseInput* getOverridenPoseInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Sets whether the OpenVR runtime should use the overriden or natural state of a pose input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param useOverridenState True if the overriden state should be used, false if the natural state should be used
	 */
	void setUseOverridenPoseInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState);

	/**
	 * @brief Returns whether the OpenVR runtime is using the overriden or natural state of a pose input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return True if the input is using its overriden state, false otherwise
	 */
	bool getUseOverridenPoseInputState(uint32_t deviceIndex, const std::string& path);

	/**************************************************
	* @brief Eye tracking input commands
	**************************************************/

	/**
	 * @brief Sets the overriden state of an eye tracking input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param newInput The new overriden eye tracking input state
	 */
	void setOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path, const EyeTrackingInput newInput);

	/**
	 * @brief Returns the natural (non-overriden) state of an eye tracking input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the eye tracking input if successful, nullptr otherwise
	 */
	const EyeTrackingInput* getNaturalEyeTrackingInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the overriden state of an eye tracking input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return A pointer to the eye tracking input if successful, nullptr otherwise
	 */
	const EyeTrackingInput* getOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Sets whether the OpenVR runtime should use the overriden or natural state of an eye tracking input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @param useOverridenState True if the overriden state should be used, false if the natural state should be used
	 */
	void setUseOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path, bool useOverridenState);

	/**
	 * @brief Returns whether the OpenVR runtime is using the overriden or natural state of an eye tracking input
	 * @param deviceIndex The device index of the device
	 * @param path The input path of the input
	 * @return True if the input is using its overriden state, false otherwise
	 */
	bool getUseOverridenEyeTrackingInputState(uint32_t deviceIndex, const std::string& path);
};