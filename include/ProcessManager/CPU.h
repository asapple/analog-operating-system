#ifndef CPU_H
#define CPU_H
#include <QThread>
#include <QMutex>

#include "include/Common/Common.h"
#include "include/ProcessManager/Process.h"

namespace os {
    /**
     * @brief CPU类，单独作为一个线程运行。
     * 维护整个系统的时序逻辑
     * 负责执行进程的指令
     * 负责中断处理和调用
     */
    class CPU: public QThread {
        Q_OBJECT
    private:
        int time_slot_;
        tick_t cur_tick_;
        pid_t current_run_;
        int ExecuteInstruction(PCB& pcb);
    public:
        CPU(int time_slot);
        ~CPU();
        void run() override;
    };
}
#endif // CPU_H
