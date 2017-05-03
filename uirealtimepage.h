#ifndef UIREALTIMEPAGE_H
#define UIREALTIMEPAGE_H

#include <QDialog>
#include <QMutex>
#include <QLabel>
#include <QTimer>
#include <QMouseEvent>

#define NORMAL_COLOR    "background-color:white"
#define UDP_COLOR       "background-color:rgb(0,170,170)"
#define ER_COLOR        "background-color:rgb(170,0,170)"
#define MUTE_COLOR      "background-color:rgb(170,170,0)"
#define CLAMP_COLOR     "background-color:rgb(68,137,170)"
#define LDP_COLOR       "background-color:rgb(170,170,170)"
#define CLOSING_COLOR   "background-color:rgb(0,170,170)"
#define PRESSING_COLOR  "background-color:rgb(170,0,0)"
#define OPENING_COLOR   "background-color:rgb(170,0,170)"

namespace Ui {
class uiRealTimePage;
}

class uiRealTimePage : public QDialog
{
    Q_OBJECT

public:
    explicit uiRealTimePage(QWidget *parent = 0);
    ~uiRealTimePage();
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *eKey);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void setShowContent();

private:
    Ui::uiRealTimePage *ui;
    QWidget *pGLWidget;     //折弯仿真显示区
    QTimer *pShowTimer;
    int mouse_x,mouse_y;
    QLabel *pLabel[8];
    QMutex mutex;

public slots:
    void showTimeOutSlot();
};

#endif // UIREALTIMEPAGE_H
