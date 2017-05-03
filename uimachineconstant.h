#ifndef UIMACHINECONSTANT_H
#define UIMACHINECONSTANT_H

#include <QDialog>
#include <QLineEdit>
#include "CNCGlobalFuncs.h"
#include "sysglobalvars.h"

namespace Ui {
class uiMachineConstant;
}

class uiMachineConstant : public QDialog
{
    Q_OBJECT

public:
    explicit uiMachineConstant(QWidget *parent = 0);
    ~uiMachineConstant();

private slots:
    void on_cancel_clicked();
    void on_sure_clicked();

private:
    Ui::uiMachineConstant *ui;
    QLineEdit *pGeneral[8];
    QLineEdit *pAxis[8];
    QString dbName;
};

#endif // UIMACHINECONSTANT_H
