#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QString>
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

namespace os {
    class FileManager {
    public:
        QString List();
        int ChangeDirectory(const QString& directory_name);
        int MakeFile(const QString& file_name);
        int MakeDirectory(const QString& directory_name);
        int RemoveFile(const QString& file_name);
        int RemoveDirectory(const QString& directory_name);
        int ReadFile(const QString& file_name, QByteArray& content);
    };
}
#endif // FILEMANAGER_H
