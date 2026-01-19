#pragma once
#include "openvr_driver.h"

#include "DeviceStateModelDriver.h"

#include <thread>
#include <chrono>

class Main {
public:
	static Main& getInstance();

	void main(); // Main loop

private:
	void pollEvents();
};