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
    QVector<QString> filename_vector;
    static QTableWidgetItem* item;

    for(int i = 0; i < busy_vector.length(); i++)
    {
        QString filename;
        int inode = dim.blocks_[i].owner_;
        filename = FileManager::Instance().GetFullPath(inode);
//        QStringList list = filename.split("/");
//        filename = list.last();
        filename_vector.append(filename);
    }
    ui->totTab_label->setText("磁盘空间: "+QString::number(dim.disk_size_)+" Bytes");
    ui->usedTab_label->setText("已使用: "+QString::number(busy_vector.length()*64)+" Bytes");
    ui->freeTab_label->setText("剩余: "+QString::number(dim.disk_size_-busy_vector.length()*64)+" Bytes");


    for(int i = 0; i< dim.block_num_; i++)
    {
        int flag = 0;
        for(int j = 0; j< busy_vector.length(); j++)
        {
            if(busy_vector[j] == i) flag = 1;
        }
        if (flag)
        {
            createItemsARow(i,i,64,64,0,"   used by " + filename_vector[i]);
        }
        else
        {
            createItemsARow(i,i,64,0,64);
        }
    }
}
