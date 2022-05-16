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
}
#endif // COMMON_H
