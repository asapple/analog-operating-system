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
    for(int i = 0; i < busy_vector.length(); i++)
    {
        QString filename;
        int inode = dim.blocks_[i].owner_;
        filename = FileManager::Instance().GetFullPath(inode);
        QStringList list = filename.split("/");
        filename = list.last();
        filename_vector.append(filename);
    }
}
