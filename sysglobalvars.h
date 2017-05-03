#ifndef SYSGLOBALVARS_H
#define SYSGLOBALVARS_H
#include <QString>
#include <QtNetwork/QTcpSocket>
#include <QThread>
#include "recordClient.h"
#include "SysGLData.h"
//#include "bendglwidget.h"

extern QString dbPath;
extern QString filePath;
extern QString machinePath;
extern QString select_filepath;
extern QString savePath;
extern QString picPath;

extern float Y_factor;  //计算折弯展开长度会用到,根据材料的不同设定不同的值
extern float machineWidth;
extern QVector<float> toolWidth;
extern QVector<float> toolPosition;
extern QVector<int> toolPunch;
extern QVector<int> toolDie;
extern QVector<float> toolMargin;
extern float tool_margin;
extern float dieHeight;
extern float die_width;
extern float workpiece_thickness;

extern QTcpSocket *tcpClient;
extern RecordClient *mClient;
extern QThread *clientThread;
extern bool bLinked;    //网络是否连接标志位
extern QString strHost;

extern pthread_mutex_t dataAreaMutex;
extern stu_stateData stateData;

//模具信息配置的相关参数
extern int punch_id;
extern int die_id;
extern float tool_width;
extern float tool_position;
extern float margin;


extern float circle_radius;

extern float over_ratio;  //翻转系数
extern float return_ratio;//调头系数
extern float tool_ratio;  //模具更换系数

extern float y_max;     //Y轴最大值
extern float y_min;     //Y轴最小值
extern float x_max;     //X轴最大值
extern float x_min;     //X轴最小值
extern float r_max;     //R轴最大值
extern float r_min;     //R轴最小值
extern float z_max;     //Z轴最大值
extern float z_min;     //Z轴最小值
extern bool x1_enable;
extern bool x2_enable;
extern bool r1_enable;
extern bool r2_enable;
extern bool z1_enable;
extern bool z2_enable;

extern bool bManualState;   //是否显示展开图的折弯序号
extern QList<int> ManualOrder; //手动调整工序
extern QList<int> ManualTag; //手动调整调头标志位
extern QList<int> ManualTool;//手动调整模具选择

extern QVector<bend_surface *> pBend_Backup;
extern QVector<bend_order *> pOrder_Backup;

extern bool character_flag;
extern float zoom;
#endif // SYSGLOBALVARS_H
