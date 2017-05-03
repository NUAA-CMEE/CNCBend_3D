#ifndef SYSGLDATA_H
#define SYSGLDATA_H
#include <QString>

#define  MALLOC(n,type)  ((type*)malloc((n)*sizeof(type)))
#define  EPSINON  1e-1  //这个数字很重要，需要判断斜率,当法线中的xyz小于0.1时就认为是0
#define  PI       3.1415926
#define  Kr    0.15625   //折弯系数

#define PAGE_WIDTH      630
#define PAGE_HEIGHT     900
#define PAGE_X_MARGIN   10.0
#define PAGE_Y_MARGIN   10.0
#define CIRCLE_RADIUS   20.0


//图形变换的相关操作
enum CONVERT {CONVERT_HORIZONTAL,CONVERT_VERTICAL,CONVERT_X,CONVERT_Y,CONVERT_Z,CONVERT_ZOOM};
enum MPART {BODY,UPPER,LOWER,PUNCH,DIE,BACK_GAUNG_XL,BACK_GAUNG_RL,BACK_GAUNG_ZL,\
            BACK_GAUNG_XR,BACK_GAUNG_RR,BACK_GAUNG_ZR,WORK_PIECE};
enum AXIS {Y_AXIS,PUNCH_AXIS,DIE_AXIS,X1_AXIS,X2_AXIS,X1_AXIS_BACK,X2_AXIS_BACK,R1_AXIS,R2_AXIS,Z1_AXIS,Z2_AXIS};

#pragma pack(4)     //该编译器默认的字节对齐是8,为了和控制系统保持一致,修改默认方式为4,否则传输的文件会有问题
typedef struct Point3F  //表示空间坐标点
{
    float x;
    float y;
    float z;
}point3f;

typedef struct xform_124//变换矩阵，类型为124
{
    int typeNum;
    float R11,R12,R13,T1;
    float R21,R22,R23,T2;
    float R31,R32,R33,T3;
}xform_type;

//圆弧由两个端点及弧的一个中心确定，该圆弧始点在先，
//终点在后，并以逆时针方向画出圆弧
typedef struct arc_100  //圆弧实体
{
    int typeNum;       //实体类型号
    float ZT;          //ZT为XT，YT平面上的圆弧平行于ZT的位移量
    float X1,Y1;       //圆弧中心坐标
    float X2,Y2;       //圆弧起点坐标
    float X3,Y3;       //圆弧终点坐标
    xform_type pForm;  //变换矩阵
}arc_type;

typedef struct composite_curve_102//组合曲线实体
{
    int typeNum;        //实体类型号
    int curveCount;     //曲线的个数
    int curvePos[50];   //数组中保存曲线实体在D（索引）区的位置
}composite_curve_type;

//第一点为起点P1（X1，Y1，Z1），第二点为终点P2（X2，Y2，Z2）
typedef struct line_110 //直线实体，类型为110
{
    int typeNum;        //实体类型号
    point3f start;
    point3f end;
    bool isVisited;
    bool isBendBound;   //区分是否是折弯边界线
}line_type;

typedef struct revolution_120//旋转曲面实体,类型号为120
{
    int typeNum;        //实体类型号
    int axis;           //指向旋转轴D（索引）区的位置
    int generatrix;     //指向母线D（索引）区的位置
    float start_angle;  //起点角度
    float end_angle;    //终点角度
}revolution_type;

typedef struct nurbsCurve_126//B样条曲线，类型为126
{
    int typeNum;        //实体类型号
    int K_num;          //B样条曲线次数
    int M_num;          //基函数次数
    int prop_plane;     //是否为平面，0为非平面，1为平面
    int prop_closed;    //是否为封闭曲线，0为开放，1为封闭
    int prop_poly;      //是否为多项式，0为有理式，1为多项式
    int prop_periodic;  //是否为周期性，0为非周期，1为周期
    float *pKnot;       //结点序列数组
    float *pWeight;     //权重值数组
    float *pCtlarray;   //控制点数组
    float V0;           //起始值参数
    float V1;           //终止值参数
    float xnorm;        //单位法向
    float ynorm;
    float znorm;
    bool isVisited;
    bool isBendBound;   //区分是否是折弯边界线
}nurbsCurve_type;

