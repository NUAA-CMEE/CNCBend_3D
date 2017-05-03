#include "progressdlg.h"
#include "ui_progressdlg.h"
#include "sysglobalvars.h"
#include "SysKeyboard.h"
#include <QPropertyAnimation>

ProgressDlg::ProgressDlg(QWidget *parent,const QString strMsg, int maxvalue, bool modal) :
    QDialog(parent),
    ui_ProgressDlg(new Ui::ProgressDlg)
{
    ui_ProgressDlg->setupUi(this);
    //QTextCodec::setCodecForTr(QTextCodec::codecForName("UTF-8")); //这个为tr做准备
    setWindowFlags(Qt::FramelessWindowHint);
    int width = this->parentWidget()->width();
    int height = this->parentWidget()->height();
    move(width/2-250,height/2-75);
    pMsg = ui_ProgressDlg->label;
    pProgressBar = ui_ProgressDlg->progressBar;
    pProgressBar->setRange(0,maxvalue);
    pProgressBar->setValue(0);
    MaxValue = maxvalue;    //设置进度条范围
    if(modal)
        setWindowModality(Qt::WindowModal); //设置为模态对话框
    else
        setWindowModality(Qt::NonModal);    //设置为非模态对话框
    //设置消息确认对话框整体风格
    setStyleSheet("background-color:lightgray");
    //ui_ProgressDlg->widget->setStyleSheet(pStyle->widgetStyle);
    //pMsg->setFont(pStyle->Button_Font);

    //信息提示的显示位置
    pMsg->setText(strMsg);
    pMsg->adjustSize();
}

ProgressDlg::~ProgressDlg()
{
    delete ui_ProgressDlg;
}

/********************************************
 *function:设置进度条位置
 *adding:
 *author: xu
 *date: 20145/03/13
 *******************************************/
void ProgressDlg::setValue(int num)
{
    pProgressBar->setValue(num);
    if(num == MaxValue)
    {
        this->hide();
    }
}
