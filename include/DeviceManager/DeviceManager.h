#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H
#include "include/Common/Common.h"
#include <QVector>
namespace os
{
    struct PidTime
    { //调用设备  需要的 pid与time  主要用于创建设备内队列
        int pid_;
        int time_; //单位默认为秒
    };

    class Device
    {
    private:
        /* 是否增加创建新device功能？ */
        QVector<PidTime> now_queue_; //存储该设备当前队列
        int isInterupt_;             //该设备中断信号位  1表示当前进程完成发生中断
        int prioruty_;               //设备优先级
        enum
        {
            BUSY,
            IDLE
        } state_; //该设备当前状态
    public:
        Device();
        ~Device(){};

        int GetIsInterupt() { return isInterupt_; }; //获取该设备中断信号位
        int GetState() { return state_; };     //获取该设备当前状态
        void SetIsInterupt(int newIsInterupt); //设置该设备中断信号位

        // 为了方便操作, 我在Device类 都创建了二级接口 方便控制privite变量  QAQ
        void InsertQueue(pid_t pid, size_t time); //接收DeviceManager信息，用来响应请求
        int RemoveQueue(pid_t pid);               //接收DeviceManager信息，用来删除进程
        int UpdateTimeDevice();                   //接收DeviceManager信息，用来更新设备
    };

    class DeviceManager
    {
    private:
        QVector<Device> device_; //设备队列
        DeviceManager();
        ~DeviceManager(){};

    public:
        static DeviceManager &Instance();                        //创建单例
        int RequestDevice(pid_t pid, size_t time, dev_t dev_id); //请求设备
        int RemoveProcess(pid_t pid);                            //删除传入pid对应进程的设备使用
        int UpdateTime();                                        //更新设备管理类内设备
    };
}
#endif // DEVICEMANAGER_H