typedef struct nurbsSurface_128//B样条曲面，类型为128
{
    int typeNum;        //实体类型号
    int K1_num;         //B样条曲线1次数
    int K2_num;         //B样条曲线2次数
    int M1_num;         //基函数1次数
    int M2_num;         //基函数2次数
    int prop1_closed;   //第一个参数方向是否为封闭曲线，0为开放，1为封闭
    int prop2_closed;   //第二个参数方向是否为封闭曲线，0为开放，1为封闭
    int prop_poly;      //是否为多项式，0为有理式，1为多项式
    int prop1_periodic; //第一个参数方向是否为周期性，0为非周期，1为周期
    int prop2_periodic; //第二个参数方向是否为周期性，0为非周期，1为周期
    float *pKnot1;      //第一个结点序列数组
    float *pKnot2;      //第二个结点序列数组
    float *pWeight;     //权重值数组
    float *pCtlarray;   //控制点数组
    float U0;           //U方向起始值参数
    float U1;           //U方向终止值参数
    float V0;           //V方向起始值参数
    float V1;           //V方向终止值参数
}nurbsSurface_type;

typedef struct para_surface_142//参数曲面上的曲线，类型为142
{
    int typeNum;        //实体类型号
    int curve_way;      //已经创建的曲面上的曲线的形式,0=无说明，1=给定曲线在曲面上的投影，2=两个曲面的交线，3=等参数曲线
    int pt_surface;     //指向裁剪曲线所在的曲面的D区的位置
    int pt_curve;       //指向裁剪参数曲线实体在D区的位置
    int cptr;           //指向曲线C在D区的位置,(曲线C是指曲线B映射到欧几里德三维空间后的曲线，一般很少用)
    int pref;           //在发送系统中的缺省呈现，0=无规定，1=SoB被推荐，2=C被推荐，3=C和SoB同等地被推荐
}para_surface_type;

typedef struct trimmed_surface_144//裁剪曲面，类型为144
{
    int typeNum;        //实体类型号
    bool bBendSurface;  //是否是折弯面
    bool angleFlag;     //该参数用于判断折弯角度的正负,从旋转曲面的数据中读得,angleFlag=false则角度为负
    line_type axis;     //如果是旋转曲面,获取旋转轴
    line_type generatrix;//母线
    float start_angle;
    float end_angle;
    float angle;        //折弯面的折弯角度
    int pt_surface;     //指向曲面实体在D（索引）区的位置
    int boundary;       //0为外边界，1为其他，如果为0，则说明这个曲面在其外边界处没有进行裁剪
    int curve_num;      //简单封闭曲线的个数
    int pt_para;        //指向参数去曲面上的曲线在D（索引）区的位置
    int pt_curves[30];  //指向简单封闭曲线在D（索引）区的位置
    //指向基曲面的指针，一般包括128(有理B样条曲面)实体，122(列表柱面)实体，120(旋转面)实体
    nurbsSurface_type *pSurface;
    int surfaceType;    //该基曲面的实体类型
    void *pCurves[30][20];  //指向边界曲线的二维指针数组
    int curveType[30][20];  //边界线的曲线实体类型
    int curveCount[30];     //边界线的曲线的个数

    //此处添加曲面的轮廓线
    void *pOutlines[30][20];//轮廓线
    int outlineType[30][20];
    int outlineCount[30];
    struct trimmed_surface_144 *pNext;//指向下一个实体的指针
}trimmed_surface_type;

typedef struct GLData
{
   int typeNum;        //实体类型号
   int type_pos;       //该记录所在的行数标号
   int data_pos;       //数据存放位置标号
   int xform_pos;      //变换矩阵的位置标号,0为没有变换矩阵
   int state;          //状态号，和2D，3D有关
   int data_row;       //数据行数
   void *pData;        //指向数据区域的指针
   GLData *pBefore;  //前向指针
   GLData *pNext;    //后向指针
   int useCount;       //使用次数
}gl_data;

////工件数据结构比较复杂,需要提取的数据比较多
//typedef struct Surface //平面
//{
//    point3f point1;
//    point3f point2;
//    point3f point3;
//    point3f point4;
//}stu_surface;

typedef struct Polygon  //多边形
{
    int point_num;          //顶点个数
    QVector<point3f> point; //多边形的顶点
}stu_polygon;

typedef struct bendSurface bend_surface;  //折弯面面实体
typedef struct basicSurface //基面
{
    bool isVisited;          //是否被访问过，0为未访问，1为访问过
    bool isTop;              //区分平面是上面还是下面
    trimmed_surface_type *pSurface;//基面的裁剪曲面(基本上都是128B样条曲面)以及轮廓曲线
    float width;            //该基面的宽度
    float length;           //该基面的长度
    QVector<trimmed_surface_type *>pBendSurface;//与该基面相连的折弯面
    //QVector<stu_polygon *> pSideSurface;
    QVector<trimmed_surface_type *> pSideSurface;
}basic_surface;

