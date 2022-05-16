#ifndef COMMON_H
#define COMMON_H
/**
 * @file common.h
 * @brief 定义一些公用的数据结构或者常量
 */
namespace os {
    using pid_t = int;
    using dev_t = int;
    using inode_t = int;
    /**
     * @brief 指令类型
     */
    enum class InsType {
        COMPUTE,
        DEVICE,
        ACCESS,
        MEMORY,
        PRIORITY,
        QUIT
    };
    /**
     * @brief 指令类
     */
    struct Instruction {
        InsType type;
        union {
            int op;
            struct {
                short op1;
                short op2;
            };
        };
    };
}
#endif // COMMON_H
