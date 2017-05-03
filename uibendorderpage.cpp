#include "uibendorderpage.h"
#include "ui_uibendorderpage.h"
#include <math.h>

uiBendOrderPage::uiBendOrderPage(QWidget *parent, bend_surface *pbend, bend_order *porder, int num) :
    QDialog(parent),
    ui(new Ui::uiBendOrderPage)
{
    ui->setupUi(this);
    setWindowTitle(tr("折弯工序设置窗口"));
    bendNum = num;
    pOrderHead = porder;
    pTable = ui->tableWidget;
    pWidget = ui->widget;
    pTmp_Head = NULL;
    pTmp_Last = NULL;
    initBendOrder();
    pTable->horizontalHeader()->setStretchLastSection(true);
    pTable->verticalHeader()->setVisible(false);
    pTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    QList<int> size;
    size<<100<<200;
    ui->splitter->setSizes(size);
    ui->splitter->setStretchFactor(0,1);
    ui->splitter->setStretchFactor(1,1);
    pDeploy = new uiDeployPage(pWidget);
    pDeploy->setShowData(pbend,porder);
    int width = pWidget->width();
    int heigth = pWidget->height();
    pDeploy->resizeWindow(width,heigth);
    pDeploy->show();
    showTableData();
}

uiBendOrderPage::~uiBendOrderPage()
{
    bend_order *ptmp = pTmp_Head;
    while (ptmp) {
        pTmp_Head = ptmp->pNext;
        delete ptmp;
        ptmp = pTmp_Head;
    }
    delete pDeploy;
    pDeploy = NULL;
    delete ui;
}

void uiBendOrderPage::showTableData()
{
    pTable->setRowCount(bendNum);
    bend_order *porder = pTmp_Head;
    for(int i = 0; i < bendNum; i++)
    {
        int bend_id = porder->bend_id;
        float angle = fabs(porder->bendAngle)/PI*180;
        float radius = porder->bendRadius;
        float width = porder->bendWidth;
        int direct_h = porder->direction_h;
        int direct_v = porder->direction_v;
        int tool_id = porder->tool_id;
        pTable->setItem(i,0,new QTableWidgetItem(QString("%1").arg(bend_id)));
        pTable->item(i,0)->setTextAlignment(Qt::AlignCenter);
        pTable->setItem(i,1,new QTableWidgetItem(QString("%1").arg(angle,0,'f',2)));
        pTable->item(i,1)->setTextAlignment(Qt::AlignCenter);
        pTable->setItem(i,2,new QTableWidgetItem(QString("%1").arg(radius,0,'f',2)));
        pTable->item(i,2)->setTextAlignment(Qt::AlignCenter);
        pTable->setItem(i,3,new QTableWidgetItem(QString("%1").arg(width,0,'f',2)));
        pTable->item(i,3)->setTextAlignment(Qt::AlignCenter);
        pTable->setItem(i,4,new QTableWidgetItem(QString("%1").arg(direct_v)));
        pTable->item(i,4)->setTextAlignment(Qt::AlignCenter);
        pTable->setItem(i,5,new QTableWidgetItem(QString("%1").arg(direct_h)));
        pTable->item(i,5)->setTextAlignment(Qt::AlignCenter);
        pTable->setItem(i,6,new QTableWidgetItem(QString("%1").arg(tool_id)));
        pTable->item(i,6)->setTextAlignment(Qt::AlignCenter);
        porder = porder->pNext;
    }
    pTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    pTable->selectRow(0);
}

void uiBendOrderPage::resizeEvent(QResizeEvent *event)
{
    event = event;
    updateWindow();
}

void uiBendOrderPage::updateWindow()
{
    if(pDeploy)
    {
        int width = pWidget->width();
        int height = pWidget->height();
        pDeploy->resizeWindow(width,height);
    }
}

