#ifndef INTERUPT_H
#define INTERUPT_H

#include <QMap>
#include <QList>

#include "include/Common/Common.h"

namespace os {
// TODO 中断机制还可以考虑一下怎样呈现，目前只是单纯的状态改变，没有体现中断向量表
/**
 * @brief 中断请求
 */
struct Interupt {
    // 设备号
    dev_t dev_num_;
    // 进程号
    pid_t pid_;
    // 中断请求优先级
    priority_t priority_;
};
/**
 * @brief 中断管理系统
 * 负责管理中断请求、中断检测、中断响应和处理
 */
class InteruptManager
{
private:
    bool is_enable_; // 是否开启中断
    QMap<priority_t,QList<Interupt>> interupt_queue_; // 中断队列，按照优先级排序
public:
    static InteruptManager& Instance();
    void SetInterupt(bool is_enable); //TODO 在哪用?
    int InteruptDetect();
    int InteruptRequest(const Interupt& interupt);
};

}

#endif // INTERUPT_H
