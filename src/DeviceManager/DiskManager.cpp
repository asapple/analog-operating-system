#include "include/DeviceManager/DiskManager.h"

namespace os
{

    DiskManager::DiskManager()
    {
        QBitArray disk_bitmap = QBitArray(DISk_SIZE / BLOCK_SIZE, false); //使用QBitArray构建bitmap
    }

    // parameter: blocks to be occupied
    // return: occupied blocks id
    QVector<int> DiskManager::RequestDisk(int blocks_need)
    {
        QVector<int> return_free = QVector<int>();
        for (int i = 0; i < return_free.size() && blocks_need > 0; i++)
        {
            if (disk_bitmap.testBit(i))
            {                             //该方法表示 i处 若为1返回true，否则返回 false
                return_free.append(i);    //空闲位置则加入需要返回 块容器
                disk_bitmap.toggleBit(i); //该方法为将 i处 bitmap值置为相反的值
                blocks_need--;            //需要申请block-1
            }
        }
        if (blocks_need == 0)
            return return_free; //若成功分配，则返回占用磁盘块的vector
        else
        { //否则将部分占用磁盘块清除
            for (auto i : return_free)
            {
                disk_bitmap.toggleBit(i);
            }
            return QVector<int>(NULL); //返回空指针
        }
    }

    // parameter: reverse procedure from RequestDisk()
    int DiskManager::ReleaseDisk(QVector<int> blocks_vector)
    {
        for (auto i : blocks_vector)
        {
            disk_bitmap.toggleBit(i);
            if (disk_bitmap.testBit(i))
                return -1; //表示磁盘中存在数据从0变成1，释放资源异常,返回-1;
        }
        return 0; //释放正常返回0；
    }

    QVector<int> DiskManager::GetDisk()
    {
        QVector<int> return_occupied = QVector<int>(); //定义需 返回磁盘已经被占用的容器
        for (int i = 0; i < return_occupied.size(); i++)
        {
            if (disk_bitmap.testBit(i))
            {                              //该方法表示 i处 若为1返回true，否则返回 false
                return_occupied.append(i); //占用位置则加入需要返回 块容器
            }
        }
        return return_occupied;//返回所有位为1的列表
    }

}
