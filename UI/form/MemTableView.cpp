#include "frmmain.h"
#include "ui_frmmain.h"
#include "UI/core_common/iconhelper.h"
#include "UI/core_common/quihelper.h"
#include "include/MemoryManager/MemoryManager.h"
#include "include/ProcessManager/Instruction.h"
#include <QVector>

using namespace os;

void frmMain::updateMemTableView()
{
    static QVector<pid_t> bitmap;
    static QByteArray buffer;
    static QString content;
    static Instruction ins;
    static QTableWidgetItem* item;
    static pid_t pid;
    static int row, column;
    bitmap = MemoryManager::Instance().GetBitmap();
    for (int i = 0; i < bitmap.length(); i++) {
        row = i /8;
        column = i % 8;
        pid = bitmap[i];
        buffer = MemoryManager::Instance().GetMemContent(i);
        if (qstrlen(buffer) != buffer.size()) { // 该页内存放的是指令
            ins = buffer.data();
            content = ins.operator QString();
        } else {                                // 该页存放的是数据
            content = buffer;
        }
        item = ui->men_tableWidget->item(row, column);
        item->setText(QString("pid:%1  %2").arg(pid).arg(content));
        if (pid == 0) item->setBackground(Qt::gray);
        else item->setBackground(Qt::white);
    }
}
