#pragma once
#include "openvr_driver.h"

#include "DeviceStateModelDriver.h"

#include <thread>
#include <chrono>

/**  
 * @brief Handles the main polling loop for client updates and events
 */
class Main {
public:
	/**
	 * @brief Returns the singleton instance of the Main class
	 * @return The singleton instance
	 */
	static Main& getInstance();

	/** 
	 * @brief The main loop, to be ran in a separate thread 
	 */
	void main();

private:
	/**
	 * @brief Checks for connect and disconnect events from OpenVR, currently unused
	 */
	void pollEvents();
};