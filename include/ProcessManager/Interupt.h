#ifndef INTERUPT_H
#define INTERUPT_H

#include <QMap>
#include <QList>

#include "include/Common/Common.h"

namespace os {
// TODO 中断机制还可以考虑一下怎样呈现，目前只是单纯的状态改变，没有体现中断向量表

struct Interupt {
    dev_t dev_num_;
    pid_t pid_;
    priority_t priority_;
};

class InteruptManager
{
private:
    bool is_enable_;
    QMap<priority_t,QList<Interupt>> interupt_queue_;
public:
    static InteruptManager& Instance();
    void SetInterupt(bool is_enable); //TODO 在哪用?
    int InteruptDetect();
    int InteruptRequest(const Interupt& interupt);
};

}

#endif // INTERUPT_H
