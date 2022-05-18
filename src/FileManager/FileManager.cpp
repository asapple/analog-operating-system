#include "include/Common/Common.h"
#include "include/FileManager/FileManager.h"
#include "include/ProcessManager/Instruction.h"
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QRegExp>
#include <QStringList>
using namespace os;
/**
 * @brief Inode::Inode
 * @param attribute 文件属性
 * @param fcb_name 文件名
 * @param fcb 文件指向的FCB
 * @param size 文件大小
 * @param access_mode 访问权限
 * @param n_links 连接数
 */
Inode::Inode(int attribute, QString fcb_name,int fcb,int size = 4096, int access_mode = 0x644, int n_links = 1)
{
    attribute_ = attribute;
    access_mode_ = access_mode;
    size_ = size;
    n_links_ = n_links;
    fcb_ = fcb;
    fcb_name_ = fcb_name;
}
/**
 * @brief FileManager::InsertFCB
 * @param attribute 文件属性
 * @param fcb_name 文件名
 * @param fcb 文件绑定的FCB
 * @return 插入系统后的inode值
 */
inode_t FileManager::InsertFCB(int attribute, QString fcb_name, const FCB& fcb)
{
    int fcb_id = fm_fcb_.size();
    fm_fcb_.push_back(FCB());
    inode_t inode = fm_inodes_.size();
    fm_inodes_.push_back(Inode(attribute, fcb_name, fcb_id, fcb.data_.size(), umask_));
    return inode;
}
/**
 * @brief FileManager::ForkCreate
 * @param real_path 现实操作系统路径
 * @param pre 上一路径的inode号
 * @return 创建后的目录inode号
 */
inode_t FileManager::ForkCreate(const QString& real_path, inode_t pre)
{
    QDir dir(real_path);
    if (!dir.exists()) {
        return -1;
    }

    // 在模拟文件系统中创建对应的目录项
    inode_t inode = InsertFCB(1, dir.dirName());
    FCB& cur = fm_fcb_[fm_inodes_[inode].fcb_];
    cur.subdirs_.push_back(DirectoryEntry(inode, "."));
    cur.pre = DirectoryEntry(pre, "..");
    // 文件读入
    for (auto file_info : dir.entryInfoList(QDir::Filter::Files)) {
        FCB fcb;
        QString file_path = file_info.absoluteFilePath();
        QFile file(file_path); //对应真实操作系统中的文件
        if (file.open(QFile::ReadOnly | QIODevice::Text)) {
            while (!file.atEnd()) {
                QByteArray str = file.readLine();
                Instruction ins(str); // 从字符数组或字符串中获取指令
                fcb.data_.append(static_cast<QByteArray>(ins)); //将指令保存到数组里
            }
            file.close();
        } else {
           qDebug() << "[FileSystem] Init Error: File '" << file_path <<"' can't read.";
        }
        inode_t ino = InsertFCB(0, file_info.suffix(),fcb);
        cur.files_.push_back(ino);
    }
    // 递归目录读入
    for (auto dir_info: dir.entryInfoList(QDir::Filter::Dirs)) {
        inode_t subdir = ForkCreate(dir_info.absolutePath(), inode);
        cur.subdirs_.push_back(subdir);
    }
    return inode;
}
/**
 * @brief FileManager::ForkSave
 * @param real_path 现实操作系统路径
 * @param cur 当前inode号
 * @return -1 文件夹已存在
 * @return -2 文件夹创建失败
 * @return -3 保存失败
 */
int FileManager::ForkSave(const QString& real_path, inode_t cur)
{
    QDir dir(real_path);
    if (dir.exists()) {
        return -1;
    }
    if (!dir.mkdir(real_path)) {
        return -2;
    }
    FCB fcb = fm_fcb_[fm_inodes_[cur].fcb_];
    for (auto file = fcb.files_.begin(); file != fcb.files_.end(); file++) {
        Inode ino = fm_inodes_[file->inode_];
        QString file_path = real_path+"/"+file->name_;
        FCB ffcb = fm_fcb_[ino.fcb_];
        QFile qfile(file_path);
        if (qfile.open(QIODevice::WriteOnly)) {
            qfile.write(ffcb.data_);
            qfile.close();
        } else {
            qDebug() << "[FileSystem] Save Error: File '" << file_path <<"' can't write.";
         }
    }
    int ret = 0;
    for (auto dir = fcb.subdirs_.begin(); dir != fcb.subdirs_.end();dir++) {
        QString dir_path = real_path+"/"+dir->name_;
        if (ForkSave(real_path+"/"+dir->name_, dir->inode_) < 0) {
            ret = -3;
            qDebug() << "[FileSystem] Save Error: Directory '" << dir_path <<"' can't save successfully.";
        }
    }
    return ret;
}

