#include "toolconfigdialog.h"
#include "ui_toolconfigdialog.h"

ToolConfigDialog::ToolConfigDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolConfigDialog)
{
    ui->setupUi(this);
    setWindowTitle(tr("模具配置窗口"));
    pWidget = ui->widget;
    pTable_punch = ui->tableWidget;
    pTable_die = ui->tableWidget_2;
    plabel[0][0] = ui->label_2;
    plabel[0][1] = ui->label_4;
    plabel[1][0] = ui->label_3;
    plabel[1][1] = ui->label_5;
    pDrawTool = new DrawTools(pWidget);
    pDrawTool->show();
    pTable_punch->setColumnCount(11);
    QStringList header;
    header<<tr("上模型号")<<tr("位置")<<tr("宽度")<<tr("W1")\
          <<tr("W2")<<tr("W3")<<tr("W4")<<tr("W5")<<tr("W6")<<tr("W7")<<tr("W8");
    pTable_punch->setHorizontalHeaderLabels(header); //设置表头显示内容
    pTable_punch->setSelectionBehavior(QAbstractItemView::SelectRows);
    pTable_punch->setColumnWidth(0,120);
    pTable_punch->setColumnWidth(1,60);
    pTable_punch->setColumnWidth(2,60);
    pTable_punch->setColumnWidth(3,60);
    pTable_punch->setColumnWidth(4,60);
    pTable_punch->setColumnWidth(5,60);
    pTable_punch->setColumnWidth(6,60);
    pTable_punch->setColumnWidth(7,60);
    pTable_punch->setColumnWidth(8,60);
    pTable_punch->setColumnWidth(9,60);
    pTable_punch->setColumnWidth(10,60);

    pTable_die->setColumnCount(11);
    QStringList header_2;
    header_2<<tr("下模型号")<<tr("位置")<<tr("宽度")<<tr("W1")\
          <<tr("W2")<<tr("W3")<<tr("W4")<<tr("W5")<<tr("W6")<<tr("W7")<<tr("W8");
    pTable_die->setHorizontalHeaderLabels(header_2); //设置表头显示内容
    pTable_die->setSelectionBehavior(QAbstractItemView::SelectRows);
    pTable_die->setColumnWidth(0,120);
    pTable_die->setColumnWidth(1,60);
    pTable_die->setColumnWidth(2,60);
    pTable_die->setColumnWidth(3,60);
    pTable_die->setColumnWidth(4,60);
    pTable_die->setColumnWidth(5,60);
    pTable_die->setColumnWidth(6,60);
    pTable_die->setColumnWidth(7,60);
    pTable_die->setColumnWidth(8,60);
    pTable_die->setColumnWidth(9,60);
    pTable_die->setColumnWidth(10,60);

    model_list<<"67_9_88°"<<"120_9_90°"<<"95_6_90°"<<"70_8_88°"<<"70_8_90°"<<"67_9_90°";
    model_list2<<"46_37_90°_6_10"<<"46_50_88°_16_25"<<"46_45_86°_14_18"\
              <<"46_39_90°_8_12"<<"46_39_88°_8_12"<<"46_50_86°_16_25";
    width_list<<"0"<<"10"<<"15"<<"20"<<"40"<<"50"<<"100(R)"<<"200"<<"300";
}

ToolConfigDialog::~ToolConfigDialog()
{
    delete ui;
}

void ToolConfigDialog::resizeEvent(QResizeEvent *event)
{
    event = event;
    if(pDrawTool)
    {
        int width = pWidget->width();
        int height = pWidget->height();
        pDrawTool->resizeWindow(width,height);
    }
}

/********************************************
 *function:添加配置的槽函数
 *adding:
 *author: xu
 *date: 2016/04/13
 *******************************************/
void ToolConfigDialog::on_btn_add_clicked()
{
    rowIndex = pTable_punch->rowCount();
    pTable_punch->setRowCount(rowIndex+1);
    pTable_die->setRowCount(rowIndex+1);

    modelSelect[rowIndex][0] = new QComboBox;
    modelSelect[rowIndex][1] = new QComboBox;
    modelSelect[rowIndex][0]->addItems(model_list);
    modelSelect[rowIndex][1]->addItems(model_list2);
    connect(modelSelect[rowIndex][0],SIGNAL(currentIndexChanged(int)),this,SLOT(pictureShow_1(int)));
    connect(modelSelect[rowIndex][1],SIGNAL(currentIndexChanged(int)),this,SLOT(pictureShow_2(int)));
    for(int i=0;i<8;i++)
    {
        PunchWidth[rowIndex][i] = new QComboBox;
        dieWidth[rowIndex][i] = new QComboBox;
        PunchWidth[rowIndex][i]->addItems(width_list);
        dieWidth[rowIndex][i]->addItems(width_list);

        pTable_punch->setCellWidget(rowIndex,i+3,PunchWidth[rowIndex][i]);
        pTable_die->setCellWidget(rowIndex,i+3,dieWidth[rowIndex][i]);
//      connect(PunchWidth[rowIndex][i],SIGNAL(currentIndexChanged(int)),this,SLOT(modelData()));
//      connect(dieWidth[rowIndex][i],SIGNAL(currentIndexChanged(QString)),this,SLOT(modelData(QString)));
    }
    pTable_punch->setCellWidget(rowIndex,0,modelSelect[rowIndex][0]);
    pTable_punch->setItem(rowIndex,1,new QTableWidgetItem());
    pTable_punch->setItem(rowIndex,2,new QTableWidgetItem());
    pTable_die->setCellWidget(rowIndex,0,modelSelect[rowIndex][1]);
    pTable_die->setItem(rowIndex,1,new QTableWidgetItem());
    pTable_die->setItem(rowIndex,2,new QTableWidgetItem());
//    connect(pTable_punch->model(),SIGNAL(dataChanged(QModelIndex,QModelIndex)),this,SLOT(distanceChange(QModelIndex,QModelIndex)));

    toolPunch.push_back(rowIndex+1);
    toolDie.push_back(rowIndex+1);
    toolMargin.push_back(300);//许折宽度，暂时这样设置
    if(rowIndex < 2)
    {
        plabel[rowIndex][0]->setPixmap(QString(":/images/resource/punch_1.PNG"));
        plabel[rowIndex][1]->setPixmap(QString(":/images/resource/die_1.PNG"));
    }
}

