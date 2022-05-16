#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H
#include "include/Common/Common.h"
namespace os {
    class DeviceManager {
    private:
        DeviceManager();
    public:
        static DeviceManager& Instance();
        int RequestDevice(pid_t pid, size_t time, dev_t dev_id);
        int RemoveProcess(pid_t pid);
        int UpdateTime();
    };
}

#endif //DEVICE_MANAGER_H
