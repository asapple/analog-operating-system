#include <QObject>
#include "frmmain.h"
#include "ui_frmmain.h"
#include "UI/core_common/iconhelper.h"
#include "UI/core_common/quihelper.h"

#include "include/FileManager/FileManager.h"
#include "include/DeviceManager/DeviceManager.h"

using namespace os;

static void updateDeviceList(QTableWidget* table, int rowno, pid_t pid, int time);

void frmMain::updateDeviceView(){
    QVector<Device> device_ = DeviceManager::Instance().device_;
    QVector<QString> device_state;
    for(int i = 0; i <device_.length(); i++){
        if (device_[i].GetState() == os::IDLE)device_state.append(Ui::TEXT_COLOR_GREEN("IDLE"));
        else device_state.append(Ui::TEXT_COLOR_BLUE("BUSY"));
    }
    ui->idlabel1->setText(QString::number(device_[0].GetDevNum()));
    ui->idlabel2->setText(QString::number(device_[1].GetDevNum()));
    ui->idlabel3->setText(QString::number(device_[2].GetDevNum()));

    ui->statelabel1->setText(device_state[0]);
    ui->statelabel2->setText(device_state[1]);
    ui->statelabel3->setText(device_state[2]);

    // 设备0
    int rowCount = ui->device0table->rowCount();
    for(int i = 0; i < rowCount; ++i)
    {
         ui->device0table->removeRow(0);
    }
    for (int i = 0; i < device_[0].now_queue_.size(); i++) {
        ui->device0table->insertRow(i);
        updateDeviceList(ui->device0table, i, device_[0].now_queue_[i].pid_, device_[0].now_queue_[i].time_);
    }
    // 设备1
    rowCount = ui->device1table->rowCount();
    for(int i = 0; i < rowCount; ++i)
    {
         ui->device1table->removeRow(0);
    }
    for (int i = 0; i < device_[1].now_queue_.size(); i++) {
        ui->device1table->insertRow(i);
        updateDeviceList(ui->device1table, i, device_[1].now_queue_[i].pid_, device_[1].now_queue_[i].time_);
    }
    // 设备2
    rowCount = ui->device2table->rowCount();
    for(int i = 0; i < rowCount; ++i)
    {
         ui->device2table->removeRow(0);
    }
    for (int i = 0; i < device_[2].now_queue_.size(); i++) {
        ui->device2table->insertRow(i);
        updateDeviceList(ui->device2table, i, device_[2].now_queue_[i].pid_, device_[2].now_queue_[i].time_);
    }
}

static void updateDeviceList(QTableWidget* table, int rowno, pid_t pid, int time)
{
    table->setItem(rowno, 0, new QTableWidgetItem(QString::number(pid)));
    table->setItem(rowno, 1, new QTableWidgetItem(QString::number(time)));
}

