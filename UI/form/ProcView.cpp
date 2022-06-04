#include "frmmain.h"
#include "ui_frmmain.h"
#include "UI/core_common/iconhelper.h"
#include "UI/core_common/quihelper.h"

#include "include/FileManager/FileManager.h"
#include "include/ProcessManager/ProcessManager.h"
using namespace os;
namespace ui {
    enum  ProcessColumn {
        PID,
        PPID,
        FILENAME,
        IR,
        SSTIME,
        RTIME,
        PRI,
        SIZE
    };
}
static void updateProcessList(QTableWidget* table, int rowno, PCB& pcb);

void frmMain::updateProcView() {
    auto& pm = ProcessManager::Instance();
    pid_t run = pm.GetRunProcess();
    auto waitList = pm.GetWaitQueue();
    auto readyList = pm.GetReadyQueue();

    // set running
    PCB pcb = pm.GetPCB(run);
    ui->curTime_value->setText(QString::number(pm.GetCurTime()));
    if (run != 0) {
        ui->startTime_value->setText(QString::number(pcb.start_time_));
        ui->execTime_value->setText(QString::number(pcb.run_time_));
        ui->PID_value->setText(QString::number(pcb.pid_));
        ui->PPID_value->setText(QString::number(pcb.ppid_));
        ui->IR_value->setText(pcb.ir_);
        ui->Pri_value->setText(QString::number(pcb.priority_));
    } else {
        ui->startTime_value->clear();
        ui->execTime_value->clear();
        ui->PID_value->clear();
        ui->PPID_value->clear();
        ui->IR_value->clear();
        ui->Pri_value->clear();
    }
    int rowCount = ui->ready_table->rowCount();
    for(int i = 0; i < rowCount; ++i)
    {
         ui->ready_table->removeRow(0);
    }
    ui->ready_table->clearContents();
    for (int i = 0; i < readyList.size(); i++) {
        ui->ready_table->insertRow(i);
        updateProcessList(ui->ready_table, i, pm.GetPCB(readyList[i]));
    }
    rowCount = ui->block_table->rowCount();
    for(int i = 0; i < rowCount; ++i)
    {
         ui->block_table->removeRow(0);
    }
    ui->block_table->clearContents();
    for (int i = 0; i < waitList.size(); i++) {
        ui->block_table->insertRow(i);
        updateProcessList(ui->block_table, i, pm.GetPCB(waitList[i]));
    }
}

static void updateProcessList(QTableWidget* table, int rowno, PCB& pcb)
{
    table->setItem(rowno, ui::ProcessColumn::PID, new QTableWidgetItem(QString::number(pcb.pid_)));
    table->setItem(rowno, ui::ProcessColumn::PPID, new QTableWidgetItem(QString::number(pcb.ppid_)));
    table->setItem(rowno, ui::ProcessColumn::FILENAME, new QTableWidgetItem(pcb.file_name_));
    table->setItem(rowno, ui::ProcessColumn::IR, new QTableWidgetItem(pcb.ir_));
    table->setItem(rowno, ui::ProcessColumn::SSTIME, new QTableWidgetItem(QString::number(pcb.start_time_)));
    table->setItem(rowno, ui::ProcessColumn::RTIME, new QTableWidgetItem(QString::number(pcb.run_time_)));
    table->setItem(rowno, ui::ProcessColumn::PRI, new QTableWidgetItem(QString::number(pcb.priority_)));
    table->setItem(rowno, ui::ProcessColumn::SIZE, new QTableWidgetItem(QString::number(pcb.size_)));
}
