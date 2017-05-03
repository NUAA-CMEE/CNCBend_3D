#include "toolinfoinput.h"
#include "ui_toolinfoinput.h"
#include "sysglobalvars.h"

ToolInfoInput::ToolInfoInput(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ToolInfoInput)
{
    ui->setupUi(this);
    setWindowTitle(tr("模具配置信息输入"));
    pPunchBox = ui->comboBox;
    pDieBox = ui->comboBox_2;
    pWidth_label = ui->width;
    pPos_label = ui->position;
    pMargin_label = ui->margin;

    pPunchBox->insertItem(0,tr("1"));
    pPunchBox->insertItem(1,tr("2"));
    pPunchBox->insertItem(2,tr("3"));
    pDieBox->insertItem(0,tr("1"));
    pDieBox->insertItem(1,tr("2"));
    pDieBox->insertItem(2,tr("3"));

    pPunchBox->setCurrentText(QString("%1").arg(punch_id));
    pDieBox->setCurrentText(QString("%1").arg(die_id));
    pWidth_label->setText(QString("%1").arg(tool_width));
    pPos_label->setText(QString("%1").arg(tool_position));
    pMargin_label->setText(QString("%1").arg(margin));
}

ToolInfoInput::~ToolInfoInput()
{
    delete ui;
}

/********************************************
 *function:取消的槽函数
 *adding:
 *author: xu
 *date: 2016/04/13
 *******************************************/
void ToolInfoInput::on_btn_cancel_clicked()
{
    this->reject();
}

/********************************************
 *function:保存配置的槽函数
 *adding:
 *author: xu
 *date: 2016/04/13
 *******************************************/
void ToolInfoInput::on_btn_save_clicked()
{
    punch_id = pPunchBox->currentText().toInt();
    die_id = pDieBox->currentText().toInt();
    tool_width = pWidth_label->text().toFloat();
    tool_position = pPos_label->text().toFloat();
    margin = pMargin_label->text().toFloat();
    this->accept();
}
