#include "frmmain.h"
#include "ui_frmmain.h"
#include "UI/core_common/iconhelper.h"
#include "UI/core_common/quihelper.h"

#include "include/FileManager/FileManager.h"
#include "include/DeviceManager/DiskManager.h"

using namespace os;

void frmMain::updateDiskView(){
    auto& dim = DiskManager::Instance();
    QVector<int> busy_vector = dim.GetDisk();


    for(int i = 0; i< dim.block_num_; i++)
    {
        if (dim.disk_bitmap_.testBit(i)) {
            createItemsARow(i,i,64,64,0,
                            QString::fromUtf8("   used by %1").arg(FileManager::Instance().GetFullPath(dim.blocks_[i].owner_)));
        } else {
            createItemsARow(i,i,64,0,64);
        }
    }

    ui->totTab_label->setText(QString::fromUtf8("%1 Bytes").arg(QString::number(dim.disk_size_)));
    ui->usedTab_label->setText(QString::fromUtf8("%1 Bytes").arg(QString::number(busy_vector.length()*dim.block_size_)));
    ui->freeTab_label->setText(QString::fromUtf8("%1 Bytes").arg(dim.disk_size_-busy_vector.length()*dim.block_size_));


}