typedef struct bendSurface  //折弯面面实体
{
    bool isTop;                 //是否是上面
    bool isVisited;             //是否已被访问
    int bendNum;                //折弯点序号
    int isBended;               //是否已经折弯,0为未折弯,1为已经折弯,该参数决定绘制折弯面还是展开面
    float bendRadius;           //折弯半径
    float bendAngle;            //折弯角度(弧度表示)
    float deployLength;         //折弯面展开后的长度
    float bendWidth;            //折弯宽度
    line_type axis;             //旋转轴
    line_type generatrix;        //母线
    xform_type *pMatrix;        //正向变换矩阵,求得折弯位置
    xform_type *pInverse;       //逆矩阵,求得展开位置
    xform_type *pLeftMatrix;
    xform_type *pRightMatrix;
    xform_type *pLeftInverse;
    xform_type *pRightInverse;
    line_type *pBendLine;       //折弯线段
    nurbsSurface_type *pBendSurface;
    trimmed_surface_type *pSurface;//基面的裁剪曲面(旋转曲面)以及轮廓曲线
    stu_polygon *pDeploy;    //指向展开平面
    stu_polygon *pSideFont;    //指向展开时的侧面
    stu_polygon *pSideBack;
    nurbsSurface_type *pBendSideFont;//指向折弯时的侧面
    nurbsSurface_type *pBendSideBack;
    basic_surface *pLeftBase;   //指向其中一个基面
    basic_surface *pRightBase;  //指向另外一个基面
    bendSurface *pNext;         //指向下一个结点
    bendSurface *pParallel;     //指向与之平行的结点
}bend_surface;

typedef struct  BendOrder
{
    int bend_id;             //折弯计数
    int bendpoint;           //折弯点
    int backgauge_point;     //档料点
    int tool_id;            //刀具组序号
    float bendAngle;
    float bendWidth;
    float bendRadius;
    //图形显示用档料位置
    float gauge_position_x;  //档料点的X坐标
    float gauge_position_y;  //档料点的Y坐标
    //工艺用档料位置参数
    float position_y1;       //滑块Y1坐标
    float position_y2;       //滑块Y2坐标
    float position_x1;       //档料点的X1坐标
    float position_x2;       //档料点的X2坐标
    float position_r1;       //档料点的R1坐标
    float position_r2;       //档料点的R2坐标
    float position_z1;       //档料点的Z1坐标
    float position_z2;       //档料点的Z2坐标
    float dX1Retract;         //挡料1退让距离
    float dX2Retract;         //挡料2退让距离
    float y_open;            //Y轴开口高度
    int interefere_tag;      //干涉标志2(方向2不干涉），1（方向1不干涉），0（两个方向都不干涉），其他（干涉）
    int direction_h;         //前后掉头状态，1或-1
    int direction_v;         //上下翻转状态，1或-1
    struct BendOrder *pBefore;
    struct BendOrder *pNext;
}bend_order;

typedef struct  toolInfo
{
   int punch_id;
   float punch_width;
   float punch_pos;
   int die_id;
   float die_width;
   float die_pos;
}tool_info;

//====================两个数据结构==================
typedef struct FileHeadType //文件头，每个工艺或图形文件均有
{
    unsigned int fileid;    //程序号
    char name[20];          //图示号
    int fBendNo;            //折弯总数
    //int fBendNoAll;         //所有折弯段数(包括圆弧的折弯数)
    char fType[20];         //文件类型
    int fPunch;             //上模
    int fDie;               //下模
    //char fFilePath[100];  //文件路径
}MFileType;
//数据编程之前的几个参数
typedef struct stu_dataEdit_structure_before
{
    int iAngleSel;				//角度选择
    double dThickness;		    //厚度
    int iMaterial;				//材料类型
    double dBlackLength;	    //材料宽度
    double dLength;             //和折弯数据中的宽度相同,用于图形编程
    int link;                   //连接，数字变成用
    int sizeSel;                //尺寸选择,图形编程用
}stu_beforeBend;

