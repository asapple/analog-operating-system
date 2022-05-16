#ifndef CPU_H
#define CPU_H
#include <QThread>
#include <QMutex>

#include "include/Common/Common.h"

namespace os {
    class CPU: public QThread {
        Q_OBJECT
    private:
        int time_slot_;
        pid_t current_run_;
    public:
        CPU(int time_slot);
        ~CPU();
        void run() override;
    };
}
#endif // CPU_H
