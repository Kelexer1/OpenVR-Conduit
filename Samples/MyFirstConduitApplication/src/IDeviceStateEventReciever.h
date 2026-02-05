#pragma once
#include <stdint.h>
#include <string>

#include "DeviceTypes.h"

/**
 * @brief An interface for a state event reciever. Should be inherited and implemented by client apps. Listeners must be
 * registered using a DeviceStateCommandSender to recieve updates from the Conduit driver
 */
class IDeviceStateEventReciever {
public:
	/**
	 * @brief Notification that a boolean input was activated
	 * @param deviceIndex The index of the device that has the input
	 * @param path The path of the input being added
	 */
	virtual void DeviceInputBooleanAdded(uint32_t deviceIndex, const std::string& path) = 0;

	/**
	 * @brief Notification that a boolean input was deactivated
	 * @param deviceIndex The index of the device that had the input
	 * @param path The path of the input being removed
	 */
	virtual void DeviceInputBooleanRemoved(uint32_t deviceIndex, const std::string& path) = 0;

	/**
	 * @brief Notification that a scalar input was added
	 * @param deviceIndex The index of the device that has the input
	 * @param path The path of the input being added
	 */
	virtual void DeviceInputScalarAdded(uint32_t deviceIndex, const std::string& path) = 0;

	/**
	 * @brief Notification that a scalar input was removed
	 * @param deviceIndex The index of the device that had the input
	 * @param path The path of the input being removed
	 */
	virtual void DeviceInputScalarRemoved(uint32_t deviceIndex, const std::string& path) = 0;

	/**
	 * @brief Notification that a skeleton input was added
	 * @param deviceIndex The index of the device that has the input
	 * @param path The path of the input being added
	 */
	virtual void DeviceInputSkeletonAdded(uint32_t deviceIndex, const std::string& path) = 0;

	/**
	 * @brief Notification that a skeleton input was removed
	 * @param deviceIndex The index of the device that had the input
	 * @param path The path of the input being removed
	 */
	virtual void DeviceInputSkeletonRemoved(uint32_t deviceIndex, const std::string& path) = 0;

	/**
	 * @brief Notification that a pose input was added
	 * @param deviceIndex The index of the device that has the input
	 * @param path The path of the input being added
	 */
	virtual void DeviceInputPoseAdded(uint32_t deviceIndex, const std::string& path) = 0;

	/**
	 * @brief Notification that a pose input was removed
	 * @param deviceIndex The index of the device that had the input
	 * @param path The path of the input being removed
	 */
	virtual void DeviceInputPoseRemoved(uint32_t deviceIndex, const std::string& path) = 0;

	/**
	 * @brief Notification that an eye tracking input was added
	 * @param deviceIndex The index of the device that has the input
	 * @param path The path of the input being added
	 */
	virtual void DeviceInputEyeTrackingAdded(uint32_t deviceIndex, const std::string& path) = 0;

	/**
	 * @brief Notification that an eye tracking input was removed
	 * @param deviceIndex The index of the device that had the input
	 * @param path The path of the input being removed
	 */
	virtual void DeviceInputEyeTrackingRemoved(uint32_t deviceIndex, const std::string& path) = 0;

	/**
	 * @brief Notification that a device's pose has changed
	 * @param deviceIndex The index of the device whose pose changed
	 * @param oldPose The previous pose of the device
	 * @param newPose The updated pose of the device
	 */
	virtual void DevicePoseChanged(uint32_t deviceIndex, DevicePose oldPose, DevicePose newPose) = 0;

	/**
	 * @brief Notification that a boolean input value has changed
	 * @param deviceIndex The index of the device with the input
	 * @param path The path of the input that changed
	 * @param oldInput The previous state of the boolean input
	 * @param newInput The updated state of the boolean input
	 */
	virtual void DeviceInputBooleanChanged(uint32_t deviceIndex, std::string path, BooleanInput oldInput, BooleanInput newInput) = 0;

	/**
	 * @brief Notification that a scalar input value has changed
	 * @param deviceIndex The index of the device with the input
	 * @param path The path of the input that changed
	 * @param oldInput The previous state of the scalar input
	 * @param newInput The updated state of the scalar input
	 */
	virtual void DeviceInputScalarChanged(uint32_t deviceIndex, std::string path, ScalarInput oldInput, ScalarInput newInput) = 0;

	/**
	 * @brief Notification that a skeleton input value has changed
	 * @param deviceIndex The index of the device with the input
	 * @param path The path of the input that changed
	 * @param oldInput The previous state of the skeleton input
	 * @param newInput The updated state of the skeleton input
	 */
	virtual void DeviceInputSkeletonChanged(uint32_t deviceIndex, std::string path, SkeletonInput oldInput, SkeletonInput newInput) = 0;

	/**
	 * @brief Notification that a pose input value has changed
	 * @param deviceIndex The index of the device with the input
	 * @param path The path of the input that changed
	 * @param oldInput The previous state of the pose input
	 * @param newInput The updated state of the pose input
	 */
	virtual void DeviceInputPoseChanged(uint32_t deviceIndex, std::string path, PoseInput oldInput, PoseInput newInput) = 0;

	/**
	 * @brief Notification that an eye tracking input value has changed
	 * @param deviceIndex The index of the device with the input
	 * @param path The path of the input that changed
	 * @param oldInput The previous state of the eye tracking input
	 * @param newInput The updated state of the eye tracking input
	 */
	virtual void DeviceInputEyeTrackingChanged(uint32_t deviceIndex, std::string path, EyeTrackingInput oldInput, EyeTrackingInput newInput) = 0;
};