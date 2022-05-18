#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H
#include <QHash>
#include <QMap>
#include <QVector>
#include <QList>
#include <memory>

#include "include/Common/Common.h"
#include "include/ProcessManager/Process.h"

namespace os {
class ProcessManager;
class Schedular;

enum class SchedulerType {
    FCFS,
    RR,
    PRI,
};
/**
 * @brief 调度器，负责管理就绪队列，并为就绪队列提供各种调度方法
 */
class Scheduler {
public:
    virtual int PushProcess(pid_t process) = 0;
    virtual int RemoveProcess(pid_t pid) = 0;
    virtual pid_t PollProcess() = 0;
    virtual const QList<pid_t> GetReadyQueue() = 0;
};
/**
 * @brief 进程管理者类，采用单例模式，管理、调度进程资源
 */
class ProcessManager {
    friend class CPU;
private:
    // 最大进程数
    int kMaxProcessNum_;
    // 是否为抢占式
    bool is_preemptive_;
    // 当前调度器
    std::unique_ptr<Scheduler> scheduler_;
    // 控制所有进程资源
    QVector<PCB> process_list_;
    // 等待队列
    QList<pid_t> wait_queue_;
    // 当前运行进程
    pid_t run_process_;
    // 当前时间
    tick_t cur_time_;
    // 上次调度的索引
    int last_index_;

    ProcessManager(int kMaxProcessNum, bool is_preemptive = false, SchedulerType type = SchedulerType::FCFS);
public:
    static ProcessManager& Instance(int kMaxProcessNum = 0, bool is_preemptive = false, SchedulerType type = SchedulerType::FCFS);

    inline int pushProcess(pid_t pid) { return scheduler_->PushProcess(pid);}
    inline pid_t PollProcess() { return scheduler_->PollProcess(); };

    inline PCB& GetPCB(pid_t pid) { return process_list_[pid]; };
    inline const QList<pid_t> GetReadyQueue() { return scheduler_->GetReadyQueue(); };
    void UpdateTime();

    int CheckKilled();
    int Execute(QString file_name);
    int Fork(pid_t ppid);
    int Kill(pid_t pid);
    int ProcessState(QVector<PCB>& running_queue, QVector<PCB>& wait_queue, QVector<PCB>& ready_queue);
};

class FCFSScheduler: public Scheduler {
private:
    // 根据进程号索引到对应的迭代器
    QHash<pid_t, QList<pid_t>::Iterator> hash_table_;
    // 就绪队列
    QList<pid_t> ready_queue_;
public:
    int PushProcess(pid_t process) override;
    pid_t PollProcess() override;
    int RemoveProcess(pid_t pid) override;
    const QList<pid_t> GetReadyQueue() override;
};

class PriorityScheduler: public Scheduler {
private:
    // 优先级就绪队列
    QMap<priority_t, QList<pid_t>> ready_queue_;
public:
    int PushProcess(pid_t process) override;
    pid_t PollProcess() override;
    int RemoveProcess(pid_t pid) override;
    const QList<pid_t> GetReadyQueue() override;
};
}
#endif // PROCESSMANAGER_H
