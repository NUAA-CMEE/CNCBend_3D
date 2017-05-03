#include "drawtools.h"
#include <QPaintEvent>

DrawTools::DrawTools(QWidget *parent) :
    QWidget(parent)
{
//    resize(width,height);
//    iWidth = width;
//    iHeight = height;
}

DrawTools::~DrawTools()
{
}

void DrawTools::resizeWindow(int width, int height)
{
    setGeometry(0,0,width,height);
    update();
}

void DrawTools::refreshGraph()
{
    update();
}

void DrawTools::paintEvent(QPaintEvent *e)
{
    e = e;
    iWidth = e->rect().width();
    iHeight = e->rect().height();
    QPainter painter(this);
    QPixmap *pixMap = drawPix();
    QRect target(0,0,iWidth,iHeight);
    QRect source(0,0,iWidth,iHeight);
    painter.drawPixmap(target,*pixMap,source);
    delete pixMap;
    pixMap = NULL;
    return;
}

QPixmap *DrawTools::drawPix()
{
    QPixmap *pixMap = new QPixmap(iWidth,iHeight);
    pixMap->fill(Qt::white);
    QPainter painter;
    painter.begin(pixMap);
    QPen pen(Qt::gray,1,Qt::SolidLine);
    QBrush brush(Qt::gray);
    int num1 = 4;
    QPointF point1[num1];
    int num2 = 4;
    QPointF point2[num2];
    point1[0] = QPointF(0,0);
    point1[1] = QPointF(0,iHeight/8.0);
    point1[2] = QPointF(iWidth,iHeight/8.0);
    point1[3] = QPointF(iWidth,0);
    point2[0] = QPointF(0,iHeight*7/8.0);
    point2[1] = QPointF(0,iHeight);
    point2[2] = QPointF(iWidth,iHeight);
    point2[3] = QPointF(iWidth,iHeight*7/8.0);
    painter.setPen(pen);
    painter.setBrush(brush);
    painter.drawPolygon(point1,num1);
    painter.drawPolygon(point2,num2);
    painter.setPen(Qt::black);
    float width = iWidth/40.0;
    for(int i = 0; i <= 40; i++)
    {
        if(i%10 == 0)
        {
            pen = QPen(Qt::black,2,Qt::SolidLine);
            painter.setPen(pen);
            painter.drawLine(QPointF(width*i,iHeight/8.0),QPointF(width*i,iHeight/8.0-10));
            painter.drawLine(QPointF(width*i,iHeight*7/8.0),QPointF(width*i,iHeight*7/8.0+10));
//            if(i == 0)
//                painter.drawText(QRect(width*i+5,iHeight/8.0+5,30,10),QString("%1").arg(0));
//            else if(i == 40)
//                painter.drawText(QRect(width*i-30,iHeight/8.0+5,30,10),QString("%1").arg(machineWidth));
//            else
//                painter.drawText(QRect(width*i-10,iHeight/8.0+5,30,10),QString("%1").arg(i/40.0*machineWidth));
        }
        else
        {
            pen = QPen(Qt::black,1,Qt::SolidLine);
            painter.setPen(pen);
            painter.drawLine(QPointF(width*i,iHeight/8.0),QPointF(width*i,iHeight/8.0-5));
            painter.drawLine(QPointF(width*i,iHeight*7/8.0),QPointF(width*i,iHeight*7/8.0+5));
        }
    }
    //绘制块夹
    float width_2 = 3.5*iWidth/69;
    float width_3 = 2.5*iWidth/69;
    for(int i=0;i<20;i++)
    {
        //painter.setPen(Qt::black);
        painter.setBrush(Qt::red);
        painter.drawRect(i*width_2,iHeight/10.0,width_3,iHeight/10);
    }
    //绘制模具模型
    int num = toolWidth.size();
    for(int i = 0; i < num; i++)
    {
        float x_start = toolPosition[i]/machineWidth*iWidth;
        float x_end = (toolPosition[i]+toolWidth[i])/machineWidth*iWidth;
//        pen = QPen(Qt::green,2,Qt::SolidLine);
//        painter.setPen(pen);
//        painter.setFont(QFont("微软雅黑",15));
//        painter.drawLine(QPointF(x_start,iHeight/10.0),QPointF(x_start,iHeight/8.0));
//        painter.drawLine(QPointF(x_end,iHeight/10.0),QPointF(x_end,iHeight/8.0));
//        painter.drawText(QRect((x_start+x_end)/2-5,iHeight/2-10,20,20),QString("%1").arg(i+1));
        //绘制下模模型
        pen = QPen(Qt::white,1,Qt::SolidLine);
        painter.setPen(pen);
        brush = QBrush(Qt::blue);
        painter.setBrush(brush);
        int num3 = 4;
        QPointF point3[num3];
        point3[0] = QPointF(x_start,iHeight*7/8.0);
        point3[1] = QPointF(x_end,iHeight*7/8.0);
        point3[2] = QPointF(x_end,iHeight*6/8.0);
        point3[3] = QPointF(x_start,iHeight*6/8.0);
        painter.drawPolygon(point3,num3);
        //绘制上模模型
//        brush = QBrush(Qt::green);
//        painter.setBrush(brush);
//        int num4 = 6;
//        QPointF point4[num4];
//        float indent = toolWidth[i]*iWidth/(6.0*machineWidth);
//        point4[0] = QPointF(x_start+indent,iHeight/8.0);
//        point4[1] = QPointF(x_start+indent,iHeight/5.0);
//        point4[2] = QPointF(x_start,iHeight/4.0);
//        point4[3] = QPointF(x_end,iHeight/4.0);
//        point4[4] = QPointF(x_end-indent,iHeight/5.0);
//        point4[5] = QPointF(x_end-indent,iHeight/8.0);
//        painter.drawPolygon(point4,num4);
        int num4 = 4;
        QPointF point4[num4];
        point4[0] = QPointF(x_start,iHeight*2/8.0);
        point4[1] = QPointF(x_end,iHeight*2/8.0);
        point4[2] = QPointF(x_end,iHeight/8.0);
        point4[3] = QPointF(x_start,iHeight/8.0);
        painter.drawPolygon(point4,num4);
    }
    //绘制分割线
    //float margin = 200.0;
//    int num = toolWidth.size();
//    float width_sum = (num-1)*tool_margin;
//    for(int m = 0; m < num; m++)
//        width_sum += toolWidth[m];
//    float wid = width_sum/machineWidth*iWidth;
//    for(int m = 0; m < num; m++)
//    {
//        float x_start;
//        if(m == 0)
//            x_start = iWidth/2.0-wid/2.0+m*tool_margin/machineWidth*iWidth;
//        else
//            x_start = iWidth/2.0-wid/2.0+(m*tool_margin+toolWidth[m-1])/machineWidth*iWidth;
//        float x_end = x_start+toolWidth[m]/machineWidth*iWidth;
//        pen = QPen(Qt::black,1,Qt::SolidLine);
//        painter.setPen(pen);
//        painter.drawLine(QPointF(x_start,iHeight/8.0),QPointF(x_start,iHeight*7/8.0));
//        painter.drawLine(QPointF(x_end,iHeight/8.0),QPointF(x_end,iHeight*7/8.0));
//        //绘制下模模型
//        brush = QBrush(Qt::blue);
//        painter.setBrush(brush);
//        int num3 = 4;
//        QPointF point3[num3];
//        point3[0] = QPointF(x_start,iHeight*7/8.0);
//        point3[1] = QPointF(x_end,iHeight*7/8.0);
//        point3[2] = QPointF(x_end,iHeight*6/8.0);
//        point3[3] = QPointF(x_start,iHeight*6/8.0);
//        painter.drawPolygon(point3,num3);
//        //绘制上模模型
//    //    brush = QBrush(Qt::green);
//    //    painter.setBrush(brush);
//        int num4 = 6;
//        QPointF point4[num4];
//        float indent = toolWidth[m]*iWidth/(6.0*machineWidth);
//        point4[0] = QPointF(x_start+indent,iHeight/8.0);
//        point4[1] = QPointF(x_start+indent,iHeight/5.0);
//        point4[2] = QPointF(x_start,iHeight/4.0);
//        point4[3] = QPointF(x_end,iHeight/4.0);
//        point4[4] = QPointF(x_end-indent,iHeight/5.0);
//        point4[5] = QPointF(x_end-indent,iHeight/8.0);
//        painter.drawPolygon(point4,num4);
//    }
    painter.end();
//      toolWidth.clear();
//      toolPosition.clear();
    return pixMap;
}
