#include "include/ProcessManager/CPU.h"
#include <QDebug>
#include <QMutexLocker>
namespace os {
CPU::CPU(int time_slot)
    : time_slot_(time_slot)
{

}
void CPU::run() {
    while (!isInterruptionRequested()) {
        // do sth...
        qDebug() << "check 100ms...";
        msleep(time_slot_);
    }
}
CPU::~CPU() {
    requestInterruption();
    quit();
    wait();
}
}
