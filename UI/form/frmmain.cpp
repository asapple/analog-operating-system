#include "frmmain.h"
#include "ui_frmmain.h"
#include "UI/core_common/iconhelper.h"
#include "UI/core_common/quihelper.h"

#include "include/FileManager/FileManager.h"
#include "include/ProcessManager/ProcessManager.h"

frmMain::frmMain(QWidget *parent) : QDialog(parent), ui(new Ui::frmMain)
{
    ui->setupUi(this);
    this->initForm();

}

frmMain::~frmMain()
{
    delete ui;
}

bool frmMain::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::MouseButtonDblClick) {
        if (watched == ui->widgetTitle) {
            on_btnMenu_Max_clicked();
            return true;
        }
    }

    return QWidget::eventFilter(watched, event);
}
void frmMain::initForm()
{
    //设置无边框
    QUIHelper::setFramelessForm(this);
    //设置图标
    IconHelper::setIcon(ui->labIco, 0xf099, 35);
    IconHelper::setIcon(ui->btnMenu_Min, 0xf068);
    IconHelper::setIcon(ui->btnMenu_Max, 0xf067);
    IconHelper::setIcon(ui->btnMenu_Close, 0xf00d);
    //ui->widgetMenu->setVisible(false);
    ui->widgetTitle->installEventFilter(this);
    ui->widgetTitle->setProperty("form", "title");
    ui->widgetTop->setProperty("nav", "top");

    QFont font;
    font.setPixelSize(25);
    ui->labTitle->setFont(font);
    ui->labTitle->setText("模拟操作系统");
    this->setWindowTitle(ui->labTitle->text());

    ui->stackedWidget->setStyleSheet("QLabel{font:15px;}");

    //单独设置指示器大小
    int addWidth = 20;
    int addHeight = 10;
    int rbtnWidth = 15;
    int ckWidth = 13;
    int scrWidth = 12;
    int borderWidth = 3;

    QStringList qss;
    qss << QString("QComboBox::drop-down,QDateEdit::drop-down,QTimeEdit::drop-down,QDateTimeEdit::drop-down{width:%1px;}").arg(addWidth);
    qss << QString("QComboBox::down-arrow,QDateEdit[calendarPopup=\"true\"]::down-arrow,QTimeEdit[calendarPopup=\"true\"]::down-arrow,"
                   "QDateTimeEdit[calendarPopup=\"true\"]::down-arrow{width:%1px;height:%1px;right:2px;}").arg(addHeight);
    qss << QString("QRadioButton::indicator{width:%1px;height:%1px;}").arg(rbtnWidth);
    qss << QString("QCheckBox::indicator,QGroupBox::indicator,QTreeWidget::indicator,QListWidget::indicator{width:%1px;height:%1px;}").arg(ckWidth);
    qss << QString("QScrollBar:horizontal{min-height:%1px;border-radius:%2px;}QScrollBar::handle:horizontal{border-radius:%2px;}"
                   "QScrollBar:vertical{min-width:%1px;border-radius:%2px;}QScrollBar::handle:vertical{border-radius:%2px;}").arg(scrWidth).arg(scrWidth / 2);
    qss << QString("QWidget#widget_top>QToolButton:pressed,QWidget#widget_top>QToolButton:hover,"
                   "QWidget#widget_top>QToolButton:checked,QWidget#widget_top>QLabel:hover{"
                   "border-width:0px 0px %1px 0px;}").arg(borderWidth);
    qss << QString("QWidget#widgetleft>QPushButton:checked,QWidget#widgetleft>QToolButton:checked,"
                   "QWidget#widgetleft>QPushButton:pressed,QWidget#widgetleft>QToolButton:pressed{"
                   "border-width:0px 0px 0px %1px;}").arg(borderWidth);
    this->setStyleSheet(qss.join(""));

    QSize icoSize(32, 32);
    int icoWidth = 85;

    //设置顶部导航按钮
    QList<QToolButton *> tbtns = ui->widgetTop->findChildren<QToolButton *>();
    foreach (QToolButton *btn, tbtns) {
        btn->setIconSize(icoSize);
        btn->setMinimumWidth(icoWidth);
        btn->setCheckable(true);
        connect(btn, SIGNAL(clicked()), this, SLOT(buttonClick()));
    }
    // 设置Desk的表头
    QTableWidgetItem *headerItem;
    QStringList headerText;
    headerText<<"磁 块 号"<<"磁 块 大 小"<<"已 使 用"<<"空 闲 空 间";  //表头标题用QStringList来表示
    //ui->tableWidget->setHorizontalHeaderLabels(headerText);
    ui->tableWidget->setColumnCount(headerText.count());//列数设置为与 headerText的行数相等
    for (int i=0;i<ui->tableWidget->columnCount();i++)//列编号从0开始
    {
        //cellItem=ui->tableInfo->horizontalHeaderItem(i);
        headerItem=new QTableWidgetItem(headerText.at(i)); //新建一个QTableWidgetItem， headerText.at(i)获取headerText的i行字符串
        QFont font=headerItem->font();//获取原有字体设置
        font.setBold(true);//设置为粗体
        font.setPointSize(12);//字体大小
        headerItem->setForeground(Qt::blue);//字体颜色

        headerItem->setFont(font);//设置字体
        ui->tableWidget->setHorizontalHeaderItem(i,headerItem); //设置表头单元格的Item
    }
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setDeskTable();

    QStringList sl;
    sl<<"地址"<<"内容";
    ui->men_tableWidget->setColumnCount(8);
    //    for (int i=0;i<ui->men_tableWidget->columnCount();i++)//列编号从0开始
    //    {
    //        //cellItem=ui->tableInfo->horizontalHeaderItem(i);
    //        headerItem=new QTableWidgetItem(sl.at(i)); //新建一个QTableWidgetItem， headerText.at(i)获取headerText的i行字符串
    //        QFont font=headerItem->font();//获取原有字体设置
    //        font.setBold(true);//设置为粗体
    //        font.setPointSize(12);//字体大小
    //        headerItem->setForeground(Qt::blue);//字体颜色

    //        headerItem->setFont(font);//设置字体
    //        ui->men_tableWidget->setHorizontalHeaderItem(i,headerItem); //设置表头单元格的Item
    //    }
    ui->men_tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    setMemTable();
    ui->btnMain->click();
}
void frmMain::setMemTable(){
    for(int i=0;i<10;i++){
        ui->men_tableWidget->insertRow(i);
        for(int j=0;j<8;j++){
            QTableWidgetItem    *item;
            item=new  QTableWidgetItem(QString("%1:%2").arg((i*8+j)*8).arg(0));
            item->setBackground(Qt::green);// 用不同的颜色表示该内存是否被分配
            ui->men_tableWidget->setItem(i,j,item);
        }
    }
}
void frmMain::mousePressEvent(QMouseEvent *event) {
    mouse_is_press = true;
    mouse_move_len = event->globalPos() - this->pos();
}
void frmMain::mouseMoveEvent(QMouseEvent *event)  {
    //(event->buttons() && Qt::LeftButton)  //按下鼠标并且按下的是左键
    //获取鼠标移动中，移动后窗口在整个屏幕的坐标，将移动之前窗口的位置 move到 这个坐标（移动后窗口的位置）
    //通过事件event->globalPos()知道鼠标坐标，鼠标坐标减去鼠标相对于窗口位置，就是窗口在整个屏幕的坐标
    if (  mouse_is_press && (event->buttons() && Qt::LeftButton) &&
          (event->globalPos()-mouse_move_len).manhattanLength() > QApplication::startDragDistance()) //这句话其实就是：鼠标移动的距离大于最小可识别的距离
    {
        move(event->globalPos() - mouse_move_len);
        mouse_move_len = event->globalPos() - pos();
    }
}
void frmMain::mouseReleaseEvent(QMouseEvent *event) {
    mouse_is_press = false;
}


