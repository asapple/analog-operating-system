#include "include/UI/mainwindow.h"
#include "include/Common/Common.h"
#include "include/MemoryManager/MemoryManager.h"
#include <QApplication>
#include <QDebug>

void TestMemory();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    TestMemory();
    return a.exec();
}

void TestMemory()
{
    using namespace os;
    MemoryManager::Instance().InitMemory(1047, "just a name");
    QVector<frame_t> occupy;
    qDebug() << "\n占用内存空间(byte)： " << MemoryManager::Instance().PrintOccupying(1047,occupy);
    qDebug() << "占用的物理帧：";
    for (auto i = occupy.begin(); i != occupy.end(); ++i) {
        qDebug() << "pid:" << 1047 << "   " << "frame:" << *i;
    }
    qDebug() << "";

    QByteArray code;
    MemoryManager::Instance().GetCode(1047, 0, code);
    qDebug() << "pid:" << 1047 << "   " << "page:0" << "   " << "content:" << code.data();
    MemoryManager::Instance().GetCode(1047, 8, code);
    qDebug() << "pid:" << 1047 << "   " << "page:1" << "   " << "content:" << code.data();
    MemoryManager::Instance().GetCode(1047, 16, code);
    qDebug() << "pid:" << 1047 << "   " << "page:2" << "   " << "content:" << code.data();
    MemoryManager::Instance().GetCode(1047, 24, code);
    qDebug() << "pid:" << 1047 << "   " << "page:3" << "   " << "content:" << code.data();
    MemoryManager::Instance().GetCode(1047, 0, code);
    qDebug() << "pid:" << 1047 << "   " << "page:0" << "   " << "content:" << code.data();

    qDebug() << "\n占用内存空间(byte)： " << MemoryManager::Instance().PrintOccupying(1047,occupy);
    qDebug() << "占用的物理帧：";
    for (auto i = occupy.begin(); i != occupy.end(); ++i) {
        qDebug() << "pid:" << 1047 << "   " << "frame:" << *i;
    }
    qDebug() << "";

    MemoryManager::Instance().AccessMemory(1047, 108);
    MemoryManager::Instance().AccessMemory(1047, 72);
    MemoryManager::Instance().AccessMemory(1047, 256);
    MemoryManager::Instance().AccessMemory(1047, 321);

    MemoryManager::Instance().MoreMemory(1047, 0);
    MemoryManager::Instance().MoreMemory(1047, 2);

    MemoryManager::Instance().AccessMemory(1047, 256);
    MemoryManager::Instance().AccessMemory(1047, 215);
    MemoryManager::Instance().AccessMemory(1047, 228);
    MemoryManager::Instance().AccessMemory(1047, 321);
    MemoryManager::Instance().AccessMemory(1047, 315);

    qDebug() << "\n占用内存空间(byte)： " << MemoryManager::Instance().PrintOccupying(1047,occupy);
    qDebug() << "占用的物理帧：";
    for (auto i = occupy.begin(); i != occupy.end(); ++i) {
        qDebug() << "pid:" << 1047 << "   " << "frame:" << *i;
    }
    qDebug() << "";

    MemoryManager::Instance().ForkMemory(1048, 1047);
    qDebug() << "\n占用内存空间(byte)： " << MemoryManager::Instance().PrintOccupying(1048,occupy);
    qDebug() << "占用的物理帧：";
    for (auto i = occupy.begin(); i != occupy.end(); ++i) {
        qDebug() << "pid:" << 1048 << "   " << "frame:" << *i;
    }
    qDebug() << "";
    MemoryManager::Instance().GetCode(1048, 0, code);
    qDebug() << "pid:" << 1048 << "   " << "page:0" << "   " << "content:" << code.data();
    MemoryManager::Instance().GetCode(1048, 8, code);
    qDebug() << "pid:" << 1048 << "   " << "page:1" << "   " << "content:" << code.data();
    MemoryManager::Instance().GetCode(1048, 16, code);
    qDebug() << "pid:" << 1048 << "   " << "page:2" << "   " << "content:" << code.data();
    MemoryManager::Instance().GetCode(1048, 24, code);
    qDebug() << "pid:" << 1048 << "   " << "page:3" << "   " << "content:" << code.data();
    MemoryManager::Instance().GetCode(1048, 0, code);
    qDebug() << "pid:" << 1048 << "   " << "page:0" << "   " << "content:" << code.data();
}
