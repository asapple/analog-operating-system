#ifndef CPU_H
#define CPU_H
#include <QThread>
#include <QMutex>

#include "include/Common/Common.h"
#include "include/ProcessManager/Process.h"

namespace os {
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