// 初始化DeskTable
void frmMain::setDeskTable(){
    //使用createItemsARow()
    ui->tableWidget->insertRow(0);//添加第0行
    createItemsARow(0,0,64,64,0);//往第0行里写入数据
    // 写个for循环一行一行的加入
}
void frmMain::createItemsARow(int rowNo,int addr,int size,int used,int free)
{

    QTableWidgetItem    *item;
    item=new  QTableWidgetItem(QString::number(addr));
    ui->tableWidget->setItem(rowNo,0,item);

    item=new  QTableWidgetItem(QString::number(size));
    ui->tableWidget->setItem(rowNo,1,item);

    item=new  QTableWidgetItem(QString::number(used));
    ui->tableWidget->setItem(rowNo,2,item);

    item=new  QTableWidgetItem(QString::number(free));
    if(free == 0){// 使用不同颜色的文字，表示空间是否空余
        item->setForeground(Qt::red);
    }else {
        item->setForeground(Qt::green);
    }
    ui->tableWidget->setItem(rowNo,3,item);
}
void frmMain::buttonClick()
{
    QToolButton *b = (QToolButton *)sender();
    QString name = b->text();

    QList<QToolButton *> tbtns = ui->widgetTop->findChildren<QToolButton *>();
    foreach (QToolButton *btn, tbtns) {
        btn->setChecked(btn == b);
    }

    if (name == "Shell") {
        ui->stackedWidget->setCurrentIndex(0);
    } else if (name == "Memory") {
        ui->stackedWidget->setCurrentIndex(1);
    } else if (name == "MemTable") {
        ui->stackedWidget->setCurrentIndex(2);
    } else if (name == "Process") {
        ui->stackedWidget->setCurrentIndex(3);
    }
    else if (name == "DeskTable") {
        ui->stackedWidget->setCurrentIndex(4);
    }else if (name == "Desk") {
        ui->stackedWidget->setCurrentIndex(5);
    } else if (name == "Device") {
        ui->stackedWidget->setCurrentIndex(6);
    }
}

