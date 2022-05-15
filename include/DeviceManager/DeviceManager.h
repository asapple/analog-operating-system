#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H
#include "include/Common/Common.h"
namespace os {
    class DeviceManager {
    public:
        int RequestDevice(pid_t pid, size_t time, dev_t dev_id);
        int UpdateTime();
    };
}

#endif //DEVICE_MANAGER_H
