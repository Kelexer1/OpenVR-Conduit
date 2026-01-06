#include "globals.h"
SharedDeviceMemoryIO sharedMemoryIO(nullptr);
DevicePoseCommandSender commandSender(&sharedMemoryIO);