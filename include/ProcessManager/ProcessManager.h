#ifndef PROCESSMANAGER_H
#define PROCESSMANAGER_H
#include "include/Common/Common.h"
namespace os {
    enum class ProcessState {

            READY,
            RUNNING,
            WAIT,
            KILLED,
    };
    class ProcessManager {
    public:

    };
}
#endif // PROCESSMANAGER_H
