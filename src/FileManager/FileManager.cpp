#include "include/FileManager/FileManager.h"
#include <QDebug>
#include <QRegExp>
#include <QStringList>
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

// MakeDiretory()
// parameters: dir name
// return value: success or not
int FileManager::MakeDirectory(const QString &directory_name)
{
    if (directory_name.length() == 0) // nothing input
    {
        if (!init_flag_)
        {
            qDebug() << "Illegal character '' inside." << endl;
            return -1;
        }
        init_flag_ = false;
    }
    if (directory_name.indexOf("/")) // Illegal character '/'
    {
        qDebug() << "Illegal character '/' inside." << endl;
        return -1;
    }

    Inode inode = Inode(0, 777, 0, 1); // TODO: dir size init val?
    FCB fcb = FCB(inode, directory_name);

    // TODO: fail occasion
    fm_fcb_.push_back(fcb);
<<<<<<< HEAD
    return 0;
=======
    inode2fcb_[inode] = fcb;
    return 1;
>>>>>>> 92c26a8c822bb34389bc4d84ea4aef787d055176
}

FileManager::FileManager()
{
    fm_fcb_.clear();
    cwd_.clear();

    // initial
    QString root_dir_name = "";
    MakeDirectory(root_dir_name);

    FCB root_fcb = fm_fcb_.back();
    cwd_.push_back(root_fcb);
    inode2fcb_[root_fcb.fcb_inode_] = root_fcb;
}

FileManager::~FileManager()
{
    // TODO: inside elements to be free

    fm_fcb_.clear();
    cwd_.clear();
}

// List()
// return: list command output result
QString FileManager::List()
{
    QString list_text;
    for (auto dir_entry : cwd_.back().dir_)
    {
        list_text.append(dir_entry.name_);
    }
    if (list_text.length() != 0)
    {
        list_text.chop(1);
    }
    return list_text;
}

// ChangeDirectory()
// parameter: change target
// return: change success or not
int FileManager::ChangeDirectory(const QString &directory_name)
{
    QVector copy_cwd(cwd_);
    QString inside_dir_name(directory_name);
    if (inside_dir_name.indexOf("//") >= 0)
    {
        qDebug() << "Illegal input!" << endl;
        return -1;
    }

    if (inside_dir_name.at(inside_dir_name.size() - 1) == '/')
    {
        inside_dir_name.chop(1);
    }
    QStringList list = inside_dir_name.split(QLatin1Char('/'));
    for (auto dir_piece : list)
    {
        if (dir_piece == "")
        { // full path
            FCB root_fcb = cwd_.first();
            cwd_.clear();
            cwd_.push_back(root_fcb);
            continue;
        }
        if (dir_piece == ".")
        { // now dir
            continue;
        }
        if (dir_piece == "..")
        { // parent dir
            cwd_.pop_back();
            continue;
        }

        // match occasion
        QVector<DirectoryEntry> now_dir = cwd_.back().dir_;
        for (DirectoryEntry dir_entry : now_dir)
        {
            if (dir_entry.name_ != dir_piece)
            {
                continue;
            }
            // matched
            cwd_.push_back(inode2fcb_[dir_entry.inode_]);
            return 0;
        }

        // fail to match
        cwd_ = copy_cwd;
        qDebug() << "Illegal input directory!" << endl;
        return -1;
    }
}
