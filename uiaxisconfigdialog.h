#ifndef UIAXISCONFIGDIALOG_H
#define UIAXISCONFIGDIALOG_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QLabel>
#include "CNCGlobalFuncs.h"
#include "sysglobalvars.h"

namespace Ui {
class uiAxisConfigDialog;
}

class uiAxisConfigDialog : public QDialog
{
    Q_OBJECT

public:
    explicit uiAxisConfigDialog(QWidget *parent = 0);
    ~uiAxisConfigDialog();

private:
    void updateDBdata();

private slots:
    void on_cancel_clicked();
    void on_sure_clicked();
    void on_type_currentIndexChanged(int index);

private:
    Ui::uiAxisConfigDialog *ui;
    QComboBox *pX1_Axis;
    QComboBox *pX2_Axis;
    QComboBox *pR1_Axis;
    QComboBox *pR2_Axis;
    QComboBox *pZ1_Axis;
    QComboBox *pZ2_Axis;
    QComboBox *pType;
    QLineEdit *pX_max;
    QLineEdit *pX_min;
    QLineEdit *pR_max;
    QLineEdit *pR_min;
    QLineEdit *pZ_max;
    QLineEdit *pZ_min;
    QLineEdit *pBendRate;
    QLabel    *pType_Show;

    QString dbName;
};

#endif // UIAXISCONFIGDIALOG_H
