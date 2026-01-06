#pragma once
#include <openvr_driver.h>
#include <MinHook.h>

#include "HookManager.h"

/**
 * Initializes the Conduit driver as a SteamVR driver
 */
class DeviceProvider : public vr::IServerTrackedDeviceProvider {
public:
	/**
	 * @brief Initializes the Conduit driver, shared memory, and controllers
	 * @return The error code (usually success)
	 */
	virtual vr::EVRInitError Init(vr::IVRDriverContext* pDriverContext) override;

	/**
	 * @brief Uninitializes MinHook gracefully when the driver is shutting down
	 */
	virtual void Cleanup() override;

	///=========================
	/// Empty Functions
	///=========================

	virtual const char* const* GetInterfaceVersions();

	virtual void RunFrame() override;

	virtual bool ShouldBlockStandbyMode() override;
	virtual void EnterStandby() override;
	virtual void LeaveStandby() override;
};