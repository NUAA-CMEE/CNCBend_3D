#include "uimachineconstant.h"
#include "ui_uimachineconstant.h"

uiMachineConstant::uiMachineConstant(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uiMachineConstant)
{
    ui->setupUi(this);
    setWindowTitle(tr("机床参数设置窗口"));
    pGeneral[0] = ui->machine_length;
    pGeneral[1] = ui->column_length;
    pGeneral[2] = ui->stage_inert;
    pGeneral[3] = ui->slider_inert;
    pGeneral[4] = ui->firm;
    pGeneral[5] = ui->struct_firm;
    pGeneral[6] = ui->reference_distance;
    pGeneral[7] = ui->correct;
    pAxis[0] = ui->y_max;
    pAxis[1] = ui->y_min;
    pAxis[2] = ui->x_max;
    pAxis[3] = ui->x_min;
    pAxis[4] = ui->r_max;
    pAxis[5] = ui->r_min;
    pAxis[6] = ui->z_max;
    pAxis[7] = ui->z_min;
    //读取数据库数据
    dbName = dbPath + "machineconstants.db";
    QString strSQL1 = "select machine_length,column_length,stage_inert,slider_inert,\
            firm,struct_firm,reference_distance,correct from General where id = 1";
    QString strSQL2 = "select y_max,y_min,x_max,x_min,r_max,r_min,z_max,z_min from AxisSet where id = 1";
    QStringList strList1 = ReadSqlData(dbName,strSQL1);
    QStringList strList2 = ReadSqlData(dbName,strSQL2);
    if(!strList1.isEmpty() && !strList2.isEmpty())
    {
        for(int i = 0; i < 8; i++)
        {
            pGeneral[i]->setText(strList1.at(i));
            pAxis[i]->setText(strList2.at(i));
        }
    }
}

uiMachineConstant::~uiMachineConstant()
{
    delete ui;
}

void uiMachineConstant::on_cancel_clicked()
{
    this->accept();
}

void uiMachineConstant::on_sure_clicked()
{
    float machine_length = pGeneral[0]->text().toFloat();
    float column_length = pGeneral[1]->text().toFloat();
    int stage_inert = pGeneral[2]->text().toInt();
    int slider_inert = pGeneral[3]->text().toInt();
    float firm = pGeneral[4]->text().toFloat();
    float struct_firm = pGeneral[5]->text().toFloat();
    float reference_distance = pGeneral[6]->text().toFloat();
    float correct = pGeneral[7]->text().toFloat();
    y_max = pAxis[0]->text().toFloat();
    y_min = pAxis[1]->text().toFloat();
    x_max = pAxis[2]->text().toFloat();
    x_min = pAxis[3]->text().toFloat();
    r_max = pAxis[4]->text().toFloat();
    r_min = pAxis[5]->text().toFloat();
    z_max = pAxis[6]->text().toFloat();
    z_min = pAxis[7]->text().toFloat();
    //将修改的数据更新入数据库
    QString strSQL1 = QString("update General set machine_length='%1',column_length='%2',stage_inert='%3',\
            slider_inert='%4',firm='%5',struct_firm='%6',reference_distance='%7',correct='%8' where id = 1")\
            .arg(machine_length,0,'f',2).arg(column_length,0,'f',2).arg(stage_inert).arg(slider_inert).\
            arg(firm,0,'f',3).arg(struct_firm,0,'f',2).arg(reference_distance,0,'f',2).arg(correct,0,'f',2);
    QString strSQL2 = QString("update AxisSet set y_max='%1',y_min='%2',x_max='%3',x_min='%4',r_max='%5',r_min='%6',\
            z_max='%7',z_min='%8' where id = 1").arg(y_max,0,'f',2).arg(y_min,0,'f',2).arg(x_max,0,'f',2).\
            arg(x_min,0,'f',2).arg(r_max,0,'f',2).arg(r_min,0,'f',2).arg(z_max,0,'f',2).arg(z_min,0,'f',2);
    UpdateSqlData(dbName,strSQL1);
    UpdateSqlData(dbName,strSQL2);
    this->accept();
}
