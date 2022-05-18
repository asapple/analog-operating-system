#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <QString>
#include <QByteArray>
namespace os {
/**
 * @brief 指令类型
 */
enum class InsType {
    COMPUTE = 'C',
    FORK = 'F',
    DEVICE = 'D',
    ACCESS = 'A',
    MEMORY = 'M',
    PRIORITY   = 'P',
    QUIT = 'Q',
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
    Instruction()
        :type(InsType::QUIT), op(0)
    {}
    Instruction(InsType type, int op = 0)
        :type(type), op(op)
    {}
    Instruction(InsType type, short op1, short op2)
        :type(type), op1(op1), op2(op2)
    {}
    Instruction(QString line) {
        type = static_cast<InsType>(line.at(0).toLatin1());
        if (type != InsType::DEVICE) {
            op = line.section(' ',1).toInt();
        } else {
            op1 = line.section(' ',1).toInt();
            op2 = line.section(' ',2).toInt();
        }
    }
    inline Instruction(char* buf) {
        memmove(this, buf, sizeof(Instruction));
    }
    inline operator QByteArray() {
        return QByteArray(reinterpret_cast<char*>(this),sizeof(Instruction));
    }
};
}
#endif // INSTRUCTION_H
