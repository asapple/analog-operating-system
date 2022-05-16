#ifndef COMMON_H
#define COMMON_H
/**
 * @file common.h
 * @brief 定义一些公用的数据结构或者常量
 */
namespace os {
using pid_t = int; // Process ID
using dev_t = int; // Device ID
using inode_t = int; // INode
using priority_t = int; // Priority
using tick_t = int;
inline constexpr int MEMORY_CODE_INIT =  2;
inline constexpr int MEMORY_CODE_MAX = 3;
inline constexpr int MEMORY_DATA_MAX  = 3;
inline constexpr int MEMORY_VIRTUAL_SIZE = 128;
inline constexpr int MEMORY_TOTAL_SIZE = 512;
inline constexpr int MEMORY_PAGE_SIZE = 8;

}
#endif // COMMON_H
