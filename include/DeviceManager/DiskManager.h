#ifndef DISKMANAGER_H
#define DISKMANAGER_H
#include "include/Common/Common.h"
#include <QVector>
namespace os {
    class DiskManager {
    public:
        // parameter: blocks to be occupied
        // return: occupied blocks id
        QVector<int> RequestDisk(int blocks_need);

        // parameter: reverse procedure from RequestDisk()
        int ReleaseDisk(QVector<int> blocks_vector);
    };
}
#endif // DISKMANAGER_H