/**
 * @brief FileManager::FileManager
 * @param path 真实文件系统路径
 * @param umask 初始访问权限
 */
FileManager::FileManager(const QString& path, int umask)
    :umask_(umask)
{
    root_path_ = QDir(path).absolutePath();
    root_ = ForkCreate(path);
    cwd_ = root_;
}
/**
 * @brief FileManager::~FileManager
 * 析构对象，保存对应的文件系统
 */
FileManager::~FileManager()
{
    QDir dir(root_path_);
    dir.removeRecursively();
    ForkSave(root_path_, root_);
}

FileManager& FileManager::Instance(const QString& forkroot)
{
    static FileManager fm(forkroot);
    return fm;
}

/**
 * @brief FileManager::MakeDirectory
 * @param directory_name 创建文件名
 * @return 0 成功
 * @return -1 目录名为空
 * @return -2 目录名含有非法字符
 * @return -3 目录名已存在
 * @return -4 创建目录项失败
 */
int FileManager::MakeDirectory(QString directory_name)
{
    inode_t pre = cwd_;
    if (directory_name.length() == 0)
    {
        qDebug() << "can not create empty directory";
        return -1;
    }
    if (directory_name.indexOf("/"))
    {
        int ind = directory_name.lastIndexOf("/");
        pre = Str2Path(directory_name.left(ind));
        directory_name = directory_name.right(directory_name.size()-ind);
        if (pre < 0 || fm_inodes_[pre].attribute_ == 1) {
            return -2;
        }
    }
    FCB& cur = fm_fcb_[fm_inodes_[pre].fcb_];
    for (auto dir:cur.subdirs_) {
        if (dir.name_ == directory_name) {
            qDebug() << "name has already existed.";
            return -3;
        }
    }
    inode_t inode = InsertFCB(0, directory_name);
    FCB& fcb = fm_fcb_[fm_inodes_[inode].fcb_];
    fcb.pre = DirectoryEntry(pre,"..");
    fcb.subdirs_.push_back(DirectoryEntry(inode,"."));
    if (inode < 0) {
        return -4;
    }
    cur.subdirs_.push_back(inode);
    return 0;
}



/**
 * @brief list command output result
 * @param files 当前目录下文件类型的文件名
 * @param dirs 当前目录下目录类型的文件名
 * @return 0, 成功返回
 */
int FileManager::List(QVector<QString>& files, QVector<QString>& dirs )
{
    FCB cur = fm_fcb_[fm_inodes_[cwd_].fcb_];
    for (auto file: cur.files_) {
        files.push_back(file.name_);
    }
    for (auto dir: cur.subdirs_) {
        files.push_back(dir.name_);
    }
    return 0;
}
/**
 * @brief FileManager::Str2Path
 * @param path 路径 包括绝对路径、层次路径、相对路径
 * @return 成功返回对应路径的inode号
 * @return -1 路径下不存在文件
 */
inode_t FileManager::Str2Path(const QString& path)
{
    inode_t cur = cwd_;
    QString curpath = path;
    if (path.startsWith("/")) {
        curpath = path.mid(1);
        cur = root_;
    }
    auto strlist = curpath.split("/");
    for (auto it = strlist.begin(); it !=strlist.end(); it++) {
        FCB fcb = fm_fcb_[fm_inodes_[cur].fcb_];
        bool is_find = false;
        // 从子目录中查找
        for (auto dir = fcb.subdirs_.begin(); dir != fcb.subdirs_.end(); dir++) {
            if (dir->name_ == *it) {
                is_find = true;
                cur = dir->inode_;
                break;
            }
        }
        // 若为最后一项，则可以从文件中查找
        if (!is_find && it+1 == strlist.end()) {
            for (auto file = fcb.files_.begin(); file != fcb.files_.end(); file++) {
                if (file->name_ == *it) {
                    is_find = true;
                    cur = file->inode_;
                    break;
                }
            }
        }
        if (!is_find) {
            qDebug() << "[FileSystem] Path Not Exist";
            return -1;
        }
    }
    return cur;
}
/**
 * @brief FileManager::ChangeDirectory
 * @param directory_name
 * @return 0 正常返回
 * @return -1 非法路径
 * @return -2 路径并非目录
 */
int FileManager::ChangeDirectory(QString path)
{
    inode_t ino = Str2Path(path);
    if (ino < 0) {
        return -1;
    }
    if (fm_inodes_[ino].attribute_ != 0) {
        qDebug() << "Path is not a directory";
        return -2;
    }
    cwd_ = ino;
    return 0;
}

