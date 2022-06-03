#include "UI/Form/frmmain.h"
#include "include/Common/Common.h"
#include "include/MemoryManager/MemoryManager.h"
#include <QApplication>
#include <QDebug>

using namespace os;

void TestMemory();
void PrintFrame(frame_t frame);
void PrintBitmap();
void PrintProceMem(int pid);

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    frmMain w;
    w.show();
    TestMemory();
    return a.exec();
}

void TestMemory()
{
    // MemoryManager单元测试
    int pid = 1047;
    QByteArray code;

    // InitMemory
    qDebug() << "-----   InitMemory Test   -----";
    qDebug() << "Test PID:" <<pid;
    MemoryManager::Instance().InitMemory(pid, "just a name"); // 初始化进程内存
    // 打印内存情况
    PrintBitmap();
    // 打印进程所占内存的内容
    PrintProceMem(pid);
    qDebug() << "\n";

    // GetCode
    qDebug() << "-----   GetCode Test   -----";
    qDebug() << "Test PID:" <<pid;
    MemoryManager::Instance().GetCode(pid, 0, code); // 情况1，在内存中找到指令
    qDebug() << "[GetCode]  pid:" << pid << " " << "page:0" << " " << "content:" << code.data();
    MemoryManager::Instance().GetCode(pid, 16, code); // 情况2，增页并填充
    qDebug() << "[GetCode]  pid:" << pid << " " << "page:2" << " " << "content:" << code.data();
    MemoryManager::Instance().GetCode(pid, 24, code); // 情况3，换页并填充
    qDebug() << "[GetCode]  pid:" << pid << " " << "page:3" << " " << "content:" << code.data();
    PrintBitmap();
    PrintProceMem(pid);
    qDebug() << "\n";

    // AccessMemory
    qDebug() << "-----   AccessMemory Test   -----";
    qDebug() << "Test PID:" <<pid;
    // 情况1，空间充足时增页换页
    MemoryManager::Instance().AccessMemory(pid, 108); // 增页
    MemoryManager::Instance().AccessMemory(pid, 72); // 增页
    MemoryManager::Instance().AccessMemory(pid, 256); // 增页
    MemoryManager::Instance().AccessMemory(pid, 72); // 找到页
    MemoryManager::Instance().AccessMemory(pid, 321); // 换页
    qDebug() << "\n";

    // MoreMemory
    qDebug() << "-----   MoreMemory Test   -----";
    pid = 1048;
    qDebug() << "Test PID:" << pid;
    MemoryManager::Instance().InitMemory(pid, "more");
    MemoryManager::Instance().MoreMemory(pid, 0); // 查看页上限
    PrintBitmap();
    MemoryManager::Instance().GetCode(pid, 24, code); // 增页
    MemoryManager::Instance().AccessMemory(pid, 111); // 增页，但是内存不足
    MemoryManager::Instance().MoreMemory(pid, 2); // 增添页上限
    qDebug() << "\n";

    // Add2Change
    qDebug() << "-----   Add2Change Test   -----";
    pid = 1048;
    qDebug() << "ReleaseMemory:" << pid;
    MemoryManager::Instance().ReleaseMemory(pid); // 释放内存
    MemoryManager::Instance().InitMemory(pid, "more");
    MemoryManager::Instance().AccessMemory(pid, 111); // 增页
    MemoryManager::Instance().MoreMemory(pid, 0); // 查看页上限
    PrintBitmap();
    MemoryManager::Instance().GetCode(pid, 24, code); // 增页转换页
    MemoryManager::Instance().AccessMemory(pid, 121); // 增页转换页
    qDebug() << "\n";

    // ForkMemory
    qDebug() << "-----   ForkMemory Test   -----";
    pid = 1049;
    qDebug() << "Test PID:" << pid;
    qDebug() << "ReleaseMemory:" << 1047;
    MemoryManager::Instance().ReleaseMemory(1047); // 释放内存
    PrintBitmap();
    MemoryManager::Instance().ForkMemory(pid, 1048);
    PrintBitmap();
    qDebug() << "pid:1048";
    PrintProceMem(1048);
    qDebug() << "pid:1049";
    PrintProceMem(1049);
}

void PrintFrame(frame_t frame)
{
    qDebug() << "frame:" << frame << "  "
             << "content:" << MemoryManager::Instance().GetMemContent(frame).data();
}

void PrintBitmap()
{
    QVector<frame_t> bitmap = MemoryManager::Instance().GetBitmap();
    QString str, arg;
    qDebug() << QString("内存情况：");
    for (int i = 0; i < bitmap.size(); ++i) {
        arg = QString("%1").arg(bitmap[i], 6, 10, QChar(' '));
        str = str + arg;
        if ((i+1) % 8 == 0) {
            qDebug() << str;
            str.clear();
        }
    }
}

void PrintProceMem(int pid)
{
    QVector<frame_t> occupy;
    qDebug() << "占用内存空间(byte)： " << MemoryManager::Instance().GetOccupying(pid,occupy);
    for (auto i = occupy.begin(); i != occupy.end(); i++) {
        PrintFrame(*i);
    }
}
