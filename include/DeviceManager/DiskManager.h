#ifndef DISKMANAGER_H
#define DISKMANAGER_H
#include "include/Common/Common.h"
namespace os {
    class DiskManager {
    public:
        int RequestDisk(inode_t file, size_t size/* TODO: 添加一个表示磁盘号返回数的引用做参数  */);
        int ReleaseDisk(inode_t file);
    };
}
#endif // DISKMANAGER_H
