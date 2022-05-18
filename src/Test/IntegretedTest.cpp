#include "UI/form/frmmain.h"
#include "include/Common/Common.h"
#include "include/ProcessManager/CPU.h"
#include "include/ProcessManager/ProcessManager.h"
#include "include/ProcessManager/Interupt.h"
#include <QApplication>
#include <QDebug>

void Init()
{
    os::ProcessManager::Instance(100,true);
    os::InteruptManager::Instance().SetInterupt(true);
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
    os::CPU cpu(500);
    cpu.start();
    QApplication a(argc, argv);
    frmMain w;
    w.show();
    return a.exec();
}
