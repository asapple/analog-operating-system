#ifndef DISKMANAGER_H
#define DISKMANAGER_H
#include "include/Common/Common.h"
#include <QVector>
#include <QBitArray>

namespace os
{
    struct DiskBlock {
        // 磁盘号
        int dno_;
        // 所属文件号
        inode_t owner_;
    };

    class DiskManager
    {
    private:
        DiskManager(int block_size = BLOCK_SIZE, int disk_size = DISk_SIZE);
    public:
        const int block_size_;
        const int disk_size_;
        const int block_num_;

        QBitArray disk_bitmap_; //表示磁盘的位图
        QVector<DiskBlock> blocks_;

        static DiskManager& Instance(int block_size = BLOCK_SIZE, int disk_size = DISk_SIZE);
        int RequestDisk(inode_t file, int blocks_need);   //申请磁盘资源 ，返回占用的磁盘块的列表， 连续存储
        int ReleaseDisk(inode_t indode); // 释放磁盘资源

        QVector<int> GetDisk(); //获取整个磁盘的占用情况
    };
}
#endif // DISKMANAGER_H
