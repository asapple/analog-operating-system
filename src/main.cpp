#include "include/UI/mainwindow.h"
#include "include/Common/Common.h"
#include "include/ProcessManager/CPU.h"
#include "include/ProcessManager/ProcessManager.h"
#include <QApplication>
#include <QDebug>

void TestProcessManagerPriorityScheduler();

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