/**
 * @brief FileManager::MakeFile
 * @param file_name 文件路径，包括各种相对路径或绝对路径
 * @return 0 成功
 * @return -1 文件名为空
 * @return -2 文件路径不存在
 * @return -3 文件名已存在
 * @return -4 创建文件失败
 */
int FileManager::MakeFile(QString file_name)
{
    inode_t pre = cwd_;
    if (file_name.length() == 0)
    {
        qDebug() << "can not create empty directory";
        return -1;
    }
    if (file_name.indexOf("/"))
    {
        int ind = file_name.lastIndexOf("/");
        pre = Str2Path(file_name.left(ind));
        file_name = file_name.right(file_name.size()-ind);
        if (pre < 0 || fm_inodes_[pre].attribute_ == 1) {
            return -2;
        }
    }
    FCB& cur = fm_fcb_[fm_inodes_[pre].fcb_];
    for (auto file = cur.files_.begin();file != cur.files_.end(); file++) {
        if (file->name_ == file_name) {
            qDebug() << "file name has already existed.";
            return -3;
        }
    }
    inode_t inode = InsertFCB(1, file_name);
    if (inode < 0) {
        return -4;
    }
    cur.files_.push_back(inode);
    return 0;
}

/**
 * @brief FileManager::RemoveFile
 * @param file_name
 * @return 0 成功
 * @return -1 文件名为空
 * @return -2 文件不存在
 */
int FileManager::RemoveFile(QString file_name)
{
    inode_t pre = cwd_;
    if (file_name.length() == 0)
    {
        qDebug() << "can not create empty directory";
        return -1;
    }
    if (file_name.indexOf("/"))
    {
        int ind = file_name.lastIndexOf("/");
        pre = Str2Path(file_name.left(ind));
        file_name = file_name.right(file_name.size()-ind);
        if (pre < 0 || fm_inodes_[pre].attribute_ == 1) {
            return -2;
        }
    }
    FCB& cur = fm_fcb_[fm_inodes_[pre].fcb_];
    for (auto iter = cur.files_.begin(); iter != cur.files_.end(); iter++) {
        if (iter->name_ == file_name) {
            cur.files_.erase(iter);
            return 0;
        }
    }
    return -2;
}

/**
 * @brief FileManager::RemoveDirectory
 * @param directory_name
 * @return 0 成功
 * @return -1 目录名为空
 * @return -2 目录不存在
 */
int FileManager::RemoveDirectory(QString directory_name)
{
    inode_t pre = cwd_;
    if (directory_name.length() == 0)
    {
        qDebug() << "can not create empty directory";
        return -1;
    }
    if (directory_name.indexOf("/"))
    {
        int ind = directory_name.lastIndexOf("/");
        pre = Str2Path(directory_name.left(ind));
        directory_name = directory_name.right(directory_name.size()-ind);
        if (pre < 0 || fm_inodes_[pre].attribute_ == 1) {
            return -2;
        }
    }
    FCB& cur = fm_fcb_[fm_inodes_[pre].fcb_];
    for (auto iter = cur.subdirs_.begin(); iter != cur.subdirs_.end(); iter++) {
        if (iter->name_ == directory_name) {
            cur.subdirs_.erase(iter);
            return 0;
        }
    }
    return -2;
}

/**
 * @brief FileManager::ReadFile
 * @param file_name 文件名
 * @param content 内容
 * @return 0 成功
 * @return -1 文件未能成功读入
 */
int FileManager::ReadFile(QString file_name, QByteArray &content)
{
    inode_t pre = cwd_;
    if (file_name.length() == 0)
    {
        qDebug() << "can not create empty directory";
        return -1;
    }
    if (file_name.indexOf("/"))
    {
        int ind = file_name.lastIndexOf("/");
        pre = Str2Path(file_name.left(ind));
        file_name = file_name.right(file_name.size()-ind);
        if (pre < 0 || fm_inodes_[pre].attribute_ == 1) {
            return -2;
        }
    }
    FCB fcb = fm_fcb_[fm_inodes_[pre].fcb_];
    inode_t ino = -1;
    for (auto file = fcb.files_.begin(); file != fcb.files_.end(); file++) {
        if (file->name_ == file_name) {
            ino = file->inode_;
            break;
        }
    }
    if (ino == -1) {
        return -1;
    }
    FCB cur = fm_fcb_[fm_inodes_[pre].fcb_];
    content = cur.data_;
    return 0;
}

QString FileManager::GetCWD()
{
    QString file_name;
    inode_t  cur = cwd_;
    while (cur != -1) {
        file_name.push_front("/");
        Inode ino = fm_inodes_[cur];
        file_name.push_front(ino.fcb_name_);
        cur = fm_fcb_[ino.fcb_].pre.inode_;
    }
    return file_name;
}
