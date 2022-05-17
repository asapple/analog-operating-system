#include "include/DeviceManager/DeviceManager.h"

namespace os
{

    Device::Device()
    {
        isInterupt_ = 0; //未发生中断
        prioruty_ = 1;   //默认优先级为1
        state_ = IDLE;   //默认空闲状态
    }

    void Device::SetIsInterupt(int newIsInterupt)
    {
        isInterupt_ = newIsInterupt;
    }

    void Device::InsertQueue(pid_t pid, size_t time)
    {
        PidTime temp_pid_time_; //创建进程结构体
        temp_pid_time_.pid_ = pid;
        temp_pid_time_.time_ = time;
        if (now_queue_.empty())
            state_ = BUSY;                 //队列为空则将状态设置为繁忙
        now_queue_.append(temp_pid_time_); //加入当前设备的队列末尾
    }

    int Device::RemoveQueue(pid_t pid)
    {
        for (auto i = now_queue_.begin(); i != now_queue_.end(); i++)
        {
            if (i->pid_ == pid) //查找到进程pid 则删除
            {
                now_queue_.erase(i);
                if (i == now_queue_.begin())
                    isInterupt_ = 1; //若删除的pid为当前设备正在执行进程，产生中断告诉CPU已经完成当前任务
                return 0;            //删除成功返回0
            }
        }
        return -1; //删除失败，未找到pid
    }

    int Device::UpdateTimeDevice()
    {
        now_queue_.begin()->time_--;
        if (now_queue_.begin()->time_ == 0)
        {                    //当前任务执行完成
            isInterupt_ = 1; //中断标志位置1
            /*
             * 上报CPU处理,不会写了
             */
            now_queue_.remove(0); //删除队首任务;
            if (now_queue_.empty())
                state_ = IDLE; //若执行完成后队列为空 改变设备状态；
        }
        return 0;
    }

    DeviceManager::DeviceManager()
    {
        for (int i = 0; i < DEVICE_SIZE; i++)
        { //创建系统中设备
            Device temp_device_;
            device_.insert(i, temp_device_);
        }
    }
    DeviceManager &DeviceManager::Instance()
    {
        static DeviceManager dm;
        return dm;
    }

    int DeviceManager::RequestDevice(pid_t pid, size_t time, dev_t dev_id)
    {
        QT_TRY // 成功创建返回0
        {
            device_[dev_id].InsertQueue(pid, time);
            return 0;
        }
        QT_CATCH(int ex) // 若出现异常返回-1
        {
            return -1;
        }
    }

    int DeviceManager::RemoveProcess(pid_t pid)
    {
        for (int i = 0; i < DEVICE_SIZE; i++)
        {
            if (!device_[i].RemoveQueue(pid))
                return 0; //删除成功 返回0
        }
        return -1; //删除失败，未找到pid
    }

    int DeviceManager::UpdateTime()
    {
        QT_TRY
        {
            for (int i = 0; i < DEVICE_SIZE; i++)
            {
                device_[i].UpdateTimeDevice(); //分别执行对应设备更新函数
            }
            return 0;
        }
        QT_CATCH(int ex) // 若出现异常返回-1
        {
            return -1;
        }
    }

}
