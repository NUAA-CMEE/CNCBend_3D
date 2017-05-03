#include "sysglobalvars.h"

//QString dbPath = "D:/QT-WIN-PROJECT/CNCBend_3D/file/database/";
//QString filePath = "D:/QT-WIN-PROJECT/CNCBend_3D/file/origin/";
//QString machinePath = "D:/QT-WIN-PROJECT/CNCBend_3D/file/origin/machine/";
//QString savePath = "D:/QT-WIN-PROJECT/";
QString dbPath = "./file/database/";
QString filePath = "./file/origin/";
QString machinePath = "./file/origin/machine/";
QString savePath = "./file/";
QString select_filepath;
QString picPath = "./file/print/";

float Y_factor = 0.5;   //pro/e中默认的y因子的值,可以更具不同的材料进行修改
float machineWidth = 4200.0;
QVector<float> toolWidth;
QVector<float> toolPosition;
QVector<int> toolPunch;
QVector<int> toolDie;
QVector<float> toolMargin;
float tool_margin = 300.0;
float dieHeight = 86.0;
float die_width = 16.0;
float workpiece_thickness = 0.0;

QTcpSocket *tcpClient = NULL;
RecordClient *mClient = NULL;
QThread *clientThread = NULL;
bool bLinked = false;
//QString strHost = "120.27.35.52";//服务器ip地址
QString strHost = "192.168.0.100";//服务器ip地址
//QString strHost = "192.168.1.113";//服务器ip地址

pthread_mutex_t dataAreaMutex;   //数据区互斥体
stu_stateData stateData;

//模具信息配置的相关参数
int punch_id = 1;
int die_id = 1;
float tool_width = 200.0;
float tool_position = 0.0;
float margin = 300.0;

float circle_radius = 1500;//1500;

float over_ratio = 0.0;  //翻转系数
float return_ratio = 0.0;//调头系数
float tool_ratio = 0.0;  //模具更换系数

float y_max = 0.00;     //Y轴最大值
float y_min = 0.00;     //Y轴最小值
float x_max = 0.00;     //X轴最大值
float x_min = 0.00;     //X轴最小值
float r_max = 0.00;     //R轴最大值
float r_min = 0.00;     //R轴最小值
float z_max = 0.00;     //Z轴最大值
float z_min = 0.00;     //Z轴最小值
bool x1_enable;
bool x2_enable;
bool r1_enable;
bool r2_enable;
bool z1_enable;
bool z2_enable;
bool character_flag = false;

bool bManualState = false;
QList<int> ManualOrder;
QList<int> ManualTag;
QList<int> ManualTool;
QVector<bend_surface *> pBend_Backup;
QVector<bend_order *> pOrder_Backup;
float zoom = 1.0;
