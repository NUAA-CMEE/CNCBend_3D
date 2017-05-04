#ifndef FILEOPERATE_H
#define FILEOPERATE_H

//作为模型管理器,对外提供对模型的数据进行操作,将数据模型和显示分开
//该类使用单例模式

#include "filereader.h"
//#include <QTimer>
#include <QtOpenGL/QtOpenGL>
#include <QtPrintSupport>
//#include <GL/glu.h>
#include <gl/glu.h>
#include <QtOpenGL>
#include <gl/gl.h>
#include <time.h>

#define T 20
#define M 10

#define MODEL_CHANGE  200
#define TURN_ABOUT    50
#define UPSIDE_DOWN   30
#define OPERATE_RATE  100


class fileOperate : public fileReader
{
public:
    static fileOperate *getInstance();
    trimmed_surface_type *getFileModel(QString path, MPART part);
    bend_surface *getWorkPieceModel(QString path, bool bDeploy);
    bend_surface *getBendHead();
    bend_order *getBendOrder(bend_surface *pHead = NULL);
    void formAxisData(AXIS axis, float pos);
    bend_order *getCurOrder();
    bend_surface *getDeployHead();
    void showOriginPiece();
    void initBendAxisData();
    void formBendFile(QString bendPath);
    void drawDataTable(QPainter *painter);
    void turnAboutPiece(bend_order *pcur);
    int getBendNum();
    float getThickness();
    void clearToolData();
    void initWorkSpacePos();
    void initToolPos(MPART part,point3f off);
    int bendorderResult(QWidget *parent);
    QVector<int>  drawBendAction();
    QVector<int>  drawBendAction(bend_order *porder);
    void drawToBefore(int sourceNum, int destNum);
    void convertToPoint(int sourceRow, int destRow);
    void resetWorkPos();
    QVector<int> drawToMachine(int bendnum);
    QVector<int> drawToBend(int bendnum);
    QVector<int> drawToDeploy(int bendnum);
    void drawDeployAction();
    bool getBendedState();
    void destroyWorkPiece();
    void destroyClass();
    void formBendorderFile();
    bend_order *findBendOrder(int bendnum, bend_order *pHead);
    point3f findBasicData(bool);
    point3f findBackBasicData(bool isbendDataFlow);
    void adjustWorkpiece(xform_type rotateMatrix, bool flag);
    float findBsicSurfaceMaxLength();

    void ManualCancel(int index);
    void ManualSure(int index);
    void resetToDeploy();
    void resetToBend();
    void bendorder_toolconfig();
    void ModelChange(bend_order *pCur,int num);

    void converToNum(int num);
    void deploy_num(int num);


private:
    void offToolData(trimmed_surface_type *pTrimmed, point3f off);
    void offToolData(trimmed_surface_type *pTrimmed, float x,float y,float z);
    void convertToBendPos(bend_order *pOrder,bend_surface *pHead);
    void formPuchBendData(float depth);
    int judgeInterFere(int bendnum);
    int judgeInterFere_inherit(int bendnum, bend_surface *pbend_head, bend_order *porder_head);
    void recoverTo(QVector<int> pos, QVector<int> tag, int num);
    int computeOptimumSolution(QWidget *parent,int **order, int **tag, int sum, int bendnum);
    void formBendOrder(int res[],int tag[]);
    void formBendOrder(QList<int> res, QList<int> tag, QList<int> tool);

    void computeBackGaungPos(bend_order *pOrder, bendSurface *pHead);
    void updateBackGaungPos(bend_order *pCur);
    void destroyBendData(bend_surface *pHead);

    bend_order *readBendOrderFile(QString name);
    bend_order *formData(FILE *fp, int id, bend_order *pCur, bool bDiv, int link = 0);
    QVector<int> getDivNum();

    //遗传算法
    void convertBackUpToBend(int num);
    int computeSuit(QVector<int> order, int index);
    QVector<QVector<int> > pick(QVector<QVector<int> > population);
    void cross(QVector<QVector<int> > &population, QVector<QVector<int> > &inter);
    void mutation();
    void deploy_inherit(QVector<int> order, int index, int bendnum);
    void bend_inherit(QVector<int> order, int index,int bend_id);

public:
    //绘制上下模具和机床本体
    void updateVisibleState(bool bBody, bool bPunch, bool bDie, bool bX, bool bR, bool bZ);
    void drawMachine();
    void drawEntity(trimmed_surface_type *pTrimmed, float color[], bool flag = false);
    //绘制工件
    void drawWorkPiece();
    //凹多边形分格化
    inline GLUtesselator *tessor();
    inline void drawBaseSurface(trimmed_surface_type *pTmpCur);

private:
    fileOperate();
    static fileOperate *m_Instance;
public:
    float X1_Pos;        //X轴当前所在位置
    float X1_Retract;    //X轴退让距离
    float X2_Pos;        //X轴当前所在位置
    float X2_Retract;    //X轴退让距离
    float Y_Pos;        //y轴当前所在位置
    float R1_Pos;        //r轴当前所在位置
    float R2_Pos;        //r轴当前所在位置
    float Z1_Pos;        //z轴当前所在位置
    float Z2_Pos;        //z轴当前所在位置
    bool bendFlag;      //区分是折弯还是回程

private:
    bool bShow_body;
    bool bShow_punch;
    bool bShow_die;
    bool bShow_X;
    bool bShow_R;
    bool bShow_Z;
    GLUnurbsObj *pNurb;
    gl_data *pGL_Machine;
    gl_data *pGL_Lower;
    gl_data *pGL_Upper;
    gl_data *pGL_Gaung_XL;
    gl_data *pGL_Gaung_XR;
    gl_data *pGL_Gaung_RL;
    gl_data *pGL_Gaung_RR;
    gl_data *pGL_Gaung_ZL;
    gl_data *pGL_Gaung_ZR;
    gl_data *pGL_WorkPiece;
    gl_data *pGL_WorkPiece_Deploy;
    QVector<gl_data *> pGL_Punch;
    QVector<gl_data *> pGL_Die;

    trimmed_surface_type *pMachine;  //机床本体指针
    trimmed_surface_type *pLower;    //机床下部外形指针
    trimmed_surface_type *pUpper;    //上滑块指针
    QVector<trimmed_surface_type *>pPunch;//上模指针
    QVector<trimmed_surface_type *>pDie;//下模指针
    trimmed_surface_type *pGaung_XL;//后挡料X轴指针(左侧)
    trimmed_surface_type *pGaung_ZL;//后挡料Z轴指针(左侧)
    trimmed_surface_type *pGaung_RL;//后挡料R轴指针(左侧)
    trimmed_surface_type *pGaung_XR;//后挡料X轴指针(右侧)
    trimmed_surface_type *pGaung_ZR;//后挡料Z轴指针(右侧)
    trimmed_surface_type *pGaung_RR;//后挡料R轴指针(右侧)
    trimmed_surface_type *pWorkPiece;//工件指针
    trimmed_surface_type *pWorkPiece_Deploy;//工件指针

    bend_surface *pBendHead;

    bend_surface *pDeployHead;

    bend_order *pOrderHead;
    bend_order *pOrderCur;
    bend_order *pOrderLast;

    float Y_open;               //y轴开口高度
    QVector<float>  tool_x_off; //模具X轴方向的偏移

    bool bRun;          //表示界面是否在仿真运动中
    //QTimer *pTimer;
    float bendangle;

public slots:
//    void timeOutSlot();
};

#endif // FILEOPERATE_H
