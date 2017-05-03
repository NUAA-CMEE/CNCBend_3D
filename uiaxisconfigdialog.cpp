#include "uiaxisconfigdialog.h"
#include "ui_uiaxisconfigdialog.h"
#include <QMessageBox>

uiAxisConfigDialog::uiAxisConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uiAxisConfigDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("轴配置窗口"));
    pX1_Axis = ui->x1;
    pX2_Axis = ui->x2;
    pR1_Axis = ui->r1;
    pR2_Axis = ui->r2;
    pZ1_Axis = ui->z1;
    pZ2_Axis = ui->z2;
    pType = ui->type;
    pX_max = ui->x_max;
    pX_min = ui->x_min;
    pR_max = ui->r_max;
    pR_min = ui->r_min;
    pZ_max = ui->z_max;
    pZ_min = ui->z_min;
    pBendRate = ui->bendRate;
    pType_Show = ui->type_show;
    //读取数据库数据
    dbName = dbPath + "machineconstants.db";
    QString strSQL = "select x_max,x_min,r_max,r_min,z_max,z_min,bendRate from AxisSet where id = 1";
    QStringList strList = ReadSqlData(dbName,strSQL);
    if(!strList.isEmpty())
    {
        pX_max->setText(strList.at(0));
        pX_min->setText(strList.at(1));
        pR_max->setText(strList.at(2));
        pR_min->setText(strList.at(3));
        pZ_max->setText(strList.at(4));
        pZ_min->setText(strList.at(5));
        pBendRate->setText(strList.at(6));
    }
    strSQL = "select x1_enable,x2_enable,r1_enable,r2_enable,z1_enable,z2_enable,type from AxisSet where id = 1";
    QStringList strList2 = ReadSqlData(dbName,strSQL);
    if(!strList2.isEmpty())
    {
        pX1_Axis->setCurrentIndex(strList2.at(0).toInt());
        pX2_Axis->setCurrentIndex(strList2.at(1).toInt());
        pR1_Axis->setCurrentIndex(strList2.at(2).toInt());
        pR2_Axis->setCurrentIndex(strList2.at(3).toInt());
        pZ1_Axis->setCurrentIndex(strList2.at(4).toInt());
        pZ2_Axis->setCurrentIndex(strList2.at(5).toInt());
        pType->setCurrentIndex(strList2.at(6).toInt()-1);
    }
}

uiAxisConfigDialog::~uiAxisConfigDialog()
{
    delete ui;
}

void uiAxisConfigDialog::updateDBdata()
{
    x_max = pX_max->text().toFloat();
    x_min = pX_min->text().toFloat();
    r_max = pR_max->text().toFloat();
    r_min = pR_min->text().toFloat();
    z_max = pZ_max->text().toFloat();
    z_min = pZ_min->text().toFloat();
    Y_factor = pBendRate->text().toFloat();
    int x1_enable = pX1_Axis->currentIndex();
    int x2_enable = pX2_Axis->currentIndex();
    int r1_enable = pR1_Axis->currentIndex();
    int r2_enable = pR2_Axis->currentIndex();
    int z1_enable = pZ1_Axis->currentIndex();
    int z2_enable = pZ2_Axis->currentIndex();
    int type = pType->currentIndex()+1;
    QString strSQL = QString("update AxisSet set x_max='%1',x_min='%2',r_max='%3',r_min='%4',z_max='%5',\
            z_min='%6',x1_enable='%7',x2_enable='%8',r1_enable='%9',r2_enable='%10',z1_enable='%11',\
            z2_enable='%12',bendRate='%13',type='%14' where id = 1").arg(x_max,0,'f',2).arg(x_min,0,'f',2).arg(r_max,0,'f',2)\
            .arg(r_min,0,'f',2).arg(z_max,0,'f',2).arg(z_min,0,'f',2).arg(x1_enable).arg(x2_enable)\
            .arg(r1_enable).arg(r2_enable).arg(z1_enable).arg(z2_enable).arg(Y_factor).arg(type);
    int res = UpdateSqlData(dbName,strSQL);
    if(res != 0)
    {
        QMessageBox::warning(this,tr("程序提示"),tr("更新数据库错误"),QMessageBox::Yes);
    }
}

void uiAxisConfigDialog::on_cancel_clicked()
{
    this->reject();
}

void uiAxisConfigDialog::on_sure_clicked()
{
    updateDBdata();
    this->accept();
}

void uiAxisConfigDialog::on_type_currentIndexChanged(int index)
{
    switch (index) {
    case 0:
    {
        pType_Show->setPixmap(QString::fromUtf8(":/images/gauge_1"));
        break;
    }
    case 1:
    {
        pType_Show->setPixmap(QString::fromUtf8(":/images/gauge_2"));
        break;
    }
    case 2:
    {
        pType_Show->setPixmap(QString::fromUtf8(":/images/gauge_3"));
        break;
    }
    case 3:
    {
        pType_Show->setPixmap(QString::fromUtf8(":/images/gauge_4"));
        break;
    }
    default:
        break;
    }
}
