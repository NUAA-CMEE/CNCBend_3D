#ifndef BENDGLWIDGET_H
#define BENDGLWIDGET_H

#include <QtOpenGL/QtOpenGL>
#include <QtPrintSupport>
//#include <GL/glu.h>
#include <gl/glu.h>
#include <QtOpenGL>
#include <gl/gl.h>
//#include <qopenglfunctions.h>
//#include <glut>
#include "SysGLData.h"
#include <png.h>
#include <stdlib.h>
#include <string.h>
#include "fileoperate.h"

#define ACCURACY_NUM    50
#define MAX_SIZE        4096
#define RECT_SIZE       15.0
#define MAX_RESULT      30      //折弯工序规划最多取30个结果
#define TIME_INTERNAL   10     //单位为毫秒

class bendGLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit bendGLWidget(QWidget *parent = 0);
    ~bendGLWidget();
    void resizeWindow(int width,int height);
    void rotateGraph(float xrote,float yrote,float zrote);
    void convertGraph(float para,CONVERT convert);
    void resetGraph();
    void setShowModel(fileOperate *pData);
    void formBendImage(QString strName);
    void setPrintMode(bool flag,int num);

    void sideView();
    void anti_sideView();
    void mainView();
    void anti_mainview();
    void topView();
    void anti_topView();
    void axisSide();
    void anti_axisside();
    void axisBesideView(point3f a );
    bend_surface* adjustDeployWorkpiece(fileOperate *pData);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

private:
    float xRote,yRote,zRote;
    float vMove,hMove,zMove;
    bool isAxisBeside;
    bool isFirstLoad;
public:
    GLfloat ex,ey,ez,centerx,centery,centerz,upx,upy,upz;
    GLfloat ex1,ey1,ez1,centerx1,centery1,centerz1,upx1,upy1,upz1;
    fileOperate *pDrawData;

    //板料的相关信息
private:
    bool printFlag;     //区分是否打印当前折弯仿真画面
    int curNum;
    QString printName;  //打印图片的名称
    void drawScene();

};

#endif // BENDGLWIDGET_H
