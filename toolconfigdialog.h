#ifndef TOOLCONFIGDIALOG_H
#define TOOLCONFIGDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QTableWidget>
#include <drawtools.h>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QMessageBox>
namespace Ui {
class ToolConfigDialog;
}

class ToolConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ToolConfigDialog(QWidget *parent = 0);
    ~ToolConfigDialog();
    void resizeEvent(QResizeEvent *event);

private:
    void showTableData();

private slots:
    void on_btn_add_clicked();
    void on_btn_delete_clicked();
    void on_btn_apply_clicked();
    void on_btn_modify_clicked();
    void modelData();
    void pictureShow_1(int);
    void pictureShow_2(int);
    void distanceChange(QModelIndex,QModelIndex);
private:
    Ui::ToolConfigDialog *ui;
    QPushButton *pButton[4];
    DrawTools *pDrawTool;
    QWidget *pWidget;
    QTableWidget *pTable_punch;
    QTableWidget *pTable_die;

    QComboBox *PunchWidth[10][8];
    QComboBox *dieWidth[10][8];
    QComboBox *modelSelect[10][2];

    QLabel *plabel[2][2];

    QStringList model_list,model_list2,width_list;
    int rowIndex;
    int t_width[10][8];
    int t_position[10][8];

};

#endif // TOOLCONFIGDIALOG_H
