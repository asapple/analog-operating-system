#ifndef FILEMANAGER_H
#define FILEMANAGER_H
#include <QString>
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
