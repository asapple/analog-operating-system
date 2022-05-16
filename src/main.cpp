#include "include/UI/mainwindow.h"
#include "include/Common/Common.h"
#include "include/ProcessManager/CPU.h"
#include <QApplication>
#include <thread>
#include <chrono>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    os::CPU* cpu = new os::CPU(100);
    cpu->start();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    delete cpu;
    return a.exec();
}
