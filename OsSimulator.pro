QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/DeviceManager/DiskManager.cpp \
    src/DeviceManager/DeviceManager.cpp \
    src/ProcessManager/CPU.cpp \
    src/ProcessManager/Interupt.cpp \
    src/ProcessManager/ProcessManager.cpp \
    src/MemoryManager/MemoryManager.cpp \
    src/FileManager/FileManager.cpp \
    #src/Test/MemoryTest.cpp \
    #src/Test/MemoryTest.cpp \
    #src/Test/ProcessTest.cpp \
    #src/Test/ProcessMemoryTest.cpp \
    src/Test/IntegretedTest.cpp \
#    src/main.cpp

HEADERS += \
    include/Common/Common.h \
    include/DeviceManager/DeviceManager.h \
    include/DeviceManager/DiskManager.h \
    include/FileManager/FileManager.h \
    include/MemoryManager/MemoryManager.h \
    include/ProcessManager/CPU.h \
    include/ProcessManager/Instruction.h \
    include/ProcessManager/Interupt.h \
    include/ProcessManager/Process.h \
    include/ProcessManager/ProcessManager.h \

# UI模块引入
SOURCES += $$files(UI/*.cpp, true)
HEADERS += $$files(UI/*.h, true)
RESOURCES += $$files(UI/*.qrc, true)
FORMS += UI/form/frmmain.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


# 在.pro 中增加如下配置 ==》 Qt默认是不开启异常的
CONFIG += exception
