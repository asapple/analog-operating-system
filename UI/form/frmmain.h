#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QDialog>
#include<QStringListModel>

namespace Ui {
inline QString TEXT_COLOR_GREEN(QString str) {
    return  "<font color=green>" +str +"</font>" + "<font color=black></font>";
}

inline QString TEXT_COLOR_BLUE(QString str) {
    return  "<font color=blue>" +str +"</font>" + "<font color=black></font>";
}

class frmMain;
}

class frmMain : public QDialog
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    void updateProcView();
    /**
     * @brief 更新内存位图视图
     */
    void updateMemTableView();
    /**
     * @brief 更新磁盘位图视图
     */
    void updateDiskView();
    /**
     * @brief 更新设备视图
     */
    void updateDeviceView();
    ~frmMain();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmMain *ui;
    QStringListModel *m_model;
    void setDeskTable();
    void setMemTable();
    void createItemsARow(int rowNo, int addr,int size,int used,int free);

private slots:
    void initForm();
    void buttonClick();
    //void initPage1();
private slots:
    void on_btnMenu_Min_clicked();
    void on_btnMenu_Max_clicked();
    void on_btnMenu_Close_clicked();
    void on_cmd_lineEdit_returnPressed();

protected:
    virtual void mousePressEvent(QMouseEvent *event);    //鼠标点击事件
    virtual void mouseMoveEvent(QMouseEvent *event);     //鼠标移动事件
    virtual void mouseReleaseEvent(QMouseEvent *event);  //鼠标释放事件
    bool mouse_is_press;     //鼠标是否按下
    QPoint  mouse_move_len;  //鼠标移动事件中，移动的距离
};

#endif // UIDEMO01_H
