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

        /**
         * @brief 申请磁盘资源，连续存储
         * @param file 申请磁盘资源的文件
         * @param blocks_need 该文件需要申请的磁盘大小
         * @return 正确执行时，返回所申请到的磁盘空间首位的编号
         * @return 错误码-1，当前磁盘不存在足够大连续空间分配给该文件
         */
        int RequestDisk(inode_t file, int blocks_need);
        /**
         * @brief 释放磁盘资源
         * @param file 释放磁盘资源的文件
         * @return 正确执行时，返回0
         */
        int ReleaseDisk(inode_t indode);
        /**
         * @brief 获取整个磁盘的占用情况
         * @return 正确执行时，返回已分配空间位置的列表
         */
        QVector<int> GetDisk(); //
    };
}
#endif // DISKMANAGER_H