void uiBendOrderPage::initBendOrder()
{
    bend_order *ptmp_order = pOrderHead;
    while (ptmp_order) {
        bend_order *ptmp = new bend_order;
        memcpy(ptmp,ptmp_order,sizeof(bend_order));
        if(pTmp_Head == NULL)
        {
            pTmp_Head = ptmp;
            pTmp_Last = ptmp;
            pTmp_Head->pBefore = NULL;
            pTmp_Last->pNext = NULL;
        }
        else
        {
            pTmp_Last->pNext = ptmp;
            ptmp->pBefore = pTmp_Last;
            pTmp_Last = ptmp;
            pTmp_Last->pNext = NULL;
        }
        ptmp_order = ptmp_order->pNext;
    }
}

void uiBendOrderPage::copyData()
{
    //应用则要将折弯工序的数据进行修改
    bend_order *ptmp = pTmp_Head;
    bend_order *porder = pOrderHead;
    while (ptmp) {
        porder->bend_id = ptmp->bend_id;
        porder->bendpoint = ptmp->bendpoint;
        porder->direction_h = ptmp->direction_h;
        porder->direction_v = ptmp->direction_v;
        porder->backgauge_point = ptmp->backgauge_point;
        porder->tool_id = ptmp->tool_id;
        porder = porder->pNext;
        ptmp = ptmp->pNext;
    }
}

void uiBendOrderPage::on_up_clicked()
{
    pTable->setFocus();
    int curNum = pTable->currentRow();
    if(curNum == 0)
        return;
    int upNum = curNum-1;
    for(int i = 1; i < 7; i++)
    {
        QString tmp = pTable->item(curNum,i)->text();
        pTable->item(curNum,i)->setText(pTable->item(upNum,i)->text());
        pTable->item(upNum,i)->setText(tmp);
    }
    pTable->selectRow(upNum);
    bend_order *ptmp = pTmp_Head;
    while (ptmp) {
        if(ptmp->bend_id == curNum+1)
            break;
        ptmp = ptmp->pNext;
    }
    bend_order *pUp = ptmp->pBefore;
    ptmp->pBefore = pUp->pBefore;
    if(pUp->pBefore != NULL)
        pUp->pBefore->pNext = ptmp;
    else
        pTmp_Head = ptmp;
    pUp->pNext = ptmp->pNext;
    if(ptmp->pNext != NULL)
        ptmp->pNext->pBefore = pUp;
    ptmp->pNext = pUp;
    pUp->pBefore = ptmp;
    ptmp->bend_id = upNum+1;
    pUp->bend_id = curNum+1;
    pDeploy->updateBendOrder(pTmp_Head);
}

void uiBendOrderPage::on_down_clicked()
{
    pTable->setFocus();
    int curNum = pTable->currentRow();
    if(curNum == bendNum-1)
        return;
    int downNum = curNum+1;
    for(int i = 1; i < 7; i++)
    {
        QString tmp = pTable->item(curNum,i)->text();
        pTable->item(curNum,i)->setText(pTable->item(downNum,i)->text());
        pTable->item(downNum,i)->setText(tmp);
    }
    pTable->selectRow(downNum);
    bend_order *ptmp = pTmp_Head;
    while (ptmp) {
        if(ptmp->bend_id == curNum+1)
            break;
        ptmp = ptmp->pNext;
    }
    bend_order *pDown = ptmp->pNext;
    pDown->pBefore = ptmp->pBefore;
    if(ptmp->pBefore != NULL)
        ptmp->pBefore->pNext = pDown;
    else
        pTmp_Head = pDown;
    ptmp->pNext = pDown->pNext;
    if(pDown->pNext != NULL)
        pDown->pNext->pBefore = ptmp;
    pDown->pNext = ptmp;
    ptmp->pBefore = pDown;
    ptmp->bend_id = downNum+1;
    pDown->bend_id = curNum+1;
    pDeploy->updateBendOrder(pTmp_Head);
}

void uiBendOrderPage::on_cancel_clicked()
{
    this->reject();
}

void uiBendOrderPage::on_sure_clicked()
{
    this->accept();
}

void uiBendOrderPage::on_del_clicked()
{

}

void uiBendOrderPage::on_splitter_splitterMoved(int pos, int index)
{
    pos = pos;
    index = index;
    updateWindow();
}
