#include "include/ProcessManager/Interupt.h"
#include "include/ProcessManager/ProcessManager.h"


namespace os
{
InteruptManager& InteruptManager::Instance()
{
    static InteruptManager im;
    return im;
}
void InteruptManager::SetInterupt(bool is_enable)
{
    is_enable_ = is_enable;
}
int InteruptManager::InteruptDetect() {
    // 是否开启中断
    if (is_enable_) {
        ProcessManager& pm = ProcessManager::Instance();
        // 将中断的进程恢复就绪状态
        for (auto listit = interupt_queue_.begin(); listit != interupt_queue_.end(); listit++) {
            for (auto it = listit->begin(); it != listit->end(); it++) {
                PCB& pcb = pm.GetPCB(it->pid_);
                if (pcb.state_ == ProcessState::WAIT) {
                    pcb.state_ = ProcessState::READY;
                    pm.pushProcess(pcb.pid_);
                }
            }
        }
    }
}
int InteruptManager::InteruptRequest(const Interupt& interupt){
    interupt_queue_[interupt.priority_].append(interupt);
    return 0;
}
}
