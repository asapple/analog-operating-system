#include "include/UI/mainwindow.h"
#include "include/Common/Common.h"
#include "include/ProcessManager/CPU.h"
#include "include/ProcessManager/ProcessManager.h"
#include <QApplication>
#include <QDebug>
void TesFIFOScheduler();
void void TestPriorityScheduler();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    TestProcessManagerPriorityScheduler();
    return a.exec();
}


void TesFIFOScheduler()
{
    using namespace os;
    ProcessManager::Instance(10);
    ProcessManager::Instance().pushProcess(1);
    ProcessManager::Instance().pushProcess(4);
    ProcessManager::Instance().pushProcess(2);
    ProcessManager::Instance().pushProcess(3);
    qDebug() << ProcessManager::Instance().PollProcess(); // 1
    assert(-1 ==ProcessManager::Instance().pushProcess(4)); // 不能相等
    ProcessManager::Instance().pushProcess(1);
    qDebug() << ProcessManager::Instance().PollProcess(); // 4
    qDebug() << ProcessManager::Instance().PollProcess(); // 2
    ProcessManager::Instance().pushProcess(9);
    qDebug() << ProcessManager::Instance().PollProcess(); // 3
    ProcessManager::Instance().pushProcess(5);
    qDebug() << ProcessManager::Instance().PollProcess(); // 1
    ProcessManager::Instance().pushProcess(8);
    qDebug() << ProcessManager::Instance().PollProcess(); // 9
    qDebug() << ProcessManager::Instance().PollProcess(); // 5
    qDebug() << ProcessManager::Instance().PollProcess(); // 8
    ProcessManager::Instance().pushProcess(10);
    qDebug() << ProcessManager::Instance().PollProcess(); // 10
    ProcessManager::Instance().pushProcess(1);
    ProcessManager::Instance().pushProcess(2);
    ProcessManager::Instance().pushProcess(3);
    ProcessManager::Instance().pushProcess(4);
    QString str;
    for (const auto e: ProcessManager::Instance().GetReadyQueue()) {
        str += e;
        str += ',';
    }
    qDebug() << str; // 1,2,3,4
}

void TestPriorityScheduler()
{
    using namespace os;
    // phase1
    ProcessManager::Instance(10,false,SchedulerType::PRI);
    for (pid_t pid = 1; pid <= 4; pid++) {
        ProcessManager::Instance().GetPCB(pid).priority_ = pid;
    }
    ProcessManager::Instance().GetPCB(5).priority_ = 1;
    ProcessManager::Instance().pushProcess(1);
    assert(-1 == ProcessManager::Instance().pushProcess(1));
    ProcessManager::Instance().pushProcess(2);
    ProcessManager::Instance().pushProcess(3);
    ProcessManager::Instance().pushProcess(4);
     ProcessManager::Instance().pushProcess(5);
    qDebug() << ProcessManager::Instance().PollProcess(); // 4
    qDebug() << ProcessManager::Instance().PollProcess(); // 3
    QString str;
    for (const auto e: ProcessManager::Instance().GetReadyQueue()) {
        str += e;
        str += ',';
    }
    qDebug() << str; // 2,1,5
    // phase 2
    for (int i = 1; i <= 3; i++) {
        ProcessManager::Instance().GetPCB(5+i).priority_ = i;
        ProcessManager::Instance().pushProcess(5+i);
    }
    qDebug() << ProcessManager::Instance().PollProcess(); // 8
    ProcessManager::Instance().GetPCB(8).priority_ = 1;
    ProcessManager::Instance().pushProcess(8);
    str.clear();
    for (const auto e: ProcessManager::Instance().GetReadyQueue()) {
        qDebug() << e; //2,7,1,5,6,8
    }
}
