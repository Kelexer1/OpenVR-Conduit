#pragma once
#include <openvr_driver.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "SharedDeviceMemoryDriver.h"
#include "ObjectSchemas.h"
#include "HookFunctions.h"

/**
 * @brief Represents the internal state of all devices for the Conduit driver, including mapping to internal OpenVR
 * handles, overwritten states, and natural states
 */
class DeviceStateModel {
public:
	/**
	 * @brief Returns the singleton instance of the DeviceStateModel class
	 * @return The singleton
	 */
	static DeviceStateModel& getInstance();

	/**************************************************
	* @brief Index To Property Container management
	**************************************************/

	/**
	 * @brief Registers a mapping between a device index and an OpenVR PropertyContainerHandle_t
	 * @param deviceIndex The device index of the device
	 * @param propertyContainer The associated PropertyContainerHandle_t
	 */
	void addDeviceIndexToContainerMapping(uint32_t deviceIndex, vr::PropertyContainerHandle_t propertyContainer);

	/**
	 * @brief Removes all mappings between a device index and an OpenVR PropertyContainerHandle_t
	 * @param deviceIndex The device index of the device
	 */
	void removeDeviceIndexToContainerMapping(uint32_t deviceIndex);

	/**
	 * @brief Returns the device index associated with an OpenVR PropertyContainerHandle_t
	 * @param propertyContainer The PropertyContainer_t associated to a device index
	 * @return The device index if successful, UINT32_MAX otherwise
	 */
	uint32_t getDeviceIndexFromPropertyContainer(vr::PropertyContainerHandle_t propertyContainer);

	/**
	 * @brief Returns the OpenVR PropertyContainerHandle_t associated with a device index
	 * @param deviceIndex The device index of the device
	 * @return A pointer to the PropertyContainerHandle_t if successful, nullptr otherwise
	 */
	vr::PropertyContainerHandle_t* getPropertyContainerFromDeviceIndex(uint32_t deviceIndex);

	/**************************************************
	* @brief Device Poses
	**************************************************/

	/**
	 * @brief Returns the device pose state of a device
	 * @param deviceIndex The device index of the device
	 * @return A pointer to the pose data if successfull, nullptr otherwise
	 */
	ModelDevicePoseSerialized* getDevicePose(uint32_t deviceIndex);

	/**
	 * @brief Notifies the model and event listeners that a device pose has changed
	 * @param deviceIndex The device index of the device whose pose changed
	 */
	void setDevicePoseChanged(uint32_t deviceIndex);

	/**
	 * @brief Registers a new device pose
	 * @param deviceIndex The device index of the device
	 */
	void addDevicePose(uint32_t deviceIndex);

	/**
	 * @brief Removes an existing device pose, if it exists
	 * @param deviceIndex The device index of the device
	 */
	void removeDevicePose(uint32_t deviceIndex);

	/**************************************************
	* @brief Boolean Inputs
	**************************************************/

	/**
	 * @brief Returns the boolean input associated with a deviceIndex and input path
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/trigger/click')
	 */
	ModelDeviceInputBooleanSerialized* getBooleanInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the boolean input associated with a component handle
	 * @param componentHandle The component handle
	 * @return A pointer to the serialized input if successful, nullptr otherwise
	 */
	ModelDeviceInputBooleanSerialized* getBooleanInput(vr::VRInputComponentHandle_t componentHandle);

	/**
	 * @brief Notifies the model and event listeners that the boolean input associated with the deviceIndex and path
	 * has changed
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/trigger/click')
	 */
	void setInputBooleanChanged(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Notifies the model and event listeners that the boolean input associated with the component handle has
	 * changed
	 * @param componentHandle The component handle
	 */
	void setInputBooleanChanged(vr::VRInputComponentHandle_t componentHandle);

	/**
	 * @brief Registers a new boolean input
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/trigger/click')
	 * @param componentHandle The component handle of the input
	 */
	void addBooleanInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle);

	/**
	 * @brief Removes an existing boolean input, if it exists
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/trigger/click')
	 */
	void removeBooleanInput(uint32_t deviceIndex, const std::string& path);

