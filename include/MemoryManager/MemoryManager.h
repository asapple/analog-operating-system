#ifndef MEMORYMANAGER_H
#define MEMORYMANAGER_H
#define MEMORY_CODE_INIT 2
#define MEMORY_CODE_MAX 3
#define MEMORY_DATA_MAX 3
#define MEMORY_VIRTUAL_SIZE 128
#define MEMORY_TOTAL_SIZE 512
#define MEMORY_PAGE_SIZE 8
#include <QString>
#include <QHash>
#include <QVector>
#include <QByteArray>
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
        void SetAddr(frame_t address) { frame_addr = address; } // 设置物理帧号
        frame_t GetAddr() { return frame_addr; } // 获取物理帧号
    };

    class PageTable {
    private:
        pid_t pid; // 页表所属进程
        int occupy; // 已缓存页的数量
        int code_now; // 已缓存的代码页数量
        int code_max; // 代码段最多能缓存的代码页数量
        int data_now; // 已缓存的数据页数量
        int data_max; // 数据段最多能缓存的代码页数量
        QHash<page_t, PtItem> table; // 页表内容，键为虚拟页号
        QVector<page_t> code_queue; // 代码段牺牲队列
        QVector<page_t> data_queue; // 数据段牺牲队列
    public:
        PageTable(QVector<frame_t> initpages, pid_t id);
        ~PageTable() {}
        pid_t GetPid() { return pid; } // 获取页表所属进程
        int GetOccupy() { return occupy; } // 获取页表中已缓存页的数量
        void SetMax(size_t size, int typ) { if (typ == 0) code_max = size; else data_max = size; } // 修改代码段或数据段的内存页上限
        int GetCodeNum() { return code_now; } // 获取代码段的有效页数
        int GetCodeMax() { return code_max; } // 获取代码段的页数上限
        int GetDataNum() { return data_now; } // 获取数据段的有效页数
        int GetDataMax() { return data_max; } // 获取数据段的页数上限
        void AddPage(page_t vaddress, frame_t paddress); // 增加一个有效物理帧
        frame_t ReplaceCode(page_t new_vaddress); // 替换一个代码页，返回牺牲页的物理帧号
        frame_t ReplaceData(page_t new_vaddress); // 替换一个数据页，返回牺牲页的物理帧号
        int VisitCode(page_t vaddress, frame_t& frame); // 访问代码页，取得该页物理帧号，返回值这次访问的结果
        int VisitData(page_t vaddress, frame_t& frame); // 访问数据页，取得该页物理帧号，返回值这次访问的结果
        QVector<frame_t> PrintOccupying(); // 返回该进程正在占用的内存块的物理帧号
    };

    class MemoryManager {
    private:
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
    public:
        MemoryManager();
        ~MemoryManager() {}
        int InitMemory(pid_t pid, const QString& file_name);
        int GetCode(pid_t pid, size_t virt_addr, QByteArray& content);
        int AccessMemory(pid_t pid, size_t virt_addr); //ps. content 默认缺省，无实际含义
        int MoreMemory(pid_t pid, size_t size);
        int ReleaseMemory(pid_t pid);
        int PrintOccupying(pid_t pid, QVector<frame_t> Occupying);
    };
}
#endif // MEMORYMANAGER_H
