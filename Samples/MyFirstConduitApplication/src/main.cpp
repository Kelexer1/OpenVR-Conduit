#include "DeviceStateCommandSender.h"
#include "MyEventReciever.h"
#include "openvr.h"
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

    DeviceStateCommandSender myCommandSender;
    int initCode = myCommandSender.initialize();
    if (initCode != 0) {
        std::cout << "Failed to initialize Conduit with code " << initCode;
        while (true) {} // Prevent console from auto closing
    }

    MyEventReciever myEventReciever;
    myCommandSender.addEventListener(myEventReciever);
    
    // Keep program from quitting
    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    
    return 0;
} 
