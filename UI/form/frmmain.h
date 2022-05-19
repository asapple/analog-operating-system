#ifndef FRMMAIN_H
#define FRMMAIN_H

#include <QDialog>

namespace Ui {
class frmMain;
}

class frmMain : public QDialog
{
    Q_OBJECT

public:
    explicit frmMain(QWidget *parent = 0);
    ~frmMain();

protected:
    bool eventFilter(QObject *watched, QEvent *event);

private:
    Ui::frmMain *ui;
    void setDeskTable();
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
};

#endif // UIDEMO01_H
