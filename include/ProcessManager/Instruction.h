#ifndef INSTRUCTION_H
#define INSTRUCTION_H
#include <QString>
#include <QByteArray>
#include <QStringList>
#include <QDebug>
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
    SEM_P = 'p',
    SEM_V  = 'v',
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
        QTextStream qts(&line);
        char str;
        qts >> str;
        type = static_cast<InsType>(str);
        if (type == InsType::DEVICE) {
           qts >> op1 >> op2;
        } else if (type != InsType::QUIT && type != InsType::FORK){
            qts >> op;
        }
    }
    inline Instruction(char* buf) {
        memmove(this, buf, sizeof(Instruction));
    }
    inline operator QByteArray() {
        return QByteArray(reinterpret_cast<char*>(this),sizeof(Instruction));
    }
    inline operator QString() {
        if (type == InsType::DEVICE) {
            return QString("%1 %2 %3").arg(QString((char)type), QString::number(op1), QString::number(op2));
        } else if (type == InsType::QUIT || type == InsType::FORK) {
            return QString((char)type);
        } else {
            return QString("%1 %2").arg(QString((char)type), QString::number(op));
        }
    }
};
}
#endif // INSTRUCTION_H
