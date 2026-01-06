#pragma once
#include <Windows.h>
#include <vector>
#include <string>
#include <atomic>
#include <cstdint>
#include <cstring>
#include <thread>

#include "DeviceTypes.h"
#include "IDevicePoseEventListener.h"

constexpr const char* SHM_NAME = "Local\\Conduit_Buffer";
constexpr const char* SHM_MUTEX_NAME = "Local\\Conduit_Buffer_Mutex";
constexpr size_t MAX_POSE_SLOTS = 128;
constexpr size_t MAX_INPUT_SLOTS = 128;

/** 
 * @brief Represents a single devices state 
 */
struct SharedDevicePoseSlot {
	uint32_t deviceIndex;

	DevicePose overridenPose;
	DevicePose naturalPose;

	bool useCustomPose;
	bool usePingPong = false;

	std::atomic<bool> requestPoseUpdate;
	std::atomic<bool> respondPoseUpdate;

	bool valid;

	std::atomic<int32_t> naturalPoseUpdateCounter;
};

struct SharedDeviceInputSlot {
	uint32_t deviceIndex;
	char inputName[128];

};

/**
 * @brief Represents the shared memory for all devices
 * 
 * Uses array indexing corresponding to device index
 */
struct SharedDeviceMemory {
	SharedDevicePoseSlot poseSlots[MAX_POSE_SLOTS];
	SharedDeviceInputSlot inputSlots[MAX_POSE_SLOTS];
};

/**
 * @brief Manages reading and writing the shared memory
 * 
 * Thread safe
 */
class SharedDeviceMemoryIO {
public:
	SharedDeviceMemoryIO(IDevicePoseEventListener* eventListener);
	~SharedDeviceMemoryIO();

	/**
	 * @brief Initializes the shared memory buffer
	 * 
	 * This method must be called before any read/write operations occur
	 * 
	 * @param isDriver Whether the current application is the driver, should NOT be true for any client applications
	 * @return True if initialization succeeded, false if it failed
	 */
	bool initialize(bool isDriver = false);

	/**
	 * @brief Writes the overriden pose for a device
	 * @param deviceIndex The index of the device
	 * @param pose The new pose for the device
	 */
	void writeOverridenPose(uint32_t deviceIndex, const DevicePose& pose);

	/**
	 * @brief Reads the overriden pose for a device
	 * @param deviceIndex The index of the device
	 * @param outPose The pose object that should be filled with the output
	 * @return True if the operation succeeded, false if it failed
	 */
	bool readOverridenPose(uint32_t deviceIndex, DevicePose& outPose) const;

	/**
	 * @brief Writes the natural pose for a device
	 * 
	 * This method should not be called by the client, it will be automatically updated by the driver
	 * 
	 * @param deviceIndex The index of the device
	 * @param pose The new pose for the device
	 */
	void writeNaturalPose(uint32_t deviceIndex, const DevicePose& pose);

	/**
	 * @brief Reads the natural pose for a device
	 * @param deviceIndex The index of the device
	 * @param outPose The pose object that should be filled with the output
	 * @return True if the operation succeeded, false if it failed
	 */
	bool readNaturalPose(uint32_t deviceIndex, DevicePose& outPose) const;

	/**
	 * @brief Writes whether a device should use its overriden or natural pose
	 * @param deviceIndex The index of the device
	 * @param useCustomPose True if the device should use its overriden pose, false if it should use its natural pose
	 */
	void writeUseCustomPose(uint32_t deviceIndex, const bool useCustomPose);

	/**
	 * @brief Reads whether a device should use its overriden or natural pose
	 * @param deviceIndex The index of the device
	 * @param outUseCustomPose The bool object that should be filled with the output
	 * @return True if the operation succeeded, false if it failed
	 */
	bool readUseCustomPose(uint32_t deviceIndex, bool& outUseCustomPose) const;

	/**
	 * @brief Reads whether a device shared memory slot is active
	 * @param deviceIndex The index of the device
	 * @param outIsOccupied The bool object that should be filled with the output
	 * @return True if the operation succeeded, false if it failed
	 */
	bool isDeviceSlotActive(uint32_t deviceIndex, bool& outIsOccupied) const;

	/**
	 * @brief Reads the natural pose update count for a device
	 * @param deviceIndex The index of the device
	 * @param outCounter The int32_t object that should be filled with the output
	 * @return True if the operation succeeded, false if it failed
	 */
	bool readDeviceNaturalPoseCounter(uint32_t deviceIndex, int32_t& outCounter);

	/**
	 * @brief Reads whether a device should use a ping-pong style pose update system
	 * @param deviceIndex The index of the device
	 * @param outUsePingPong The bool object that should be filled with the output
	 * @return True if the operation succeeded, false if it failed
	 */
	bool readUsePingPong(uint32_t deviceIndex, bool& outUsePingPong);

	/**
	 * @brief Writes whether a device should use ping-pong communication to override poses
	 * @param deviceIndex The index of the device
	 * @param usePingPong True if the device should use ping-pong communication, false if it should not
	 */
	void writeUsePingPong(uint32_t deviceIndex, const bool usePingPong);

	/**
	 * @brief Manages a ping-pong round of communication between driver and client
	 * 
	 * The driver requests a pose update, and waits until the client provides one to apply it
	 * This method should not be called by the client
	 */
	DevicePose pingPong(uint32_t deviceIndex);

	/**
	 * @brief Checks for any relavent updates and notifies the event listener
	 */
	void pollForUpdates();

	/**
	 * @brief Returns a pointer to the raw shared memory
	 * 
	 * This method should not be used unless nesessary, it is not thread safe
	 * 
	 * @return A pointer to the raw shared memory
	 */
	SharedDeviceMemory* getSharedDeviceMemory() const;

private:
	/// The shared memory handle
	HANDLE hMapFile;

	/// The mutex handle
	HANDLE hMutex;

	/// A pointer to the shared memory struct
	SharedDeviceMemory* sharedMemory;

	/// The event reciever that should be notified when required
	IDevicePoseEventListener* eventListener;

	/// The thread that checks for updates
	std::thread pollThread;

	/// A flag for signalling that the polling thread should exit
	std::atomic<bool> pollExitRequested;
};