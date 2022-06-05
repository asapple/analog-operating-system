#include "include/DeviceManager/DiskManager.h"

namespace os
{
    DiskManager::DiskManager(int block_size, int disk_size)
        :block_size_(block_size),
         disk_size_(disk_size),
         block_num_((disk_size+block_size-1) / block_size),

         disk_bitmap_(QBitArray(block_num_, false)),
         blocks_(block_num_)
    {
        for (int i = 0; i < blocks_.size(); i++) {
            blocks_[i].owner_ = -1;
            blocks_[i].dno_ = i;
        }
    }
    DiskManager& DiskManager::Instance(int block_size, int disk_size)
    {
        static DiskManager dim(block_size, disk_size);
        return dim;
    }


    int DiskManager::RequestDisk(inode_t file, int blocks_need)
    {
        bool is_found = false;
        for (int i = 0; i < block_num_; i++) {
            // 查找连续blocks_need个空闲块
            if (!disk_bitmap_.testBit(i)) {
                is_found = true;
                int j = 0;
                for (j = 0; j < blocks_need; j++) {
                    if (disk_bitmap_.testBit(i+j)) {
                        is_found = false;
                        break;
                    }
                }
                // 找到了
                if (is_found) {
                    for (int k = i; k < i + j; k++) {
                        disk_bitmap_.setBit(k);
                        blocks_[k].owner_ = file;
                    }
                    return i;
                } else {
                    // 没有找到，则跳转到下一个
                    i += j;
                }
            }
        }
        return -1;
    }

    int DiskManager::ReleaseDisk(inode_t inode)
    {
        for (int i = 0; i < block_num_;i++) {
            if (disk_bitmap_.testBit(i)) {
                if (blocks_[i].owner_ == inode) {
                    blocks_[i].owner_ = -1;
                    disk_bitmap_.clearBit(i);
                }
            }
        }
        return 0;
    }

    QVector<int> DiskManager::GetDisk()
    {
        QVector<int> return_occupied = QVector<int>(); //定义需返回磁盘已经被占用的容器
        for (int i = 0; i < block_num_; i++)
        {
            if (disk_bitmap_.testBit(i))
            {                              //该方法表示 磁盘号为i处 若为1返回true，否则返回 false
                return_occupied.append(i); //占用位置则加入需要返回 块容器
            }
        }
        return return_occupied;//返回所有位为1的列表
    }

}
