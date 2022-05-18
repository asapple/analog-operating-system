#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <QVector>
#include <QByteArray>
#include <QHash>

#include "include/Common/Common.h"

namespace os
{
    struct Inode
    {
        int access_mode_; // 0x777, 0x003, 0x000 ...
        int fcb_;
        int size_;
        int n_links_;
        int attribute_; // 0 : dir, 1 : file
        QString fcb_name_;

        Inode(int attribute, QString fcb_name, int fcb, int size, int access_mode, int n_links);
    };

    class File
    {
    public:
        QByteArray data_; // 暂不绑定指针, 直接返回整个inode绑定的文件内容
    };

    struct DirectoryEntry
    {
        inode_t inode_;
        QString name_;
        DirectoryEntry(inode_t inode = -1, QString name = ""):
            inode_(inode),
            name_(name)
        {}
    };

    class Directory
    {
    public:
        QVector<DirectoryEntry> files_;
        QVector<DirectoryEntry> subdirs_;
        DirectoryEntry pre;
    };

    class FCB : public File, public Directory
    {
    public:
        FCB() = default;
    };

    // FileManager method return value: 0: success / -1: fail
    class FileManager
    {
    private:
        QVector<Inode> fm_inodes_;
        QVector<FCB> fm_fcb_;
        // 根目录
        inode_t root_;
        // 当前目录
        inode_t cwd_;
        // umask
        int umask_;
        // 根目录映射到现实操作系统的路径
        QString root_path_;

        inode_t InsertFCB(int attribute, QString fcb_name, const FCB& fcb = FCB());
        int ForkCreate(const QString& real_path, inode_t pre = -1);
        int ForkSave(const QString& real_path, inode_t cur);
        inode_t Str2Path(const QString& path);
        FileManager(const QString& path, int umask = 0x644);  // ok
        ~FileManager();
    public:
        FileManager& Instance(const QString& forkroot = "./files");
        QString GetCWD();

        int List(QVector<QString>& files, QVector<QString>& dirs );                                              // ok
        int ChangeDirectory(QString path);          // ok
        int MakeFile(QString file_name);                      // ok
        int MakeDirectory(QString directory_name);            // ok
        int RemoveFile(QString file_name);                    // ok
        int RemoveDirectory(QString directory_name);          // ok
        int ReadFile(QString file_name, QByteArray &content); // ok
    };
}

#endif // FILEMANAGER_H
