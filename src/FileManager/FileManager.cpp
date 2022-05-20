#include "include/Common/Common.h"
#include "include/FileManager/FileManager.h"
#include "include/ProcessManager/Instruction.h"
#include "include/DeviceManager/DiskManager.h"
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
    fm_fcb_.push_back(fcb);
    inode_t inode = fm_inodes_.size();
    fm_inodes_.push_back(Inode(attribute, fcb_name, fcb_id, fcb.data_.size(), umask_));
    const int block_size = DiskManager::Instance().block_size_;
    fm_inodes_[inode].dnum_ = 1 + (fm_inodes_[inode].size_ + block_size - 1)/block_size ;
    fm_inodes_[inode].dno_ = DiskManager::Instance().RequestDisk(inode, fm_inodes_[inode].dnum_ );
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
        if (!dir.mkdir(dir.absolutePath())) {
            return -1;
        } else {
            return ForkCreate(real_path, pre);
        }
    }

    // 在模拟文件系统中创建对应的目录项
    inode_t inode = InsertFCB(0, dir.dirName());
    int fcb_no = fm_inodes_[inode].fcb_;
    fm_fcb_[fcb_no].subdirs_.push_back(DirectoryEntry(inode, "."));
    fm_fcb_[fcb_no].pre = DirectoryEntry(pre, "..");
    // 文件读入
    auto file_infos = dir.entryInfoList(QDir::Filter::Files);
    for (auto file_info = file_infos.begin(); file_info != file_infos.end(); file_info++) {
        FCB fcb;
        QString file_path = file_info->absoluteFilePath();
        QFile file(file_path); //对应真实操作系统中的文件
        if (file.open(QFile::ReadOnly | QIODevice::Text)) {
            while (!file.atEnd()) {
                QTextStream qts(&file);
                QByteArray str = file.readLine();
                Instruction ins(str); // 从字符数组或字符串中获取指令
                fcb.data_.append(static_cast<QByteArray>(ins)); //将指令保存到数组里
            }
            file.close();
        } else {
           qDebug() << "[FileSystem] Init Error: File '" << file_path <<"' can't read.";
        }
        inode_t ino = InsertFCB(1, file_info->fileName(),fcb);
        fm_fcb_[fcb_no].files_.push_back(DirectoryEntry(ino, file_info->fileName()));
    }
    // 递归目录读入
    auto dir_infos = dir.entryInfoList(QDir::Filter::Dirs);
    for (auto dir_info =  dir_infos.begin(); dir_info != dir_infos.end(); dir_info++) {
        if (dir_info->fileName().startsWith(".")) {
            continue;
        }
        inode_t subdir = ForkCreate(dir_info->absoluteFilePath(), inode);
        fm_fcb_[fcb_no].subdirs_.push_back(DirectoryEntry(subdir, dir_info->fileName()));
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
        QTextStream out(&qfile);
        if (qfile.open(QIODevice::WriteOnly)) {
            int sz = ffcb.data_.size();
            for (int i = 0; i < sz; i+=8) {
                Instruction ins(ffcb.data_.mid(i, MEMORY_INSTR_SIZE).data());
                QString str(ins);
                out << str;
            }
            qfile.close();
        } else {
            qDebug() << "[FileSystem] Save Error: File '" << file_path <<"' can't write.";
         }
    }
    int ret = 0;
    for (auto dir = fcb.subdirs_.begin(); dir != fcb.subdirs_.end();dir++) {
        if (dir->name_.contains(".")) {
            continue;
        }
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
        qDebug() << "[MakeDir] can not create empty directory";
        return -1;
    }
    if (directory_name.contains("/"))
    {
        int ind = directory_name.lastIndexOf("/");
        pre = Str2Path(directory_name.left(ind));
        directory_name = directory_name.right(directory_name.size()-ind-1);
        if (pre < 0 || fm_inodes_[pre].attribute_ == 1) {
            return -2;
        }
    }
    int fcb_no = fm_inodes_[pre].fcb_;
    for (auto dir:fm_fcb_[fcb_no].subdirs_) {
        if (dir.name_ == directory_name) {
            qDebug() << "[MakeDir] name has already existed.";
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
    fm_fcb_[fcb_no].subdirs_.push_back(DirectoryEntry(inode,directory_name));
    return 0;
}



/**
 * @brief list command output result
 * @param files 用于返回当前目录下文件类型的文件名
 * @param dirs 用于返回当前目录下目录类型的文件名
 * @return 0  成功返回
 * @return -1 路径错误，访问失败
 */
int FileManager::List(QVector<QString>& files, QVector<QString>& dirs, QString path)
{
    inode_t inode = Str2Path(path);
    if (inode < 0) {
        return -1;
    }
    FCB cur = fm_fcb_[fm_inodes_[inode].fcb_];
    for (auto file: cur.files_) {
        files.push_back(file.name_);
    }
    dirs.push_back(cur.pre.name_);
    for (auto dir: cur.subdirs_) {
        dirs.push_back(dir.name_);
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
        if (*it == fcb.pre.name_) {
            is_find = true;
            cur = fcb.pre.inode_;
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
    if (file_name.contains("/"))
    {
        int ind = file_name.lastIndexOf("/");
        pre = Str2Path(file_name.left(ind));
        file_name = file_name.right(file_name.size()-ind-1);
        if (pre < 0 || fm_inodes_[pre].attribute_ != 0) {
            return -2;
        }
    }

    int fcb_no = fm_inodes_[pre].fcb_;
    for (auto file = fm_fcb_[fcb_no].files_.begin();file != fm_fcb_[fcb_no].files_.end(); file++) {
        if (file->name_ == file_name) {
            qDebug() << "file name has already existed.";
            return -3;
        }
    }
    inode_t inode = InsertFCB(1, file_name);
    if (inode < 0) {
        return -4;
    }
    fm_fcb_[fcb_no].files_.push_back(DirectoryEntry(inode,file_name));
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
    if (file_name.contains("/"))
    {
        int ind = file_name.lastIndexOf("/");
        pre = Str2Path(file_name.left(ind));
        file_name = file_name.right(file_name.size()-ind);
        if (pre < 0 || fm_inodes_[pre].attribute_ != 0) {
            return -2;
        }
    }
    int fcb_no = fm_inodes_[pre].fcb_;
    for (auto iter = fm_fcb_[fcb_no].files_.begin(); iter != fm_fcb_[fcb_no].files_.end(); iter++) {
        if (iter->name_ == file_name) {
            DiskManager::Instance().ReleaseDisk(iter->inode_);
            fm_fcb_[fcb_no].files_.erase(iter);
            return 0;
        }
    }
    return -2;
}

/**
 * @brief FileManager::RemoveMyDirectory
 * @param directory_name
 * @return 0 成功
 * @return -1 目录名为空
 * @return -2 目录不存在
 */
int FileManager::RemoveMyDirectory(QString directory_name)
{
    inode_t pre = cwd_;
    if (directory_name.length() == 0)
    {
        qDebug() << "can not create empty directory";
        return -1;
    }
    if (directory_name.contains("/"))
    {
        int ind = directory_name.lastIndexOf("/");
        pre = Str2Path(directory_name.left(ind));
        directory_name = directory_name.right(directory_name.size()-ind);
        if (pre < 0 || fm_inodes_[pre].attribute_ != 0) {
            return -2;
        }
    }
    int fcb_no = fm_inodes_[pre].fcb_;

    for (auto iter = fm_fcb_[fcb_no].subdirs_.begin(); iter != fm_fcb_[fcb_no].subdirs_.end(); iter++) {
        if (iter->name_ == directory_name) {
            int dir_no = fm_inodes_[iter->inode_].fcb_;
            // 删除子文件
            for (auto it = fm_fcb_[dir_no].files_.begin(); it != fm_fcb_[dir_no].files_.end(); it++) {
                DiskManager::Instance().ReleaseDisk(it->inode_);
            }
            fm_fcb_[dir_no].files_.clear();
            // 递归删除子目录
            for (auto it = fm_fcb_[dir_no].subdirs_.begin(); it != fm_fcb_[dir_no].subdirs_.end(); it++) {
                if (it->name_.contains(".")) {
                    continue;
                }
                RemoveMyDirectory(directory_name+"/"+it->name_);
            }
            fm_fcb_[dir_no].subdirs_.clear();
            // 删除目录文件本身
            DiskManager::Instance().ReleaseDisk(iter->inode_);
            fm_fcb_[fcb_no].subdirs_.erase(iter);
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
    if (file_name.contains("/"))
    {
        int ind = file_name.lastIndexOf("/");
        pre = Str2Path(file_name.left(ind));
        file_name = file_name.right(file_name.size()-ind-1);
        if (pre < 0 || fm_inodes_[pre].attribute_ != 0) {
            return -2;
        }
    }
    int fcb_no = fm_inodes_[pre].fcb_;
    inode_t ino = -1;
    for (auto file = fm_fcb_[fcb_no].files_.begin(); file != fm_fcb_[fcb_no].files_.end(); file++) {
        if (file->name_ == file_name) {
            ino = file->inode_;
            break;
        }
    }
    if (ino == -1) {
        return -1;
    }
    FCB cur = fm_fcb_[fm_inodes_[ino].fcb_];
    content = cur.data_;
    return 0;
}
/**
 * @brief FileManager::GetFullPath
 * @param inode i节点号
 * @return i节点对应的完整路径
 */
QString FileManager::GetFullPath(inode_t inode)
{
    QString file_name;
    inode_t  cur = inode;
    while (cur != 0) {
        file_name.push_front("/");
        Inode ino = fm_inodes_[cur];
        file_name.push_front(ino.fcb_name_);
        cur = fm_fcb_[ino.fcb_].pre.inode_;
    }
    file_name.push_front("/");
    return file_name;
}
/**
 * @brief FileManager::GetCWD
 * @return 当前目录路径
 */
QString FileManager::GetCWD()
{
    return GetFullPath(cwd_);
}
