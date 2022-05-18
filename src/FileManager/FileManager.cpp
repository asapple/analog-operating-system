#include "include/FileManager/FileManager.h"
#include <QDebug>
#include <QRegExp>
#include <QStringList>
using namespace os;

Inode::Inode(int attribute, int access_mode = 0x644, int size = 4096, int n_links = 1)
{
    attribute_ = attribute;
    access_mode_ = access_mode;
    size_ = size;
    n_links_ = n_links;
}

Inode::~Inode() {}

FCB::FCB(Inode fcb_inode, const QString fcb_name) : fcb_inode_(fcb_inode)
{
    fcb_name_ = fcb_name;
}

FCB::~FCB() {}

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
    QVector<DirectoryEntry> now_dir = cwd_.back().dir_;
    for (DirectoryEntry dir_entry : now_dir)
    {
        if (dir_entry.name_ == directory_name) // confilct name
        {
            qDebug() << "directory name confilct!" << endl;
            return -1;
        }
    }

    Inode inode = Inode(0, 0x755, 4096, 2); // dir size init val = 4096
    FCB fcb = FCB(inode, directory_name);

    // TODO: fail occasion
    fm_fcb_.push_back(fcb);
    inode2fcb_[inode] = fcb;
<<<<<<< HEAD
    return 0;
=======
    return 1;
>>>>>>> d95f463820242a612e676eba0b81a6e59f14793a
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
// PS: support absolute & relative directory path && Hierarchical directory
int FileManager::ChangeDirectory(const QString &directory_name)
{
    QVector copy_cwd(cwd_);
    QString inside_dir_name(directory_name);
    if (inside_dir_name.indexOf("//") >= 0) // example: '/first/second//next'
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
            continue;
        }

        // fail to match
        cwd_ = copy_cwd;
        qDebug() << "Illegal input directory!" << endl;
        return -1;
    }
    return 0;
}

// MakeFile()
// parameters: file name
// return value: success or not
// PS: only accept to create new file in current directory.
int FileManager::MakeFile(const QString &file_name)
{
    if (file_name.length() == 0) // nothing input
    {
        qDebug() << "nothing input." << endl;
        return -1;
    }
    if (file_name.indexOf("/")) // Illegal character '/'
    {
        qDebug() << "Illegal character '/' inside." << endl;
        return -1;
    }
    QVector<DirectoryEntry> now_dir = cwd_.back().dir_;
    for (DirectoryEntry dir_entry : now_dir)
    {
        if (dir_entry.name_ == file_name) // confilct name
        {
            qDebug() << "file name confilct!" << endl;
            return -1;
        }
    }

    // conrect input
    Inode inode = Inode(0, 0x644, 0, 1); // file size init val = 0
    FCB fcb = FCB(inode, file_name);

    // TODO: fail occasion
    fm_fcb_.push_back(fcb);
    inode2fcb_[inode] = fcb;
    return 0;
}

// RemoveFile()
// parameters: file name
// return value: success or not
// PS: only accept to remove file in current directory.
int FileManager::RemoveFile(const QString &file_name)
{
    if (file_name.length() == 0) // nothing input
    {
        qDebug() << "nothing input." << endl;
        return -1;
    }
    if (file_name.indexOf("/")) // Illegal character '/'
    {
        qDebug() << "Illegal character '/' inside." << endl;
        return -1;
    }
    QVector<DirectoryEntry> now_dir = cwd_.back().dir_;
    for (DirectoryEntry dir_entry : now_dir)
    {
        if (dir_entry.name_ == file_name) // target matched
        {
            FCB target_file = inode2fcb_[dir_entry.inode_];
            if (target_file.fcb_inode_.attribute_ == 1)
            {
                now_dir.removeOne(dir_entry);
                return 0;
            }
            else // attempt to remove a direfctory of the same name
            {
                qDebug() << "target name is not a file!" << endl;
                return -1;
            }
        }
    }
    // target not exist
    qDebug() << "target file doesn\'t exist!" << endl;
    return -1;
}

// RemoveDirectory()
// parameters: dir name
// return value: success or not
// PS: only accept to remove directory in current directory.
int FileManager::RemoveDirectory(const QString &directory_name)
{
    QString inside_dir_name(directory_name);
    if (inside_dir_name.indexOf("//") >= 0) // example: 'first//'
    {
        qDebug() << "Illegal input!" << endl;
        return -1;
    }
    if (inside_dir_name.length() == 0) // nothing input
    {
        qDebug() << "nothing input." << endl;
        return -1;
    }

    if (inside_dir_name.at(inside_dir_name.size() - 1) == '/') // example: 'first/'
    {
        inside_dir_name.chop(1);
    }
    if (inside_dir_name.indexOf("/")) // Illegal character '/' inside
    {
        qDebug() << "Illegal character '/' inside." << endl;
        return -1;
    }
    QVector<DirectoryEntry> now_dir = cwd_.back().dir_;
    for (DirectoryEntry dir_entry : now_dir)
    {
        if (dir_entry.name_ == inside_dir_name) // target matched
        {
            FCB target_file = inode2fcb_[dir_entry.inode_];
            if (target_file.fcb_inode_.attribute_ == 0)
            {
                now_dir.removeOne(dir_entry);
                return 0;
            }
            else // attempt to remove a file of the same name
            {
                qDebug() << "target name is not a directory!" << endl;
                return -1;
            }
        }
    }
    // target not exist
    qDebug() << "target file doesn\'t exist!" << endl;
    return -1;
}

// ReadFile()
// parameters: file name,
// return value: success or not
// PS: only accept to remove directory in current directory.
int FileManager::ReadFile(const QString &file_name, QByteArray &content)
{
    if (file_name.length() == 0) // nothing input
    {
        qDebug() << "nothing input." << endl;
        return -1;
    }
    if (file_name.indexOf("/")) // Illegal character '/'
    {
        qDebug() << "Illegal character '/' inside." << endl;
        return -1;
    }
    QVector<DirectoryEntry> now_dir = cwd_.back().dir_;
    for (DirectoryEntry dir_entry : now_dir)
    {
        if (dir_entry.name_ == file_name) // target matched
        {
            FCB target_file = inode2fcb_[dir_entry.inode_];
            if (target_file.fcb_inode_.attribute_ == 1)
            {
                content = target_file.data_;
                return 0;
            }
            else // attempt to read a direfctory of the same name
            {
                qDebug() << "target name is not a directory!" << endl;
                return -1;
            }
        }
    }
    // target not exist
    qDebug() << "target file doesn\'t exist!" << endl;
    return -1;
}
