#ifndef UIBENDORDERPAGE_H
#define UIBENDORDERPAGE_H

#include <QDialog>
#include <QString>
#include <QTableWidget>
#include "SysGLData.h"
#include "uideploypage.h"

namespace Ui {
class uiBendOrderPage;
}

class uiBendOrderPage : public QDialog
{
    Q_OBJECT

public:
    explicit uiBendOrderPage(QWidget *parent,bend_surface *pbend, bend_order *porder,int num);
    ~uiBendOrderPage();
    void showTableData();
    bend_surface *findBendSurface(int bendnum);
    void resizeEvent(QResizeEvent *event);
    void updateWindow();
    void initBendOrder();
    void copyData();

private slots:
    void on_up_clicked();
    void on_down_clicked();
    void on_cancel_clicked();
    void on_sure_clicked();
    void on_del_clicked();
    void on_splitter_splitterMoved(int pos, int index);

private:
    Ui::uiBendOrderPage *ui;
    bend_order *pOrderHead;
    QTableWidget *pTable;
    QWidget *pWidget;
    uiDeployPage *pDeploy;
    int bendNum;
    bend_order *pTmp_Head;
    bend_order *pTmp_Last;
};

#endif // UIBENDORDERPAGE_H
