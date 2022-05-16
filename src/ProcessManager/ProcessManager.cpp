#include <memory>
#include <QHash>

#include "include/ProcessManager/Process.h"
#include "include/ProcessManager/ProcessManager.h"
#include "include/MemoryManager/MemoryManager.h"
#include "include/DeviceManager/DeviceManager.h"

namespace os {
ProcessManager::ProcessManager(int kMaxProcessNum, bool is_preemptive, SchedulerType type):
    kMaxProcessNum_(kMaxProcessNum),
    is_preemptive_(is_preemptive),
    process_list_(kMaxProcessNum+1)
{
    if (type == SchedulerType::FCFS) {
        scheduler_.reset(static_cast<Scheduler*>(new FCFSScheduler));
    } else if (type == SchedulerType::PRI) {
        scheduler_.reset(static_cast<Scheduler*>(new PriorityScheduler));
    }
    for (pid_t pid = 1; pid <= kMaxProcessNum; pid++) {
        process_list_[pid].pid_ = pid;
    }
}

ProcessManager& ProcessManager::Instance(int kMaxProcessNum, bool is_preemptive, SchedulerType type) {
    static ProcessManager instance(kMaxProcessNum, is_preemptive, type);
    return instance;
}

void ProcessManager::UpdateTime() {
    cur_time_++;
    if (run_process_ > 0) {
        process_list_[run_process_].run_time_++;
    }
}

int ProcessManager::Execute(QString file_name) {
    pid_t pid;
    for (pid = 0; pid < kMaxProcessNum_; pid++) {
        if (process_list_[pid].state_ == ProcessState::UNUSED) {
            process_list_[pid].state_ = ProcessState::READY;
            break;
        }
    }
    if (pid == kMaxProcessNum_) {
        return -1;
    }
    int size = MemoryManager::Instance().InitMemory(pid, file_name);
    if (size <= 0) {
        MemoryManager::Instance().ReleaseMemory(pid);
        process_list_[pid].state_ = ProcessState::UNUSED;
        return -1+size;
    }
    process_list_[pid].size_ += size;
    process_list_[pid].start_time_ = cur_time_;
    scheduler_->PushProcess(pid);
    return 0;
}

int ProcessManager::Fork(pid_t ppid) {
    pid_t pid;
    for (pid = 0; pid < kMaxProcessNum_; pid++) {
        if (process_list_[pid].state_ == ProcessState::UNUSED) {
            break;
        }
    }
    if (pid == kMaxProcessNum_) {
        return -1;
    }
    // TODO: ShareMemory
    // int size = MemoryManager::Instance().ShareMemory(pid, ppid);
//    if (size <= 0) {
//        MemoryManager::Instance().ReleaseMemory(pid);
//        process_list_[pid].state_ = ProcessState::UNUSED;
//        return -1+size;
//    }
    process_list_[pid] = process_list_[ppid];
    process_list_[pid].pid_ = pid;
    process_list_[pid].ppid_ = ppid;
    process_list_[pid].state_ = ProcessState::UNUSED;
    process_list_[pid].start_time_ = cur_time_;
    process_list_[pid].run_time_ = 0;
    scheduler_->PushProcess(pid);
    return 0;
}

int ProcessManager::CheckKilled()
{
    // 检查当前正在执行的进程
    if (process_list_[run_process_].state_ == ProcessState::KILLED) {
        process_list_[run_process_] = PCB();
        process_list_[run_process_].pid_ = run_process_;
    }
    // 检查就绪队列
    for (auto pid:GetReadyQueue()) {
        if (process_list_[pid].state_ == ProcessState::KILLED) {
            process_list_[pid] = PCB();
            process_list_[pid].pid_ = pid;
        }
        scheduler_->RemoveProcess(pid);
    }
    // 检查等待队列
    for (auto it = wait_queue_.begin(); it != wait_queue_.end(); it++) {
        if (process_list_[*it].state_ == ProcessState::KILLED) {
            process_list_[*it] = PCB();
            process_list_[*it].pid_ = *it;
        }
        // TODO 对应设备删除队列
        DeviceManager::Instance().RemoveProcess(*it);
    }
    return 0;
}

int ProcessManager::Kill(pid_t pid) {
    if (process_list_[pid].state_ == ProcessState::UNUSED || process_list_[pid].state_ == ProcessState::KILLED) {
        return -1;
    }
    process_list_[pid].state_ = ProcessState::KILLED;
    return 0;
}

int ProcessManager::ProcessState(QVector<PCB>& running_queue, QVector<PCB>& wait_queue, QVector<PCB>& ready_queue) {
    // running queue
    running_queue.append(process_list_[run_process_]);
    // ready queue
    for (const auto pid:scheduler_->GetReadyQueue()) {
        ready_queue.append(process_list_[pid]);
    }
    // wait queue
    for (auto it = wait_queue_.begin(); it != wait_queue_.end(); it++) {
        wait_queue.append(process_list_[*it]);
    }
    return 0;
}

int FCFSScheduler::PushProcess(pid_t pid) {
    if (hash_table_.contains(pid)) {
        return -1;
    }
    ready_queue_.push_back(pid);
    hash_table_.insert(pid,ready_queue_.end()-1);
    return 0;
}

pid_t FCFSScheduler::PollProcess() {
    if (ready_queue_.empty()) {
        return -1;
    }
    pid_t pid = ready_queue_.front();
    hash_table_.remove(pid);
    ready_queue_.pop_front();
    return pid;
}
int FCFSScheduler::RemoveProcess(pid_t pid)
{
    if (!hash_table_.contains(pid)) {
        return -1;
    }
    ready_queue_.erase(hash_table_.find(pid).value());
    return 0;
}
const QList<pid_t> FCFSScheduler::GetReadyQueue() {
    return ready_queue_;
}

int PriorityScheduler::PushProcess(pid_t pid) {
    if (ready_queue_.contains(pid)) {
        return -1;
    }
    PCB& pcb = ProcessManager::Instance().GetPCB(pid);
    ready_queue_[pcb.priority_].append(pid);
    return 0;
}

pid_t PriorityScheduler::PollProcess() {
    if (ready_queue_.empty()) {
        return -1;
    }
    auto it = ready_queue_.last();
    pid_t pid = it.front();
    it.pop_front();
    if (it.empty()) {
        ready_queue_.remove(ready_queue_.lastKey());
    }
    return pid;
}
int PriorityScheduler::RemoveProcess(pid_t pid)
{
    priority_t pri = ProcessManager::Instance().GetPCB(pid).priority_;
    if (ready_queue_.contains(pri) && ready_queue_[pri].removeOne(pid)) {
        if (ready_queue_[pri].empty()) {
            ready_queue_.remove(pri);
        }
        return 0;
    }
    return -1;
}
const QList<pid_t> PriorityScheduler::GetReadyQueue() {
    QList<pid_t> queue;
    for (auto it = ready_queue_.end()-1; it != ready_queue_.begin(); it--) {
        queue.append(it.value());
    }
    queue.append(ready_queue_.begin().value());
    return queue;
}
}
