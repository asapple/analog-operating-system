#include "include/UI/mainwindow.h"
#include "include/Common/Common.h"
#include "include/ProcessManager/ProcessManager.h"
#include "include/MemoryManager/MemoryManager.h"
#include "include/ProcessManager/Instruction.h"
#include "include/ProcessManager/CPU.h"
#include <QApplication>
#include <QDebug>

using namespace os;
void TestExec();
void TestRRExec();
void TestFork();
int MemoryManager::ReadFile(const QString& file_name, QByteArray& content)
{
    content.append(Instruction(InsType::COMPUTE, 2));
    content.append(Instruction(InsType::FORK, 2));
    content.append(Instruction(InsType::ACCESS, 33));
    content.append(Instruction(InsType::QUIT));
    return 0;
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
//    TestExec();
//    TestRRExec();
    TestFork();
    return a.exec();
}

void TestExec()
{
    ProcessManager& pm = ProcessManager::Instance(500);
    CPU cpu(10);
    cpu.start();
    pm.Execute("t1");
    pm.Execute("t2");
    pm.Execute("t3");
    pm.Execute("t4");
    pm.Execute("t5");
    pm.Execute("t6");
    std::this_thread::sleep_for(std::chrono::seconds(1000));
}

void TestRRExec()
{
    ProcessManager& pm = ProcessManager::Instance(500,true);
    CPU cpu(500);
    cpu.start();
    pm.Execute("t1");
    pm.Execute("t2");
    pm.Execute("t3");
    pm.Execute("t4");
    pm.Execute("t5");
    pm.Execute("t6");
    std::this_thread::sleep_for(std::chrono::seconds(1000));
}

void TestFork()
{
    ProcessManager& pm = ProcessManager::Instance(500,true);
    CPU cpu(10);
    cpu.start();
    pm.Execute("t1");
    std::this_thread::sleep_for(std::chrono::seconds(1000));
}