	/**************************************************
	* @brief Scalar Inputs
	**************************************************/

	/**
	 * @brief Returns the scalar input associated with a deviceIndex and input path
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/trigger/value')
	 * @return A pointer to the serialized input if successful, nullptr otherwise
	 */
	ModelDeviceInputScalarSerialized* getScalarInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the scalar input associated with a component handle
	 * @param componentHandle The component handle
	 * @return A pointer to the serialized input if successful, nullptr otherwise
	 */
	ModelDeviceInputScalarSerialized* getScalarInput(vr::VRInputComponentHandle_t componentHandle);

	/**
	 * @brief Notifies the model and event listeners that the scalar input associated with the deviceIndex and path
	 * has changed
	 * @param index The device index of the device
	 * @param path The path of the input (ex. '/input/trigger/value')
	 */
	void setInputScalarChanged(uint32_t index, const std::string& path);

	/**
	 * @brief Notifies the model and event listeners that the scalar input associated with the component handle has
	 * changed
	 * @param componentHandle The component handle
	 */
	void setInputScalarChanged(vr::VRInputComponentHandle_t componentHandle);

	/**
	 * @brief Registers a new scalar input
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/trigger/value')
	 * @param componentHandle The component handle of the input
	 */
	void addScalarInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle);

	/**
	 * @brief Removes an existing scalar input, if it exists
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/trigger/value')
	 */
	void removeScalarInput(uint32_t deviceIndex, const std::string& path);
	
	/**************************************************
	* @brief Skeleton Inputs
	**************************************************/

	/**
	 * @brief Returns the skeleton input associated with a deviceIndex and input path
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/skeleton/left')
	 * @return A pointer to the serialized input if successful, nullptr otherwise
	 */
	ModelDeviceInputSkeletonSerialized* getSkeletonInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the skeleton input associated with a component handle
	 * @param componentHandle The component handle
	 * @return A pointer to the serialized input if successful, nullptr otherwise
	 */
	ModelDeviceInputSkeletonSerialized* getSkeletonInput(vr::VRInputComponentHandle_t componentHandle);

	/**
	 * @brief Notifies the model and event listeners that the skeleton input associated with the deviceIndex and path
	 * has changed
	 * @param index The device index of the device
	 * @param path The path of the input (ex. '/input/skeleton/left')
	 */
	void setInputSkeletonChanged(uint32_t index, const std::string& path);

	/**
	 * @brief Notifies the model and event listeners that the skeleton input associated with the component handle has
	 * changed
	 * @param componentHandle The component handle
	 */
	void setInputSkeletonChanged(vr::VRInputComponentHandle_t componentHandle);

	/**
	 * @brief Registers a new skeleton input
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/skeleton/left')
	 * @param componentHandle The component handle of the input
	 */
	void addSkeletonInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle);

	/**
	 * @brief Removes an existing skeleton input, if it exists
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/skeleton/left')
	 */
	void removeSkeletonInput(uint32_t deviceIndex, const std::string& path);

	/**************************************************
	* @brief Pose Inputs
	**************************************************/

	/**
	 * @brief Returns the pose input associated with a deviceIndex and input path
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/grip/pose')
	 * @return A pointer to the serialized input if successful, nullptr otherwise
	 */
	ModelDeviceInputPoseSerialized* getPoseInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the pose input associated with a component handle
	 * @param componentHandle The component handle
	 * @return A pointer to the serialized input if successful, nullptr otherwise
	 */
	ModelDeviceInputPoseSerialized* getPoseInput(vr::VRInputComponentHandle_t componentHandle);

	/**
	 * @brief Notifies the model and event listeners that the pose input associated with the deviceIndex and path
	 * has changed
	 * @param index The device index of the device
	 * @param path The path of the input (ex. '/input/grip/pose')
	 */
	void setInputPoseChanged(uint32_t index, const std::string& path);

	/**
	 * @brief Notifies the model and event listeners that the pose input associated with the component handle has
	 * changed
	 * @param componentHandle The component handle
	 */
	void setInputPoseChanged(vr::VRInputComponentHandle_t componentHandle);

	/**
	 * @brief Registers a new pose input
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/grip/pose')
	 * @param componentHandle The component handle of the input
	 */
	void addPoseInput(uint32_t deviceIndex, const std::string& path, vr::VRInputComponentHandle_t* componentHandle);

	/**
	 * @brief Removes an existing pose input, if it exists
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/grip/pose')
	 */
	void removePoseInput(uint32_t deviceIndex, const std::string& path);

	/**************************************************
	* @brief Eye Tracking Inputs
	**************************************************/

	/**
	 * @brief Returns the eye tracking input associated with a deviceIndex and input path
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/eyes')
	 * @return A pointer to the serialized input if successful, nullptr otherwise
	 */
	ModelDeviceInputEyeTrackingSerialized* getEyeTrackingInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Returns the eye tracking input associated with a component handle
	 * @param componentHandle The component handle
	 * @return A pointer to the serialized input if successful, nullptr otherwise
	 */
	ModelDeviceInputEyeTrackingSerialized* getEyeTrackingInput(vr::VRInputComponentHandle_t componentHandle);

	/**
	 * @brief Notifies the model and event listeners that the eye tracking input associated with the deviceIndex and
	 * path has changed
	 * @param index The device index of the device
	 * @param path The path of the input (ex. '/input/eyes')
	 */
	void setInputEyeTrackingChanged(uint32_t index, const std::string& path);

	/**
	 * @brief Notifies the model and event listeners that the eye tracking input associated with the component handle
	 * has changed
	 * @param componentHandle The component handle
	 */
	void setInputEyeTrackingChanged(vr::VRInputComponentHandle_t componentHandle);

	/**
	 * @brief Registers a new eye tracking input
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/eyes')
	 * @param componentHandle The component handle of the input
	 */
	void addEyeTrackingInput(uint32_t deviceIndex, const std::string& path, 
		vr::VRInputComponentHandle_t* componentHandle
	);

	/**
	 * @brief Removes an existing eye tracking input, if it exists
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input (ex. '/input/eyes')
	 */
	void removeEyeTrackingInput(uint32_t deviceIndex, const std::string& path);

