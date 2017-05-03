#include "uideploypage.h"
#include <QFile>
#include <math.h>
#include <QDebug>

//颜色列表
float outlineColor[4] = {0.0,0.0,0.0,1.0};  //工件颜色
float bendlineColor[4] = {1.0,0.0,0.0,1.0};  //折弯线颜色


uiDeployPage::uiDeployPage(QWidget *parent) :
    QWidget(parent)
{
    pBendHead = NULL;
    pOrderHead = NULL;
    xRote = 90.0;
    yRote = 0.0;
    zRote = 0.0;
    zoom = 0.0;
    hMove = 0.0;
    vMove = 0.0;
    mouse_x = 0;
    mouse_y = 0;
}

uiDeployPage::~uiDeployPage()
{
}

void uiDeployPage::setShowData(bend_surface *pbend, bend_order *porder)
{
    pBendHead = pbend;
    pOrderHead = porder;
    update();
}

void uiDeployPage::resizeWindow(int width, int height)
{
    setGeometry(0,0,width,height);
    update();
}

void uiDeployPage::convertGraph(float para, CONVERT convert)
{
    switch (convert) {
    case CONVERT_HORIZONTAL://水平移动
    {
        hMove += para;
        update();
        break;
    }
    case CONVERT_VERTICAL:  //垂直移动
    {
        vMove += para;
        update();
        break;
    }
    case CONVERT_ZOOM:      //缩放
    {
        zoom += para;
        if(zoom < 0.1)
            zoom = 0.1;
        update();
        break;
    }
    default:
        break;
    }
}

void uiDeployPage::updateBendOrder(bend_order *phead)
{
    pOrderHead = phead;
    update();
}

void uiDeployPage::selectBendLine(int x, int y)
{
    mouse_x = x-iWidth/2;
    mouse_y = -y+iHeight/2;
    update();
}

void uiDeployPage::resetMousePos()
{
    mouse_x = 0;
    mouse_y = 0;
}

void uiDeployPage::paintEvent(QPaintEvent *e)
{
    e = e;
    iWidth = e->rect().width();
    iHeight = e->rect().height();
    zoom = 200.0/iWidth + 400.0/iHeight;
    QPainter painter(this);
    QPixmap *pixMap = drawPix();
    QRect target(0,0,iWidth,iHeight);
    QRect source(0,0,iWidth,iHeight);
    painter.drawPixmap(target,*pixMap,source);
    delete pixMap;
    pixMap = NULL;
    return;
}

