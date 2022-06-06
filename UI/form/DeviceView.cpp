#include <QObject>
#include "frmmain.h"
#include "ui_frmmain.h"
#include "UI/core_common/iconhelper.h"
#include "UI/core_common/quihelper.h"

#include "include/FileManager/FileManager.h"
#include "include/DeviceManager/DeviceManager.h"

using namespace os;

void frmMain::updateDeviceView(){
    QVector<Device> device_ = DeviceManager::Instance().device_;
    QVector<QString> device_state;
    for(int i = 0; i <device_.length(); i++){
        if (device_[i].GetState() == os::IDLE)device_state.append("空闲");
        else device_state.append("繁忙");
    }
    ui->idlabel1->setText("设备号:  "+QString::number(device_[0].GetDevNum()));
    ui->idlabel2->setText("设备号:  "+QString::number(device_[1].GetDevNum()));
    ui->idlabel3->setText("设备号:  "+QString::number(device_[2].GetDevNum()));

    ui->statelabel1->setText("设备状态:  "+ device_state[0]);
    ui->statelabel2->setText("设备状态:  "+ device_state[1]);
    ui->statelabel3->setText("设备状态:  "+ device_state[2]);

    QVector<QString> device_queue;//存放三个设备的队列信息，转换为QString
    for (int i = 0; i<device_.length(); i++)
    {
        device_queue.append("PID            time        \n");
        for(int j = 0; j < device_[i].now_queue_.length(); j++)
        {
            //获取每个队列内的pid和time 转成QString格式。
            device_queue[i]+=QString::number(device_[i].now_queue_[j].pid_)+"             "+ QString::number(device_[i].now_queue_[j].time_)+"\n";
        }
    }
//    ui->textBrowser1->append("为什么行");

    //下面全给注释了能跑，但是我输出不了队列信息
    //为什么不行
//    ui->textBrowser1->clear();

    //下面代码为什么不行QAQ
//    ui->textBrowser1->setText("为什么不行");
    ui->textBrowser1->setText(device_queue[0]);
    ui->textBrowser2->setText(device_queue[1]);
    ui->textBrowser3->setText(device_queue[2]);
}