void frmMain::on_btnMenu_Min_clicked()
{
    showMinimized();
}

void frmMain::on_btnMenu_Max_clicked()
{
    static bool max = false;
    static QRect location = this->geometry();

    if (max) {
        this->setGeometry(location);
    } else {
        location = this->geometry();
        this->setGeometry(QUIHelper::getScreenRect());
    }

    this->setProperty("canMove", max);
    max = !max;
}

void frmMain::on_btnMenu_Close_clicked()
{
    close();
}

void frmMain::on_cmd_lineEdit_returnPressed()
{
    using namespace os;
    using namespace Ui;

    QString cmds = ui->cmd_lineEdit->text();
    ui->cmd_lineEdit->clear();// 清空命令行
    ui->echo_textBrowser->append("$ "+cmds);
    QStringList cmd = cmds.split(" ");

    if(cmd.empty())
        return;
    else if (cmd[0]=="ls") {
        // 调用文件系统的FileManager::List()方法
        bool isDetail = (cmd.indexOf("-a") != -1);
        QString path;
        for (int i = 1; i < cmd.size(); i++) {
            if (!cmd[i].startsWith('-')) {
                path = cmd[i];
                break;
            }
        }
        QVector<QString> files, dirs;
        int ret;
        if (path == "") {
            ret = FileManager::Instance().List(files, dirs);
        } else {
            ret = FileManager::Instance().List(files, dirs,path);
        }
        if (ret < 0) {
            ui->echo_textBrowser->append("path error");
        }
        QString str;
        for (auto file = files.begin(); file != files.end(); file++) {
            str += TEXT_COLOR_GREEN(*file) + "  ";
        }
        for (auto dir = dirs.begin(); dir != dirs.end(); dir++) {
            if (dir->indexOf('.') != -1 && !isDetail) {
                continue;
            }
            str += TEXT_COLOR_BLUE(*dir+"/") + "  ";
        }
        ui->echo_textBrowser->append(str);
    }else if (cmd[0]=="cd") {
        // 调用文件系统的FileManager::ChangeDirectory()方法
        if (os::FileManager::Instance().ChangeDirectory(cmd[1]) < 0) {
            ui->echo_textBrowser->append("change directory error");
        }
        ui->pwd_label->setText("CWD: "+os::FileManager::Instance().GetCWD());
    }else if (cmd[0]=="clear") {
        ui->echo_textBrowser->clear();
    }else if (cmd[0]=="rm") {
        // 调用文件系统的FileManager::RemoveFile()方法
        QString str="";
        if (os::FileManager::Instance().RemoveFile(cmd[1]) < 0) {
            str.append("remove file error");
        }
        ui->echo_textBrowser->append(str);
    }else if (cmd[0]=="rmdir") {
        // 调用文件系统的FileManager::RemoveMyDirectory()方法
        QString str="remove directory suscess";
        if (os::FileManager::Instance().RemoveMyDirectory(cmd[1]) < 0) {
            str = ("remove directory error");
        }
        ui->echo_textBrowser->append(str);
    }else if (cmd[0]=="mkdir") {
        // 调用文件系统的FileManager::MakeDirectory()方法
        QString str="create directory success";
        if (os::FileManager::Instance().MakeDirectory(cmd[1]) < 0) {
            str = "create directory error.";
        }
        ui->echo_textBrowser->append(str);
    }else if (cmd[0]=="mkfile") {
        // 调用文件系统的FileManager::MakeFile()方法
        QString str="create file success";
        if (os::FileManager::Instance().MakeFile(cmd[1]) < 0) {
            str = "create file error";
        }
        ui->echo_textBrowser->append(str);
    }else if (cmd[0]=="exec") {
        // 调用文件系统的FileManager::Execute()方法
        if (os::ProcessManager::Instance().Execute(cmd[1]) < 0) {
            ui->echo_textBrowser->append("create error!");
        }
    }else if (cmd[0]=="kill") {
        // 调用文件系统的FileManager::Kill()方法
        if (os::ProcessManager::Instance().Kill(cmd[1].toInt()) < 0) {
            ui->echo_textBrowser->append("kill error: process already dead.");
        }
    }else if (cmd[0]=="ps") {
        // 调用文件系统的FileManager::ProcessState()方法
        QString str;
        QString format = "%1\t%2\t%3\t%4\t%5\t%6\t%7\n";
        str.append(QString(format).arg("PID","PPID", "S","SIZE","PRI", "TIME","CMD"));
        auto pid = ProcessManager::Instance().GetRunProcess();
        if (pid != 0) {
            PCB pcb = ProcessManager::Instance().GetPCB(pid);
            str.append(QString(format).arg(QString::number(pcb.pid_), QString::number(pcb.ppid_), "Run", QString::number((int)pcb.size_), QString::number(pcb.priority_), QString::number(pcb.run_time_), pcb.file_name_));
        }
        for (auto pid:ProcessManager::Instance().GetReadyQueue()) {
            PCB pcb = ProcessManager::Instance().GetPCB(pid);
            str.append(QString(format).arg(QString::number(pcb.pid_), QString::number(pcb.ppid_), "Ready", QString::number((int)pcb.size_), QString::number(pcb.priority_), QString::number(pcb.run_time_), pcb.file_name_));
        }
        for (auto pid:ProcessManager::Instance().GetWaitQueue()) {
            PCB pcb = ProcessManager::Instance().GetPCB(pid);
            str.append(QString(format).arg(QString::number(pcb.pid_), QString::number(pcb.ppid_), "Wait", QString::number((int)pcb.size_), QString::number(pcb.priority_), QString::number(pcb.run_time_), pcb.file_name_));
        }
        ui->echo_textBrowser->append(str);
    } else if (cmd[0] == "quit") {
        this->on_btnMenu_Close_clicked();
    } else {
        ui->echo_textBrowser->append(QString("command not definded;"));
    }
    return;
}