/********************************************
 *function:保存配置的槽函数
 *adding:
 *author: xu
 *date: 2016/04/13
 *******************************************/
void ToolConfigDialog::on_btn_modify_clicked()
{
    int currentRow = pTable_punch->currentRow();
    int width_tmp = 0;
    for(int i=0;i<8;i++)
    {
        t_width[currentRow][i] = PunchWidth[currentRow][i]->currentText().left(3).toInt();
        toolWidth.push_back(t_width[currentRow][i]);
    }
    int n1 = pTable_punch->item(currentRow,1)->text().toInt();
    t_position[currentRow][0] = n1;
    toolPosition.push_back(t_position[currentRow][0]);
    for(int i=1;i<8;i++)
    {
        t_position[currentRow][i] = t_width[currentRow][i-1]+t_position[currentRow][i-1];
        toolPosition.push_back(t_position[currentRow][i]);
    }
    pDrawTool->refreshGraph();
    for(int i = 0;i<8;i++)
    {
        width_tmp += PunchWidth[currentRow][i]->currentText().left(3).toInt();
    }
    pTable_punch->item(currentRow,2)->setText(tr("%1").arg(width_tmp));
}

void ToolConfigDialog::modelData()
{
//    int cur_Index = pTable_punch->currentRow();
//    int width_tmp = 0;
//    for(int i = 0;i<8;i++)
//    {
//        width_tmp += PunchWidth[cur_Index][i]->currentText().left(3).toInt();
//    }
//    pTable_punch->setItem(cur_Index,1,new QTableWidgetItem(width_tmp));
//    pTable_die->setItem(cur_Index,1,new QTableWidgetItem(width_tmp));
}

void ToolConfigDialog::pictureShow_1(int j)
{
    if(rowIndex > 1)
        return;
    for(int i=0;i<rowIndex+1;i++)
    {
        plabel[rowIndex][0]->setPixmap(QString(":/images/resource/punch_%1.PNG").arg(j+1));
    }
}

void ToolConfigDialog::pictureShow_2(int j)
{
    if(rowIndex > 1)
        return;
    for(int i=0;i<rowIndex+1;i++)
    {
        plabel[rowIndex][1]->setPixmap(QString(":/images/resource/die_%1.PNG").arg(j+1));
    }
}

void ToolConfigDialog::distanceChange(QModelIndex, QModelIndex)
{

}

/********************************************
 *function:删除配置的槽函数
 *adding:
 *author: xu
 *date: 2016/04/13
 *******************************************/
void ToolConfigDialog::on_btn_delete_clicked()
{
    int rowIndex = pTable_punch->currentRow();
    if(rowIndex !=-1)
        {
            pTable_punch->removeRow(rowIndex);
            pTable_die->removeRow(rowIndex);
        }
    toolWidth.erase(toolWidth.begin()+rowIndex*8,toolWidth.begin()+rowIndex*8+8);
    toolPosition.erase(toolPosition.begin()+rowIndex*8,toolPosition.begin()+rowIndex*8+8);
    pDrawTool->refreshGraph();

    toolPunch.erase(toolPunch.begin()+rowIndex);
    toolDie.erase(toolDie.begin()+rowIndex);
    toolMargin.erase(toolMargin.begin()+rowIndex);
}

/********************************************
 *function:应用的槽函数
 *adding:
 *author: xu
 *date: 2016/04/13
 *******************************************/
void ToolConfigDialog::on_btn_apply_clicked()
{
    int rows = pTable_punch->rowCount();
    toolWidth.clear();
    toolPosition.clear();
    int width_tmp;
    int position_tmp;
    for(int i=0;i<rows;i++)
    {
        width_tmp = 0;
        position_tmp = pTable_punch->item(i,1)->text().toInt();
        for(int j=0;j<8;j++)
        {
            width_tmp += PunchWidth[i][j]->currentText().left(3).toInt();
        }
        toolWidth.push_back(width_tmp);
        toolPosition.push_back(position_tmp);
    }
    if(workpiece_thickness > 1.0)
        QMessageBox::about(this,"Waring","模具不支持的板料厚度!");
    this->accept();
}
