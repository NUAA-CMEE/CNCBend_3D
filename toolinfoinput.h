#ifndef TOOLINFOINPUT_H
#define TOOLINFOINPUT_H

#include <QDialog>
#include <QComboBox>
#include <QLineEdit>
#include <QKeyEvent>

namespace Ui {
class ToolInfoInput;
}

class ToolInfoInput : public QDialog
{
    Q_OBJECT

public:
    explicit ToolInfoInput(QWidget *parent = 0);
    ~ToolInfoInput();

private slots:
    void on_btn_cancel_clicked();
    void on_btn_save_clicked();

private:
    Ui::ToolInfoInput *ui;
    QComboBox *pPunchBox;
    QComboBox *pDieBox;
    QLineEdit *pWidth_label;
    QLineEdit *pPos_label;
    QLineEdit *pMargin_label;
};

#endif // TOOLINFOINPUT_H
