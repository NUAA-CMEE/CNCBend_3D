#ifndef UIDEPLOYPAGE_H
#define UIDEPLOYPAGE_H

#include <QWidget>
#include "SysGLData.h"
#include "sysglobalvars.h"
#include <QPainter>
#include <QPaintEvent>

class uiDeployPage : public QWidget
{
    Q_OBJECT
public:
    explicit uiDeployPage(QWidget *parent = 0);
    ~uiDeployPage();
    void setShowData(bend_surface *pbend,bend_order *porder);
    void resizeWindow(int width,int height);
    void convertGraph(float para, CONVERT convert);
    void updateBendOrder(bend_order *phead);
    void selectBendLine(int x,int y);
    void resetMousePos();

protected:
    void paintEvent(QPaintEvent *e);
    QPixmap *drawPix();
    void drawBasicSurface(QPainter *painter,basic_surface *pBasic);
    void resetVisitFlag(bend_surface *pHead);
private:
    bend_surface *pBendHead;
    bend_order *pOrderHead;

    float xRote,yRote,zRote;
    float zoom,vMove,hMove;

    int iWidth;
    int iHeight;
    int mouse_x;
    int mouse_y;
signals:
    void sendSelectInfo(int bendnum);

};

#endif // UIDEPLOYPAGE_H
