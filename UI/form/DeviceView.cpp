#include "frmmain.h"
#include "ui_frmmain.h"
#include "UI/core_common/iconhelper.h"
#include "UI/core_common/quihelper.h"

#include "include/FileManager/FileManager.h"
#include "include/DeviceManager/DeviceManager.h"

using namespace os;

void frmMain::updateDeviceView(){
    auto& dem = DeviceManager::Instance();
    QVector<Device> device_ = dem.device_;
    for(int i; i<device_.length(); i++){
        QVector<PidTime> now_queue_ = device_[i].GetNowQueue();
        state state = device_[i].GetState();
        int num = device_[i].GetDevNum();
    }
}
