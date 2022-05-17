#include "include/FileManager/FileManager.h"
#include <QDebug>
#include <QRegExp>
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
    if (directory_name.length() == 0) // nothing input
    {
        if (!init_flag_)
        {
            QDebug() << "Illegal character '' inside." << std::endl;
            return -1;
        }
        init_flag_ = false;
    }
    if (directory_name.indexOf("/")) // Illegal character '/'
    {
        QDebug() << "Illegal character '/' inside." << std::endl;
        return -1;
    }

    Inode inode = Inode(0, 777, 0, 1); //TODO: dir size init val?
    FCB fcb = FCB(inode, directory_name);

    //TODO: 
    fm_fcb_.push_back(fcb);
    return 1;
}

FileManager::FileManager()
{
    fm_fcb_.clear();
    cwd_.clear();

    // initial
    
    if(MakeDirectory("") )
    cwd_.push_back()
}

~FileManager::FileManager()
{
    //TODO: inside elements free

    fm_fcb_.clear();
    cwd_.clear();
}