private:
	/** @brief Maps device indexes to unique PropertyContainerHandle_t's */
	std::unordered_map<uint32_t, vr::PropertyContainerHandle_t> indexTable;

	/** @brief Maps device indexes to device pose states */
	std::unordered_map<uint32_t, ModelDevicePoseSerialized> devicePoses;

	/** @brief Maps device indexes and paths to both the associated component handle and boolean input */
	std::unordered_map<uint32_t, 
		std::unordered_map<std::string, 
			std::pair<vr::VRInputComponentHandle_t, ModelDeviceInputBooleanSerialized>
		>
	> booleanInputs;

	/** @brief Maps device indexes and paths to both the associated component handle and scalar input */
	std::unordered_map<uint32_t, 
		std::unordered_map<std::string, 
			std::pair<vr::VRInputComponentHandle_t, ModelDeviceInputScalarSerialized>
		>
	> scalarInputs;

	/** @brief Maps device indexes and paths to both the associated component handle and skeleton input */
	std::unordered_map<uint32_t, 
		std::unordered_map<std::string, 
			std::pair<vr::VRInputComponentHandle_t, ModelDeviceInputSkeletonSerialized>
		>
	> skeletonInputs;

	/** @brief Maps device indexes and paths to both the associated component handle and pose input */
	std::unordered_map<uint32_t, 
		std::unordered_map<std::string, 
			std::pair<vr::VRInputComponentHandle_t, ModelDeviceInputPoseSerialized>
		>
	> poseInputs;

	/** @brief Maps device indexes and paths to both the associated component handle and eye tracking input */
	std::unordered_map<uint32_t, 
		std::unordered_map<std::string, 
			std::pair<vr::VRInputComponentHandle_t, ModelDeviceInputEyeTrackingSerialized>
		>
	> eyeTrackingInputs;

	/** 
	 * @brief Default constructor for DeviceStateModel, private to prevent instantiation
	 */
	DeviceStateModel() = default;
};