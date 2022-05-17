#include "include/FileManager/FileManager.h"
#include <QDebug>
using namespace os;

Inode::Inode(int attribute, int size, int access_mode = 0x777, int n_links = 1)
{
    attribute_ = attribute;
    access_mode_ = access_mode;
    size_ = size;
    n_links_ = n_links;
}

FCB::FCB(Inode fcb_inode, const QString fcb_name) : fcb_inode_(fcb_inode)
{
    fcb_name_ = fcb_name;
}

// MakeDiretory
// parameters: dir name
// return value: success or not
int FileManager::MakeDirectory(const QString &directory_name)
{
    Inode inode = Inode(0, 777, 0, 1);
    FCB fcb = FCB(inode, directory_name);

    fm_fcb_.push_back(fcb);
}

FileManager::FileManager()
{
    fm_fcb_.clear();
    cwd_.clear();

    // initial
}
