#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H

#include <QString>
#include <QHash>
#include <QVector>
#include <QByteArray>
#include <math.h>
#include "include/Common/Common.h"
namespace os {
    using page_t = int; // 虚拟页号
    using frame_t = int; // 物理帧号
    using offset_t = int; // 页偏移量

    class PtItem {
    private:
        bool vi_bit; // 有效无效位
        frame_t frame_addr; // 物理帧号
    public:
        PtItem() { vi_bit = false; }
        ~PtItem() {}
        void SetVi(bool value) { vi_bit = value; } // 设置有效无效位
        bool GetVi() { return vi_bit; } // 获取有效无效位
        void SetFrame(frame_t address) { frame_addr = address; } // 设置物理帧号
        frame_t GetFrame() { return frame_addr; } // 获取物理帧号
    };

    class PageTable {
    private:
        const bool CODE;
        const bool DATA;
        pid_t pid; // 页表所属进程
        size_t occupy; // 已缓存页的数量
        size_t code_now; // 已缓存的代码页数量
        size_t code_max; // 代码段最多能缓存的代码页数量
        size_t data_now; // 已缓存的数据页数量
        size_t data_max; // 数据段最多能缓存的代码页数量
        QHash<page_t, PtItem> table; // 页表内容，键为虚拟页号
        QVector<page_t> code_queue; // 代码段牺牲队列
        QVector<page_t> data_queue; // 数据段牺牲队列
    public:
        PageTable();
        PageTable(QVector<frame_t> initpages, pid_t id);
        PageTable& operator=(const PageTable& p);
        ~PageTable() {}
        pid_t GetPid() { return pid; } // 获取页表所属进程
        const QHash<page_t, PtItem> GetTable() { return table; } // 返回只读的页表
        size_t GetOccupy() { return occupy; } // 获取页表中已缓存页的数量
        size_t GetNow(bool typ) { if (typ == CODE) return code_now; else return data_now; } // 获取代码段或数据段的有效页数
        size_t GetMax(bool typ) { if (typ == CODE) return code_max; else return data_max; } // 获取代码段或数据段的页数上限
        QVector<page_t> GetQueue(bool typ) { if (typ == CODE) return code_queue; else return data_queue; } // 获取代码段或数据段的牺牲队列
        inline void SetOccupy(size_t size) { occupy = size; } // 修改页表中已缓存页的数量
        inline void SetMax(size_t size, bool typ) { if (typ == CODE) code_max = size; else data_max = size; } // 修改代码段或数据段的内存页上限
        inline void SetNow(size_t size, bool typ) { if (typ == CODE) code_now = size; else data_now = size; } // 修改代码段或数据段的当前页数量
        inline void SetQueue(QVector<page_t> queue, bool typ) { if (typ == CODE) code_queue = queue; else data_queue = queue; } // 修改代码段或数据段的牺牲队列
        void AddPage(page_t vaddress, frame_t paddress); // 增加一个有效物理帧
        frame_t ReplaceCode(page_t new_vaddress); // 替换一个代码页，返回牺牲页的物理帧号
        frame_t ReplaceData(page_t new_vaddress); // 替换一个数据页，返回牺牲页的物理帧号
        int VisitCode(page_t vaddress, frame_t& frame); // 访问代码页，取得该页物理帧号，返回值这次访问的结果
        int VisitData(page_t vaddress, frame_t& frame); // 访问数据页，取得该页物理帧号，返回值这次访问的结果
        QVector<frame_t> PrintOccupying(); // 返回该进程正在占用的内存块的物理帧号
    };

    class MemoryManager {
    private:
        const bool CODE;
        const bool DATA;
        QByteArray memory;
        QVector<pid_t> bitmap;
        QHash<pid_t, PageTable> pt_meta;
        QString exec_name;
        page_t Virt2Page(size_t virt_addr); // 将虚拟地址的页号取出
        offset_t Virt2Offset(size_t virt_addr); // 将虚拟地址的页偏移取出
        bool GetFrame(size_t size, QVector<frame_t>& frames); // 寻找指定数量的空闲帧，返回值表示是否成功
        void CopyBytes(QByteArray& dest, int dstart, QByteArray& sour, int sstart, size_t size); // 从源数组复制8个字节到目的数组，没有边界检查
        int ReadBytes(QString file_name, page_t page, offset_t offset, size_t size, QByteArray& content); // 从文件指定位置读取指定长度的字节串
        int ReadFile(const QString& file_name, QByteArray& content); // 测试桩
        void PrintInfo(QString str); // 打印信息
    public:
        static MemoryManager& Instance();
        MemoryManager();
        /*
         * @brief 初始化进程的内存空间
         * @param pid 进程的pid号
         * @param file_name 进程的可执行文件名
         * @return 正确执行时，返回初始分配给进程的空间大小，单位为字节
         * @return 错误码-1：传入的pid已拥有页表
         * @return 错误码-2，内存不足
         * @return 错误码-4，可执行文件长度不是指令的整数倍
         * @return 错误码-5，文件系统读取出错
         */
        int InitMemory(pid_t pid, const QString& file_name);
        /*
         * @brief 取出指定位置的一条指令
         * @param pid 进程的pid号
         * @param virt_addr 所需指令的虚拟地址
         * @param content 用于接收指令的字节数组
         * @return 正确执行时，返回0
         * @return 错误码-1：该进程没有页表
         * @return 错误码-3，给出的地址超出可执行文件范围
         * @return 错误码-4，从给出的地址到文件结束的长度不足一条指令的长度
         * @return 错误码-5，文件系统读取出错
         */
        int GetCode(pid_t pid, size_t virt_addr, QByteArray& content);
        /*
         * @brief 读取指定位置的一页数据
         * @param pid 进程的pid号
         * @param virt_addr 数据所在页的虚拟地址
         * @return 正确执行时，返回0
         * @return 错误码-1：该进程没有页表
         * @return 错误码-2，内存不足
         */
        int AccessMemory(pid_t pid, size_t virt_addr); //ps. content 默认缺省，无实际含义
        /*
         * @brief 使用一个进程的内存空间初始化另一个进程
         * @param pid 被初始话的进程的pid号
         * @param ppid 已存在的进程的pid号
         * @return 正确执行时，返回0
         * @return 错误码-1：ppid进程没有页表
         * @return 错误码-2，内存不足
         */
        int ForkMemory(pid_t pid, pid_t ppid); // 拷贝一个进程的内存作为另一进程的初始内存
        /*
         * @brief 提高一个进程所能使用的内存空间上限
         * @param pid 进程的pid号
         * @param size 进程想要增加的空间大小，单位为字节
         * @return 正确执行时，返回0
         * @return 错误码-1：该进程没有页表
         */
        int MoreMemory(pid_t pid, size_t size);
        /*
         * @brief 回收释放一个进程的全部内存空间
         * @param pid 进程的pid号
         * @return 正确执行时，返回0
         * @return 错误码-1：该进程没有页表
         */
        int ReleaseMemory(pid_t pid);
        /*
         * @brief 打印一个进程所占有的物理帧的真号
         * @param pid 进程的pid号
         * @return 正确执行时，返回0
         * @return 错误码-1：该进程没有页表
         */
        int PrintOccupying(pid_t pid, QVector<frame_t>& Occupying);
    };
}
#endif // MEMORYMANAGER_H
