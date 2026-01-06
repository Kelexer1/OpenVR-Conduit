#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <Windows.h>
#include <MinHook.h>
#include <openvr_driver.h>

/**
 * @brief Integer offsets from the IVRServerDriverHost interface vtable.
 * 
 * These offsets correspond to the ordering of the methods as found in <openvr_driver.h> 
 * within the IVRServerDriverHost class. For example, TrackedDevicePoseUpdated() is the
 * second entry under IVRServerDriverHost, so it's index is 1
 */
enum VTableOffsets_IVRServerDriverHost {
	Offset_TrackedDevicePoseUpdated = 1		// IVRServerDriverHost->TrackedDevicePoseUpdated()
};

enum VTableOffsets_IVRDriverInput {
	Offset_CreateBooleanComponent = 0,		// IVRDriverInput->CreateBooleanComponent()
	Offset_UpdateBooleanComponent = 1,		// IVRDriverInput->UpdateBooleanComponent()
	Offset_CreateScalarComponent = 2,		// IVRDriverInput->CreateScalarComponent()
	Offset_UpdateScalarComponent = 3,		// IVRDriverInput->UpdateScalarComponent()
	Offset_CreateHapticComponent = 4,		// IVRDriverInput->CreateHapticComponent()
	Offset_CreateSkeletonComponent = 5,		// IVRDriverInput->CreateSkeletonComponent()
	Offset_UpdateSkeletonComponent = 6,		// IVRDriverInput->UpdateSkeletonComponent()
	Offset_CreatePoseComponent = 7,			// IVRDriverInput->CreatePoseComponent()
	Offset_UpdatePoseComponent = 8,			// IVRDriverInput->UpdatePoseComponent()
	Offset_CreateEyeTrackingComponent = 9,	// IVRDriverInput->CreateEyeTrackingComponent()
	Offset_UpdateEyeTrackingComponent = 10	// IVRDriverInput->UpdateEyeTrackingComponent()
};

/**
 * @brief Manages OpenVR method hooks throughout the driver lifespan
 */
class HookManager {
public:
	/**
	 * @brief Initializes MinHook
	 */
	static void initializeHooks();

	/**
	 * @brief Initializes MinHook and hooks to all required methods under IVRServerDriverHost
	 * @param host A pointer to the IVRServerDriverHost interface
	 */
	static void setupHooks_IVRServerDriverHost(void* host);

	/**
	 * @brief Initializes MinHook and hooks to all required methods under IVRDriverInput
	 * @param input A pointer to the IVRDriverInput interface
	 */
	static void setupHooks_IVRDriverInput(void* input);

	/**
	 * @brief Gracefully shuts down MinHook in the event of an error
	 */
	static void shutdown(std::string reason);

	/**
	 * @brief Frees the associated module and exits the thread
	 */
	static DWORD __stdcall ejectThread(LPVOID);

	/**
	 * @brief Sets the associated hModule
	 */
	static void setHModule(HMODULE hModule);
private:
	/// The associated hModule
	static HMODULE hModule;

	static void dumpVTableFromHost(void* host);

	static void createHook(void* targetFunction, void* detourFunction, void** originalFunction, const std::string& name);
};