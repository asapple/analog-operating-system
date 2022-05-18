#include "frmmain.h"
#include "ui_frmmain.h"
#include "UI/core_common/iconhelper.h"
#include "UI/core_common/quihelper.h"

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

    ui->stackedWidget->setStyleSheet("QLabel{font:60px;}");

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

    ui->btnMain->click();
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
    } else if (name == "Process") {
        ui->stackedWidget->setCurrentIndex(2);
    } else if (name == "Desk") {
        ui->stackedWidget->setCurrentIndex(3);
    } else if (name == "Device") {
        ui->stackedWidget->setCurrentIndex(4);
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

void frmMain::on_exec_pushButton_clicked()
{
    QString cmds = ui->cmd_lineEdit->text();
    ui->cmd_lineEdit->clear();// 清空命令行
    ui->echo_textBrowser->append("$ "+cmds);
    QStringList cmd = cmds.split(" ");
    if(cmd.empty())
        return;
    else if (cmd[0]=="ls") {
        // 调用文件系统的FileManager::List()方法
        QString str="";

        ui->echo_textBrowser->append(str);
    }else if (cmd[0]=="cd") {
        // 调用文件系统的FileManager::ChangeDirectory()方法
        QString str="";

        ui->pwd_label->setText(str);
    }else if (cmd[0]=="clear") {
        ui->echo_textBrowser->clear();
    }else if (cmd[0]=="rm") {
        // 调用文件系统的FileManager::RemoveFile()方法
        QString str="";
        ui->echo_textBrowser->append(str);
    }else if (cmd[0]=="rmdir") {
            // 调用文件系统的FileManager::RemoveDirectory()方法
            QString str="";
            ui->echo_textBrowser->append(str);
    }else if (cmd[0]=="mkdir") {
    // 调用文件系统的FileManager::MakeDirectory()方法
    QString str="";
    ui->echo_textBrowser->append(str);
    }else if (cmd[0]=="mkfile") {
        // 调用文件系统的FileManager::MakeFile()方法
        QString str="";
        ui->echo_textBrowser->append(str);
    }else if (cmd[0]=="exec") {
        // 调用文件系统的FileManager::Execute()方法
//        QString str="";
        os::ProcessManager::Instance().Execute(cmd[1]);
//        ui->echo_textBrowser->append(str);
    }else if (cmd[0]=="kill") {
        // 调用文件系统的FileManager::Kill()方法
        QString str="";
        ui->echo_textBrowser->append(str);
    }else if (cmd[0]=="ps") {
        // 调用文件系统的FileManager::ProcessState()方法
        QString str="";
        ui->echo_textBrowser->append(str);
    }else{
        ui->echo_textBrowser->append(QString("command not definded;"));
    }
    return;
}
