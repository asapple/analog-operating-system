#include "include/ProcessManager/CPU.h"
#include "include/ProcessManager/ProcessManager.h"
#include "include/MemoryManager/MemoryManager.h"
namespace os {
CPU::CPU(int time_slot)
    : time_slot_(time_slot)
{}
void CPU::run() {
    while (!isInterruptionRequested()) {
        ProcessManager& pm = ProcessManager::Instance();
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
                pm.pushProcess(current_run_);
            }
            current_run_ = next_run;
        }
        MemoryManager
        msleep(time_slot_);
    }
}
CPU::~CPU() {
    requestInterruption();
    quit();
    wait();
}
}
