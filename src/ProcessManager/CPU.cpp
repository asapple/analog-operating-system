#include <QMutexLocker>
#include <QDebug>

#include "include/ProcessManager/CPU.h"
#include "include/ProcessManager/ProcessManager.h"
#include "include/ProcessManager/Interupt.h"
#include "include/MemoryManager/MemoryManager.h"
#include "include/DeviceManager/DeviceManager.h"
#include "UI/form/frmmain.h"

namespace os {
CPU::CPU(int time_slot, frmMain* ui)
    : time_slot_(time_slot), cur_tick_(0), current_run_(0), ui_(ui)
{
    connect(this,&CPU::refresh, ui, &frmMain::updateView);

}
/**
 * @brief CPU::run
 * 线程执行函数，CPU不断循环该周期
 * 负责控制系统的时序节拍
 * 负责进程代码的执行和处理
 */
void CPU::run() {
    while (!isInterruptionRequested()) {
        ProcessManager& pm = ProcessManager::Instance();
        MemoryManager& mm = MemoryManager::Instance();
        DeviceManager& dm = DeviceManager::Instance();
        PCB& last_pcb = pm.GetPCB(current_run_);
        pid_t next_run = -1;
        // 若为抢占式或者当前进程不再是运行状态，则进行调度
        if (pm.is_preemptive_ || last_pcb.state_ != ProcessState::RUNNING) {
            // 若当前进程不再为运行状态，则重新设置当前进程
            if (last_pcb.state_ != ProcessState::RUNNING) {
                current_run_  = 0;
            }
            next_run = pm.PollProcess();
            // 成功进行调度，则换下当前进程，更换为下一个进程
            if (next_run != -1) {
                // 该进程被抢占，则需要加入就绪队列
                if (last_pcb.state_ == ProcessState::RUNNING) {
                    last_pcb.state_ = ProcessState::READY;
                    pm.pushProcess(current_run_);
                }
                current_run_ = next_run;
            }
            pm.run_process_ = current_run_;
        }
        // 当前有任务需要处理
        if (current_run_ != 0) {
            PCB& cpcb = pm.GetPCB(current_run_);
            cpcb.state_ = ProcessState::RUNNING;
            // 取指令
            if (cpcb.ins_time_ == 0) {
                QByteArray buffer;
                int error = mm.GetCode(current_run_, cpcb.pc_, buffer);
                if (error < 0) {
                    // TODO: 错误处理
                }
                cpcb.ir_ = buffer.data();
                cpcb.pc_ += sizeof cpcb.ir_;
            }
            // 执行指令
            if (ExecuteInstruction(cpcb) < 0) {
                //TODO: 错误处理
                qDebug() << "[" << cpcb.pid_ <<"]: [ERROR] Illegal Instruction";
            }
        }

        // 时间片休眠，模拟各种执行过程，等待下一次时间中断
        msleep(time_slot_);
        // 更新时间
        cur_tick_++;
        pm.UpdateTime();
        dm.UpdateTime();
        // 检查死亡进程，回收对应资源
        pm.CheckKilled();
        // 中断处理
        InteruptManager::Instance().InteruptDetect();

        // 更新UI
//        ui_->updateProcView();
//        ui_->updateMemTableView();
//        ui_->updateDiskView();
//        ui_->updateDeviceView();
        emit(this->refresh());
    }
}

int CPU::ExecuteInstruction(PCB& pcb)
{
    int size;
    int sem_num;
    sem_t sem_val;
    pid_t child;
    switch (pcb.ir_.type) {
         // 访存指令
    case InsType::ACCESS:
        if (MemoryManager::Instance().AccessMemory(pcb.pid_, pcb.ir_.op) < 0) {
            // TODO:错误处理
        }
        qDebug() <<"[" <<  pcb.pid_ << "]: access " << pcb.ir_.op;
        break;
         // 计算指令
    case InsType::COMPUTE:
        if (pcb.ins_time_ == 0) {
            pcb.ins_time_ = pcb.ir_.op;
        }
        pcb.ins_time_--;
        qDebug() <<"[" <<  pcb.pid_ << "]: run compute 1 cycle";
        break;
         // 设备访问指令
    case InsType::DEVICE:
        if (DeviceManager::Instance().RequestDevice(pcb.pid_, pcb.ir_.op1, pcb.ir_.op2) < 0) {
            // TODO: 错误处理
        }
        pcb.state_ = ProcessState::WAIT;
        ProcessManager::Instance().wait_queue_.push_back(pcb.pid_);
        qDebug() <<"[" <<  pcb.pid_ << "]: wait for device" << pcb.ir_.op2 << "access" << pcb.ir_.op1 << "cycles";
        break;
         // fork指令
    case InsType::FORK:
        child = ProcessManager::Instance().Fork(pcb.pid_);
        if (child < 0) {
            // TODO: 错误处理
        }
        qDebug() <<"[" <<  pcb.pid_ << "]: has been Forked by child Process " << child;
        break;
        // 优先级改变指令
    case InsType::PRIORITY:
        pcb.priority_ = pcb.ir_.op;
        break;
        // 访存指令
    case InsType::MEMORY:
        size = MemoryManager::Instance().MoreMemory(pcb.pid_, pcb.ir_.op);
        if (size < 0) {
            // TODO: 错误处理
        } else {
            pcb.size_ += pcb.ir_.op;
        }
        break;
        // 进程同步P操作
    case InsType::SEM_P:
        sem_num = pcb.ir_.op;
        if (!ProcessManager::Instance().P(pcb.pid_, sem_num)) {
            pcb.state_ = ProcessState::KILLED;
            return -1;
        }
        break;
        // 进程同步V操作
    case InsType::SEM_V:
        sem_num = pcb.ir_.op;
        if (!ProcessManager::Instance().V(pcb.pid_, sem_num)) {
            pcb.state_ = ProcessState::KILLED;
            return -1;
        }
        break;
        // 进程退出指令
    case InsType::QUIT:
        pcb.state_ = ProcessState::KILLED;
        qDebug() <<"[" <<  pcb.pid_ << "]: quit";
        break;
    default:
        // 异常退出
        pcb.state_ = ProcessState::KILLED;
        return -1;
    }
    return 0;
}
/**
 * @brief CPU::~CPU
 * 析构函数，在这里关闭CPU的主线程
 */
CPU::~CPU() {
    requestInterruption();
    quit();
    wait();
}
}
