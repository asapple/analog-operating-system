#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <QVector>
// TODO: 从现实中的某个文件系统根目录读取目录树，并拷贝到模拟操作系统的文件系统中
// TODO: 读取现实操作系统中的文件的时候，逐行读取，转化为一个指令
/*
 类似于这样：
    QFile file("file_name.txt"); //对应真实操作系统中的文件
    QByteArray data_; //文件数据
    if (file.open(QFile::ReadOnly | QIODevice::Text)) {
        while (!file.atEnd()) {
            QByteArray str = file.readLine();
            os::Instruction ins(str); // 从字符数组或字符串中获取指令
            data_.append(static_cast<QByteArray>(ins)); //将指令保存到数组里
        }
        file.close();
    } else {
       // 错误
    }
 */

namespace os
{
    class Inode
    {
    private:
        int attribute_;   // 0 : dir, 1 : file
        int access_mode_; // 0x777, 0x003, 0x000 ...
        int size_;
        int n_links_;
        //        QVector<char> data_;    // 暂不绑定指针, 直接返回整个inode绑定的文件内容

    public:
        Inode(int attribute, int access_mode, int size, int n_links);
    };

    class File
    {
    private:
        bool file_mode_;
        bool file_flags_;
        bool file_count_;
        QVector<char> data_; // 暂不绑定指针, 直接返回整个inode绑定的文件内容
    };

    class DirectorEntry
    {
    private:
        int inode_;
        QString name_;
    };

    class Director
    {
    private:
        QVector<DirectorEntry> dir_;
    };

    class FCB : public File, public Director
    {
    private:
        class Inode fcb_inode_;
        QString fcb_name_;

    public:
        FCB(Inode fcb_inode, const QString fcb_name);
    };

    class FileManager
    {
    private:
        QVector<FCB> fm_fcb_;
        QVector<QString> cwd_;

    public:
        FileManager();

        QString List();
        int ChangeDirectory(const QString &directory_name);
        int MakeFile(const QString &file_name);
        int MakeDirectory(const QString &directory_name);
        int RemoveFile(const QString &file_name);
        int RemoveDirectory(const QString &directory_name);
        int ReadFile(const QString &file_name, QByteArray &content);
    };
}

#endif // FILEMANAGER_H
