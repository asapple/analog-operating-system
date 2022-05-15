#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H
#include <QString>
#include "include/Common/Common.h"
namespace os {
    class MemoryManager {
    public:
        int InitMemory(pid_t pid, const QString& file_name);
        int GetCode(pid_t pid, size_t virt_addr, QString& content);

        int AccessMemory(pid_t pid, size_t virt_addr); //ps. content 默认缺省，无实际含义
        int MoreMemory(pid_t pid, size_t size);
        int ReleaseMemory(pid_t pid);
    };
}
#endif // MEMORYMANAGER_H
