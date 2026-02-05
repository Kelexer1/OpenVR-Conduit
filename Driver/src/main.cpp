#include "main.h"

Main& Main::getInstance() {
	static Main instance;
	return instance;
}

void Main::main() {
	double POLL_PERIOD_MICROSECONDS = 1000000.0 / POLL_RATE;

	while (true) {
		SharedDeviceMemoryDriver::getInstance().pollForClientUpdates();

		this->pollEvents();

		std::this_thread::sleep_for(std::chrono::microseconds((int)POLL_PERIOD_MICROSECONDS));
	}
}

void Main::pollEvents() {
	vr::VREvent_t event;
	if (vr::VRServerDriverHost()->PollNextEvent(&event, sizeof(vr::VREvent_t))) {
		switch (event.eventType) {
		case vr::VREvent_TrackedDeviceActivated:
			break;

		case vr::VREvent_TrackedDeviceDeactivated:
			break;
		}
	}
}