#include "DeviceStateCommandSender.h"
#include "Model.h"

#include <iostream>
#include <chrono>
#include <thread>

int main()
{
    DeviceStateCommandSender commandSender;
    int code = commandSender.initialize();
    if (code != 0) {
        printf("Failed to initialize Conduit with code %d", code);
        while (true) {}
    }

    const auto frameDuration = std::chrono::milliseconds(16); // 60Hz
    
    while (true) {
        #ifdef _WIN32
            system("cls");
        #else
            system("clear");
        #endif
        commandSender.pollForDriverUpdates();
        std::cout << Model::getInstance().toString();

        std::this_thread::sleep_for(frameDuration);
    }
    
    return 0;
}
