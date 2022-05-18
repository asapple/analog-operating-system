#ifndef COMMON_H
#define COMMON_H
/**
 * @file common.h
 * @brief 定义一些公用的数据结构或者常量
 */
namespace os {
using pid_t = int; // Process ID
using dev_t = int; // Device ID
using priority_t = int; // Priority
using tick_t = int;
inline constexpr int MEMORY_CODE_INIT =  2; // 进程初始化时分配的代码页数量
inline constexpr int MEMORY_CODE_MAX = 3; // 进程所能拥有的最大代码页数量
inline constexpr int MEMORY_DATA_MAX  = 3; // 进程所能拥有的最大数据页数量
inline constexpr int MEMORY_VIRTUAL_SIZE = 128; // 进程的虚拟地址空间大小，单位为页
inline constexpr int MEMORY_TOTAL_SIZE = 512; // 内存总大小，单位为页，必须是内存页大小的整数倍
inline constexpr int MEMORY_ADDR_SIZE = 16; // 地址操作数的位数
inline constexpr int MEMORY_PAGE_SIZE = 8; // 一个内存页的大小，单位为字节，必须是指令长度的整数倍
inline constexpr int MEMORY_INSTR_SIZE = 8; // 指令长度，单位为字节

// File const
using inode_t = int; // INode

// Device const
inline constexpr int DEVICE_SIZE = 3; // 总设备个数
inline constexpr int BLOCK_SIZE = 4096;    // 磁盘块大小，单位为字节
inline constexpr int DISk_SIZE = 64 * 1024 * 1024;  // 磁盘存储容量大小，单位为字节
                                                    // 共64MB，16 * 1024块
}
#endif // COMMON_H
