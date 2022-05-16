#include <QMutexLocker>

#include "include/ProcessManager/CPU.h"
#include "include/ProcessManager/ProcessManager.h"
#include "include/ProcessManager/Interupt.h"
#include "include/MemoryManager/MemoryManager.h"
#include "include/DeviceManager/DeviceManager.h"

namespace os {
CPU::CPU(int time_slot)
    : time_slot_(time_slot)
{}

void CPU::run() {
    while (!isInterruptionRequested()) {
        ProcessManager& pm = ProcessManager::Instance();
        MemoryManager& mm = MemoryManager::Instance();
        DeviceManager& dm = DeviceManager::Instance();
        PCB& pcb = pm.GetPCB(current_run_);
        pid_t next_run = -1;
        // 若为抢占式或者当前进程不再是运行状态，则进行调度
        if (pm.is_preemptive_ || pcb.state_ != ProcessState::RUNNING) {
            next_run = pm.PollProcess();
        }
        // 成功进行调度调度，则换下当前进程，更换为下一个进程
        if (next_run != -1) {
            // 该进程被抢占，则需要加入就绪队列
            if (pcb.state_ == ProcessState::RUNNING) {
                pcb.state_ = ProcessState::READY;
                pm.pushProcess(current_run_);
            }
            current_run_ = next_run;
        }
        pm.run_process_ = current_run_;
        // 当前有任务需要处理
        if (current_run_ != 0) {
            pcb = pm.GetPCB(current_run_);
            // 取指令
            if (pcb.ins_time_ == 0) {
                QByteArray buffer;
                int error = mm.GetCode(current_run_, pcb.pc_, buffer);
                if (error < 0) {
                    // TODO: 错误处理
                }
                pcb.ir_ = buffer.data();
                pcb.pc_ += sizeof pcb.ir_;
            }
            // 执行指令
            if (ExecuteInstruction(pcb) < 0) {
                //TODO: 错误处理
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
    }
}

int CPU::ExecuteInstruction(PCB& pcb)
{
    int size;
    switch (pcb.ir_.type) {
         // 访存指令
    case InsType::ACCESS:
        if (MemoryManager::Instance().AccessMemory(pcb.pid_, pcb.ir_.op) < 0) {
            // TODO:错误处理
        }
        break;
         // 计算指令
    case InsType::COMPUTE:
        if (pcb.ins_time_ == 0) {
            pcb.ins_time_ = pcb.ir_.op;
        }
        pcb.ins_time_--;
        break;
         // 设备访问指令
    case InsType::DEVICE:
        if (DeviceManager::Instance().RequestDevice(pcb.pid_, pcb.ir_.op1, pcb.ir_.op2) < 0) {
            // TODO: 错误处理
        }
        pcb.state_ = ProcessState::WAIT;
        ProcessManager::Instance().wait_queue_.push_back(pcb.pid_);
        break;
         // fork指令
    case InsType::FORK:
        if (ProcessManager::Instance().Fork(pcb.pid_) < 0) {
            // TODO: 错误处理
        }
        break;
        // 优先级改变指令
    case InsType::PRIORITY:
        pcb.priority_ = pcb.ir_.op;
        break;
        // 访存指令
    case InsType::MEMORY:
        size = MemoryManager::Instance().MoreMemory(pcb.pid_, pcb.size_);
        if (size < 0) {
            // TODO: 错误处理
        } else {
            pcb.size_ += size;
        }
        break;
        // 进程退出指令
    case InsType::QUIT:
        pcb.state_ = ProcessState::KILLED;
        break;
    default:
        // 异常退出
        pcb.state_ = ProcessState::KILLED;
        return -1;
    }
    return 0;
}

CPU::~CPU() {
    requestInterruption();
    quit();
    wait();
}
}
