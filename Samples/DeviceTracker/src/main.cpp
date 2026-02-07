#include "DeviceStateCommandSender.h"
#include "Model.h"
#include "openvr.h"

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
    int initCode = commandSender.initialize();
    if (initCode != 0) {
        std::cout << "Failed to initialize Conduit with code " << initCode;
        while (true) {} // Prevent console from auto closing
    }

    Model model(commandSender);
    commandSender.addEventListener(model);
    
    while (true) {
        system("cls");
        model.print();
        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 60hz
    }
    
    return 0;
} 
