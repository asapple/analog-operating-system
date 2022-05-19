#include "include/DeviceManager/DeviceManager.h"

using namespace os;

void Init()
{
    os::DeviceManager::Instance();
}

int main(int argc, char *argv[])
{
    Init();
    os::DeviceManager::Instance().RequestDevice(1,3,0);
    os::DeviceManager::Instance().RequestDevice(2,5,0);
    os::DeviceManager::Instance().RequestDevice(3,1,1);
    os::DeviceManager::Instance().RequestDevice(4,2,2);
    os::DeviceManager::Instance().RequestDevice(5,2,1);
    os::DeviceManager::Instance().RequestDevice(6,3,0);
    os::DeviceManager::Instance().RemoveProcess(2);
    for (int i  = 0; i<5; i++){
        os::DeviceManager::Instance().UpdateTime();
    }
}
