#include "DeviceStateCommandSender.h"
#include "StateEventReceiver.h"
#include <iostream>
#include <chrono>
#include <thread>

int main() {
    vr::EVRInitError error = vr::VRInitError_None;
    vr::IVRSystem* vr = vr::VR_Init(&error, vr::VRApplication_Background);

    if (error != vr::VRInitError_None) {
        std::cout << "Failed to initialize OpenVR: " << vr::VR_GetVRInitErrorAsEnglishDescription(error) << "\n";
        while (true) {}
    }

    DeviceStateCommandSender commandSender;
    int conduitInitCode = commandSender.initialize();
    if (conduitInitCode != 0) {
        std::cout << "Failed to initialize Conduit with code " << conduitInitCode;
        while (true) {} // Prevent console from auto closing
    }

    StateEventReceiver eventReceiver(commandSender);
    commandSender.addEventListener(eventReceiver);
    
    std::cout << "Started live modification of devices, controllers should have positions fixed at the origin. " <<
                 "This is a silly and trivial sample application, though it opens up possibilities for endless " <<
                 "applications in categories such as accessibility and community tools.\n";

    // Keep program from quitting
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    return 0;
} 
