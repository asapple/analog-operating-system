#ifndef PROCESS_H
#define PROCESS_H
#include <QString>

#include "include/Common/Common.h"
#include "include/ProcessManager/Instruction.h"
namespace os {
    enum class ProcessState {
        // 进程尚未使用
            UNUSED,
        // 进程已经就绪，等待调度
            READY,
        // 进程正在运行
            RUNNING,
        // 进程被阻塞
            WAIT,
         // 已经被终止，等待资源回收
            KILLED,
    };

    struct PCB {
        // 当前状态
        ProcessState state_;
        // 进程号
        pid_t pid_;
        // 父进程号
        pid_t ppid_;

        // 文件名
        QString file_name_;
        // 进程大小
        size_t size_;

        // 优先级
        priority_t priority_;

        // 当前指令地址
        size_t pc_;
        // 指令
        Instruction ir_;

        // 创建时间
        tick_t start_time_;
        // 运行时间
        tick_t run_time_;
        // 本条指令已经执行的时间
        tick_t ins_time_;
        PCB()
            :state_(ProcessState::UNUSED),
              pid_(0),
              ppid_(0),
              file_name_(""),
              size_(0),
              priority_(0),
              pc_(0),
              ir_(),
              start_time_(0),
              run_time_(0),
              ins_time_(0)
        {}
    };
}
#endif // PROCESS_H
