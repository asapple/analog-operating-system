#ifndef CPU_H
#define CPU_H
#include <QThread>
#include <QMutex>

#include "include/Common/Common.h"
#include "include/ProcessManager/Process.h"
#include "UI/form/frmmain.h"

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

        frmMain* ui_;
    public:
        CPU(int time_slot, frmMain* ui);
        ~CPU();
        void run() override;

    signals:
        void refresh();
    };
}
#endif // CPU_H