QPixmap *uiDeployPage::drawPix()
{
    QPixmap *pixMap = new QPixmap(iWidth,iHeight);
    pixMap->fill(Qt::white);
    QPainter painter;
    painter.begin(pixMap);
    QMatrix matrix(1,0,0,-1,iWidth/2,iHeight/2);
    painter.setMatrix(matrix);
    resetVisitFlag(pBendHead);
    QFont font("Arial",13,QFont::Bold,true);
    painter.setFont(font);
    bend_order *porder = pOrderHead;
    while (porder) {
        //首先找到当前折弯所对应的折弯数据
        bend_surface *pbend = pBendHead;
        while (pbend) {
            if(pbend->bendNum == porder->bendpoint)
                break;
            pbend = pbend->pNext;
        }
        line_type *pbendline = pbend->pBendLine;
        float start_x = pbendline->start.x/zoom;
        float end_x = pbendline->end.x/zoom;
        float start_y = pbendline->start.z/zoom;
        float end_y = pbendline->end.z/zoom;
        QPen pen1(Qt::red,1,Qt::DotLine);
        painter.setPen(pen1);
        painter.drawLine(start_x,start_y,end_x,end_y);
        if(bManualState == false)
        {
            painter.scale(1,-1);    //文字方向需要变换
            int pos_x = (pbendline->start.x+pbendline->end.x)/(2*zoom);
            int pos_y = (pbendline->start.z+pbendline->end.z)/(2*zoom);
            painter.drawText(QPoint(pos_x-3,-pos_y+5),tr("%1").arg(porder->bend_id));
            painter.setMatrix(QMatrix(1,0,0,-1,iWidth/2,iHeight/2));
        }
        else
        {

            int len = ManualOrder.size();
            for(int i = 0; i < len; i++)
            {
                if(ManualOrder[i] == porder->bendpoint)
                {
                    QPen pen2(Qt::red,2,Qt::SolidLine);
                    painter.setPen(pen2);
                    painter.drawLine(start_x,start_y,end_x,end_y);
                    painter.scale(1,-1);    //文字方向需要变换
                    int pos_x = (pbendline->start.x+pbendline->end.x)/(2*zoom);
                    int pos_y = (pbendline->start.z+pbendline->end.z)/(2*zoom);
                    painter.drawText(QPoint(pos_x-3,-pos_y+5),tr("%1").arg(i+1));
                    painter.setMatrix(QMatrix(1,0,0,-1,iWidth/2,iHeight/2));
                    break;
                }
            }
            if(start_x > end_x)
            {
                float tmp = start_x;
                start_x = end_x;
                end_x = tmp;
            }
            if(start_y > end_y)
            {
                float tmp = start_y;
                start_y = end_y;
                end_y = tmp;
            }
            int tmp_margin = 5;
            if(mouse_x >= (start_x-tmp_margin) && mouse_x <= (end_x+tmp_margin) && mouse_y >= (start_y-tmp_margin) && mouse_y <= (end_y+tmp_margin))
            {
                bool flag = false;
                for(int i = 0; i < len; i++)
                {
                    if(ManualOrder.at(i) == porder->bendpoint)
                    {
                        flag = true;
                        break;
                    }
                }
                if(flag == false){
                    ManualOrder.push_back(porder->bendpoint);
                    //ManualTag.push_back(0);
                    QPen pen2(Qt::red,2,Qt::SolidLine);
                    painter.setPen(pen2);
                    painter.drawLine(start_x,start_y,end_x,end_y);
                    painter.scale(1,-1);    //文字方向需要变换
                    int pos_x = (pbendline->start.x+pbendline->end.x)/(2*zoom);
                    int pos_y = (pbendline->start.z+pbendline->end.z)/(2*zoom);
                    painter.drawText(QPoint(pos_x-3,-pos_y+5),tr("%1").arg(ManualOrder.size()));
                    painter.setMatrix(QMatrix(1,0,0,-1,iWidth/2,iHeight/2));
                    emit sendSelectInfo(porder->bendpoint);
                }
            }
        }

        QPen pen2(Qt::black,1,Qt::SolidLine);
        painter.setPen(pen2);
        QVector<point3f> point = pbend->pDeploy->point;
        painter.drawLine(point[0].x/zoom,point[0].z/zoom,point[3].x/zoom,point[3].z/zoom);
        painter.drawLine(point[1].x/zoom,point[1].z/zoom,point[2].x/zoom,point[2].z/zoom);
        basic_surface *pleft = pbend->pLeftBase;
        basic_surface *pright = pbend->pRightBase;
        if(pleft->isVisited == 0)
        {
            drawBasicSurface(&painter,pleft);
            pleft->isVisited = 1;
        }
        if(pright->isVisited == 0)
        {
            drawBasicSurface(&painter,pright);
            pright->isVisited = 1;
        }
        porder = porder->pNext;
    }
    resetVisitFlag(pBendHead);
    painter.end();
    return pixMap;
}

void uiDeployPage::drawBasicSurface(QPainter *painter, basic_surface *pBasic)
{
    trimmed_surface_type *pSurface = pBasic->pSurface;
    for(int i = 0; i < pSurface->curve_num+1; i++)
    {
        for(int j = 0; j < pSurface->curveCount[i]; j++)
        {
            int type = pSurface->outlineType[i][j];
            if(type == 110)//直线实体
            {
                line_type *pline = (line_type *)pSurface->pOutlines[i][j];
                if(pline->isBendBound)
                    continue;
                painter->drawLine(pline->start.x/zoom,pline->start.z/zoom,\
                                 pline->end.x/zoom,pline->end.z/zoom);
            }
            else if(100 == type){
                arc_type *pArc = (arc_type *)pSurface->pOutlines[i][j];
            }
            else if(type == 126)//B样条曲线实体
            {
                nurbsCurve_type *pNurbsCurve = (nurbsCurve_type *)pSurface->pOutlines[i][j];
                if(pNurbsCurve->isBendBound)
                    continue;
                int num = pNurbsCurve->K_num;
                float *data = pNurbsCurve->pCtlarray;
                if(num == 1) //说明只有两个控制点
                {
                    painter->drawLine(data[0]/zoom,data[2]/zoom,\
                            data[3]/zoom,data[5]/zoom);
                }
                else
                {
                    //painter.setRenderHint(QPainter::Antialiasing,true);
                    QPainterPath path;
                    path.moveTo(data[0]/zoom,data[2]/zoom);
                    for(int m = 1; m < num-2; m++)
                    {
                        path.cubicTo(data[3*m+3]/zoom,data[3*m+5]/zoom,data[3*m+6]/zoom,\
                                data[3*m+8]/zoom,data[3*m+9]/zoom,data[3*m+11]/zoom);
                    }
                    painter->drawPath(path);
                    //painter.setRenderHint(QPainter::Antialiasing,false);
                }
            }
        }
    }
}

void uiDeployPage::resetVisitFlag(bend_surface *pHead)
{
    bend_surface *pbend = pHead;
    while (pbend) {
        pbend->pLeftBase->isVisited = 0;
        pbend->pRightBase->isVisited = 0;
        pbend->pParallel->pLeftBase->isVisited = 0;
        pbend->pParallel->pRightBase->isVisited = 0;
        pbend = pbend->pNext;
    }
}
