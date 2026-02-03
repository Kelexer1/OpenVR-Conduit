#include "DeviceStateCommandSender.h"
#include "Model.h"

#include <chrono>
#include <thread>

int main() {
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
