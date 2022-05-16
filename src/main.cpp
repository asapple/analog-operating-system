#include "include/UI/mainwindow.h"
#include "include/ProcessManager/CPU.h"
#include <QDebug>
#include "include/MemoryManager/MemoryManager.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    QByteArray text(10, 'a');
    qDebug() << text.data();
    QByteArray test(10, 'b');
    text = test;
    qDebug() << text.data();
    test[0] = 'c';
    qDebug() << text.data();
    qDebug() << test.data();
    return a.exec();
}
