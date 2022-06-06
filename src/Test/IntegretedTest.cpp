#include "UI/form/frmmain.h"
#include "include/Common/Common.h"
#include "include/ProcessManager/CPU.h"
#include "include/ProcessManager/ProcessManager.h"
#include "include/ProcessManager/Interupt.h"
#include "include/FileManager/FileManager.h"
#include <QApplication>
#include <QDebug>

void Init()
{
    os::ProcessManager::Instance(100,true);
    os::InteruptManager::Instance().SetInterupt(true);
    os::FileManager::Instance("files");
}

int main(int argc, char *argv[])
{
    //设置不应用操作系统设置比如字体
    QApplication::setDesktopSettingsAware(false);
#if (QT_VERSION >= QT_VERSION_CHECK(5,0,0))
    QApplication::setAttribute(Qt::AA_Use96Dpi);
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,14,0))
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::Floor);
#endif
    Init();
    QApplication a(argc, argv);
    frmMain w;
    os::CPU cpu(1500,&w);
    cpu.start();
    for (int i = 0; i < 5; i++) {
        os::FileManager::Instance().GetFullPath(i);
    }
    w.show();

    return a.exec();
}