typedef struct  DataEdit_structure
{
    //bend_order pstu_bend;
    unsigned int uiBendNum;
    //====================================
    //Axis function轴在当前工步的速度占最大速度的百分比
    double dVX_percent;
    double dVR_percent;
    //====================================
    //gauge function
    unsigned int uiGaugePos;    //挡料位置，即板料放在第几个台阶上定位
    double dRfingerPosition;	//与板料接触的挡指位置沿R轴方向的高度
    double dXfingerPosition;	//与板料接触的挡指位置沿X轴方向的高度
    //******page 1*************
    //bend
    unsigned int uiPunch;       //上模
    unsigned int uiDie;         //下模
    unsigned int iMethod;       //折弯方式
    double dLength;             //宽度
    //angle
    double dAngleOrBendPosition;//角度或折弯位置，取决于Y轴的编程方式，折弯位置的确定有两种方法,Y1位置
    double dPositionY2;         //Y2位置
    double dOpeningY;           //开口高度，折弯后，上下模之间的高度，相对于速度转换点
    double dMutePosition;       //速度转换点
    //辅助轴
    double dXStartPosition;		//开始位置，为了定位，
    double dXRetract;			//X轴在当前工步的退让距离
    double dRStartPosition;		//R轴位置
    double dCptCrown;           //挠度补偿

    //********page 2***********
    double dParallism;			//平行度，左右油缸之间的差值，此参数在加紧点以下有效
    unsigned int uiCode;		//折弯结束后，下一道折弯参数何时载入内存执行，即何时换步
    double delaytime;			//换步之前的延时时间
    int uiReputition;           //换步之前的循环次数
    double dForce;				//压制时的吨位	//系统自动计算
    double dwellTime;			//滑块在折弯点的保压时间
    double decoppression;		//卸压距离//Decompression stroke after bending to release the working pressure
    double dSpeed;				//滑块工进时的速度
    double decompSpeed;			//卸压速度
    double dAngleNumber;		//该工步是从左到右计数的第几个折弯工步
    double dClamp_Pos;          //夹紧点的绝对位置
    unsigned int uiBackgaugeNum;//板料定位时与后挡料接触的位置，该位置是板料上从左到右，从零开始计数的可定位位置
    unsigned int uiMachineUpperSide;	//机床的上部外形
    unsigned int uiMachineLowerSide;	 //机床的下部外形

    //补偿数据
    unsigned int workpiece_count;   //工件计数
    double dSingleCptAngle1;        //单步角度补偿1
    double dSingleCptAngle2;        //单步角度补偿2
    double dOverallCptAngle;        //全局角度补偿
    double dSingleCptX1;            //X1轴的单步补偿
    double dSingleCptX2;            //X2轴的单步补偿
    double dOverallCptX;            //X1和X2轴的全局补偿
    double dSingleCptR;             //R轴的单步补偿
    double dSingleCptCrown;         //挠度轴的单步校正


    /*
    注意，
    1，如果对上面的4个参数进行了正确的设定，
        就可以在自动模式的视图显示功能中
        对工件的折弯情况进行模拟只需要输入图形所需参数和板料的长度即可；
    2，如果选择继续设定新折弯的参数，
        系统所显示的新折弯的相关参数是前一折弯的拷贝，
        只需要将被折弯与前一折弯不同的参数进行适当修改即可
    */
    /*辅助轴---不知道怎么控制
    double dRaxis;
    double dZ1axis;
    double dZ2axis
    unsigned int uiPartSupprot;
    unsigned int uiOnOff;
    */
    struct  DataEdit_structure *pBefore;
    struct  DataEdit_structure *pNext;
}stu_BendData;

//机床状态结构体，实时向上位机发送
typedef struct stu_manualData
{
    volatile long command;
    long Bend_Fin;      //
    long C_Start;       // 0:已停止 1:已启动 2:正在回参
    long Work_State;
    long Ref_State;
    long Y_State;		// 0:初始 1:快下 2:MUTE 3:工进 4:保压 5:卸荷 6:快上 7:工步结束
    long Y1_data;
    long Y2_data;
    long X1_data;
    long R_data;
    long X_Des;
    long Err_State;
    long Internal_Warning_State;//内部报警标志位，用于未开油泵报警等
    long Pos_Fixed;             //定位结束判断位

    long plc_input;
    long plc_output;
    long plc_M;
    long plc_Warning;
    long plc_C;
    long plc_T;
    long Y_Manual_Flag;     //手动移动轴中显示调整的标志位
    long Diag_HardWare;

    //long Pump_State;
}stu_stateData;

typedef struct data{
    double y;
    double x;
}stu_data;

#endif // SYSGLDATA_H
