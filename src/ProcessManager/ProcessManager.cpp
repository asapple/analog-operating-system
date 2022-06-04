#include <memory>
#include <QHash>
#include <QDebug>

#include "include/ProcessManager/Process.h"
#include "include/ProcessManager/ProcessManager.h"
#include "include/MemoryManager/MemoryManager.h"
#include "include/DeviceManager/DeviceManager.h"

namespace os {
/**
 * @brief ProcessManager::ProcessManager 进程管理系统构造函数
 * @param kMaxProcessNum 最大进程数
 * @param is_preemptive 就绪队列调度是否抢占
 * @param type 就绪队列调度的方法
 */
ProcessManager::ProcessManager(int kMaxProcessNum, bool is_preemptive, SchedulerType type):
    kMaxProcessNum_(kMaxProcessNum),
    is_preemptive_(is_preemptive),
    process_list_(kMaxProcessNum+1),
    last_index_(0)
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
/**
 * @brief ProcessManager::Instance 返回进程管理系统单例
 * @param kMaxProcessNum 最大进程数，仅在初始化时设置
 * @param is_preemptive 就绪队列调度是否抢占，仅在初始化时设置
 * @param type 就绪队列调度的方法，仅在初始化时设置
 * @return 进程管理系统单例
 */
ProcessManager& ProcessManager::Instance(int kMaxProcessNum, bool is_preemptive, SchedulerType type) {
    static ProcessManager instance(kMaxProcessNum, is_preemptive, type);
    return instance;
}
/**
 * @brief ProcessManager::UpdateTime
 * 更新时序
 */
void ProcessManager::UpdateTime() {
    cur_time_++;
    if (run_process_ > 0) {
        process_list_[run_process_].run_time_++;
    }
}
/**
 * @brief ProcessManager::Execute
 * @param file_name 可执行文件名
 * @return 返回码，成功返回0，失败返回-1
 */
int ProcessManager::Execute(QString file_name) {
    pid_t pid;
    bool flag = false;
    for (int cnt = 0; cnt < kMaxProcessNum_; cnt++) {
        pid = (last_index_ + cnt) % kMaxProcessNum_ + 1;
        if (process_list_[pid].state_ == ProcessState::UNUSED) {
            last_index_ = pid;
            process_list_[pid].state_ = ProcessState::READY;
            flag = true;
            break;
        }
    }
    if (!flag) {
        return -1;
    }
    qDebug() << "[" << pid << "]: Initializing execute " << file_name << "......";
    int size = MemoryManager::Instance().InitMemory(pid, file_name);

    if (size <= 0) {
        MemoryManager::Instance().ReleaseMemory(pid);
        process_list_[pid].state_ = ProcessState::UNUSED;
        qDebug() << "[" << pid << "]: Error Alloc Memory When initialize " << file_name << "!";
        return -1+size;
    }
    process_list_[pid].size_ += size;
    process_list_[pid].start_time_ = cur_time_;
    process_list_[pid].file_name_ = file_name;
    scheduler_->PushProcess(pid);
    qDebug() << "[" << pid << "]: Sucecessfully initialize " << file_name << ".";
    return 0;
}
/**
 * @brief ProcessManager::Fork
 * @param ppid 父进程号
 * @return 成功返回0，失败返回-1，内存分配出错返回具体的返回码
 */
int ProcessManager::Fork(pid_t ppid) {
    pid_t pid;
    bool flag = false;
    for (int cnt = 0; cnt < kMaxProcessNum_; cnt++) {
        pid = (last_index_ + cnt) % kMaxProcessNum_ + 1;
        if (process_list_[pid].state_ == ProcessState::UNUSED) {
            last_index_ = pid;
            process_list_[pid].state_ = ProcessState::READY;
            flag = true;
            break;
        }
    }
    if (!flag) {
        return -1;
    }
    int size = MemoryManager::Instance().ForkMemory(pid, ppid);
    if (size < 0) {
        MemoryManager::Instance().ReleaseMemory(pid);
        process_list_[pid].state_ = ProcessState::UNUSED;
        return -1+size;
    }
    process_list_[pid] = process_list_[ppid];
    process_list_[pid].pid_ = pid;
    process_list_[pid].ppid_ = ppid;
    process_list_[pid].state_ = ProcessState::READY;
    process_list_[pid].start_time_ = cur_time_;
    process_list_[pid].run_time_ = 0;
    scheduler_->PushProcess(pid);
    qDebug() <<"[" <<  pid << "]: Initialized by Fork Process " << ppid;
    return 0;
}
/**
 * @brief ProcessManager::CheckKilled
 * 检查是否有需要回收的进程资源
 * @return 返回码，成功返回0，失败返回-1
 */
int ProcessManager::CheckKilled()
{
    // 检查当前正在执行的进程
    if (process_list_[run_process_].state_ == ProcessState::KILLED) {
        MemoryManager::Instance().ReleaseMemory(run_process_);
        qDebug() << "[" << run_process_ << "]("<<"FileName:" <<process_list_[run_process_] .file_name_ << ","<< "Parent:" << process_list_[run_process_].ppid_ << "): has already been killed.";
        process_list_[run_process_] = PCB();
        process_list_[run_process_].pid_ = run_process_;
    }
    // 检查就绪队列
    for (auto pid:GetReadyQueue()) {
        if (process_list_[pid].state_ == ProcessState::KILLED) {
            MemoryManager::Instance().ReleaseMemory(pid);
            qDebug() << "[" << pid << "]("<<"FileName:" <<process_list_[pid] .file_name_ << ","<< "Parent:" << process_list_[pid].ppid_ << "): has already been killed.";
            process_list_[pid] = PCB();
            process_list_[pid].pid_ = pid;
            scheduler_->RemoveProcess(pid);
        }
    }
    // 检查等待队列
    for (auto it = wait_queue_.begin(); it != wait_queue_.end(); it++) {
        if (process_list_[*it].state_ == ProcessState::KILLED) {
            MemoryManager::Instance().ReleaseMemory(*it);
            qDebug() << "[" << *it << "]("<<"FileName:" <<process_list_[*it] .file_name_ << ","<< "Parent:" << process_list_[*it].ppid_ << "): has already been killed.";
            process_list_[*it] = PCB();
            process_list_[*it].pid_ = *it;
            // TODO 对应设备删除队列
            DeviceManager::Instance().RemoveProcess(*it);
            wait_queue_.removeOne(*it);
        }
    }
    return 0;
}
/**
 * @brief ProcessManager::Kill
 * 终止进程
 * @param pid 进程号
 * @return 返回码
 * @return 成功返回0
 * @return 若进程号不合法为-1
 * @return 若进程已经被终止则失败返回-2
 */
int ProcessManager::Kill(pid_t pid) {
    if (pid<0 || pid > kMaxProcessNum_) {
        return -1;
    }
    if (process_list_[pid].state_ == ProcessState::UNUSED || process_list_[pid].state_ == ProcessState::KILLED) {
        return -2;
    }
    process_list_[pid].state_ = ProcessState::KILLED;
    return 0;
}
/**
 * @brief ProcessManager::ProcessState
 * 展示设备状态
 * @param running_queue 执行队列引用，用于返回当前执行队列
 * @param wait_queue 等待队列引用， 用于返回当前等待队列
 * @param ready_queue 就绪队列引用，用于返回当前就绪队列
 * @return 返回码，成功返回0
 */
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
/**
 * @brief FCFSScheduler::PushProcess
 * @param pid 进程号
 * @return 返回码，成功返回0，若该进程已经在队列中则返回-1
 */
int FCFSScheduler::PushProcess(pid_t pid) {
    if (hash_table_.contains(pid)) {
        return -1;
    }
    ready_queue_.push_back(pid);
    hash_table_.insert(pid,ready_queue_.end()-1);
    return 0;
}
/**
 * @brief FCFSScheduler::PollProcess
 * 获得下一个待执行进程
 * @return 返回码/进程号，若队列为空则返回-1，否则返回进程号
 */
pid_t FCFSScheduler::PollProcess() {
    if (ready_queue_.empty()) {
        return -1;
    }
    pid_t pid = ready_queue_.front();
    hash_table_.remove(pid);
    ready_queue_.pop_front();
    return pid;
}
/**
 * @brief FCFSScheduler::RemoveProcess
 * 从队列中移除某个进程
 * @param pid 进程号
 * @return 返回码，成功删除返回0，若已经被移除了则返回-1
 */
int FCFSScheduler::RemoveProcess(pid_t pid)
{
    if (!hash_table_.contains(pid)) {
        return -1;
    }
    ready_queue_.erase(hash_table_[pid]);
    return 0;
}
/**
 * @brief FCFSScheduler::GetReadyQueue
 * 以列表形式返回就绪队列
 * @return 就绪队列，按照优先级先后排列
 */
const QList<pid_t> FCFSScheduler::GetReadyQueue() {
    return ready_queue_;
}
/**
 * @brief PriorityScheduler::PushProcess
 * @param pid 进程号
 * @return 返回码，成功返回0，若该进程已经在队列中则返回-1
 */
int PriorityScheduler::PushProcess(pid_t pid) {
    if (ready_queue_.contains(pid)) {
        return -1;
    }
    PCB& pcb = ProcessManager::Instance().GetPCB(pid);
    ready_queue_[pcb.priority_].append(pid);
    return 0;
}
/**
 * @brief PriorityScheduler::PollProcess
 * 获得下一个待执行进程
 * @return 返回码/进程号，若队列为空则返回-1，否则返回进程号
 */
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
/**
 * @brief PriorityScheduler::RemoveProcess
 * 从队列中删除进程
 * @param pid 待删除的进程号
 * @return 返回码，成功返回0，若已经被删除了则返回-1
 */
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
/**
 * @brief PriorityScheduler::GetReadyQueue
 * 获得当前进程队列
 * @return 当前进程队列，按照优先级先后排列
 */
const QList<pid_t> PriorityScheduler::GetReadyQueue() {
    QList<pid_t> queue;
    for (auto it = ready_queue_.end()-1; it != ready_queue_.begin(); it--) {
        queue.append(it.value());
    }
    queue.append(ready_queue_.begin().value());
    return queue;
}
}
