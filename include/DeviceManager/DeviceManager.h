#ifndef DEVICEMANAGER_H
#define DEVICEMANAGER_H
#include "include/Common/Common.h"
#include <QVector>
namespace os
{
    enum state
    {
        BUSY,
        IDLE
    };
    struct PidTime
    { //调用设备  需要的 pid与time  主要用于创建设备内队列
        int pid_;
        int time_; //单位默认为秒
    };

    class Device
    {
    private:
        int isInterupt_;             //该设备中断信号位  1表示当前进程完成发生中断
        int priority_;               //设备优先级
        int dev_num_;                // 设备号
        state state_;                      //该设备当前状态
    public:
        QVector<PidTime> now_queue_; //存储该设备当前队列

        Device(dev_t dev_num = 0);

        int GetIsInterupt() { return isInterupt_; }; //获取该设备中断信号位
        state GetState() { return state_; };         //获取该设备当前状态
        int GetDevNum() { return dev_num_; };        //获取该设备设备号
        void SetIsInterupt(int newIsInterupt);       //设置该设备中断信号位

        void InsertQueue(pid_t pid, size_t time); //接收DeviceManager信息，用来响应请求
        int RemoveQueue(pid_t pid);               //接收DeviceManager信息，用来删除进程
        int UpdateTimeDevice();                   //接收DeviceManager信息，用来更新设备
    };

    class DeviceManager
    {
    private:
        DeviceManager();
        ~DeviceManager(){};

    public:
        QVector<Device> device_; //设备队列
        static DeviceManager &Instance();

        /**
         * @brief 请求设备
         * @param pid 请求设备的进程号
         * @param time 需要占用该设备的时间
         * @param dev_id 请求设备的设备号
         * @return 正确执行时，返回0
         * @return 错误码-1，表示申请失败
         */
        int RequestDevice(pid_t pid, size_t time, dev_t dev_id);
        /**
         * @brief 删除pid对应进程的设备使用
         * @param pid 删除设备进程的进程号
         * @return 正确执行时，返回0
         * @return 错误码-1，表示未找到设备队列中的pid
         */
        int RemoveProcess(pid_t pid);

        /**
         * @brief 更新设备管理类内设备
         */
        int UpdateTime();
    };
}
#endif // DEVICEMANAGER_H
