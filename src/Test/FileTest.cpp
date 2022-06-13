#include "./include/FileManager/FileManager.h"
#include "./include/DeviceManager/DeviceManager.h"
#include "include/Common/Common.h"
#include <QApplication>
#include <QDebug>


using namespace os;

void Init()
{
    os::FileManager::Instance("./files");
}

int main(int argc, char *argv[])
{
    Init();
    qDebug() << "-----   InitFile Test   -----";
    qDebug() << os::FileManager::Instance("./files").GetCWD();

    os::FileManager::Instance("./files").MakeFile("NEVERGONNA", 1000);
    os::FileManager::Instance("./files").MakeFile("never", 10);
    QVector<QString> files, dirs;
    os::FileManager::Instance("./files").List(files, dirs);
    qDebug() << files << dirs;
}
