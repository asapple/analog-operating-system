#ifndef COMMON_H
#define COMMON_H
/**
 * @file common.h
 * @brief 定义一些公用的数据结构或者常量
 */
#include <QString>
namespace os {
    using pid_t = int;
    using dev_t = int;
    using inode_t = int;
    /**
     * @brief 指令类型
     */
    enum class InsType {
        COMPUTE = 'C',
        DEVICE = 'D',
        ACCESS = 'A',
        MEMORY = 'M',
        PRIORITY   = 'P',
        QUIT = 'Q'
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
        Instruction(QString line) {
            type = static_cast<InsType>(line.at(0).toLatin1());
            if (type != InsType::DEVICE) {
                op = line.section(' ',1).toInt();
            } else {
                op1 = line.section(' ',1).toInt();
                op2 = line.section(' ',2).toInt();
            }
        }
        Instruction(char* buf) {
            memmove(this, buf, sizeof(Instruction));
        }
        operator QByteArray() {
            return QByteArray(reinterpret_cast<char*>(this),sizeof(Instruction));
        }

    };
}
#endif // COMMON_H
