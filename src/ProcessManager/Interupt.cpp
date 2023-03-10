#include "include/ProcessManager/Interupt.h"
#include "include/ProcessManager/ProcessManager.h"

#include <QDebug>

namespace os
{
/**
 * @brief InteruptManager::Instance
 * @return 中断管理系统单例
 */
InteruptManager& InteruptManager::Instance()
{
    static InteruptManager im;
    return im;
}
/**
 * @brief InteruptManager::SetInterupt
 * @param is_enable 设置中断是否开启
 */
void InteruptManager::SetInterupt(bool is_enable)
{
    is_enable_ = is_enable;
}
/**
 * @brief InteruptManager::InteruptDetect
 * 检测是否发生中断
 * @return 返回码，成功返回0，失败返回-1
 */
int InteruptManager::InteruptDetect() {
    // 是否开启中断
    if (is_enable_) {
        ProcessManager& pm = ProcessManager::Instance();
        // 将中断的进程恢复就绪状态
        for (int i = 0; i < interupt_queue_.size(); i++) {
            auto listit = interupt_queue_[i];
            if (!listit.empty()) {
                qDebug() << "[Interupt] handling interupt for priority"<< i << "...";
            }
            for (auto it = listit.begin(); it != listit.end(); it++) {
                PCB& pcb = pm.GetPCB(it->pid_);
                if (pcb.state_ == ProcessState::WAIT) {
                    qDebug() << "[" << it->pid_ <<"] "<< "wake up from wait device" << it->dev_num_;
                    pcb.state_ = ProcessState::READY;
                    pm.pushProcess(pcb.pid_);
                    pm.RemoveWaitQueue(pcb.pid_);
                }
            }
            interupt_queue_[i].clear();
        }
    }
    return 0;
}
/**
 * @brief InteruptManager::InteruptRequest
 * 向中断管理系统发起中断请求
 * @param interupt 中断请求
 * @return 返回码，成功返回0，失败返回-1
 */
int InteruptManager::InteruptRequest(const Interupt& interupt){
    interupt_queue_[interupt.priority_].append(interupt);
    return 0;
}
}
