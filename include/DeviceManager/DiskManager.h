#ifndef DISKMANAGER_H
#define DISKMANAGER_H
#include "include/Common/Common.h"
#include <QVector>
#include <QBitArray>

namespace os
{
    class DiskManager
    {
        QBitArray disk_bitmap; //表示磁盘的位图

        DiskManager();
        ~DiskManager(){};

    public:
        QVector<int> RequestDisk(int blocks_need);   //申请磁盘资源 ，返回占用的磁盘块的列表
        int ReleaseDisk(QVector<int> blocks_vector); // 释放磁盘资源

        QVector<int> GetDisk(); //获取整个磁盘的占用情况
    };
}
#endif // DISKMANAGER_H
