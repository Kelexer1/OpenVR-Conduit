#include "DeviceStateCommandSender.h"
#include "Model.h"
#include "StateEventReciever.h"

#include <chrono>
#include <thread>

int main() {
    DeviceStateCommandSender commandSender;
    int code = commandSender.initialize();
    if (code != 0) {
        printf("Failed to initialize Conduit with code %d", code);
        while (true) {} // Prevent console from auto closing
    }

    StateEventReciever eventReceiver;
    commandSender.addEventListener(eventReceiver);
    
    while (true) {
        system("cls");

        Model::getInstance().print();

        std::this_thread::sleep_for(std::chrono::milliseconds(16)); // 60hz
    }
    
    return 0;
} 
