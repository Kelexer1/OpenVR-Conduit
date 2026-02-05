#pragma once
#include "ObjectSchemas.h"
#include "IDeviceStateEventReciever.h"

#include <unordered_map>
#include <string>

/**
 * @brief Represents the internal state of all inputs (and poses) as described by the Conduit driver,
 * using device index and input path to map to unique inputs
 */
class DeviceStateModelClient {
public:
	/**
	 * @brief Returns the singleton instance of the DeviceStateModelClient class
	 * @return The singleton
	 */
	static DeviceStateModelClient& getInstance();

	/**
	 * @brief Subscribes an event reciever to state updates from the model
	 * @param listener The event reciever
	 */
	void addEventListener(const IDeviceStateEventReciever& listener);

	/**
	 * @brief Stops an event reciever from recieving state updates from the model.
	 * Does nothing if <listener> is not added beforehand
	 * @param listener The event reciever
	 */
	void removeEventListener(const IDeviceStateEventReciever& listener);

	/**
	 * @brief Notifies all listeners that an input was registered
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 * @param type The type of input that is being registered
	 */
	void notifyListenersInputAdded(uint32_t deviceIndex, const std::string& path, ObjectType type);

	/**
	 * @brief Notifies all listeners that an input was removed
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 * @param type The type of input that is being removed
	 */
	void notifyListenersInputRemoved(uint32_t deviceIndex, const std::string& path, ObjectType type);

	/**
	 * @brief Returns the modelled state of a device pose for a device
	 * @param deviceIndex The device index of the device
	 * @return A pointer to the modelled state if <deviceIndex> is associated with a valid pose, nullptr otherwise
	 */
	ModelDevicePoseSerialized* getDevicePose(uint32_t deviceIndex);

	/**
	 * @brief Registers a new device pose to the model
	 * @param deviceIndex The device index of the device
	 */
	void addDevicePose(uint32_t deviceIndex);

	/**
	 * @brief Removes an existing device pose from the model, if it exists
	 * @param deviceIndex The device index of the device
	 */
	void removeDevicePose(uint32_t deviceIndex);

	/**
	 * @brief Notifies all listeners that the natural state of a device pose has changed
	 * @param deviceIndex The device index of the device
	 * @param oldPose The old natural state of the pose
	 * @param newPose The new natural state of the pose
	 */
	void notifyListenersDevicePoseUpdated(
		uint32_t deviceIndex,
		const DevicePoseSerialized oldPose,
		const DevicePoseSerialized newPose
	);

	/**
	 * @brief Returns the modelled state of a boolean input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 * @return A pointer to the modelled state if the input exists, nullptr otherwise
	 */
	ModelDeviceInputBooleanSerialized* getBooleanInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Registers a new boolean input to the model
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void addBooleanInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Removes an existing boolean input from the model, if it exists
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void removeBooleanInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Notifies all listeners that the state of a boolean input has changed
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 * @param oldInput The old state of the input
	 * @param newInput The new state of the input
	 */
	void notifyListenersBooleanInputUpdated(
		uint32_t deviceIndex,
		const std::string& path,
		const DeviceInputBooleanSerialized oldInput,
		const DeviceInputBooleanSerialized newInput
	);

	/**
	 * @brief Returns the modelled state of a scalar input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 * @return A pointer to the modelled state if the input exists, nullptr otherwise
	 */
	ModelDeviceInputScalarSerialized* getScalarInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Registers a new scalar input to the model
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void addScalarInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Removes an existing scalar input from the model, if it exists
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void removeScalarInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Notifies all listeners that the state of a scalar input has changed
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 * @param oldInput The old state of the input
	 * @param newInput The new state of the input
	 */
	void notifyListenersScalarInputUpdated(uint32_t deviceIndex,
		const std::string& path,
		const DeviceInputScalarSerialized oldInput,
		const DeviceInputScalarSerialized newInput
	);

	/**
	 * @brief Returns the modelled state of a skeleton input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 * @return A pointer to the modelled state if the input exists, nullptr otherwise
	 */
	ModelDeviceInputSkeletonSerialized* getSkeletonInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Registers a new skeleton input to the model
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void addSkeletonInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Removes an existing skeleton input from the model, if it exists
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void removeSkeletonInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Notifies all listeners that the state of a skeleton input has changed
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 * @param oldInput The old state of the input
	 * @param newInput The new state of the input
	 */
	void notifyListenersSkeletonInputUpdated(uint32_t deviceIndex,
		const std::string& path,
		const DeviceInputSkeletonSerialized oldInput,
		const DeviceInputSkeletonSerialized newInput
	);

	/**
	 * @brief Returns the modelled state of a pose input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 * @return A pointer to the modelled state if the input exists, nullptr otherwise
	 */
	ModelDeviceInputPoseSerialized* getPoseInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Registers a new pose input to the model
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void addPoseInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Removes an existing pose input from the model, if it exists
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void removePoseInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Notifies all listeners that the state of a pose input has changed
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 * @param oldInput The old state of the input
	 * @param newInput The new state of the input
	 */
	void notifyListenersPoseInputUpdated(uint32_t deviceIndex,
		const std::string& path,
		const DeviceInputPoseSerialized oldInput,
		const DeviceInputPoseSerialized newInput
	);

	/**
	 * @brief Returns the modelled state of an eye tracking input for a device
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 * @return A pointer to the modelled state if the input exists, nullptr otherwise
	 */
	ModelDeviceInputEyeTrackingSerialized* getEyeTrackingInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Registers a new eye tracking input to the model
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void addEyeTrackingInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Removes an existing eye tracking input from the model, if it exists
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 */
	void removeEyeTrackingInput(uint32_t deviceIndex, const std::string& path);

	/**
	 * @brief Notifies all listeners that the state of an eye tracking input has changed
	 * @param deviceIndex The device index of the device
	 * @param path The path of the input
	 * @param oldInput The old state of the input
	 * @param newInput The new state of the input
	 */
	void notifyListenersEyeTrackingInputUpdated(
		uint32_t deviceIndex,
		const std::string& path,
		const DeviceInputEyeTrackingSerialized oldInput,
		const DeviceInputEyeTrackingSerialized newInput
	);
private:
	/** @brief Collection of registered event listeners */
	std::vector<IDeviceStateEventReciever*> eventListeners;

	/** @brief Maps device indices to their pose states */
	std::unordered_map<uint32_t, ModelDevicePoseSerialized> devicePoses;

	/** @brief Maps device indices and paths to boolean input states */
	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputBooleanSerialized>> booleanInputs;

	/** @brief Maps device indices and paths to scalar input states */
	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputScalarSerialized>> scalarInputs;

	/** @brief Maps device indices and paths to skeleton input states */
	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputSkeletonSerialized>> skeletonInputs;

	/** @brief Maps device indices and paths to pose input states */
	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputPoseSerialized>> poseInputs;

	/** @brief Maps device indices and paths to eye tracking input states */
	std::unordered_map<uint32_t, std::unordered_map<std::string, ModelDeviceInputEyeTrackingSerialized>> eyeTrackingInputs;

	/** @brief Private constructor for singleton pattern */
	DeviceStateModelClient() = default;
};