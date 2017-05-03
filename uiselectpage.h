#ifndef UISELECTPAGE_H
#define UISELECTPAGE_H

#include <QDialog>
#include <QListWidget>
#include "bendglwidget.h"

namespace Ui {
class uiSelectPage;
}

class uiSelectPage : public QDialog
{
    Q_OBJECT

public:
    explicit uiSelectPage(QWidget *parent = 0,QString path = "");
    ~uiSelectPage();
    void resizeEvent(QResizeEvent *event);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

private slots:
    void on_btn_cancel_clicked();
    void on_btn_sure_clicked();

private:
    Ui::uiSelectPage *ui;
    QWidget *pGLWidget;
    //QListWidget *pList;
    QPushButton *pButton[2];
    float mouse_x;
    float mouse_y;
    QString workpice_path;
};

#endif // UISELECTPAGE_H
