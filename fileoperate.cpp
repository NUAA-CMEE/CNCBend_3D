#include "fileoperate.h"
#include "progressdlg.h"

GLfloat trimmed_data[5][2] = {{0.98,1.0},{0.02,1.0},{0.02,0.0},{0.98,0.0},{0.98,1.0}};


//颜色列表
GLfloat machineColor[4] = {0.1,0.1,0.1,1.0};    //机床本体颜色
GLfloat lowerColor[4] = {0.1,0.1,0.1,1.0};    //下部外形颜色
GLfloat upperColor[4] = {1.0,0.0,0.0,1.0};      //滑块颜色
GLfloat punchColor[4] = {0.4,0.0,1.0,1.0};   //上模颜色
GLfloat dieColor[4] = {0.4,0.0,1.0,1.0};     //下模颜色
GLfloat SelectColor[4] = {1.0,0.0,0.0,1.0};     //下模颜色

GLfloat backgaung_rColor[4] = {0.2,0.8,0.2,1.0};  //后挡料颜色
GLfloat backgaung_xColor[4] = {0.8,0.8,0.5,1.0};  //后挡料颜色
GLfloat backgaung_zColor[4] = {0.3,0.2,0.3,1.0};  //后挡料颜色
GLfloat workpieceColor[4] = {1.0,0.6,0.2,1.0};  //工件颜色
GLfloat bac_workpieceColor[4] = {0.0,0.75,1.0,1.0};  //工件颜色
GLfloat bendColor[3] = {0.8,0.0,0.0};//折弯颜色

void CALLBACK vertexCallback(GLvoid *vertex)
{
    const GLdouble *pointer = (GLdouble *)vertex;
    glColor4fv(workpieceColor);
    glVertex3dv(pointer);
}

void CALLBACK beginCallback(GLenum which)
{
    glBegin(which);
}

void CALLBACK endCallback()
{
    glEnd();
}

fileOperate *fileOperate::m_Instance = NULL;
fileOperate *fileOperate::getInstance()
{
    if(m_Instance == NULL)
    {
        m_Instance = new fileOperate();
//        bShow_body = false;
//        bShow_punch = false;
//        bShow_die = false;
//        bShow_X = false;
//        bShow_R = false;
//        bShow_Z = false;
    }
    return m_Instance;
}


fileOperate::fileOperate() :
    fileReader()
{
    pGL_Machine = NULL;
    pGL_Lower = NULL;
    pGL_Upper = NULL;
    pGL_Gaung_XL = NULL;
    pGL_Gaung_XR = NULL;
    pGL_Gaung_RL = NULL;
    pGL_Gaung_RR = NULL;
    pGL_Gaung_ZL = NULL;
    pGL_Gaung_ZR = NULL;
    pGL_WorkPiece = NULL;
    pGL_WorkPiece_Deploy = NULL;
    pMachine = NULL;
    pLower = NULL;
    pUpper = NULL;
    pGaung_XL = NULL;
    pGaung_RL = NULL;
    pGaung_ZL = NULL;
    pGaung_XR = NULL;
    pGaung_RR = NULL;
    pGaung_ZR = NULL;
    pWorkPiece = NULL;
    pWorkPiece_Deploy = NULL;
    pBendHead = NULL;
    pDeployHead = NULL;
    pOrderHead = NULL;
    pOrderCur = NULL;
    pOrderLast = NULL;

    Y_open = 100;
    X1_Pos = 0.0;
    X1_Retract = 0.0;
    X2_Pos = 0.0;
    X2_Retract = 0.0;
    Y_Pos = 0.0;
    R1_Pos = 0.0;
    R2_Pos = 0.0;
    Z1_Pos = 0.0;
    Z2_Pos = 0.0;
    bRun = false;
    bendFlag = false;
    bendangle = PI;
    qsrand(time(NULL));
}

/********************************************
 *function:获取机床各部分的模型指针
 *adding:
 *author: xu
 *date: 2016/06/18
 *******************************************/
trimmed_surface_type *fileOperate::getFileModel(QString path,MPART part)
{
    trimmed_surface_type *pResult = NULL;
    switch (part) {
    case BODY:
    {
        if(pMachine)
        {
            destroyTrimmed(pMachine);
            destroyEntity(pGL_Machine);
            pMachine = NULL;
        }
        pGL_Machine = readOriginFile(path);
        pMachine = transform_Brep(pGL_Machine);
        pResult = pMachine;
        break;
    }
    case UPPER:
    {
        if(pUpper)
        {
            destroyTrimmed(pUpper);
            destroyEntity(pGL_Upper);
            pUpper = NULL;
        }
        pGL_Upper = readOriginFile(path);
        pUpper = transform_Brep(pGL_Upper);
        pResult = pUpper;
        break;
    }
    case LOWER:
    {
        if(pLower)
        {
            destroyTrimmed(pLower);
            destroyEntity(pGL_Lower);
            pLower = NULL;
        }
        pGL_Lower = readOriginFile(path);
        pLower = transform_Brep(pGL_Lower);
        pResult = pLower;
        break;
    }
    case PUNCH:
    {
        gl_data *pgldata = readOriginFile(path);
        pGL_Punch.push_back(pgldata);
        pResult = transform_Brep(pgldata);
        pPunch.push_back(pResult);
        break;
    }
    case DIE:
    {
        gl_data *pgldata = readOriginFile(path);
        pGL_Die.push_back(pgldata);
        pResult = transform_Brep(pgldata);
        pDie.push_back(pResult);
        break;
    }
    case BACK_GAUNG_XL:
    {
        if(pGaung_XL)
        {
            destroyTrimmed(pGaung_XL);
            destroyEntity(pGL_Gaung_XL);
            pGaung_XL = NULL;
        }
        pGL_Gaung_XL = readOriginFile(path);
        pGaung_XL = transform_Brep(pGL_Gaung_XL);
        pResult = pGaung_XL;
        break;
    }
    case BACK_GAUNG_RL:
    {
        if(pGaung_RL)
        {
            destroyTrimmed(pGaung_RL);
            destroyEntity(pGL_Gaung_RL);
            pGaung_RL = NULL;
        }
        pGL_Gaung_RL = readOriginFile(path);
        pGaung_RL = transform_Brep(pGL_Gaung_RL);
        pResult = pGaung_RL;
        break;
    }
    case BACK_GAUNG_ZL:
    {
        if(pGaung_ZL)
        {
            destroyTrimmed(pGaung_ZL);
            destroyEntity(pGL_Gaung_ZL);
            pGaung_ZL = NULL;
        }
        pGL_Gaung_ZL = readOriginFile(path);
        pGaung_ZL = transform_Brep(pGL_Gaung_ZL);
        pResult = pGaung_ZL;
        break;
    }
    case BACK_GAUNG_XR:
    {
        if(pGaung_XR)
        {
            destroyTrimmed(pGaung_XR);
            destroyEntity(pGL_Gaung_XR);
            pGaung_XR = NULL;
        }
        pGL_Gaung_XR = readOriginFile(path);
        pGaung_XR = transform_Brep(pGL_Gaung_XR);
        pResult = pGaung_XR;
        break;
    }
    case BACK_GAUNG_RR:
    {
        if(pGaung_RR)
        {
            destroyTrimmed(pGaung_RR);
            destroyEntity(pGL_Gaung_RR);
            pGaung_RR = NULL;
        }
        pGL_Gaung_RR = readOriginFile(path);
        pGaung_RR = transform_Brep(pGL_Gaung_RR);
        pResult = pGaung_RR;
        break;
    }
    case BACK_GAUNG_ZR:
    {
        if(pGaung_ZR)
        {
            destroyTrimmed(pGaung_ZR);
            destroyEntity(pGL_Gaung_ZR);
            pGaung_ZR = NULL;
        }
        pGL_Gaung_ZR = readOriginFile(path);
        pGaung_ZR = transform_Brep(pGL_Gaung_ZR);
        pResult = pGaung_ZR;
        break;
    }
    default:
        break;
    }
    return pResult;
}

bend_surface *fileOperate::getWorkPieceModel(QString path,bool bDeploy)
{
    if(bDeploy == false)
    {
        fileName = path;
        pGL_WorkPiece = readOriginFile(path);
        pWorkPiece = transform_Brep(pGL_WorkPiece);
        pBendHead = getWorkPieceInfo(pWorkPiece);
//为遗传算法提供的备用工件数据
//        gl_data *pgl;
//        trimmed_surface_type *ptrimmed;
//        for(int i = 0; i < M; i++){
//            pgl = readOriginFile(path);
//            ptrimmed = transform_Brep(pgl);
//            pBend_Backup.push_back(getWorkPieceInfo(ptrimmed));
//        }
        return pBendHead;
    }
    else
    {
        pGL_WorkPiece_Deploy = readOriginFile(path);
        pWorkPiece_Deploy = transform_Brep(pGL_WorkPiece_Deploy);
        pDeployHead = getWorkPieceInfo(pWorkPiece_Deploy);
        return pDeployHead;
    }
}

bend_surface *fileOperate::getBendHead()
{
    return pBendHead;
}

/********************************************
 *function:初始化折弯顺序
 *adding:
 *author: xu
 *date: 2015/10/31
 *******************************************/
bend_order *fileOperate::getBendOrder(bend_surface *pHead)
{
    if(pOrderHead)      //如果已经存在,则直接返回
        return pOrderHead;
    //不存在,则初始化折弯顺序
    bend_surface *pbend = pHead;
    int count = 1;
    while (pbend) {
        bend_order *ptmp = new bend_order;
        memset(ptmp,0,sizeof(bend_order));
        ptmp->bend_id = count;
        ptmp->bendpoint = pbend->bendNum;
        ptmp->y_open = Y_open;
        ptmp->bendAngle = pbend->bendAngle;
        ptmp->bendWidth = pbend->bendWidth;
        ptmp->bendRadius = pbend->bendRadius;
        if(pbend->bendAngle >= 0)//折弯角度大于0，则上下翻转标志位为1
            ptmp->direction_v = 1;
        else                    //折弯角度小于0，则上下翻转标志位为-1
            ptmp->direction_v = -1;
        ptmp->direction_h = 0;  //初始化都不干涉,-1为都干涉,1位调头后不干涉
        if(pOrderHead == NULL)
        {
            pOrderHead = ptmp;
            pOrderLast = ptmp;
            pOrderHead->pBefore = NULL;
            pOrderLast->pNext = NULL;
        }
        else
        {
            pOrderLast->pNext = ptmp;
            ptmp->pBefore = pOrderLast;
            pOrderLast = ptmp;
            pOrderLast->pNext = NULL;
        }
        count++;
        pbend = pbend->pNext;
    }
    pOrderCur = pOrderHead;
    return pOrderHead;
}

/********************************************
*function:对某一轴进行数据变换
 *adding:
 *author: xu
 *date: 2016/05/27
 *******************************************/
void fileOperate::formAxisData(AXIS axis, float pos)
{
    switch (axis) {
    case Y_AXIS:
    {
        //formPuchBendData(pos);
        pOrderCur->y_open = pos;
        break;
    }
    case X1_AXIS:
    {
        float relative_pos = pos-(X1_Pos+X1_Retract);
        X1_Pos = pos;
        offToolData(pGaung_XL,0.0,0.0,-relative_pos);
        offToolData(pGaung_RL,0.0,0.0,-relative_pos);
        pOrderCur->position_x1 = X1_Pos;
        break;
    }
    case X2_AXIS:
    {
        float relative_pos = pos-(X2_Pos+X2_Retract);
        X2_Pos = pos;
        offToolData(pGaung_XR,0.0,0.0,-relative_pos);
        offToolData(pGaung_RR,0.0,0.0,-relative_pos);
        pOrderCur->position_x2 = X2_Pos;
        break;
    }
    case X1_AXIS_BACK:   //用来标记退让距离
    {
        float relative_pos = pos-X1_Retract;
        X1_Retract = pos;
        offToolData(pGaung_XL,0.0,0.0,-relative_pos);
        offToolData(pGaung_RL,0.0,0.0,-relative_pos);
        pOrderCur->dX1Retract = X1_Retract;
        break;
    }
    case X2_AXIS_BACK:   //用来标记退让距离
    {
        float relative_pos = pos-X2_Retract;
        X2_Retract = pos;
        offToolData(pGaung_XR,0.0,0.0,-relative_pos);
        offToolData(pGaung_RR,0.0,0.0,-relative_pos);
        pOrderCur->dX2Retract = X2_Retract;
        break;
    }
    case R1_AXIS:
    {
        float relative_pos = pos-R1_Pos;
        R1_Pos = pos;
        offToolData(pGaung_RL,0.0,relative_pos,0.0);
        pOrderCur->position_r1 = R1_Pos;
        break;
    }
    case R2_AXIS:
    {
        float relative_pos = pos-R2_Pos;
        R2_Pos = pos;
        offToolData(pGaung_RR,0.0,relative_pos,0.0);
        pOrderCur->position_r2 = R2_Pos;
        break;
    }
    case Z1_AXIS:
    {
        float relative_pos = pos-Z1_Pos;
        Z1_Pos = pos;
        offToolData(pGaung_XL,relative_pos,0.0,0.0);
        offToolData(pGaung_RL,relative_pos,0.0,0.0);
        offToolData(pGaung_ZL,relative_pos,0.0,0.0);
        pOrderCur->position_z1 = Z1_Pos;
        break;
    }
    case Z2_AXIS:
    {
        float relative_pos = pos-Z2_Pos;
        Z2_Pos = pos;
        offToolData(pGaung_XR,relative_pos,0.0,0.0);
        offToolData(pGaung_RR,relative_pos,0.0,0.0);
        offToolData(pGaung_ZR,relative_pos,0.0,0.0);
        pOrderCur->position_z2 = Z2_Pos;
        break;
    }
    default:
        break;
    }
}

bend_order *fileOperate::getCurOrder()
{
    return pOrderCur;
}

bend_surface *fileOperate::getDeployHead()
{
    return pDeployHead;
}

/********************************************
 *function:显示原始工件模型
 *adding:
 *author: xu
 *date: 2016/03/28
 *******************************************/
void fileOperate::showOriginPiece()
{
    while(pOrderCur)
    {
        if(bendFlag == false)   //回程状态下,执行展开操作
        {
            bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
            MatrixTotalConvert(pBendHead,pbend->pInverse,false);
            MatrixTotalConvert(pBendHead,pbend->pInverse,true);
            pOrderCur = pOrderCur->pBefore;
            bendFlag = true;    //重置为折弯状态
        }
        else                    //折弯状态下，执行展开操作
        {
            bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
            pbend->isBended = 0;
            pbend->pParallel->isBended = 0;
            resetVisitFlag(pBendHead);
            MatrixPartConvert(pbend->pLeftInverse,pBendHead,pbend,pbend->pLeftBase,false);
            MatrixPartConvert(pbend->pParallel->pLeftInverse,pBendHead,pbend->pParallel,pbend->pParallel->pLeftBase,true);
            MatrixPartConvert(pbend->pRightInverse,pBendHead,pbend,pbend->pRightBase,false);
            MatrixPartConvert(pbend->pParallel->pRightInverse,pBendHead,pbend->pParallel,pbend->pParallel->pRightBase,true);
            resetVisitFlag(pBendHead);
            bendFlag = false;   //重置为回程状态
        }
    }
}

void fileOperate::initBendAxisData()
{
   bendFlag = false;
   pOrderCur = pOrderHead;
   convertToBendPos(pOrderCur,pBendHead);
   computeBackGaungPos(pOrderCur,pBendHead);
   //首先让板料折一遍然后再恢复到原始位置
   while(1){
       if(bendFlag == false)   //回程状态下,执行折弯操作
       {
           if(pOrderCur == NULL)
               break;
           //生成上模和滑块的折弯模型数据
           formBendedData(pOrderCur->bendpoint,false,pBendHead);
           formBendedData(pOrderCur->bendpoint,true,pBendHead);
           formBendSideSurface(pOrderCur->bendpoint,pBendHead);
           bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
           pOrderCur->position_y1 = Y_bendDepth(pbend->bendAngle);
           pOrderCur->position_y2 = pOrderCur->position_y1;
           bendFlag = true;    //重置为折弯状态
       }
       else                    //折弯状态下，执行回程操作
       {
           if(pOrderCur->pNext == NULL)
               break;
           //生成上模和滑块的回程模型数据
           pOrderCur = pOrderCur->pNext;//转到下一道折弯
           convertToBendPos(pOrderCur,pBendHead);
           computeBackGaungPos(pOrderCur,pBendHead);
           bendFlag = false;   //重置为回程状态
       }
   }
   while(1){
       if(bendFlag == false)   //回程状态下,执行展开操作
       {
           if(pOrderCur == pOrderHead)//当前状态下无法展开
               break;
           bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
           MatrixTotalConvert(pBendHead,pbend->pInverse,false);
           MatrixTotalConvert(pBendHead,pbend->pInverse,true);
           pOrderCur = pOrderCur->pBefore;
           bendFlag = true;
       }
       else                    //折弯状态下，执行展开操作
       {
           if(pOrderCur == NULL)
               break;
           bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
           //生成上模和滑块的折弯模型数据
           pbend->isBended = 0;
           pbend->pParallel->isBended = 0;
           resetVisitFlag(pBendHead);
           MatrixPartConvert(pbend->pLeftInverse,pBendHead,pbend,pbend->pLeftBase,false);
           MatrixPartConvert(pbend->pParallel->pLeftInverse,pBendHead,pbend->pParallel,pbend->pParallel->pLeftBase,true);
           MatrixPartConvert(pbend->pRightInverse,pBendHead,pbend,pbend->pRightBase,false);
           MatrixPartConvert(pbend->pParallel->pRightInverse,pBendHead,pbend->pParallel,pbend->pParallel->pRightBase,true);
           resetVisitFlag(pBendHead);
           bendFlag = false;   //重置为回程状态
       }
   }
   pOrderCur = pOrderHead;
   updateBackGaungPos(pOrderCur);
}

/********************************************
 *function:生成折弯程序
 *adding:
 *author: xu
 *date: 2016/03/29
 *******************************************/
void fileOperate::formBendFile(QString bendPath)
{
    int len = bendPath.size();
    int pos = bendPath.lastIndexOf("/");
    QString name = bendPath.right(len-pos-1);
    pos = name.indexOf(".");
    name = name.left(pos);
    int id = name.toInt();
    QByteArray ba = bendPath.toLatin1();
    char *path = ba.data();
    FILE *fp;
    int flag;
    if((fp = fopen(path,"wb+")) == NULL)
        return;
    if(!z1_enable && !z2_enable && toolWidth.size() > 1)
    {
        QVector<int> div = getDivNum();//获取各部分个数
        int num = div.size();
        flag = fwrite(&num,sizeof(int),1,fp);
        if(flag != 1)
        {
            fclose(fp);
            return;
        }
        int index = id;
        for(int i = 0; i < num; i++){
            index += i;
            flag = fwrite(&index,sizeof(int),1,fp);//写入程序号
            if(flag != 1)
            {
                fclose(fp);
                return;
            }
            flag = fwrite(&div[i],sizeof(int),1,fp);//写入折弯数
            if(flag != 1)
            {
                fclose(fp);
                return;
            }
        }
        bend_order *ptmp = pOrderHead;
        for(int i = 0; i < num; i++)
        {
            if(i == num-1)
                ptmp = formData(fp,id+i,ptmp,true,0);
            else
                ptmp = formData(fp,id+i,ptmp,true,id+i+1);
        }
    }
    else
    {
        int num = 1;
        flag = fwrite(&num,sizeof(int),1,fp);
        if(flag != 1)
        {
            fclose(fp);
            return;
        }
        flag = fwrite(&id,sizeof(int),1,fp);
        if(flag != 1)
        {
            fclose(fp);
            return;
        }
        flag = fwrite(&bendNum,sizeof(int),1,fp);
        if(flag != 1)
        {
            fclose(fp);
            return;
        }
        bend_order *ptmp = pOrderHead;
        ptmp = formData(fp,id,ptmp,false);
    }
    fclose(fp);
}

void fileOperate::drawDataTable(QPainter *painter)
{
    int bendnum = bendNum;
    painter->drawLine(QPointF(PAGE_X_MARGIN,600),QPointF(PAGE_WIDTH-PAGE_X_MARGIN,600));
    //painter.drawLine(QPointF(PAGE_X_MARGIN,600),QPointF(PAGE_X_MARGIN,600+20*(bendnum+1)));
    painter->drawLine(QPointF(PAGE_WIDTH-PAGE_X_MARGIN,600),QPointF(PAGE_WIDTH-PAGE_X_MARGIN,600+20*(bendnum+1)));
    painter->drawLine(QPointF(PAGE_X_MARGIN,600+20*(bendnum+1)),QPointF(PAGE_WIDTH-PAGE_X_MARGIN,600+20*(bendnum+1)));
    float margin = (PAGE_WIDTH-2*PAGE_X_MARGIN)/11.0;
    bend_order *ptmp_order = pOrderHead;
    for(int i = 0; i < bendNum; i++)
    {
        float width = ptmp_order->bendWidth;
        int m = 0;
        for(m = 0; m < toolWidth.size(); m++)
        {
            if(fabs(toolWidth[m]-width) < toolMargin[m])
                break;
        }
        painter->drawLine(QPointF(PAGE_X_MARGIN,620+20*i),QPointF(PAGE_WIDTH-PAGE_X_MARGIN,620+20*i));
        painter->drawText(QRectF(PAGE_X_MARGIN+20,623+20*i,margin,20),QObject::tr("%1").arg(i+1));
        painter->drawText(QRectF(PAGE_X_MARGIN+20+margin,623+20*i,margin,20),QObject::tr("%1/%2").arg(toolPunch[m]).arg(toolDie[m]));
        painter->drawText(QRectF(PAGE_X_MARGIN+15+margin*2,623+20*i,margin,20),QObject::tr("%1").arg(fabs(ptmp_order->bendAngle)/PI*180.0,0,'f',2));
        painter->drawText(QRectF(PAGE_X_MARGIN+10+margin*3,623+20*i,margin,20),QObject::tr("%1").arg(ptmp_order->bendWidth,0,'f',2));
        painter->drawText(QRectF(PAGE_X_MARGIN+20+margin*4,623+20*i,margin,20),QObject::tr("%1").arg(ptmp_order->bendRadius,0,'f',2));
        painter->drawText(QRectF(PAGE_X_MARGIN+15+margin*5,623+20*i,margin,20),QObject::tr("%1").arg(ptmp_order->position_x1,0,'f',2));
        painter->drawText(QRectF(PAGE_X_MARGIN+15+margin*6,623+20*i,margin,20),QObject::tr("%1").arg(ptmp_order->position_x2,0,'f',2));
        painter->drawText(QRectF(PAGE_X_MARGIN+15+margin*7,623+20*i,margin,20),QObject::tr("%1").arg(ptmp_order->position_r1,0,'f',2));
        painter->drawText(QRectF(PAGE_X_MARGIN+15+margin*8,623+20*i,margin,20),QObject::tr("%1").arg(ptmp_order->position_r2,0,'f',2));
        painter->drawText(QRectF(PAGE_X_MARGIN+5+margin*9,623+20*i,margin,20),QObject::tr("%1").arg(ptmp_order->position_z1,0,'f',2));
        painter->drawText(QRectF(PAGE_X_MARGIN+5+margin*10,623+20*i,margin,20),QObject::tr("%1").arg(ptmp_order->position_z2,0,'f',2));
        ptmp_order = ptmp_order->pNext;
    }
    for(int i = 0; i < 11; i++)
    {
        painter->drawLine(QPointF(PAGE_X_MARGIN+i*margin,600),QPointF(PAGE_X_MARGIN+i*margin,600+20*(bendNum+1)));
    }

    painter->drawText(QRectF(PAGE_X_MARGIN+20,603,margin,20),QObject::tr("折弯"));
    painter->drawText(QRectF(PAGE_X_MARGIN+margin+20,603,margin,20),QObject::tr("模具"));
    painter->drawText(QRectF(PAGE_X_MARGIN+margin*2+20,603,margin,20),QObject::tr("角度"));
    painter->drawText(QRectF(PAGE_X_MARGIN+margin*3+20,603,margin,20),QObject::tr("长度"));
    painter->drawText(QRectF(PAGE_X_MARGIN+margin*4+20,603,margin,20),QObject::tr("半径"));
    painter->drawText(QRectF(PAGE_X_MARGIN+margin*5+20,603,margin,20),QObject::tr("X1"));
    painter->drawText(QRectF(PAGE_X_MARGIN+margin*6+20,603,margin,20),QObject::tr("X2"));
    painter->drawText(QRectF(PAGE_X_MARGIN+margin*7+20,603,margin,20),QObject::tr("R1"));
    painter->drawText(QRectF(PAGE_X_MARGIN+margin*8+20,603,margin,20),QObject::tr("R2"));
    painter->drawText(QRectF(PAGE_X_MARGIN+margin*9+20,603,margin,20),QObject::tr("Z1"));
    painter->drawText(QRectF(PAGE_X_MARGIN+margin*10+20,603,margin,20),QObject::tr("Z2"));
}

/********************************************
 *function:前后调头工件
 *adding:
 *author: xu
 *date: 2016/03/29
 *******************************************/
void fileOperate::turnAboutPiece(bend_order *pcur)
{
   if(pcur == NULL)
       return;
   int bendnum = pcur->bendpoint;
   xform_type *matrix = formTurnAboutMatrix(bendnum,pBendHead);
   MatrixTotalConvert(pBendHead,matrix,false);
   MatrixTotalConvert(pBendHead,matrix,true);
   //折弯调头后原来设定的逆函数需要重新计算
   bend_surface *pbend = findBendSurface(bendnum,pBendHead);
   if(bendFlag == false)    //展开状态
   {
       xform_type *tmp = MatrixMultiply(pbend->pMatrix,matrix);
       xform_type *inverse = computeInverseMatrix(tmp);
       delete tmp;
       delete pbend->pInverse;
       pbend->pInverse = inverse;
   }
   delete matrix;
   computeBackGaungPos(pcur,pBendHead);
}

int fileOperate::getBendNum()
{
    return bendNum;
}

float fileOperate::getThickness()
{
    return thickness;
}

/********************************************
 *function:清理模具数据
 *adding:
 *author: xu
 *date: 2016/04/15
 *******************************************/
void fileOperate::clearToolData()
{
   int num = pPunch.size();
   for(int i = 0; i < num; i++)
   {
       destroyTrimmed(pPunch[i]);
       pPunch[i] = NULL;
       destroyTrimmed(pDie[i]);
       pDie[i] = NULL;
   }
   pPunch.clear();
   pDie.clear();
}

/********************************************
 *function:初始化工作空间的位置
 *adding:  主要包括工件的位置以及机床各轴的位置
 *author: xu
 *date: 2015/10/31
 *******************************************/
void fileOperate::initWorkSpacePos()
{
    pOrderCur = pOrderHead; //当前工序为链表的头结点
    //工件的位置
    convertToBendPos(pOrderCur,pBendHead);
    //computeBackGaungPos(pOrderCur,pBendHead);
    bendFlag = false;
}

/********************************************
 *function:根据上下模的配置数据,自动调整上下模结构体数据
 *adding:
 *author: xu
 *date: 2015/10/31
 *******************************************/
void fileOperate::initToolPos(MPART part,point3f off)
{
    switch (part) {
    case PUNCH: //上下模放在一起进行处理
    case DIE:
    {
        point3f off_tmp;
        tool_x_off.clear();
        for(int i = 0; i < pPunch.size(); i++)
        {
            off_tmp.x = (toolPosition[i]+toolWidth[i]/2.0)-(machineWidth/2.0);
            off_tmp.y = 0.0;
            off_tmp.z = 0.0;
            tool_x_off.push_back(off_tmp.x);
            offToolData(pPunch[i],off_tmp);
            offToolData(pDie[i],off_tmp);
        }
        break;
    }
    case BODY:
    {
        if(pMachine == NULL)
            return;
        offToolData(pMachine,off);
        break;
    }
    case UPPER:
    {
        if(pUpper == NULL)
            return;
        offToolData(pUpper,off);
        Y_Pos = 0.0;
        break;
    }
    case LOWER:
    {
        if(pLower == NULL)
            return;
        offToolData(pLower,off);
        break;
    }
    case BACK_GAUNG_XL:
    {
        if(pGaung_XL == NULL)
            return;
        offToolData(pGaung_XL,off);
        X1_Pos = off.z;
        break;
    }
    case BACK_GAUNG_RL:
    {
        if(pGaung_RL == NULL)
            return;
        offToolData(pGaung_RL,off);
        R1_Pos = off.z;
        break;
    }
    case BACK_GAUNG_ZL:
    {
        if(pGaung_ZL == NULL)
            return;
        offToolData(pGaung_ZL,off);
        Z1_Pos = off.x;
        break;
    }
    case BACK_GAUNG_XR:
    {
        if(pGaung_XR == NULL)
            return;
        offToolData(pGaung_XR,off);
        X2_Pos = off.z;
        break;
    }
    case BACK_GAUNG_RR:
    {
        if(pGaung_RR == NULL)
            return;
        offToolData(pGaung_RR,off);
        R2_Pos = off.z;
        break;
    }
    case BACK_GAUNG_ZR:
    {
        if(pGaung_ZR == NULL)
            return;
        offToolData(pGaung_ZR,off);
        Z2_Pos = off.x;
        break;
    }
    default:
        break;
    }
}

/********************************************
 *function:寻求最优解,
 *adding: 1~6个折弯点时，全局搜索
 *      在做规划的时候最多选出20种情况作为备选方案
 * 返回值--0为正确计算出折弯工序
 *        -1为未找到可行的折弯工序
 *        1为机床环境未完全导入
 *author: xu
 *date: 2015/12/27
 *******************************************/
int fileOperate::bendorderResult(QWidget *parent)
{
    if(!pUpper || !pLower || !pPunch.size() || !pDie.size())
    {
        //当机床本体等没有导入时,不进行折弯工序规划
        return -2;
    }
    resetToDeploy();//将折弯数据返回到初始化的数据
    int bendnum = bendNum;
    //在此处进行区分,1~6道折弯进行全局搜索
    //6道折弯以上则进行优化算法
    //首先将使用同一模具的折弯序号分别放在一组中
    //如果分成的组中的折弯数大于6,则继续分割
    //*****************1~6道折弯的全局搜索***************************/
    if(bendnum >= 1 && bendnum <= 6)
    {
        //计算程序需要执行的次数
        int num = bendnum;
        int sum = pow(num,num);
        ProgressDlg progress(parent,QObject::tr("折弯工序计算中......"),sum,true);
        progress.show();
        progress.setValue(1);
        QVector< QVector<int> > res_order;
        QVector< QVector<int> > res_tag;
        //1~7个折弯点时,全局搜索
        int tag1,tag2,tag3,tag4,tag5,tag6;
        for(int i1 = 1; i1 <= num; i1++)
        {
            //进度条显示
            int count1 = pow(num,num-1)*(i1-1);
            progress.setValue(count1);
            tag1 = judgeInterFere(i1);
            if(tag1 == -1) //有干涉,则返回继续
                continue;
            if(num == 1)    //有1个折弯点
            {
                //折弯数为1时的处理函数
            }
            //说明第一道折弯没有干涉
            for(int i2 = 1; i2 <= num; i2++)
            {
                int count2 = count1+pow(num,num-2)*(i2-1);
                progress.setValue(count2);
                QVector<int> pos2,inter2;
                pos2.push_back(i1);
                inter2.push_back(tag1);
                if(i2 == i1)
                {
                    if(i2 == num)//已经循环到最后
                        recoverTo(pos2,inter2,num);
                    continue;
                }
                //进度条显示
                tag2 = judgeInterFere(i2);
                if(tag2 == -1)//有干涉,则返回继续
                {
                    if(i2 == num)//已经循环到最后
                        recoverTo(pos2,inter2,num);
                    continue;
                }
                if(num == 2)    //有2个折弯点
                {
                    //折弯数为2时的处理函数
                    QVector<int> pos,inter;
                    pos.push_back(i2);pos.push_back(i1);
                    inter.push_back(tag2);inter.push_back(tag1);
                    //将折弯顺序和调头标志保存到数组中
                    res_order.push_back(pos);
                    res_tag.push_back(inter);
                    recoverTo(pos,inter,num);
                    continue;
                }
                for(int i3 = 1; i3 <= num; i3++)
                {
                    int count3 = count2+pow(num,num-3)*(i3-1);
                    progress.setValue(count3);
                    QVector<int> pos3,inter3;
                    pos3.push_back(i2);pos3.push_back(i1);
                    inter3.push_back(tag2);inter3.push_back(tag1);
                    if(i3 == i2 || i3 == i1)
                    {
                        if(i3 == num)//已经循环到最后
                           recoverTo(pos3,inter3,num);
                        continue;
                    }
                    //进度条显示
                    tag3 = judgeInterFere(i3);
                    if(tag3 == -1)
                    {
                        if(i3 == num)//已经循环到最后
                           recoverTo(pos3,inter3,num);
                        continue;
                    }
                    if(num == 3)
                    {
                        //折弯数为3时的处理函数
                        QVector<int> pos,inter;
                        pos.push_back(i3);pos.push_back(i2);pos.push_back(i1);
                        inter.push_back(tag3);inter.push_back(tag2);inter.push_back(tag1);
                        //将折弯顺序和调头标志保存到数组中
                        res_order.push_back(pos);
                        res_tag.push_back(inter);
                        recoverTo(pos,inter,num);
                        continue;
                    }
                    for(int i4 = 1; i4 <= num; i4++)
                    {
                        int count4 = count3+pow(num,num-4)*(i4-1);
                        progress.setValue(count4);
                        QVector<int> pos4,inter4;
                        pos4.push_back(i3);pos4.push_back(i2);pos4.push_back(i1);
                        inter4.push_back(tag3);inter4.push_back(tag2);inter4.push_back(tag1);
                        if(i4 == i3 || i4 == i2 || i4 == i1)
                        {
                            if(i4 == num)//已经循环到最后
                                recoverTo(pos4,inter4,num);
                            continue;
                        }
                        //进度条显示
                        tag4 = judgeInterFere(i4);
                        if(tag4 == -1)
                        {
                            if(i4 == num)//已经循环到最后
                                recoverTo(pos4,inter4,num);
                            continue;
                        }
                        if(num == 4)
                        {
                            //折弯数为4时的处理函数
                            QVector<int> pos,inter;
                            pos.push_back(i4);pos.push_back(i3);
                            pos.push_back(i2);pos.push_back(i1);
                            inter.push_back(tag4);inter.push_back(tag3);
                            inter.push_back(tag2);inter.push_back(tag1);
                            //将折弯顺序和调头标志保存到数组中
                            res_order.push_back(pos);
                            res_tag.push_back(inter);
                            recoverTo(pos,inter,num);
                            continue;
                        }
                        for(int i5 = 1; i5 <= num; i5++)
                        {
                            int count5 = count4+pow(num,num-5)*(i5-1);
                            progress.setValue(count5);
                            QVector<int> pos5,inter5;
                            pos5.push_back(i4);pos5.push_back(i3);
                            pos5.push_back(i2);pos5.push_back(i1);
                            inter5.push_back(tag4);inter5.push_back(tag3);
                            inter5.push_back(tag2);inter5.push_back(tag1);
                            if(i5 == i4 || i5 == i3 || i5 == i2 || i5 == i1)
                            {
                                if(i5 == num)//已经循环到最后
                                    recoverTo(pos5,inter5,num);
                                continue;
                            }
                            //进度条显示
                            tag5 = judgeInterFere(i5);
                            if(tag5 == -1)
                            {
                                if(i5 == num)//已经循环到最后
                                    recoverTo(pos5,inter5,num);
                                continue;
                            }
                            if(num == 5)    //折弯段有5段
                            {
                                //折弯数为5时的处理函数
                                QVector<int> pos,inter;
                                pos.push_back(i5);pos.push_back(i4);pos.push_back(i3);
                                pos.push_back(i2);pos.push_back(i1);
                                inter.push_back(tag5);inter.push_back(tag4);inter.push_back(tag3);
                                inter.push_back(tag2);inter.push_back(tag1);
                                //将折弯顺序和调头标志保存到数组中
                                res_order.push_back(pos);
                                res_tag.push_back(inter);
                                recoverTo(pos,inter,num);
                                continue;
                            }
                            for(int i6 = 1; i6 <= num; i6++)
                            {
                                int count6 = count5+pow(num,num-6)*(i6-1);
                                progress.setValue(count6);
                                QVector<int> pos6,inter6;
                                pos6.push_back(i5);pos6.push_back(i4);
                                pos6.push_back(i3);pos6.push_back(i2);pos6.push_back(i1);
                                inter6.push_back(tag5);inter6.push_back(tag4);
                                inter6.push_back(tag3);inter6.push_back(tag2);inter6.push_back(tag1);
                                if(i6 == i5 || i6 == i4 || i6 == i3 || i6 == i2 || i6 == i1)
                                {
                                    if(i6 == num)//需要进行判断,折弯数据将回退到哪个折弯点
                                        recoverTo(pos6,inter6,num);
                                    continue;
                                }
                                //进度条显示
                                tag6 = judgeInterFere(i6);
                                if(tag6 == -1)
                                {
                                    if(i6 == num)//已经循环到最后
                                        recoverTo(pos6,inter6,num);
                                    continue;
                                }
                                if(num == 6)
                                {
                                    //折弯数为6时的处理函数
                                    QVector<int> pos,inter;
                                    pos.push_back(i6);pos.push_back(i5);pos.push_back(i4);
                                    pos.push_back(i3);pos.push_back(i2);pos.push_back(i1);
                                    inter.push_back(tag6);inter.push_back(tag5);inter.push_back(tag4);
                                    inter.push_back(tag3);inter.push_back(tag2);inter.push_back(tag1);
                                    //将折弯顺序和调头标志保存到数组中
                                    res_order.push_back(pos);
                                    res_tag.push_back(inter);
                                    recoverTo(pos,inter,num);
                                    continue;
                                }
                            }
                        }
                    }
                }
            }
        }
        progress.setValue(sum);
        int final_count = res_order.size();
        if(final_count == 0)    //未找到合适的折弯
        {
            pOrderCur = pOrderHead;
            return -1;
        }
        int **final_order = new int *[final_count];
        int **final_tag = new int *[final_count];
        for(int i = 0; i < final_count; i++)
        {
            final_order[i] = new int[bendnum];
            final_tag[i] = new int[bendnum];
        }
        for(int i = 0; i < final_count; i++)
        {
            int size = res_order[i].size();
            for(int j = size-1; j >= 0; j--)
            {
                final_order[i][size-1-j] = res_order[i][j];
                final_tag[i][size-1-j] = res_tag[i][j];
            }
        }
        formPuchBendData(Y_Pos-Y_open);
        int optimum = computeOptimumSolution(parent,final_order,final_tag,final_count,bendnum);
        //初始化计算结果的第一组数据
        formBendOrder(final_order[optimum],final_tag[optimum]);
        convertToBendPos(pOrderCur,pBendHead);
        bendFlag = false;
        computeBackGaungPos(pOrderCur,pBendHead);

        //需要析构之前申请的内存
        for(int i = 0; i < final_count; i++)
        {
            delete []final_order[i];
            delete []final_tag[i];
        }
        delete []final_order;
        delete []final_tag;
    }
    //****************6~15道折弯以上的搜索方式**************************/
    else
    {
        //根据模具的配置将折弯号分为若干组
        int STD_COUNT = 6;  //以6道折弯为界进行划分
        QVector<QVector<int> > part_order;
        bend_surface *pbend = pBendHead;
        for(int i = 0; i < toolWidth.size(); i++)
        {
            pbend = pBendHead;
            QVector<int> tmp;
            for(int j = 0; j < bendnum; j++)
            {
                if(fabs(pbend->bendWidth-toolWidth[i]) < toolMargin[i])
                {
                    tmp.push_back(pbend->bendNum);
                }
                pbend = pbend->pNext;
            }
            int size = tmp.size();
            int dev_count = size/STD_COUNT+1;   //可分成的部分个数
            int circle = size/dev_count;
            if(size > STD_COUNT)
            {
                for(int j = 0; j < dev_count; j++)
                {
                    QVector<int> tmp_dev;
                    int start = j*circle;
                    int end = (j+1)*circle;
                    if(j == dev_count-1)
                        end = size;
                    for(int n = start; n < end; n++)
                    {
                        tmp_dev.push_back(tmp[n]);
                    }
                    part_order.push_back(tmp_dev);
                }
            }
            else
            {
                part_order.push_back(tmp);
            }
        }
        //进行判断,确保每组的折弯数都小于6道折弯,超过6道折弯进行对半分
        int part_count = part_order.size();
        int sum = 0;
        //计算程序需要执行的次数
        for(int i = 0; i < part_count; i++)
        {
            int base = part_order[i].size();
            sum += pow(base,base);
        }
        ProgressDlg progress(parent,QObject::tr("折弯工序计算中......"),sum,true);
        progress.show();
        progress.setValue(1);
        //*************进行可行解计算***********************************/
        QVector<int> res_count; //记录每一部分的可行解的数量
        QVector< QVector<int> > res_order;//最终的折弯顺序数据
        QVector< QVector<int> > res_tag;//最终的每道折弯的调头标志数据
        int count = 0;
        for(int n = 0; n < part_count; n++)
        {
            int num = part_order[n].size();
            int last = part_order[n][num-1];
            if(n != 0)
            {
                int tmp = part_order[n-1].size();
                count += pow(tmp,tmp);
            }
            progress.setValue(count);
            QVector< QVector<int> > order_tmp;//临时的可行解顺序
            QVector< QVector<int> > order_tag;//临时的可行解顺序的调头标志
            //1~6个折弯点时,全局搜索
            int tag1,tag2,tag3,tag4,tag5,tag6;
            for(int i1 = 0; i1 < num; i1++)
            {
                int count1 = count+pow(num,num-1)*i1;
                progress.setValue(count1);
                int cur1 = part_order[n][i1];
                //进度条显示
                tag1 = judgeInterFere(cur1);
                if(tag1 == -1)  //有干涉,则返回继续
                    continue;
                if(num == 1)    //有1个折弯点
                {
                    //折弯数为1时的处理函数
                    QVector<int> pos,inter;
                    pos.push_back(cur1);
                    inter.push_back(tag1);
                    //将折弯顺序和调头标志保存到数组中
                    order_tmp.push_back(pos);
                    order_tag.push_back(inter);
                    recoverTo(pos,inter,last);
                    continue;
                }
                //说明第一道折弯没有干涉
                for(int i2 = 0; i2 < num; i2++)
                {
                    int count2 = count1+pow(num,num-2)*i2;
                    progress.setValue(count2);
                    int cur2 = part_order[n][i2];
                    QVector<int> pos2,inter2;
                    pos2.push_back(cur1);
                    inter2.push_back(tag1);
                    if(i2 == i1)
                    {
                        if(i2 == num-1)//已经循环到最后
                            recoverTo(pos2,inter2,last);
                        continue;
                    }
                    tag2 = judgeInterFere(cur2);
                    if(tag2 == -1)//有干涉,则返回继续
                    {
                        if(i2 == num-1)//已经循环到最后
                            recoverTo(pos2,inter2,last);
                        continue;
                    }
                    if(num == 2)    //有2个折弯点
                    {
                        //折弯数为2时的处理函数
                        QVector<int> pos,inter;
                        pos.push_back(cur2);pos.push_back(cur1);
                        inter.push_back(tag2);inter.push_back(tag1);
                        //将折弯顺序和调头标志保存到数组中
                        order_tmp.push_back(pos);
                        order_tag.push_back(inter);
                        recoverTo(pos,inter,last);
                        continue;
                    }
                    for(int i3 = 0; i3 < num; i3++)
                    {
                        int count3 = count2+pow(num,num-3)*i3;
                        progress.setValue(count3);
                        int cur3 = part_order[n][i3];
                        QVector<int> pos3,inter3;
                        pos3.push_back(cur2);pos3.push_back(cur1);
                        inter3.push_back(tag2);inter3.push_back(tag1);
                        if(i3 == i2 || i3 == i1)
                        {
                            if(i3 == num-1)//已经循环到最后
                               recoverTo(pos3,inter3,last);
                            continue;
                        }
                        tag3 = judgeInterFere(cur3);
                        if(tag3 == -1)
                        {
                            if(i3 == num-1)//已经循环到最后
                               recoverTo(pos3,inter3,last);
                            continue;
                        }
                        if(num == 3)
                        {
                            //折弯数为3时的处理函数
                            QVector<int> pos,inter;
                            pos.push_back(cur3);pos.push_back(cur2);pos.push_back(cur1);
                            inter.push_back(tag3);inter.push_back(tag2);inter.push_back(tag1);
                            //将折弯顺序和调头标志保存到数组中
                            order_tmp.push_back(pos);
                            order_tag.push_back(inter);
                            recoverTo(pos,inter,last);
                            continue;
                        }
                        for(int i4 = 0; i4 < num; i4++)
                        {
                            int count4 = count3+pow(num,num-4)*i4;
                            progress.setValue(count4);
                            int cur4 = part_order[n][i4];
                            QVector<int> pos4,inter4;
                            pos4.push_back(cur3);pos4.push_back(cur2);pos4.push_back(cur1);
                            inter4.push_back(tag3);inter4.push_back(tag2);inter4.push_back(tag1);
                            if(i4 == i3 || i4 == i2 || i4 == i1)
                            {
                                if(i4 == num-1)//已经循环到最后
                                    recoverTo(pos4,inter4,last);
                                continue;
                            }
                            //进度条显示
                            tag4 = judgeInterFere(cur4);
                            if(tag4 == -1)
                            {
                                if(i4 == num-1)//已经循环到最后
                                    recoverTo(pos4,inter4,last);
                                continue;
                            }
                            if(num == 4)
                            {
                                //折弯数为4时的处理函数
                                QVector<int> pos,inter;
                                pos.push_back(cur4);pos.push_back(cur3);
                                pos.push_back(cur2);pos.push_back(cur1);
                                inter.push_back(tag4);inter.push_back(tag3);
                                inter.push_back(tag2);inter.push_back(tag1);
                                //将折弯顺序和调头标志保存到数组中
                                order_tmp.push_back(pos);
                                order_tag.push_back(inter);
                                recoverTo(pos,inter,last);
                                continue;
                            }
                            for(int i5 = 0; i5 < num; i5++)
                            {
                                int count5 = count4+pow(num,num-5)*i5;
                                progress.setValue(count5);
                                int cur5 = part_order[n][i5];
                                QVector<int> pos5,inter5;
                                pos5.push_back(cur4);pos5.push_back(cur3);
                                pos5.push_back(cur2);pos5.push_back(cur1);
                                inter5.push_back(tag4);inter5.push_back(tag3);
                                inter5.push_back(tag2);inter5.push_back(tag1);
                                if(i5 == i4 || i5 == i3 || i5 == i2 || i5 == i1)
                                {
                                    if(i5 == num-1)//已经循环到最后
                                        recoverTo(pos5,inter5,last);
                                    continue;
                                }
                                //进度条显示
                                tag5 = judgeInterFere(cur5);
                                if(tag5 == -1)
                                {
                                    if(i5 == num-1)//已经循环到最后
                                        recoverTo(pos5,inter5,last);
                                    continue;
                                }
                                if(num == 5)    //折弯段有5段
                                {
                                    //折弯数为5时的处理函数
                                    QVector<int> pos,inter;
                                    pos.push_back(cur5);pos.push_back(cur4);pos.push_back(cur3);
                                    pos.push_back(cur2);pos.push_back(cur1);
                                    inter.push_back(tag5);inter.push_back(tag4);inter.push_back(tag3);
                                    inter.push_back(tag2);inter.push_back(tag1);
                                    //将折弯顺序和调头标志保存到数组中
                                    order_tmp.push_back(pos);
                                    order_tag.push_back(inter);
                                    recoverTo(pos,inter,last);
                                    continue;
                                }
                                for(int i6 = 1; i6 < num; i6++)
                                {
                                    int count6 = count5+pow(num,num-6)*i6;
                                    progress.setValue(count6);
                                    int cur6 = part_order[n][i6];
                                    QVector<int> pos6,inter6;
                                    pos6.push_back(cur5);pos6.push_back(cur4);
                                    pos6.push_back(cur3);pos6.push_back(cur2);pos6.push_back(cur1);
                                    inter6.push_back(tag5);inter6.push_back(tag4);
                                    inter6.push_back(tag3);inter6.push_back(tag2);inter6.push_back(tag1);
                                    if(i6 == i5 || i6 == i4 || i6 == i3 || i6 == i2 || i6 == i1)
                                    {
                                        if(i6 == num-1)//需要进行判断,折弯数据将回退到哪个折弯点
                                            recoverTo(pos6,inter6,last);
                                        continue;
                                    }
                                    //进度条显示
                                    tag6 = judgeInterFere(cur6);
                                    if(tag6 == -1)
                                    {
                                        if(i6 == num-1)//已经循环到最后
                                            recoverTo(pos6,inter6,last);
                                        continue;
                                    }
                                    if(num == 6)
                                    {
                                        //折弯数为6时的处理函数
                                        QVector<int> pos,inter;
                                        pos.push_back(cur6);pos.push_back(cur5);pos.push_back(cur4);
                                        pos.push_back(cur3);pos.push_back(cur2);pos.push_back(cur1);
                                        inter.push_back(tag6);inter.push_back(tag5);inter.push_back(tag4);
                                        inter.push_back(tag3);inter.push_back(tag2);inter.push_back(tag1);
                                        //将折弯顺序和调头标志保存到数组中
                                        order_tmp.push_back(pos);
                                        order_tag.push_back(inter);
                                        recoverTo(pos,inter,last);
                                        continue;
                                    }
                                }
                            }
                        }
                    }
                }
            }
            if(order_tmp.size() == 0)   //未找到合适的折弯工序
            {
                pOrderCur = pOrderHead;
                return -1;
            }
            res_count.push_back(order_tmp.size());  //记录当前部分可行解的个数
            //取当前部分可行解的第一组数据进行折弯,将此部分都进行折弯
            for(int j = order_tmp[0].size()-1; j >= 0; j--)
            {
                bend_order *ptmp = findBendOrder(order_tmp[0][j],pOrderHead);
                convertToBendPos(ptmp,pBendHead);
                if(order_tag[0][j] == 1)
                {
                    xform_type *matrix = formTurnAboutMatrix(ptmp->bendpoint,pBendHead);
                    MatrixTotalConvert(pBendHead,matrix,false);
                    MatrixTotalConvert(pBendHead,matrix,true); //调头
                    delete matrix;
                }
                formBendedData(ptmp->bendpoint,false,pBendHead);
                formBendedData(ptmp->bendpoint,true,pBendHead);
                formBendSideSurface(ptmp->bendpoint,pBendHead);
                if(j == 0)
                {
                    bend_surface *pbend = findBendSurface(order_tmp[0][j],pBendHead);
                    formPuchBendData(Y_bendDepth(pbend->bendAngle));
                    bendFlag = true;
                }
            }
            for(int j = 0; j < order_tmp.size(); j++)
            {
                res_order.push_back(order_tmp[j]);
                res_tag.push_back(order_tag[j]);
            }
        }
        progress.setValue(sum); //计算可行解进度结束
        //对计算出来的数据进行处理,形成最终的可行解
        int final_count = 1;
        for(int i = 0; i < res_count.size(); i++)
            final_count *= res_count[i];
        int **final_order = new int *[final_count];
        int **final_tag = new int *[final_count];
        for(int i = 0; i < final_count; i++)
        {
            final_order[i] = new int[bendnum];
            final_tag[i] = new int[bendnum];
        }

        for(int i = 0; i < res_count.size(); i++)
        {
            int count = 1;
            for(int j = i+1; j < res_count.size(); j++)
                count *= res_count[j];
            int record = 0;
            int num = 0;
            for(int j = 0; j < i; j++)
            {
                num += res_order[record].size();
                record += res_count[j];
            }
            int number = final_count/count;
            for(int m = 0; m < number; m++)
            {
                int tmp = m%res_count[i]+record;
                int size = res_order[tmp].size();
                for(int j = 0; j < count; j++)
                {
                    for(int n = size-1; n >= 0; n--)
                    {
                        final_order[j+m*count][size-1-n+num] = res_order[tmp][n];
                        final_tag[j+m*count][size-1-n+num] = res_tag[tmp][n];
                    }
                }
            }
        }
        //需要将工件按照计算出来的第一个可行解进行展开
        for(int i = bendnum-1; i >= 0; i--)
        {
            int pos = final_order[0][i];
            int tag = final_tag[0][i];
            recoverBendData(pos,pBendHead,tag);
            if(i == 0)
            {
                formPuchBendData(Y_Pos-Y_open);
            }
        }
        int optimum = computeOptimumSolution(parent,final_order,final_tag,final_count,bendnum);
        //初始化计算结果的第一组数据
        formBendOrder(final_order[optimum],final_tag[optimum]);
        convertToBendPos(pOrderCur,pBendHead);
        bendFlag = false;
        computeBackGaungPos(pOrderCur,pBendHead);
        //需要析构之前申请的内存
        for(int i = 0; i < final_count; i++)
        {
            delete []final_order[i];
            delete []final_tag[i];
        }
        delete []final_order;
        delete []final_tag;
    }
    //首先让板料折一遍然后再恢复到原始位置
    while(1){
        if(bendFlag == false)   //回程状态下,执行折弯操作
        {
            if(pOrderCur == NULL)
                break;
            //生成上模和滑块的折弯模型数据
            formBendedData(pOrderCur->bendpoint,false,pBendHead);
            formBendedData(pOrderCur->bendpoint,true,pBendHead);
            formBendSideSurface(pOrderCur->bendpoint,pBendHead);
            bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
            pOrderCur->position_y1 = Y_bendDepth(pbend->bendAngle);
            pOrderCur->position_y2 = pOrderCur->position_y1;
            bendFlag = true;    //重置为折弯状态
        }
        else                    //折弯状态下，执行回程操作
        {
            if(pOrderCur->pNext == NULL)
                break;
            //生成上模和滑块的回程模型数据
            pOrderCur = pOrderCur->pNext;//转到下一道折弯
            convertToBendPos(pOrderCur,pBendHead);
            computeBackGaungPos(pOrderCur,pBendHead);
            bendFlag = false;   //重置为回程状态
        }
    }
    while(1){
        if(bendFlag == false)   //回程状态下,执行展开操作
        {
            if(pOrderCur == pOrderHead)//当前状态下无法展开
                break;
            bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
            MatrixTotalConvert(pBendHead,pbend->pInverse,false);
            MatrixTotalConvert(pBendHead,pbend->pInverse,true);
            pOrderCur = pOrderCur->pBefore;
            bendFlag = true;
        }
        else                    //折弯状态下，执行展开操作
        {
            if(pOrderCur == NULL)
                break;
            bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
            //生成上模和滑块的折弯模型数据
            pbend->isBended = 0;
            pbend->pParallel->isBended = 0;
            resetVisitFlag(pBendHead);
            MatrixPartConvert(pbend->pLeftInverse,pBendHead,pbend,pbend->pLeftBase,false);
            MatrixPartConvert(pbend->pParallel->pLeftInverse,pBendHead,pbend->pParallel,pbend->pParallel->pLeftBase,true);
            MatrixPartConvert(pbend->pRightInverse,pBendHead,pbend,pbend->pRightBase,false);
            MatrixPartConvert(pbend->pParallel->pRightInverse,pBendHead,pbend->pParallel,pbend->pParallel->pRightBase,true);
            resetVisitFlag(pBendHead);
            bendFlag = false;   //重置为回程状态
        }
    }
    pOrderCur = pOrderHead;
    updateBackGaungPos(pOrderCur);
    return 0;   //计算出可行的折弯工序
}
//int fileOperate::bendorderResult(QWidget *parent)
//{
////    if(!pUpper || !pLower || !pPunch.size() || !pDie.size())
////    {
////        //当机床本体等没有导入时,不进行折弯工序规划
////        return -2;
////    }
//    int bendnum = bendNum;
//    //在此处进行区分,1~6道折弯进行全局搜索
//    //6道折弯以上则进行优化算法
//    //首先将使用同一模具的折弯序号分别放在一组中
//    //如果分成的组中的折弯数大于6,则继续分割
//    //*****************1~6道折弯的全局搜索***************************/
//    if(bendnum >= 1 && bendnum <= 6)
//    {
//        resetToDeploy();//将折弯数据返回到初始化的数据
//        //计算程序需要执行的次数
//        int num = bendnum;
//        int sum = pow(num,num);
//        ProgressDlg progress(parent,QObject::tr("折弯工序计算中......"),sum,true);
//        progress.show();
//        progress.setValue(1);
//        QVector< QVector<int> > res_order;
//        QVector< QVector<int> > res_tag;
//        //1~7个折弯点时,全局搜索
//        int tag1,tag2,tag3,tag4,tag5,tag6;
//        for(int i1 = 1; i1 <= num; i1++)
//        {
//            //进度条显示
//            int count1 = pow(num,num-1)*(i1-1);
//            progress.setValue(count1);
//            tag1 = judgeInterFere(i1);
//            if(tag1 == -1) //有干涉,则返回继续
//                continue;
//            if(num == 1)    //有1个折弯点
//            {
//                //折弯数为1时的处理函数
//            }
//            //说明第一道折弯没有干涉
//            for(int i2 = 1; i2 <= num; i2++)
//            {
//                int count2 = count1+pow(num,num-2)*(i2-1);
//                progress.setValue(count2);
//                QVector<int> pos2,inter2;
//                pos2.push_back(i1);
//                inter2.push_back(tag1);
//                if(i2 == i1)
//                {
//                    if(i2 == num)//已经循环到最后
//                        recoverTo(pos2,inter2,num);
//                    continue;
//                }
//                //进度条显示
//                tag2 = judgeInterFere(i2);
//                if(tag2 == -1)//有干涉,则返回继续
//                {
//                    if(i2 == num)//已经循环到最后
//                        recoverTo(pos2,inter2,num);
//                    continue;
//                }
//                if(num == 2)    //有2个折弯点
//                {
//                    //折弯数为2时的处理函数
//                    QVector<int> pos,inter;
//                    pos.push_back(i2);pos.push_back(i1);
//                    inter.push_back(tag2);inter.push_back(tag1);
//                    //将折弯顺序和调头标志保存到数组中
//                    res_order.push_back(pos);
//                    res_tag.push_back(inter);
//                    recoverTo(pos,inter,num);
//                    continue;
//                }
//                for(int i3 = 1; i3 <= num; i3++)
//                {
//                    int count3 = count2+pow(num,num-3)*(i3-1);
//                    progress.setValue(count3);
//                    QVector<int> pos3,inter3;
//                    pos3.push_back(i2);pos3.push_back(i1);
//                    inter3.push_back(tag2);inter3.push_back(tag1);
//                    if(i3 == i2 || i3 == i1)
//                    {
//                        if(i3 == num)//已经循环到最后
//                           recoverTo(pos3,inter3,num);
//                        continue;
//                    }
//                    //进度条显示
//                    tag3 = judgeInterFere(i3);
//                    if(tag3 == -1)
//                    {
//                        if(i3 == num)//已经循环到最后
//                           recoverTo(pos3,inter3,num);
//                        continue;
//                    }
//                    if(num == 3)
//                    {
//                        //折弯数为3时的处理函数
//                        QVector<int> pos,inter;
//                        pos.push_back(i3);pos.push_back(i2);pos.push_back(i1);
//                        inter.push_back(tag3);inter.push_back(tag2);inter.push_back(tag1);
//                        //将折弯顺序和调头标志保存到数组中
//                        res_order.push_back(pos);
//                        res_tag.push_back(inter);
//                        recoverTo(pos,inter,num);
//                        continue;
//                    }
//                    for(int i4 = 1; i4 <= num; i4++)
//                    {
//                        int count4 = count3+pow(num,num-4)*(i4-1);
//                        progress.setValue(count4);
//                        QVector<int> pos4,inter4;
//                        pos4.push_back(i3);pos4.push_back(i2);pos4.push_back(i1);
//                        inter4.push_back(tag3);inter4.push_back(tag2);inter4.push_back(tag1);
//                        if(i4 == i3 || i4 == i2 || i4 == i1)
//                        {
//                            if(i4 == num)//已经循环到最后
//                                recoverTo(pos4,inter4,num);
//                            continue;
//                        }
//                        //进度条显示
//                        tag4 = judgeInterFere(i4);
//                        if(tag4 == -1)
//                        {
//                            if(i4 == num)//已经循环到最后
//                                recoverTo(pos4,inter4,num);
//                            continue;
//                        }
//                        if(num == 4)
//                        {
//                            //折弯数为4时的处理函数
//                            QVector<int> pos,inter;
//                            pos.push_back(i4);pos.push_back(i3);
//                            pos.push_back(i2);pos.push_back(i1);
//                            inter.push_back(tag4);inter.push_back(tag3);
//                            inter.push_back(tag2);inter.push_back(tag1);
//                            //将折弯顺序和调头标志保存到数组中
//                            res_order.push_back(pos);
//                            res_tag.push_back(inter);
//                            recoverTo(pos,inter,num);
//                            continue;
//                        }
//                        for(int i5 = 1; i5 <= num; i5++)
//                        {
//                            int count5 = count4+pow(num,num-5)*(i5-1);
//                            progress.setValue(count5);
//                            QVector<int> pos5,inter5;
//                            pos5.push_back(i4);pos5.push_back(i3);
//                            pos5.push_back(i2);pos5.push_back(i1);
//                            inter5.push_back(tag4);inter5.push_back(tag3);
//                            inter5.push_back(tag2);inter5.push_back(tag1);
//                            if(i5 == i4 || i5 == i3 || i5 == i2 || i5 == i1)
//                            {
//                                if(i5 == num)//已经循环到最后
//                                    recoverTo(pos5,inter5,num);
//                                continue;
//                            }
//                            //进度条显示
//                            tag5 = judgeInterFere(i5);
//                            if(tag5 == -1)
//                            {
//                                if(i5 == num)//已经循环到最后
//                                    recoverTo(pos5,inter5,num);
//                                continue;
//                            }
//                            if(num == 5)    //折弯段有5段
//                            {
//                                //折弯数为5时的处理函数
//                                QVector<int> pos,inter;
//                                pos.push_back(i5);pos.push_back(i4);pos.push_back(i3);
//                                pos.push_back(i2);pos.push_back(i1);
//                                inter.push_back(tag5);inter.push_back(tag4);inter.push_back(tag3);
//                                inter.push_back(tag2);inter.push_back(tag1);
//                                //将折弯顺序和调头标志保存到数组中
//                                res_order.push_back(pos);
//                                res_tag.push_back(inter);
//                                recoverTo(pos,inter,num);
//                                continue;
//                            }
//                            for(int i6 = 1; i6 <= num; i6++)
//                            {
//                                int count6 = count5+pow(num,num-6)*(i6-1);
//                                progress.setValue(count6);
//                                QVector<int> pos6,inter6;
//                                pos6.push_back(i5);pos6.push_back(i4);
//                                pos6.push_back(i3);pos6.push_back(i2);pos6.push_back(i1);
//                                inter6.push_back(tag5);inter6.push_back(tag4);
//                                inter6.push_back(tag3);inter6.push_back(tag2);inter6.push_back(tag1);
//                                if(i6 == i5 || i6 == i4 || i6 == i3 || i6 == i2 || i6 == i1)
//                                {
//                                    if(i6 == num)//需要进行判断,折弯数据将回退到哪个折弯点
//                                        recoverTo(pos6,inter6,num);
//                                    continue;
//                                }
//                                //进度条显示
//                                tag6 = judgeInterFere(i6);
//                                if(tag6 == -1)
//                                {
//                                    if(i6 == num)//已经循环到最后
//                                        recoverTo(pos6,inter6,num);
//                                    continue;
//                                }
//                                if(num == 6)
//                                {
//                                    //折弯数为6时的处理函数
//                                    QVector<int> pos,inter;
//                                    pos.push_back(i6);pos.push_back(i5);pos.push_back(i4);
//                                    pos.push_back(i3);pos.push_back(i2);pos.push_back(i1);
//                                    inter.push_back(tag6);inter.push_back(tag5);inter.push_back(tag4);
//                                    inter.push_back(tag3);inter.push_back(tag2);inter.push_back(tag1);
//                                    //将折弯顺序和调头标志保存到数组中
//                                    res_order.push_back(pos);
//                                    res_tag.push_back(inter);
//                                    recoverTo(pos,inter,num);
//                                    continue;
//                                }
//                            }
//                        }
//                    }
//                }
//            }
//        }
//        progress.setValue(sum);
//        int final_count = res_order.size();
//        if(final_count == 0)    //未找到合适的折弯
//        {
//            pOrderCur = pOrderHead;
//            return -1;
//        }
//        int **final_order = new int *[final_count];
//        int **final_tag = new int *[final_count];
//        for(int i = 0; i < final_count; i++)
//        {
//            final_order[i] = new int[bendnum];
//            final_tag[i] = new int[bendnum];
//        }
//        for(int i = 0; i < final_count; i++)
//        {
//            int size = res_order[i].size();
//            for(int j = size-1; j >= 0; j--)
//            {
//                final_order[i][size-1-j] = res_order[i][j];
//                final_tag[i][size-1-j] = res_tag[i][j];
//            }
//        }
//        formPuchBendData(Y_Pos-Y_open);
//        int optimum = computeOptimumSolution(parent,final_order,final_tag,final_count,bendnum);
//        //初始化计算结果的第一组数据
//        formBendOrder(final_order[optimum],final_tag[optimum]);
//        convertToBendPos(pOrderCur,pBendHead);
//        bendFlag = false;
//        computeBackGaungPos(pOrderCur,pBendHead);

//        //需要析构之前申请的内存
//        for(int i = 0; i < final_count; i++)
//        {
//            delete []final_order[i];
//            delete []final_tag[i];
//        }
//        delete []final_order;
//        delete []final_tag;
//    }
//    //****************7~15道折弯以上的搜索方式**************************/
//    else            //遗传算法求解
//    {
//        //*************初始种群******************************/
//        //首先让钣金成折弯状态
//        resetToBend();
//        for(int i = 0; i < M; i++){
//            convertBackUpToBend(i);
//        }
//        //找出最后一道可行的折弯序号
//        bendFlag = true;
//        QVector<int> last_num;
//        for(int i = 1; i <= bendnum; i++){
//            bend_order *pCur = findBendOrder(i,pOrderHead);
//            convertToBendPos(pCur,pBendHead);
//            if(judgeInterFere_inherit(i,pBendHead,pOrderHead) != -1)
//                last_num.push_back(i);
//        }
//        int count = last_num.size();
//        int *record = new int[bendnum];
//        //最后一道折弯只能在last_num中选择
//        QVector<QVector<int> > population;
//        QVector<QVector<int> > inter_flag; //记录交叉变异过程中因干涉无法完成的标志位
//        for(int i = 0; i < M; i++){
//            QVector<int> tmp;
//            QVector<int> tmp_inter;
//            for(int j = 0; j < bendnum; j++){
//                record[j] = 0;
//                tmp_inter.push_back(0);
//            }
//            int last = qrand()%count;
//            tmp.push_back(last_num[last]);
//            record[last_num[last]-1] = 1;
//            while(tmp.size() < bendnum){
//                int num = qrand()%bendnum;
//                if(record[num] == 0){
//                    record[num] = 1;
//                    tmp.push_back(num+1);
//                }
//            }
//            population.push_back(tmp);
//            inter_flag.push_back(tmp_inter);
//        }
//        //**************************************************/
//        //********进行选择,交叉,变异操作***********************/
//        for(int i = 0; i < T; i++){
//            population = pick(population);
//            cross(population,inter_flag);
//        }
//        int min = INT_MAX;
//        int index = 0;
//        for(int i = 0; i < M; i++){
//            int suit = computeSuit(population[i],i);
//            if(min > suit){
//                index = i;
//                min = suit;
//            }
//        }
//        pBendHead = pBend_Backup[index];
//        bend_order *porder_head = pOrder_Backup[index];

//        for(int i = 0; i < bendnum; i++){
//            bend_order *pCur = findBendOrder(population[index][i],porder_head);
//            convertToBendPos(pCur,pBendHead);
//            bend_surface *pbend = findBendSurface(population[index][i],pBendHead);
//            //生成上模和滑块的折弯模型数据
//            pbend->isBended = 0;
//            pbend->pParallel->isBended = 0;
//            resetVisitFlag(pBendHead);
//            MatrixPartConvert(pbend->pLeftInverse,pBendHead,pbend,pbend->pLeftBase,false);
//            MatrixPartConvert(pbend->pParallel->pLeftInverse,pBendHead,pbend->pParallel,pbend->pParallel->pLeftBase,true);
//            MatrixPartConvert(pbend->pRightInverse,pBendHead,pbend,pbend->pRightBase,false);
//            MatrixPartConvert(pbend->pParallel->pRightInverse,pBendHead,pbend->pParallel,pbend->pParallel->pRightBase,true);
//            resetVisitFlag(pBendHead);
//            bendFlag = false;   //重置为回程状态
//        }
//        pOrderCur = pOrderHead;
//        for(int i = bendnum-1; i >= 0; i--){
//            bend_order *porder = findBendOrder(population[index][i],porder_head);
//            pOrderCur->bendWidth = porder->bendWidth;
//            pOrderCur->bendpoint = porder->bendpoint;
//            pOrderCur->backgauge_point = porder->backgauge_point;
//            pOrderCur->bendAngle = porder->bendAngle;
//            pOrderCur->bendRadius = porder->bendRadius;
////            if(porder->direction_h == 1)
////                pOrderCur->direction_h = 0;
////            else if(porder->direction_h == 0)
////                pOrderCur->direction_h = 1;
//            pOrderCur->direction_h = porder->direction_h;
//            pOrderCur->direction_v = porder->direction_v;
//            pOrderCur->tool_id = porder->tool_id;
//            pOrderCur->y_open = porder->y_open;
//            pOrderCur = pOrderCur->pNext;
//        }
//        pOrderCur = pOrderHead;
//        formPuchBendData(Y_Pos-pOrderCur->y_open);
////        pOrderCur = pOrderHead;
////        convertToBendPos(pOrderCur,pBendHead);
//        //updateBackGaungPos(pOrderCur);
//        return 0;   //计算出可行的折弯工序
//    }

//    return 0;   //计算出可行的折弯工序
//}

/********************************************
 *function:绘制折弯过程
 *adding:
 *author: xu
 *date: 2015/10/18
 *******************************************/
QVector<int> fileOperate::drawBendAction()
{
//    if(bRun == true)   //仿真运动中,则暂时不进行折弯操作
//        return;
//    if(pOrderCur == pOrderHead && bendFlag == false){
//        convertToBendPos(pOrderCur,pBendHead);
//    }
    QVector<int> result;
    if(bendFlag == false)   //回程状态下,执行折弯操作
    {
        formBendedData(pOrderCur->bendpoint,false,pBendHead);
        formBendedData(pOrderCur->bendpoint,true,pBendHead);
        formBendSideSurface(pOrderCur->bendpoint,pBendHead);
        formPuchBendData(pOrderCur->position_y1);
        bendFlag = true;    //重置为折弯状态
        updateBackGaungPos(pOrderCur);
        for(int i = 0; i < pPunch.size(); i++)
        {
            if(InterFereWithTools(pPunch[i],pBendHead))
            {
                result.push_back(1);
                qDebug()<<"InterFereWithPunch";
            }
            if(InterFereWithTools(pDie[i],pBendHead))
            {
                result.push_back(2);
                qDebug()<<"InterFereWithDie";
            }
        }
        if(InterFereWithTools(pUpper,pBendHead))
        {
            result.push_back(3);
            qDebug()<<"InterFereWithUPPER";
        }
        if(InterFereWithTools(pLower,pBendHead))
        {
            result.push_back(4);
            qDebug()<<"InterFereWithLOWER";
        }
    }
    else                    //折弯状态下，执行回程操作
    {
        if(pOrderCur->pNext == NULL)
        {
            result.push_back(-1);
            return result;
        }
        //生成上模和滑块的回程模型数据
        formPuchBendData(Y_Pos-pOrderCur->y_open);
        pOrderCur = pOrderCur->pNext;//转到下一道折弯
        convertToBendPos(pOrderCur,pBendHead);
        bendFlag = false;   //重置为回程状态
        updateBackGaungPos(pOrderCur);
        for(int i = 0; i < pPunch.size(); i++)
        {
            if(InterFereWithTools(pPunch[i],pBendHead))
            {
                result.push_back(1);
                qDebug()<<"InterFereWithPunch";
            }
            if(InterFereWithTools(pDie[i],pBendHead))
            {
                result.push_back(2);
                qDebug()<<"InterFereWithDie";
            }
        }
        if(InterFereWithTools(pUpper,pBendHead))
        {
            result.push_back(3);
            qDebug()<<"InterFereWithUPPER";
        }
        if(InterFereWithTools(pLower,pBendHead))
        {
            result.push_back(4);
            qDebug()<<"InterFereWithLOWER";
        }
    }
    return result;
}

QVector<int> fileOperate::drawBendAction(bend_order *porder)
{
    //    if(bRun == true)   //仿真运动中,则暂时不进行折弯操作
    //        return;
//    QVector<int> result;
//    if(bendFlag == false)   //回程状态下,执行折弯操作
//    {
//        formBendedData(porder->bendpoint,false,pBendHead);
//        formBendedData(porder->bendpoint,true,pBendHead);
//        formBendSideSurface(porder->bendpoint,pBendHead);
//        formPuchBendData(porder->position_y1);
//        bendFlag = true;    //重置为折弯状态
//        updateBackGaungPos();
//        for(int i = 0; i < pPunch.size(); i++)
//        {
//            if(InterFereWithTools(pPunch[i],pBendHead))
//            {
//                result.push_back(1);
//                qDebug()<<"InterFereWithPunch";
//            }
//            if(InterFereWithTools(pDie[i],pBendHead))
//            {
//                result.push_back(2);
//                qDebug()<<"InterFereWithDie";
//            }
//        }
//        if(InterFereWithTools(pUpper,pBendHead))
//        {
//            result.push_back(3);
//            qDebug()<<"InterFereWithUPPER";
//        }
//        if(InterFereWithTools(pLower,pBendHead))
//        {
//            result.push_back(4);
//            qDebug()<<"InterFereWithLOWER";
//        }
//    }
//    else                    //折弯状态下，执行回程操作
//    {
//        if(pOrderCur->pNext == NULL)
//        {
//            result.push_back(-1);
//            return result;
//        }
//        //生成上模和滑块的回程模型数据
//        formPuchBendData(Y_Pos-porder->y_open);
//        porder = porder->pNext;//转到下一道折弯
//        convertToBendPos(pOrderCur,pBendHead);
//        bendFlag = false;   //重置为回程状态
//        updateBackGaungPos();
//        for(int i = 0; i < pPunch.size(); i++)
//        {
//            if(InterFereWithTools(pPunch[i],pBendHead))
//            {
//                result.push_back(1);
//                qDebug()<<"InterFereWithPunch";
//            }
//            if(InterFereWithTools(pDie[i],pBendHead))
//            {
//                result.push_back(2);
//                qDebug()<<"InterFereWithDie";
//            }
//        }
//        if(InterFereWithTools(pUpper,pBendHead))
//        {
//            result.push_back(3);
//            qDebug()<<"InterFereWithUPPER";
//        }
//        if(InterFereWithTools(pLower,pBendHead))
//        {
//            result.push_back(4);
//            qDebug()<<"InterFereWithLOWER";
//        }
//    }
    //    return result;
}

void fileOperate::drawToBefore(int sourceNum,int destNum)
{
    bend_surface *pbend = findBendSurface(sourceNum,pBendHead);
    if(bendFlag == true)
    {
        bend_order *pCur = findBendOrder(sourceNum,pOrderHead);
        formPuchBendData(Y_Pos-pCur->y_open);
        pbend->isBended = 0;
        pbend->pParallel->isBended = 0;
        resetVisitFlag(pBendHead);
        MatrixPartConvert(pbend->pLeftInverse,pBendHead,pbend,pbend->pLeftBase,false);
        MatrixPartConvert(pbend->pParallel->pLeftInverse,pBendHead,pbend->pParallel,pbend->pParallel->pLeftBase,true);
        MatrixPartConvert(pbend->pRightInverse,pBendHead,pbend,pbend->pRightBase,false);
        MatrixPartConvert(pbend->pParallel->pRightInverse,pBendHead,pbend->pParallel,pbend->pParallel->pRightBase,true);
        resetVisitFlag(pBendHead);
        bendFlag = false;   //重置为回程状态
    }
    //回到上一道折弯状态
    MatrixTotalConvert(pBendHead,pbend->pInverse,false);
    MatrixTotalConvert(pBendHead,pbend->pInverse,true);
    //回到上一道展开状态
    bend_surface *pDestBend = findBendSurface(destNum,pBendHead);
    bend_order *pDestOrder = findBendOrder(destNum,pOrderHead);
    formPuchBendData(Y_Pos-pDestOrder->y_open);
    pDestBend->isBended = 0;
    pDestBend->pParallel->isBended = 0;
    resetVisitFlag(pBendHead);
    MatrixPartConvert(pDestBend->pLeftInverse,pBendHead,pDestBend,pDestBend->pLeftBase,false);
    MatrixPartConvert(pDestBend->pParallel->pLeftInverse,pBendHead,pDestBend->pParallel,pDestBend->pParallel->pLeftBase,true);
    MatrixPartConvert(pDestBend->pRightInverse,pBendHead,pDestBend,pDestBend->pRightBase,false);
    MatrixPartConvert(pDestBend->pParallel->pRightInverse,pBendHead,pDestBend->pParallel,pDestBend->pParallel->pRightBase,true);
    resetVisitFlag(pBendHead);
    bendFlag = false;   //重置为回程状态
    updateBackGaungPos(pDestOrder);
}

void fileOperate::convertToPoint(int sourceRow,int destRow)
{
    if(sourceRow == destRow)    //没有变化直接返回
        return;
    if(bendFlag)   //折弯状态下，执行回程操作0
    {
        formPuchBendData(Y_Pos-Y_open);
    }
    int index = sourceRow;
    if(sourceRow > destRow)     //需要回退到之前的折弯
    {
        while(1){
            int sourceNum = ManualOrder[index];
            if(bendFlag == false)
            {
                if(index == destRow){
                    bend_order *porder = findBendOrder(sourceNum,pOrderHead);
                    updateBackGaungPos(porder);
                    break;
                }
                bend_surface *pbend = findBendSurface(sourceNum,pBendHead);
                MatrixTotalConvert(pBendHead,pbend->pInverse,false);
                MatrixTotalConvert(pBendHead,pbend->pInverse,true);
                index--;
                bendFlag = true;    //重置为折弯状态
            }
            else
            {
                bend_surface *pbend = findBendSurface(sourceNum,pBendHead);
                pbend->isBended = 0;
                pbend->pParallel->isBended = 0;
                resetVisitFlag(pBendHead);
                MatrixPartConvert(pbend->pLeftInverse,pBendHead,pbend,pbend->pLeftBase,false);
                MatrixPartConvert(pbend->pParallel->pLeftInverse,pBendHead,pbend->pParallel,pbend->pParallel->pLeftBase,true);
                MatrixPartConvert(pbend->pRightInverse,pBendHead,pbend,pbend->pRightBase,false);
                MatrixPartConvert(pbend->pParallel->pRightInverse,pBendHead,pbend->pParallel,pbend->pParallel->pRightBase,true);
                resetVisitFlag(pBendHead);
                bendFlag = false;   //重置为回程状态
            }
        }
    }
    else                     //需要继续折弯
    {
        if(bendFlag){
            index++;    //如果当前折弯已经折完,变换到下一道折弯
        }
        while(1){
            int sourceNum = ManualOrder[index];
            if(bendFlag == false)
            {
                if(index == destRow)
                    break;
                bend_order *pCur = findBendOrder(sourceNum,pOrderHead);
                formBendedData(pCur->bendpoint,false,pBendHead);
                formBendedData(pCur->bendpoint,true,pBendHead);
                formBendSideSurface(pCur->bendpoint,pBendHead);
                bendFlag = true;    //重置为折弯状态
                index++;
            }
            else
            {
                bend_order *pCur = findBendOrder(sourceNum,pOrderHead);
                convertToBendPos(pCur,pBendHead);
                bendFlag = false;   //重置为回程状态
                updateBackGaungPos(pCur);
            }
        }
    }
    bendFlag = false;
}

void fileOperate::resetWorkPos()
{
    int num = ManualOrder[0];
    bend_order *pCur = findBendOrder(num,pOrderHead);
    convertToBendPos(pCur,pBendHead);
    bendFlag = false;
}

QVector<int> fileOperate::drawToMachine(int bendnum)
{
    QVector<int> result;
    bend_order *pCur = findBendOrder(bendnum,pOrderHead);
    convertToBendPos(pCur,pBendHead);
    formPuchBendData(pCur->y_open);
    bendFlag = false;   //重置为回程状态
    updateBackGaungPos(pCur);
    for(int i = 0; i < pPunch.size(); i++)
    {
        if(InterFereWithTools(pPunch[i],pBendHead))
        {
            result.push_back(1);
            qDebug()<<"InterFereWithPunch";
        }
        if(InterFereWithTools(pDie[i],pBendHead))
        {
            result.push_back(2);
            qDebug()<<"InterFereWithDie";
        }
    }
    if(InterFereWithTools(pUpper,pBendHead))
    {
        result.push_back(3);
        qDebug()<<"InterFereWithUPPER";
    }
    if(InterFereWithTools(pLower,pBendHead))
    {
        result.push_back(4);
        qDebug()<<"InterFereWithLOWER";
    }
    return result;
}

QVector<int> fileOperate::drawToBend(int bendnum)
{
    QVector<int> result;
    if(bendFlag == false)
    {
        bend_order *pCur = findBendOrder(bendnum,pOrderHead);
        formBendedData(pCur->bendpoint,false,pBendHead);
        formBendedData(pCur->bendpoint,true,pBendHead);
        formBendSideSurface(pCur->bendpoint,pBendHead);
        formPuchBendData(Y_bendDepth(pCur->bendAngle));
        bendFlag = true;    //重置为折弯状态
        updateBackGaungPos(pCur);
        for(int i = 0; i < pPunch.size(); i++)
        {
            if(InterFereWithTools(pPunch[i],pBendHead))
            {
                result.push_back(1);
                qDebug()<<"InterFereWithPunch";
            }
            if(InterFereWithTools(pDie[i],pBendHead))
            {
                result.push_back(2);
                qDebug()<<"InterFereWithDie";
            }
        }
        if(InterFereWithTools(pUpper,pBendHead))
        {
            result.push_back(3);
            qDebug()<<"InterFereWithUPPER";
        }
        if(InterFereWithTools(pLower,pBendHead))
        {
            result.push_back(4);
            qDebug()<<"InterFereWithLOWER";
        }
    }
    return result;
}

QVector<int> fileOperate::drawToDeploy(int bendnum)
{
    QVector<int> result;
    if(bendFlag == true)
    {
        bend_surface *pbend = findBendSurface(bendnum,pBendHead);
        bend_order *pCur = findBendOrder(bendnum,pOrderHead);
        formPuchBendData(Y_Pos-pCur->y_open);
        pbend->isBended = 0;
        pbend->pParallel->isBended = 0;
        resetVisitFlag(pBendHead);
        MatrixPartConvert(pbend->pLeftInverse,pBendHead,pbend,pbend->pLeftBase,false);
        MatrixPartConvert(pbend->pParallel->pLeftInverse,pBendHead,pbend->pParallel,pbend->pParallel->pLeftBase,true);
        MatrixPartConvert(pbend->pRightInverse,pBendHead,pbend,pbend->pRightBase,false);
        MatrixPartConvert(pbend->pParallel->pRightInverse,pBendHead,pbend->pParallel,pbend->pParallel->pRightBase,true);
        resetVisitFlag(pBendHead);
        bendFlag = false;   //重置为回程状态
        updateBackGaungPos(pCur);
        for(int i = 0; i < pPunch.size(); i++)
        {
            if(InterFereWithTools(pPunch[i],pBendHead))
            {
                result.push_back(1);
                qDebug()<<"InterFereWithPunch";
            }
            if(InterFereWithTools(pDie[i],pBendHead))
            {
                result.push_back(2);
                qDebug()<<"InterFereWithDie";
            }
        }
        if(InterFereWithTools(pUpper,pBendHead))
        {
            result.push_back(3);
            qDebug()<<"InterFereWithUPPER";
        }
        if(InterFereWithTools(pLower,pBendHead))
        {
            result.push_back(4);
            qDebug()<<"InterFereWithLOWER";
        }
    }
    return result;
}

/********************************************
 *function:绘制展开过程
 *adding:
 *author: xu
 *date: 2015/10/31
 *******************************************/
void fileOperate::drawDeployAction()
{
    if(bendFlag == false)   //回程状态下,执行展开操作
    {
        if(pOrderCur == pOrderHead)//当前状态下无法展开
            return;
        bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
        MatrixTotalConvert(pBendHead,pbend->pInverse,false);
        MatrixTotalConvert(pBendHead,pbend->pInverse,true);
        pOrderCur = pOrderCur->pBefore;
        pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
        formPuchBendData(pOrderCur->position_y1);
        bendFlag = true;    //重置为折弯状态
        updateBackGaungPos(pOrderCur);
    }
    else                    //折弯状态下，执行展开操作
    {
        if(pOrderCur == NULL)
            return;
        bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
        //生成上模和滑块的折弯模型数据
        //Y_depth = Y_bendDepth(pbend->bendAngle);
        formPuchBendData(Y_Pos-pOrderCur->y_open);
        pbend->isBended = 0;
        pbend->pParallel->isBended = 0;
        resetVisitFlag(pBendHead);
        MatrixPartConvert(pbend->pLeftInverse,pBendHead,pbend,pbend->pLeftBase,false);
        MatrixPartConvert(pbend->pParallel->pLeftInverse,pBendHead,pbend->pParallel,pbend->pParallel->pLeftBase,true);
        MatrixPartConvert(pbend->pRightInverse,pBendHead,pbend,pbend->pRightBase,false);
        MatrixPartConvert(pbend->pParallel->pRightInverse,pBendHead,pbend->pParallel,pbend->pParallel->pRightBase,true);
        resetVisitFlag(pBendHead);
        bendFlag = false;   //重置为回程状态
        updateBackGaungPos(pOrderCur);
    }
}

bool fileOperate::getBendedState()
{
    return bendFlag;
}

void fileOperate::destroyWorkPiece()
{
    if(pBendHead)
    {
        destroyTrimmed(pWorkPiece);
        destroyTrimmed(pWorkPiece_Deploy);
        destroyBendData(pBendHead);
        destroyBendData(pDeployHead);
        destroyEntity(pGL_WorkPiece);
        destroyEntity(pGL_WorkPiece_Deploy);
        //析构折弯工序链表
        pOrderCur = pOrderHead;
        while (pOrderCur) {
            pOrderHead = pOrderCur->pNext;
            delete pOrderCur;
            pOrderCur = pOrderHead;
        }
        pOrderHead = NULL;
        pOrderCur = NULL;
        pOrderLast = NULL;
        pWorkPiece = NULL;
        pGL_WorkPiece = NULL;
        pBendHead = NULL;
        pDeployHead = NULL;
    }
}

void fileOperate::destroyClass()
{
    destroyWorkPiece();
    if(pMachine)
    {
        destroyTrimmed(pMachine);
        destroyEntity(pGL_Machine);
    }
    if(pUpper)
    {
        destroyTrimmed(pUpper);
        destroyEntity(pGL_Upper);
    }
    if(pLower)
    {
        destroyTrimmed(pLower);
        destroyEntity(pGL_Lower);
    }
    if(pGaung_XL)
    {
        destroyTrimmed(pGaung_XL);
        destroyEntity(pGL_Gaung_XL);
    }
    if(pGaung_RL)
    {
        destroyTrimmed(pGaung_RL);
        destroyEntity(pGL_Gaung_RL);
    }
    if(pGaung_ZL)
    {
        destroyTrimmed(pGaung_ZL);
        destroyEntity(pGL_Gaung_ZL);
    }
    if(pGaung_XR)
    {
        destroyTrimmed(pGaung_XR);
        destroyEntity(pGL_Gaung_XR);
    }
    if(pGaung_RR)
    {
        destroyTrimmed(pGaung_RR);
        destroyEntity(pGL_Gaung_RR);
    }
    if(pGaung_ZR)
    {
        destroyTrimmed(pGaung_ZR);
        destroyEntity(pGL_Gaung_ZR);
    }
}

/********************************************
 *function:判断某道折弯是否发生干涉
 *adding: 返回值: 返回-1发生了干涉,返回其他值都表明有一种可以不干涉
 *        返回0无需调头不干涉,返回1调头后不干涉
 *author: xu
 *date: 2015/12/28
 *******************************************/
int fileOperate::judgeInterFere(int bendnum)
{
    if(bendFlag)
    {
        formPuchBendData(Y_Pos-Y_open);
        bendFlag = false;
    }
    bend_order *ptmp = pOrderHead;
    while (ptmp) {
        if(ptmp->bendpoint == bendnum)
            break;
        ptmp = ptmp->pNext;
    }
    convertToBendPos(ptmp,pBendHead);
    bend_surface *pbend = findBendSurface(bendnum,pBendHead);
    float width = pbend->bendWidth;
    int num = toolWidth.size();
    int m = 0;
    for(m = 0; m < num; m++)
    {
        if(fabs(width-toolWidth[m]) < tool_margin)
            break;
    }
    bool res1 = InterFereWithTools(pPunch[m],pBendHead);
    bool res2 = InterFereWithTools(pDie[m],pBendHead);
    bool res3 = InterFereWithTools(pUpper,pBendHead);
    bool res4 = InterFereWithTools(pLower,pBendHead);
    //生成上模和滑块的折弯模型数据
    float y_off = Y_bendDepth(pbend->bendAngle);
    formPuchBendData(y_off);
    formBendedData(bendnum,false,pBendHead);
    formBendedData(bendnum,true,pBendHead);
    formBendSideSurface(bendnum,pBendHead);
    bendFlag = true;    //在折弯状态
    bool res5 = InterFereWithTools(pPunch[m],pBendHead);
    bool res6 = InterFereWithTools(pDie[m],pBendHead);
    bool res7 = InterFereWithTools(pUpper,pBendHead);
    bool res8 = InterFereWithTools(pLower,pBendHead);
    //此处加上对后挡料的控制,优先考虑使板料较长的一段放在机床的外侧
    bend_surface *ptmp_bend = pBendHead;
    int max = 0;
    int min = 0;
    while (ptmp_bend) {
        trimmed_surface_type *pleft = ptmp_bend->pLeftBase->pSurface;
        trimmed_surface_type *pright = ptmp_bend->pRightBase->pSurface;
        for(int i = 0; i < pleft->outlineCount[0]; i++)
        {
            int type = pleft->outlineType[0][i];
            if(type == 110)
            {
                line_type *pline = (line_type *)pleft->pOutlines[0][i];
                if(pline->start.z > max)
                    max = pline->start.z;
                if(pline->start.z < min)
                    min = pline->start.z;
                if(pline->end.z > max)
                    max = pline->end.z;
                if(pline->end.z < min)
                    min = pline->end.z;
            }
            else if(126 == type)
            {
                nurbsCurve_type *pcurve = (nurbsCurve_type *)pleft->pOutlines[0][i];
                if(pcurve->K_num != 1)
                    continue;
                if(pcurve->pCtlarray[2] > max)
                    max = pcurve->pCtlarray[2];
                if(pcurve->pCtlarray[2] < min)
                    min = pcurve->pCtlarray[2];
                if(pcurve->pCtlarray[5] > max)
                    max = pcurve->pCtlarray[5];
                if(pcurve->pCtlarray[5] < min)
                    min = pcurve->pCtlarray[5];
            }
        }
        for(int i = 0; i < pright->outlineCount[0]; i++)
        {
            int type = pright->outlineType[0][i];
            if(type == 110)
            {
                line_type *pline = (line_type *)pright->pOutlines[0][i];
                if(pline->start.z > max)
                    max = pline->start.z;
                if(pline->start.z < min)
                    min = pline->start.z;
                if(pline->end.z > max)
                    max = pline->end.z;
                if(pline->end.z < min)
                    min = pline->end.z;
            }
            else if(126 == type)
            {
                nurbsCurve_type *pcurve = (nurbsCurve_type *)pright->pOutlines[0][i];
                if(pcurve->K_num != 1)
                    continue;
                if(pcurve->pCtlarray[2] > max)
                    max = pcurve->pCtlarray[2];
                if(pcurve->pCtlarray[2] < min)
                    min = pcurve->pCtlarray[2];
                if(pcurve->pCtlarray[5] > max)
                    max = pcurve->pCtlarray[5];
                if(pcurve->pCtlarray[5] < min)
                    min = pcurve->pCtlarray[5];
            }
        }
        ptmp_bend = ptmp_bend->pNext;
    }
    //有干涉,则调头后继续检测
    if(res1 || res2 || res3 || res4 || res5 || res6 || res7 || res8 || (fabs(max) < fabs(min)))
    {
        //首先应当将折弯数据恢复到之前的状态
        recoverBendData(bendnum,pBendHead,0);
        formPuchBendData(Y_Pos-Y_open);
        bendFlag = false;
        //重新检验,是否干涉
        convertToBendPos(ptmp,pBendHead);
        //开始调头
        xform_type *matrix = formTurnAboutMatrix(bendnum,pBendHead);
        MatrixTotalConvert(pBendHead,matrix,false);
        MatrixTotalConvert(pBendHead,matrix,true);  //调头
        delete matrix;
        res1 = InterFereWithTools(pPunch[m],pBendHead);
        res2 = InterFereWithTools(pDie[m],pBendHead);
        res3 = InterFereWithTools(pUpper,pBendHead);
        res4 = InterFereWithTools(pLower,pBendHead);
        formPuchBendData(y_off);
        formBendedData(bendnum,false,pBendHead);
        formBendedData(bendnum,true,pBendHead);
        formBendSideSurface(bendnum,pBendHead);
        bendFlag = true;  //在折弯状态
        res5 = InterFereWithTools(pPunch[m],pBendHead);
        res6 = InterFereWithTools(pDie[m],pBendHead);
        res7 = InterFereWithTools(pUpper,pBendHead);
        res8 = InterFereWithTools(pLower,pBendHead);
        //有干涉,返回之前需要变换为上一折弯点位置
        if(res1 || res2 || res3 || res4 || res5 || res6 || res7 || res8)
        {
            recoverBendData(bendnum,pBendHead,1); //这里同时需要调头
            formPuchBendData(Y_Pos-Y_open);
            bendFlag = false;
            return -1;    //调头前后都干涉,则返回
        }
        return 1;   //调头后不干涉
    }
    return 0;       //无需调头也不干涉
}

int fileOperate::judgeInterFere_inherit(int bendnum,bend_surface *pbend_head,bend_order *porder_head)
{
    bend_order *pCur = findBendOrder(bendnum,porder_head);
    bend_surface *pbend = findBendSurface(bendnum,pbend_head);
    float y_off = Y_bendDepth(pbend->bendAngle);
    bool res1 = InterFereWithTools(pPunch[pCur->tool_id-1],pbend_head);
    bool res2 = InterFereWithTools(pDie[pCur->tool_id-1],pbend_head);
    bool res3 = InterFereWithTools(pUpper,pbend_head);
    bool res4 = InterFereWithTools(pLower,pbend_head);
    //展开后重新判断
    formPuchBendData(Y_Pos-pCur->y_open);
    pbend->isBended = 0;
    pbend->pParallel->isBended = 0;
    resetVisitFlag(pbend_head);
    MatrixPartConvert(pbend->pLeftInverse,pbend_head,pbend,pbend->pLeftBase,false);
    MatrixPartConvert(pbend->pParallel->pLeftInverse,pbend_head,pbend->pParallel,pbend->pParallel->pLeftBase,true);
    MatrixPartConvert(pbend->pRightInverse,pbend_head,pbend,pbend->pRightBase,false);
    MatrixPartConvert(pbend->pParallel->pRightInverse,pbend_head,pbend->pParallel,pbend->pParallel->pRightBase,true);
    resetVisitFlag(pbend_head);
    bendFlag = false;   //重置为回程状态
    bool res5 = InterFereWithTools(pPunch[pCur->tool_id-1],pbend_head);
    bool res6 = InterFereWithTools(pDie[pCur->tool_id-1],pbend_head);
    bool res7 = InterFereWithTools(pUpper,pbend_head);
    bool res8 = InterFereWithTools(pLower,pbend_head);
    if(res1 || res2 || res3 || res4 || res5 || res6 || res7 || res8){
        //说明存在干涉,调头后重新判断
        xform_type *matrix = formTurnAboutMatrix(bendnum,pbend_head);
        MatrixTotalConvert(pbend_head,matrix,false);
        MatrixTotalConvert(pbend_head,matrix,true);  //调头
        delete matrix;
        res1 = InterFereWithTools(pPunch[pCur->tool_id-1],pbend_head);
        res2 = InterFereWithTools(pDie[pCur->tool_id-1],pbend_head);
        res3 = InterFereWithTools(pUpper,pbend_head);
        res4 = InterFereWithTools(pLower,pbend_head);
        formPuchBendData(y_off);
        formBendedData(bendnum,false,pbend_head);
        formBendedData(bendnum,true,pbend_head);
        formBendSideSurface(bendnum,pbend_head);
        bendFlag = true;  //在折弯状态
        //标记一下当前的调头状态
        if(pCur->direction_h == 1)
            pCur->direction_h = 0;
        else
            pCur->direction_h = 1;
        res5 = InterFereWithTools(pPunch[pCur->tool_id-1],pbend_head);
        res6 = InterFereWithTools(pDie[pCur->tool_id-1],pbend_head);
        res7 = InterFereWithTools(pUpper,pbend_head);
        res8 = InterFereWithTools(pLower,pbend_head);
        //有干涉,返回之前需要变换为上一折弯点位置
        if(res1 || res2 || res3 || res4 || res5 || res6 || res7 || res8)
            return -1;    //调头前后都干涉,则返回
        else
            return 1;   //调头后不干涉
    }
    else{
        formPuchBendData(y_off);
        formBendedData(bendnum,false,pbend_head);
        formBendedData(bendnum,true,pbend_head);
        formBendSideSurface(bendnum,pbend_head);
        bendFlag = true;  //在折弯状态
        return 0;       //无需调头也不干涉
    }
    return 0;
}

/********************************************
 *function:恢复到指定的折弯
 *adding:
 *author: xu
 *date: 2015/12/28
 *******************************************/
void fileOperate::recoverTo(QVector<int> pos, QVector<int> tag, int num)
{
    for(int i = 0; i < pos.size(); i++)
    {
        recoverBendData(pos[i],pBendHead,tag[i]);
        if(pos[i] != num)
        {
            if(bendFlag)    //在折弯状态下
            {
                formPuchBendData(Y_Pos-Y_open);
                bendFlag = false;
            }
            return;
        }
    }
}

/********************************************
 *function:根据求解出来的可行解,将调头,翻转,更换模具等因素考虑
 *         进来,依据各影响因素的系数求出最优解
 *adding:
 *author: xu
 *date: 2016/05/24
 *******************************************/
int fileOperate::computeOptimumSolution(QWidget *parent, int **order, int **tag, int sum, int bendnum)
{
    if(sum == 1)    //如果可行解只有一种,则返回该条记录的序号
        return sum-1;
    ProgressDlg progress(parent,QObject::tr("共计算出%1种可行解,正在寻找最优解......").arg(sum),sum*3,true);
    progress.show();
    progress.setValue(1);
    QVector<float> result;
    QVector<QVector<int> > overTurn;//翻转标志位,1为折弯角度为正,0为折弯角度为负
    QVector<QVector<int> > tool_flag;//记录每道折弯使用哪号模具组
    for(int i = 0; i < sum; i++)
    {
        progress.setValue(i+1);
        QVector<int> tmp_over;
        QVector<int> tmp_tool;
        for(int j = 0; j < bendnum; j++)
        {
            bend_surface *pbend = findBendSurface(order[i][j],pBendHead);
            if(pbend->bendAngle > 0)
                tmp_over.push_back(1);
            else
                tmp_over.push_back(0);
            for(int m = 0; m < toolWidth.size(); m++)
            {
                if(fabs(pbend->bendWidth-toolWidth[m]) < toolMargin[m])
                {
                    tmp_tool.push_back(m);
                    break;
                }
            }
        }
        overTurn.push_back(tmp_over);
        tool_flag.push_back(tmp_tool);
    }
    for(int i = 0; i < sum; i++)
    {
        progress.setValue(sum+i+1);
        float tmp_result = 0;
        for(int j = 1; j < bendnum; j++)
        {
            if(overTurn[i][j-1] != overTurn[i][j])//两次翻转标志位不同,则认为需要翻转开销
                tmp_result += over_ratio;
            if(tag[i][j-1] == 1)    //需要进行调头
                tmp_result += return_ratio;
            if(tool_flag[i][j-1] != tool_flag[i][j])
                tmp_result += tool_ratio;
        }
        result.push_back(tmp_result);
    }
    //将最终算出的结果进行比较,求出值最小的可行解序号
    float min = result[0];
    int record = 0;
    for(int i = 1; i < sum; i++)
    {
        progress.setValue(sum*2+i+1);
        if(min > result[i])
        {
            min = result[i];
            record = i;
        }
    }
    progress.setValue(sum*3);
    return record;
}

/********************************************
 *function:根据计算出的结果生成折弯工序
 *adding:
 *author: xu
 *date: 2016/01/04
 *******************************************/
void fileOperate::formBendOrder(int res[], int tag[])
{
    int bendnum = bendNum;
    bend_order *ptmp = pOrderHead;
    for(int i = 0; i < bendnum; i++)
    {
        ptmp->bend_id = i+1;
        ptmp->bendpoint = res[i];
        bend_surface *pbend = findBendSurface(res[i],pBendHead);
        int j;
        for(j = 0; j < toolWidth.size(); j++)
        {
            if(fabs(pbend->bendWidth-toolWidth[j]) < toolMargin[j])
                break;
        }
        ptmp->tool_id = j+1;
        ptmp->direction_h = tag[i];
        ptmp->y_open = Y_open;
        ptmp->bendAngle = pbend->bendAngle;
        ptmp->bendWidth = pbend->bendWidth;
        ptmp->bendRadius = pbend->bendRadius;
        ptmp = ptmp->pNext;
    }
    pOrderCur = pOrderHead;
}

void fileOperate::formBendOrder(QList<int> res, QList<int> tag,QList<int> tool)
{
    int bendnum = bendNum;
    bend_order *ptmp = pOrderHead;
    for(int i = 0; i < bendnum; i++)
    {
        ptmp->bend_id = i+1;
        ptmp->bendpoint = res[i];
        bend_surface *pbend = findBendSurface(res[i],pBendHead);
        ptmp->tool_id = tool[i];
        ptmp->direction_h = tag[i];
        ptmp->y_open = Y_open;
        ptmp->bendAngle = pbend->bendAngle;
        ptmp->bendWidth = pbend->bendWidth;
        ptmp->bendRadius = pbend->bendRadius;
        ptmp = ptmp->pNext;
    }
    pOrderCur = pOrderHead;
}

/********************************************
 *function:模具选择之后修改折弯工序数据
 *adding:
 *author: xu
 *date: 2016/01/04
 *******************************************/
void fileOperate::bendorder_toolconfig()
{
    bend_order *ptmp = pOrderHead;
    while(ptmp){
        bend_surface *pbend = findBendSurface(ptmp->bendpoint,pBendHead);
        int j;
        for(j = 0; j < toolWidth.size(); j++)
        {
            if(fabs(pbend->bendWidth-toolWidth[j]) < toolMargin[j])
                break;
        }
        ptmp->tool_id = j+1;
        ptmp = ptmp->pNext;
    }
    pOrderCur = pOrderHead;
    ptmp = pOrderHead;
    //为遗传算法复制相同的折弯工序内存
//    for(int i = 0; i < M; i++){
//        ptmp = pOrderHead;
//        bend_order *phead = NULL;
//        bend_order *plast = NULL;
//        while(ptmp){
//            bend_order *porder = new bend_order;
//            memcpy(porder,ptmp,sizeof(bend_order));
//            if(phead == NULL)
//            {
//                phead = porder;
//                plast = porder;
//                phead->pBefore = NULL;
//                plast->pNext = NULL;
//            }
//            else
//            {
//                plast->pNext = porder;
//                porder->pBefore = plast;
//                plast = porder;
//                plast->pNext = NULL;
//            }
//            ptmp = ptmp->pNext;
//        }
//        pOrder_Backup.push_back(phead);
//    }
}

/********************************************
 *function:着弯段模具号更换
 *adding:
 *author: xu
 *date: 2016/01/04
 *******************************************/
void fileOperate::ModelChange(bend_order *pCur, int num)
{
    if(pCur->tool_id == num)
        return;
    pCur->tool_id = num;
    bend_surface *pbend = findBendSurface(pCur->bendpoint,pBendHead);
    //回退到上一步
    MatrixTotalConvert(pBendHead,pbend->pInverse,false);
    MatrixTotalConvert(pBendHead,pbend->pInverse,true);
    convertToBendPos(pCur,pBendHead);
    computeBackGaungPos(pCur,pBendHead);
    bendFlag = false;
}

void fileOperate::converToNum(int num)
{
    bend_order *pCur = findBendOrder(num,pOrderHead);
    convertToBendPos(pCur,pBendHead);

}

void fileOperate::deploy_num(int num)
{
    //bend_order *pCur = findBendOrder(num,pOrderHead);

    bend_surface *pbend = findBendSurface(num,pBendHead);
    //生成上模和滑块的折弯模型数据
    //Y_depth = Y_bendDepth(pbend->bendAngle);
    formPuchBendData(Y_Pos-pOrderCur->y_open);
    pbend->isBended = 0;
    pbend->pParallel->isBended = 0;
    resetVisitFlag(pBendHead);
    MatrixPartConvert(pbend->pLeftInverse,pBendHead,pbend,pbend->pLeftBase,false);
    MatrixPartConvert(pbend->pParallel->pLeftInverse,pBendHead,pbend->pParallel,pbend->pParallel->pLeftBase,true);
    MatrixPartConvert(pbend->pRightInverse,pBendHead,pbend,pbend->pRightBase,false);
    MatrixPartConvert(pbend->pParallel->pRightInverse,pBendHead,pbend->pParallel,pbend->pParallel->pRightBase,true);
    resetVisitFlag(pBendHead);
    bendFlag = false;   //重置为回程状态
    //updateBackGaungPos(pCur);
}

/********************************************
 *function:返回当前位置钣金件的基面中心点
 *input:无
 *output:中心点坐标
 *adding:无
 *author: wang
 *date: 2017-3-30 18:39:20
 *******************************************/
point3f fileOperate::findBasicData(bool isbendDataFlow)
{
    bend_surface *pbend ;
    if(isbendDataFlow)
    {
       pbend = pBendHead;
    }
    else
    {
        pbend = pDeployHead;
    }
    int max = 0;
    basic_surface *result = NULL;
    while(pbend)
    {
        basic_surface *pleft = pbend->pLeftBase;
        basic_surface *pright = pbend->pRightBase;
        if(pleft->pBendSurface.size() > max)
        {
            max = pleft->pBendSurface.size();
            result = pleft;
        }
        if(pright->pBendSurface.size() > max)
        {
            max = pright->pBendSurface.size();
            result = pright;
        }
        pbend = pbend->pNext;
    }

    trimmed_surface_type *ptmp= result->pSurface;
    float generalX = 0,generalY = 0,generalZ = 0;
    point3f centerPoint;
    for(int i = 0; i < ptmp->outlineCount[0]; i++)
    {
        line_type *pline = (line_type *)ptmp->pOutlines[0][i];
        generalX += pline->start.x;
        generalX += pline->end.x;
        generalY += pline->start.y;
        generalY += pline->end.y;
        generalZ += pline->start.z;
        generalZ += pline->end.z;
    }
    centerPoint.x = generalX/(2*ptmp->outlineCount[0]);
    centerPoint.y = generalY/(2*ptmp->outlineCount[0]);
    centerPoint.z = generalZ/(2*ptmp->outlineCount[0]);
    return centerPoint;

}
/********************************************
 *function:返回钣金件的下表面的基面的中心点坐标
 *input:无
 *output:反面基面中心点坐标
 *adding:
 *author: wang
 *date: 2017年5月3日16:19:11
 *******************************************/
point3f fileOperate::findBackBasicData(bool isbendDataFlow)
{
    bend_surface *pbend ;
    if(isbendDataFlow)
    {
       pbend = pBendHead->pParallel;
    }
    else
    {
        pbend = pDeployHead->pParallel;
    }

    int max = 0;
    basic_surface *result = NULL;  //要找出的面积最大（数据量最大的基面）
    while(pbend)
    {
        basic_surface *pleft = pbend->pLeftBase;
        basic_surface *pright = pbend->pRightBase;
        if(pleft->pBendSurface.size() > max)
        {
            max = pleft->pBendSurface.size();
            result = pleft;
        }
        if(pright->pBendSurface.size() > max)
        {
            max = pright->pBendSurface.size();
            result = pright;
        }
        pbend = pbend->pNext;
    }

    trimmed_surface_type *ptmp= result->pSurface;
    float generalX = 0,generalY = 0,generalZ = 0;
    point3f centerPoint;
    for(int i = 0; i < ptmp->outlineCount[0]; i++)
    {
        line_type *pline = (line_type *)ptmp->pOutlines[0][i];
        generalX += pline->start.x;
        generalX += pline->end.x;
        generalY += pline->start.y;
        generalY += pline->end.y;
        generalZ += pline->start.z;
        generalZ += pline->end.z;
    }
    centerPoint.x = generalX/(2*ptmp->outlineCount[0]);
    centerPoint.y = generalY/(2*ptmp->outlineCount[0]);
    centerPoint.z = generalZ/(2*ptmp->outlineCount[0]);
    return centerPoint;
}
/********************************************
 *function:调整初始导入钣金件时的钣金姿态，使其平行于地面
 *input:位姿调整矩阵
 *output:无
 *adding:flag为true 对折弯链表调整姿态   false对展开链表调整姿态
 *author: wang
 *date: 2017年5月3日17:12:22
 *******************************************/
void fileOperate::adjustWorkpiece(xform_type rotateMatrix, bool flag)
{
    if(flag)
    {
        MatrixTotalConvert(pBendHead,&rotateMatrix,false);
        MatrixTotalConvert(pBendHead,&rotateMatrix,true);
    }
   else
    {
        MatrixTotalConvert(pDeployHead,&rotateMatrix,false);
        MatrixTotalConvert(pDeployHead,&rotateMatrix,true);
    }
}

/********************************************
 *function:查找基面多边形的最大长度
 *input:无
 *output:返回最大长度
 *adding:
 *author: wang
 *date: 2017年5月4日10:18:19
 *******************************************/
float fileOperate::findBsicSurfaceMaxLength()
{
    bend_surface *pbend = pBendHead;
    int max = 0;
    basic_surface *result = NULL;  //要找出的面积最大（数据量最大的基面）
    while(pbend)
    {
        basic_surface *pleft = pbend->pLeftBase;
        basic_surface *pright = pbend->pRightBase;
        if(pleft->pBendSurface.size() > max)
        {
            max = pleft->pBendSurface.size();
            result = pleft;
        }
        if(pright->pBendSurface.size() > max)
        {
            max = pright->pBendSurface.size();
            result = pright;
        }
        pbend = pbend->pNext;
    }

    trimmed_surface_type *ptmp= result->pSurface;
    float generalX = 0,generalY = 0,generalZ = 0;
    float length;
    float  Maxlength = 0;
    for(int i = 0; i < ptmp->outlineCount[0]; i++)
    {
        line_type *pline = (line_type *)ptmp->pOutlines[0][i];
        generalX = pline->start.x-pline->end.x;
        generalY = pline->start.y-pline->end.y;
        generalZ = pline->start.z-pline->end.z;
        length = sqrt(generalX*generalX+generalY*generalY+generalZ*generalZ);
        qDebug()<<"长度: "<<length;
        if(length > Maxlength)
            Maxlength = length;
    }
    return Maxlength;
}

/********************************************
 *function:查找当前折弯点对应的折弯顺序
 *adding:
 *author: xu
 *date: 2016/05/20
 *******************************************/
bend_order *fileOperate::findBendOrder(int bendnum,bend_order *pHead)
{
    if(bendnum < 1 || bendnum > bendNum)
        return NULL;
    bend_order *porder = pHead;
    while (porder) {
        if(porder->bendpoint == bendnum)
            break;
        porder = porder->pNext;
    }
    return porder;
}

/********************************************
 *function:手动调整取消操作
 *adding:
 *author: xu
 *date: 2016/11/17
 *******************************************/
void fileOperate::ManualCancel(int index)
{
    if(ManualOrder.size() <= 0)
        return;
    if(bendFlag)   //折弯状态下，执行回程操作0
    {
        formPuchBendData(Y_Pos-Y_open);
    }
    //将折弯数据返回到初始化的数据
    while(1)
    {
        int num = ManualOrder[index];
        if(bendFlag == false)
        {
            if(index == 0)
                break;
            bend_surface *pbend = findBendSurface(num,pBendHead);
            MatrixTotalConvert(pBendHead,pbend->pInverse,false);
            MatrixTotalConvert(pBendHead,pbend->pInverse,true);
            bendFlag = true;    //重置为折弯状态
            index--;
        }
        else
        {
            bend_surface *pbend = findBendSurface(num,pBendHead);
            pbend->isBended = 0;
            pbend->pParallel->isBended = 0;
            resetVisitFlag(pBendHead);
            MatrixPartConvert(pbend->pLeftInverse,pBendHead,pbend,pbend->pLeftBase,false);
            MatrixPartConvert(pbend->pParallel->pLeftInverse,pBendHead,pbend->pParallel,pbend->pParallel->pLeftBase,true);
            MatrixPartConvert(pbend->pRightInverse,pBendHead,pbend,pbend->pRightBase,false);
            MatrixPartConvert(pbend->pParallel->pRightInverse,pBendHead,pbend->pParallel,pbend->pParallel->pRightBase,true);
            resetVisitFlag(pBendHead);
            bendFlag = false;   //重置为回程状态
        }
    }
    bendFlag = false;
    pOrderCur = pOrderHead;
}

void fileOperate::ManualSure(int index)
{
    ManualCancel(index);
    formBendOrder(ManualOrder,ManualTag,ManualTool);
    pOrderCur = pOrderHead; //当前工序为链表的头结点
    //工件的位置
    convertToBendPos(pOrderCur,pBendHead);
    bendFlag = false;
    //首先让板料折一遍然后再恢复到原始位置
    while(1){
        if(bendFlag == false)   //回程状态下,执行折弯操作
        {
            if(pOrderCur == NULL)
                break;
            //生成上模和滑块的折弯模型数据
            formBendedData(pOrderCur->bendpoint,false,pBendHead);
            formBendedData(pOrderCur->bendpoint,true,pBendHead);
            formBendSideSurface(pOrderCur->bendpoint,pBendHead);
            bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
            pOrderCur->position_y1 = Y_bendDepth(pbend->bendAngle);
            pOrderCur->position_y2 = pOrderCur->position_y1;
            bendFlag = true;    //重置为折弯状态
        }
        else                    //折弯状态下，执行回程操作
        {
            if(pOrderCur->pNext == NULL)
                break;
            //生成上模和滑块的回程模型数据
            pOrderCur = pOrderCur->pNext;//转到下一道折弯
            convertToBendPos(pOrderCur,pBendHead);
            computeBackGaungPos(pOrderCur,pBendHead);
            bendFlag = false;   //重置为回程状态
        }
    }
    while(1){
        if(bendFlag == false)   //回程状态下,执行展开操作
        {
            if(pOrderCur == pOrderHead)//当前状态下无法展开
                break;
            bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
            MatrixTotalConvert(pBendHead,pbend->pInverse,false);
            MatrixTotalConvert(pBendHead,pbend->pInverse,true);
            pOrderCur = pOrderCur->pBefore;
            bendFlag = true;
        }
        else                    //折弯状态下，执行展开操作
        {
            if(pOrderCur == NULL)
                break;
            bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
            //生成上模和滑块的折弯模型数据
            pbend->isBended = 0;
            pbend->pParallel->isBended = 0;
            resetVisitFlag(pBendHead);
            MatrixPartConvert(pbend->pLeftInverse,pBendHead,pbend,pbend->pLeftBase,false);
            MatrixPartConvert(pbend->pParallel->pLeftInverse,pBendHead,pbend->pParallel,pbend->pParallel->pLeftBase,true);
            MatrixPartConvert(pbend->pRightInverse,pBendHead,pbend,pbend->pRightBase,false);
            MatrixPartConvert(pbend->pParallel->pRightInverse,pBendHead,pbend->pParallel,pbend->pParallel->pRightBase,true);
            resetVisitFlag(pBendHead);
            bendFlag = false;   //重置为回程状态
        }
    }
    pOrderCur = pOrderHead;
    updateBackGaungPos(pOrderCur);
}

/********************************************
 *function:计算后挡料位置,这地方得分情况进行讨论
 *adding:
 *author: xu
 *date: 2016/01/19
 *******************************************/
void fileOperate::computeBackGaungPos(bend_order *pOrder,bendSurface *pHead)
{
    bend_surface *pbend = findBendSurface(pOrder->bendpoint,pHead);
    float tool_off = 0.0;
    if(tool_x_off.size() > 0)
        tool_off = tool_x_off[pOrder->tool_id-1];
//    float width = pbend->bendWidth;
//    float tool_off;
//    for(int m = 0; m < toolWidth.size(); m++)
//    {
//        if(fabs(toolWidth[m]-width) < toolMargin[m])
//        {
//            tool_off = tool_x_off[m];
//            break;
//        }
//    }
    float x_pos = 0.0;
    float r_pos = 0.0;
    float z1_pos = tool_off;
    float z2_pos = tool_off;
    if(pGaung_ZL && pGaung_ZR)
    {
        z1_pos = tool_off-100;
        z2_pos = tool_off+100;  //z1,z2之间的距离是200
    }
    pbend = pBendHead;
    while (pbend) {
        trimmed_surface_type *pleft = pbend->pLeftBase->pSurface;
        trimmed_surface_type *pright = pbend->pRightBase->pSurface;
        for(int i = 0; i < pleft->outlineCount[0]; i++)
        {
            int type = pleft->outlineType[0][i];
            if(type == 110) //为直线
            {
                line_type *pline = (line_type *)pleft->pOutlines[0][i];
                if(pline->start.z < x_pos)
                {
                    x_pos = pline->start.z;
                    r_pos = pline->start.y-dieHeight-thickness;
                }
                if(pline->end.z < x_pos)
                {
                    x_pos = pline->end.z;
                    r_pos = pline->start.y-dieHeight-thickness;
                }
            }
            else if(126 == type)
            {
                nurbsCurve_type *pcurve = (nurbsCurve_type *)pleft->pOutlines[0][i];
                int num = pcurve->K_num+1;
                if(num != 2)
                    continue;
                if(pcurve->pCtlarray[2] < x_pos)
                {
                    x_pos = pcurve->pCtlarray[2];
                    r_pos = pcurve->pCtlarray[1]-dieHeight-thickness;
                }
                if(pcurve->pCtlarray[5] < x_pos)
                {
                    x_pos = pcurve->pCtlarray[5];
                    r_pos = pcurve->pCtlarray[4]-dieHeight-thickness;
                }
            }
        }
        for(int i = 0; i < pright->outlineCount[0]; i++)
        {
            int type = pright->outlineType[0][i];
            if(type == 110) //为直线
            {
                line_type *pline = (line_type *)pright->pOutlines[0][i];
                if(pline->start.z < x_pos)
                {
                    x_pos = pline->start.z;
                    r_pos = pline->start.y-dieHeight-thickness;
                }
                if(pline->end.z < x_pos)
                {
                    x_pos = pline->end.z;
                    r_pos = pline->start.y-dieHeight-thickness;
                }
            }
            else if(126 == type)
            {
                nurbsCurve_type *pcurve = (nurbsCurve_type *)pright->pOutlines[0][i];
                int num = pcurve->K_num+1;
                if(num != 2)
                    continue;
                if(pcurve->pCtlarray[2] < x_pos)
                {
                    x_pos = pcurve->pCtlarray[2];
                    r_pos = pcurve->pCtlarray[1]-dieHeight-thickness;
                }
                if(pcurve->pCtlarray[5] < x_pos)
                {
                    x_pos = pcurve->pCtlarray[5];
                    r_pos = pcurve->pCtlarray[4]-dieHeight-thickness;
                }
            }
        }
        pbend = pbend->pNext;
    }
    pOrder->position_x1 = -x_pos;
    pOrder->position_x2 = -x_pos;
    pOrder->position_r1 = r_pos;
    pOrder->position_r2 = r_pos;
    pOrder->position_z1 = z1_pos;
    pOrder->position_z2 = z2_pos;
    pOrder->y_open = Y_open;
    pOrder->dX1Retract = 0.0;
    pOrder->dX2Retract = 0.0;
    float relative_x1pos = -x_pos-X1_Pos;
    float relative_x2pos = -x_pos-X2_Pos;
    float relative_r1pos = r_pos-R1_Pos;
    float relative_r2pos = r_pos-R2_Pos;
    float relative_z1pos = z1_pos-Z1_Pos;
    float relative_z2pos = z2_pos-Z2_Pos;
    X1_Pos = -x_pos;
    R1_Pos = r_pos;
    X2_Pos = -x_pos;
    R2_Pos = r_pos;
    Z1_Pos = z1_pos;
    Z2_Pos = z2_pos;
    offToolData(pGaung_XL,relative_z1pos,0.0,-relative_x1pos);
    offToolData(pGaung_RL,relative_z1pos,relative_r1pos,-relative_x1pos);
    offToolData(pGaung_ZL,relative_z1pos,0.0,0.0);
    offToolData(pGaung_XR,relative_z2pos,0.0,-relative_x2pos);
    offToolData(pGaung_RR,relative_z2pos,relative_r2pos,-relative_x2pos);
    offToolData(pGaung_ZR,relative_z2pos,0.0,0.0);
}

/********************************************
 *function:更新后挡料的位置
 *adding:
 *author: xu
 *date: 2016/03/30
 *******************************************/
void fileOperate::updateBackGaungPos(bend_order *pCur)
{
    float x1_pos = pCur->position_x1;
    float r1_pos = pCur->position_r1;
    float z1_pos = pCur->position_z1;
    float x2_pos = pCur->position_x2;
    float r2_pos = pCur->position_r2;
    float z2_pos = pCur->position_z2;
    float retract1 = pCur->dX1Retract;
    float retract2 = pCur->dX2Retract;
    float relative_x1pos,relative_x2pos;
    float relative_r1pos = r1_pos-R1_Pos;
    float relative_r2pos = r2_pos-R2_Pos;
    float relative_z1pos = z1_pos-Z1_Pos;
    float relative_z2pos = z2_pos-Z2_Pos;
    if(bendFlag)
    {
        relative_x1pos = x1_pos+retract1-(X1_Pos+X1_Retract);
        X1_Pos = x1_pos;
        X1_Retract = retract1;
        relative_x2pos = x2_pos+retract2-(X2_Pos+X2_Retract);
        X2_Pos = x2_pos;
        X2_Retract = retract2;
    }
    else
    {
        relative_x1pos = x1_pos-(X1_Pos+X1_Retract);
        X1_Pos = x1_pos;
        X1_Retract = 0.0;
        relative_x2pos = x2_pos-(X2_Pos+X2_Retract);
        X2_Pos = x2_pos;
        X2_Retract = 0.0;
    }
    R1_Pos = r1_pos;
    R2_Pos = r2_pos;
    Z1_Pos = z1_pos;
    Z2_Pos = z2_pos;
    offToolData(pGaung_XL,relative_z1pos,0.0,-relative_x1pos);
    offToolData(pGaung_RL,relative_z1pos,relative_r1pos,-relative_x1pos);
    offToolData(pGaung_ZL,relative_z1pos,0.0,0.0);
    offToolData(pGaung_XR,relative_z2pos,0.0,-relative_x2pos);
    offToolData(pGaung_RR,relative_z2pos,relative_r2pos,-relative_x2pos);
    offToolData(pGaung_ZR,relative_z2pos,0.0,0.0);
}

/********************************************
 *function:将生成的折弯工序保存到文件中
 *adding:
 *author: xu
 *date: 2016/06/04
 *******************************************/
void fileOperate::formBendorderFile()
{
    int len = fileName.size();
    int pos = fileName.lastIndexOf("/");
    QString path_tmp = fileName.left(pos+1);
    QString name = fileName.right(len-pos-1);
    pos = name.indexOf(".");
    name = name.left(pos);
    name = name+".ord";
    path_tmp = path_tmp+name;
    QByteArray ba = path_tmp.toLatin1();
    char *path = ba.data();
    FILE *fp;
    int flag;
    if((fp = fopen(path,"wb+")) == NULL)
        return;
    int num = bendNum;
    int toolnum = toolWidth.size();
    flag = fwrite(&num,sizeof(int),1,fp);
    flag = fwrite(&toolnum,sizeof(int),1,fp);
    if(flag != 1)
    {
        fclose(fp);
        return;
    }
    tool_info *pInfo = new tool_info;
    for(int i = 0; i < toolnum; i++){
        pInfo->punch_id = toolPunch[i];
        pInfo->die_id = toolDie[i];
        pInfo->punch_width = toolWidth[i];
        pInfo->die_width = toolWidth[i];
        pInfo->punch_pos = toolPosition[i];
        pInfo->die_pos = toolPosition[i];
        flag = fwrite(pInfo,sizeof(tool_info),1,fp);
        if(flag != 1)
        {
            fclose(fp);
            return;
        }
    }
    delete pInfo;
    bend_order *pcur = pOrderHead;
    while (pcur) {
        flag = fwrite(pcur,sizeof(bend_order),1,fp);
        if(flag != 1)
        {
            fclose(fp);
            return;
        }
        pcur = pcur->pNext;
    }
    fclose(fp);
}

/********************************************
 *function:析构折弯链表内存
 *adding:
 *author: xu
 *date: 2016/06/04
 *******************************************/
void fileOperate::destroyBendData(bend_surface *pHead)
{
    bend_surface *pbend = pHead;
    resetVisitFlag(pHead);
    while (pbend) {
        pHead = pbend->pNext;
        if(pbend->pMatrix)
            delete pbend->pMatrix;
        if(pbend->pInverse)
            delete pbend->pInverse;
        if(pbend->pLeftMatrix)
            delete pbend->pLeftMatrix;
        if(pbend->pLeftInverse)
            delete pbend->pLeftInverse;
        if(pbend->pRightMatrix)
            delete pbend->pRightMatrix;
        if(pbend->pRightInverse)
            delete pbend->pRightInverse;
        if(pbend->pDeploy)
            delete pbend->pDeploy;
        if(pbend->pSideBack)
            delete pbend->pSideBack;
        if(pbend->pSideFont)
            delete pbend->pSideFont;
        if(pbend->pBendLine)
            delete pbend->pBendLine;
        if(pbend->pParallel->pMatrix)
            delete pbend->pParallel->pMatrix;
        if(pbend->pParallel->pInverse)
            delete pbend->pParallel->pInverse;
        if(pbend->pParallel->pLeftMatrix)
            delete pbend->pParallel->pLeftMatrix;
        if(pbend->pParallel->pLeftInverse)
            delete pbend->pParallel->pLeftInverse;
        if(pbend->pParallel->pRightMatrix)
            delete pbend->pParallel->pRightMatrix;
        if(pbend->pParallel->pRightInverse)
            delete pbend->pParallel->pRightInverse;
        if(pbend->pParallel->pDeploy)
            delete pbend->pParallel->pDeploy;
        if(pbend->pParallel->pSideBack)
            delete pbend->pParallel->pSideBack;
        if(pbend->pParallel->pSideFont)
            delete pbend->pParallel->pSideFont;
        if(pbend->pParallel->pBendLine)
            delete pbend->pParallel->pBendLine;
        //析构B样条曲线内存
        destroyNurbsSurface(pbend->pBendSideBack);
        destroyNurbsSurface(pbend->pBendSideFont);
        destroyNurbsSurface(pbend->pBendSurface);
        destroyNurbsSurface(pbend->pParallel->pBendSideBack);
        destroyNurbsSurface(pbend->pParallel->pBendSideFont);
        destroyNurbsSurface(pbend->pParallel->pBendSurface);
        //此处释放还需要考虑一下,两个指针同时指向同一个地址,如何释放?
        //析构左右基面lia
//        if(pbend->pLeftBase)
//            delete pbend->pLeftBase;
//        pbend->pLeftBase = NULL;
//        if(pbend->pRightBase)
//            delete pbend->pRightBase;
//        pbend->pRightBase = NULL;
//        if()
//        delete pbend->pRightBase;
//        pbend->pRightBase = NULL;
//        delete pbend->pParallel->pLeftBase;
//        pbend->
//        delete pbend->pParallel->pRightBase;
        delete pbend;
        pbend = pHead;
    }
    pHead = NULL;
}

bend_order *fileOperate::readBendOrderFile(QString name)
{
    QStringList list = name.split(".");
    name = list.at(0)+".ord";
    FILE *fp;
    QByteArray ba = name.toLatin1();
    char *path = ba.data();
    if((fp = fopen(path,"rb+")) == NULL)
    {
       return NULL;
    }
    int num;
    int flag = fread(&num,sizeof(int),1,fp);
    if(flag != 1)
    {
        fclose(fp);
        return NULL;
    }
    bend_order *pHead = NULL;
    bend_order *pLast = NULL;
    for(int i = 0; i < num; i++){
        bend_order *ptmp = MALLOC(1,bend_order);
        memset(ptmp,0,sizeof(bend_order));
        flag = fread(ptmp,sizeof(bend_order),1,fp);
        if(flag != 1)
        {
            fclose(fp);
            return NULL;
        }
        if(pHead == NULL)
        {
            pHead = ptmp;
            pLast = ptmp;
            pLast->pNext = NULL;
        }
        else
        {
            pLast->pNext = ptmp;
            pLast = ptmp;
            pLast->pNext = NULL;
        }
    }
    fclose(fp);
    return pHead;
}

bend_order *fileOperate::formData(FILE *fp, int id,bend_order *pCur,bool bDiv,int link)
{
    MFileType *pFileType = MALLOC(1,MFileType);
    memset(pFileType,0,sizeof(MFileType));
    stu_beforeBend *pBendBefore = MALLOC(1,stu_beforeBend);
    memset(pBendBefore,0,sizeof(stu_beforeBend));
    if(bDiv == false)
        pFileType->fBendNo = bendNum;
    else
    {
        //获取此段折弯的折弯道数
        bend_order *ptmp = pCur;
        int id_tmp = ptmp->tool_id;
        int tmp_num = 0;
        while (ptmp != NULL) {
            if(id_tmp != ptmp->tool_id)
                break;
            tmp_num++;
            ptmp = ptmp->pNext;
        }
        pFileType->fBendNo = tmp_num;
    }
    pFileType->fileid = id;
    strcpy(pFileType->fType,"LDP"); //为离线系统所编程
    strcpy(pFileType->name,"");
    int flag = fwrite(pFileType,sizeof(MFileType),1,fp);
    if(flag != 1)
    {
        fclose(fp);
    }
    pBendBefore->iAngleSel = 0;                    //角度折弯
    pBendBefore->dThickness = thickness;            //厚度
    pBendBefore->iMaterial = 0;                     //材料
    //pBendBefore->dBlackLength = computeBendLength();//展开长度
    pBendBefore->dLength = toolWidth[0];        //工件宽度
    pBendBefore->sizeSel = 0;                   //尺寸选择
    pBendBefore->link = link;                      //连接
    flag = fwrite(pBendBefore,sizeof(stu_beforeBend),1,fp);
    if(flag != 1)
    {
        fclose(fp);
    }
    //*******************写入数据编程数据**********************************/
    stu_BendData *pBendData = MALLOC(1,stu_BendData);
    memset(pBendData,0,sizeof(stu_BendData));
    bend_order *pOrder = pCur;
    int count = 0;
    int tool_id = pOrder->tool_id;
    qDebug()<<sizeof(stu_BendData);
    while (pOrder != NULL) {
        if(tool_id != pOrder->tool_id && bDiv)
            break;
        count++;
        bend_surface *pbend = findBendSurface(pOrder->bendpoint,pBendHead);
        //pBendData->pstu_bend = *pOrder;
        pBendData->uiPunch = 4;                  //上模
        pBendData->uiDie = 1;                    //下模
        pBendData->uiMachineUpperSide = 1;       //上部外形
        pBendData->uiMachineLowerSide = 1;       //下部外形
        pBendData->uiBendNum = count;           //当前的折弯序号
        pBendData->dVX_percent = 100;            //X轴速度百分比
        pBendData->dVR_percent = 100;            //R轴速度百分比
        pBendData->iMethod = 0;                  //自由折弯
        pBendData->uiCode = 0;                   //代码
        pBendData->delaytime = 0;                //换步前延时
        pBendData->uiReputition = 1;             //循环次数
        pBendData->dwellTime = 2;                //保压时间
        pBendData->dParallism = 0;               //平行度
        pBendData->dRfingerPosition = 0;         //R轴位置
        //pBendData->uiGaugePos = gauge_pos;     //后挡料位置
        //后挡料X,R轴位置
        pBendData->dXStartPosition = pOrder->position_x1; //X轴定位
        pBendData->dRStartPosition = pOrder->position_r1; //R轴定位
        pBendData->dXRetract = 0.0;              //退让距离
        float Speed = 12.00;
        float Y_Open = 80.00;
        pBendData->dSpeed = Speed;               //工进速度
        pBendData->decompSpeed = Speed;          //泄压速度与工进速度一致
        pBendData->dOpeningY = Y_Open;           //Y轴开口高度
        float angle = fabs(pbend->bendAngle)/PI*180;
        pBendData->dAngleOrBendPosition = angle;//角度度数
        pBendData->dLength = pbend->bendWidth; //板料宽度
        //根据之前的数据需要计算得到的数据
        //**********************速度转换点，夹紧点*****************************************/
        //pBendData->dMutePosition = Mute_Tmp;    //速度转换点
        //pBendData->dClamp_Pos = Distance_Tmp-thickness+ClampRemedy;//夹紧点
        //*************************压力********************************************/
        //pBendData->dForce = Force;              //压力
        //*****************************泄荷距离***********************************/
        //pBendData->decoppression = Stiffness*Force/1000;         //泄荷距离
        //*****************************挠度补偿***********************************/
        //pBendData->dCptCrown = Deflection;
        //补偿数据
        pBendData->workpiece_count = 1;         //工件计数
        pBendData->dSingleCptAngle1 = 0;        //单步角度补偿1
        pBendData->dSingleCptAngle2 = 0;        //单步角度补偿2
        pBendData->dOverallCptAngle = 0;        //全局角度补偿
        pBendData->dSingleCptX1 = 0;            //X1轴的单步补偿
        pBendData->dSingleCptX2 = 0;            //X2轴的单步补偿
        pBendData->dOverallCptX = 0;            //X轴全局补偿
        pBendData->dSingleCptCrown = 0;         //挠度轴的单步补偿
        pBendData->dSingleCptR = 0;             //R轴的单步补偿
        //将参数写入文件中
        flag = fwrite(pBendData,sizeof(stu_BendData),1,fp);
        if(flag != 1)
        {
            fclose(fp);
        }
        pOrder = pOrder->pNext;
    }
    free(pFileType);
    free(pBendBefore);
    free(pBendData);
    return pOrder;
}

QVector<int> fileOperate::getDivNum()
{
    QVector<int> div;
    int count = 1;
    bend_order *porder = pOrderHead;
    if(porder == NULL)
        return div;
    int tool_id = porder->tool_id;
    porder = porder->pNext;
    while(porder){
        if(tool_id == porder->tool_id)
            count++;
        else{
            div.push_back(count);
            count = 1;
            tool_id = porder->tool_id;
        }
        porder = porder->pNext;
    }
    div.push_back(count);
    return div;
}

void fileOperate::convertBackUpToBend(int num)
{
    bend_order *pCur = pOrder_Backup[num];//当前工序为链表的头结点
    //工件的位置
    convertToBendPos(pCur,pBend_Backup[num]);
    bendFlag = false;
    //首先让板料折一遍然后再恢复到原始位置
    while(1){
        if(bendFlag == false)   //回程状态下,执行折弯操作
        {
            if(pCur == NULL)
                break;
            //生成上模和滑块的折弯模型数据
            formBendedData(pCur->bendpoint,false,pBend_Backup[num]);
            formBendedData(pCur->bendpoint,true,pBend_Backup[num]);
            formBendSideSurface(pCur->bendpoint,pBend_Backup[num]);
            bend_surface *pbend = findBendSurface(pCur->bendpoint,pBend_Backup[num]);
            pCur->position_y1 = Y_bendDepth(pbend->bendAngle);
            pCur->position_y2 = pCur->position_y1;
            bendFlag = true;    //重置为折弯状态
        }
        else                    //折弯状态下，执行回程操作
        {
            if(pCur->pNext == NULL)
                break;
            //生成上模和滑块的回程模型数据
            pCur = pCur->pNext;//转到下一道折弯
            convertToBendPos(pCur,pBend_Backup[num]);
            bendFlag = false;   //重置为回程状态
        }
    }
}

/********************************************
 *function:遗传算法适应度计算
 *adding:
 *author: xu
 *date: 2016/12/09
 *******************************************/
int fileOperate::computeSuit(QVector<int> order,int index)
{
    int num = order.size();
    if(num == 1)
        return 1;
    int model_count = 0;
    int turn_count = 0;
    int upside_count = 0;
    int distance = 0;
    //因为之前已经判断过最后一道折弯
    for(int i = 0; i < num; i++){
        bend_order *pCur = findBendOrder(order[i],pOrder_Backup[index]);
        if(bendFlag == false){
            bend_surface *pbend = findBendSurface(order[i],pBend_Backup[index]);
            float y_off = Y_bendDepth(pbend->bendAngle);
            formPuchBendData(y_off);
            bendFlag = true;
        }
        convertToBendPos(pCur,pBend_Backup[index]);
        if(judgeInterFere_inherit(order[i],pBend_Backup[index],pOrder_Backup[index]) == -1){
            bend_inherit(order,index,i);
            int suit = num*MODEL_CHANGE+(num-i)*MODEL_CHANGE;
            return suit;
        }
        //判断没有干涉,则将该道折弯展开
        //convertToBendPos(pCur,pBend_Backup[index]);
        deploy_inherit(order,index,i);
    }
    bend_inherit(order,index,num-1);
    bend_order *pCur = findBendOrder(order[num-1],pOrder_Backup[index]);
    bend_surface *pbend = findBendSurface(order[num-1],pBend_Backup[index]);
    line_type *pbendline = pbend->pBendLine;
    for(int i = num-2; i >= 0; i--){
        bend_order *ptmp = findBendOrder(order[i],pOrder_Backup[index]);
        bend_surface *pbend_tmp = findBendSurface(order[i],pBend_Backup[index]);
        line_type *pline_tmp = pbend_tmp->pBendLine;
        distance += fabs((pbendline->start.z+pbendline->end.z)-(pline_tmp->start.z+pline_tmp->end.z))*0.1;
        if(ptmp->tool_id != pCur->tool_id)
            model_count++;
        if(ptmp->direction_h != pCur->direction_h)
            turn_count++;
        if(ptmp->direction_v != pCur->direction_v)
            upside_count++;
        pCur = ptmp;
        pbend = pbend_tmp;
        pbendline = pline_tmp;
    }
    int suit = model_count*MODEL_CHANGE+turn_count*TURN_ABOUT+upside_count*UPSIDE_DOWN/*+distance*/;
    return suit;
}

/********************************************
 *function:遗传算法选择操作
 *adding:
 *author: xu
 *date: 2016/12/09
 *******************************************/
QVector<QVector<int> > fileOperate::pick(QVector<QVector<int> > population)
{
    QVector<int> result;
    int num = population.size();
    float sum = 0.0;
    int min = INT_MAX;
    int index = 0;
    for(int i = 0; i < num; i++){
        int suit = computeSuit(population[i],i);
        result.push_back(suit);
        sum += 1.0/(float)suit;
        if(min > suit){
            min = suit;
            index = i;
        }
    }
    //最小的适应度个体直接遗传
    QVector<QVector<int> > pick_population;
    pick_population.push_back(population[index]);
    result[0] = int(1.0/(result[0]*sum)*100);
    for(int i = 1; i < num; i++){
        result[i] = int(1.0/(result[i]*sum)*100)+result[i-1];
    }
    int sum_suit = result[num-1]; 
    for(int i = 1; i < num; i++){
        int random = qrand()%(int)sum_suit;
        for(int j = 0; j < num; j++){
            if(random <= result[j]){
                pick_population.push_back(population[j]);
                break;
            }
        }
    }
    qDebug()<<min;
    return pick_population;
}

/********************************************
 *function:遗传算法交叉操作
 *adding:
 *author: xu
 *date: 2016/12/09
 *******************************************/
void fileOperate::cross(QVector<QVector<int> > &population,QVector<QVector<int> > &inter)
{
    int num = population.size();
    int bendnum = bendNum;
    int max,min;
    for(int i = 0; i < num; i++){
        int random1 = qrand()%(bendnum-2)+2;//最小取2
        int random2 = qrand()%(bendnum-2)+2;
        if(random1 >= random2){
            max = random1;
            min = random2;
        }
        else{
            min = random1;
            max = random2;
        }
        //将max的值与min的值交换
        int tmp = population[i][min];
        population[i][min] = population[i][max];
        population[i][max] = tmp;
        //因为之前已经判断过最后一道折弯
        for(int j = 0; j < min; j++){
            bend_order *pCur = findBendOrder(population[i][j],pOrder_Backup[i]);
            if(bendFlag == false){
                bend_surface *pbend = findBendSurface(population[i][j],pBend_Backup[i]);
                float y_off = Y_bendDepth(pbend->bendAngle);
                formPuchBendData(y_off);
                bendFlag = true;
            }
            convertToBendPos(pCur,pBend_Backup[i]);
            int k = j;
            while(judgeInterFere_inherit(population[i][j],pBend_Backup[i],pOrder_Backup[i]) == -1){
                  if(k >= bendnum-1){
                      inter[i][j] = j;
                      bend_inherit(population[i],i,j);
                      //将max的值与当前值交换
                      int tmp = population[i][j];
                      population[i][j] = population[i][max];
                      population[i][max] = tmp;
                      return;
                  }
                  k++;
                  //交换序列
                  int tmp = population[i][j];
                  population[i][j] = population[i][k];
                  population[i][k] = tmp;
                  pCur = findBendOrder(population[i][j],pOrder_Backup[i]);
                  convertToBendPos(pCur,pBend_Backup[i]);
            }
            //判断没有干涉,则将该道折弯展开
            deploy_inherit(population[i],i,j);
        }
        bend_inherit(population[i],i,min-1);
    }
    return;
}

void fileOperate::deploy_inherit(QVector<int> order, int index, int bend_id)
{
    bend_order *pCur = findBendOrder(order[bend_id],pOrder_Backup[index]);
    bend_surface *pbend = findBendSurface(order[bend_id],pBend_Backup[index]);
    formPuchBendData(Y_Pos-pCur->y_open);
    pbend->isBended = 0;
    pbend->pParallel->isBended = 0;
    resetVisitFlag(pBend_Backup[index]);
    MatrixPartConvert(pbend->pLeftInverse,pBend_Backup[index],pbend,pbend->pLeftBase,false);
    MatrixPartConvert(pbend->pParallel->pLeftInverse,pBend_Backup[index],pbend->pParallel,pbend->pParallel->pLeftBase,true);
    MatrixPartConvert(pbend->pRightInverse,pBend_Backup[index],pbend,pbend->pRightBase,false);
    MatrixPartConvert(pbend->pParallel->pRightInverse,pBend_Backup[index],pbend->pParallel,pbend->pParallel->pRightBase,true);
    resetVisitFlag(pBend_Backup[index]);
    bendFlag = false;   //重置为回程状态
}

void fileOperate::bend_inherit(QVector<int> order, int index,int bend_id)
{
    //首先把需要折弯的当前折弯段折弯
    int bendnum;
    bend_order *pCur = NULL;
    bend_surface *pbend = NULL;
    if(bendFlag == false){
        bendnum = order[bend_id];
        pCur = findBendOrder(bendnum,pOrder_Backup[index]);
        pbend = findBendSurface(bendnum,pBend_Backup[index]);
        float y_off = Y_bendDepth(pbend->bendAngle);
        formPuchBendData(y_off);
        formBendedData(bendnum,false,pBend_Backup[index]);
        formBendedData(bendnum,true,pBend_Backup[index]);
        formBendSideSurface(bendnum,pBend_Backup[index]);
        bendFlag = true;
    }
    for(int i = bend_id-1; i >= 0; i--){
        bendnum = order[i];
        pCur = findBendOrder(bendnum,pOrder_Backup[index]);
        pbend = findBendSurface(bendnum,pBend_Backup[index]);
        //先转换到折弯位置
        convertToBendPos(pCur,pBend_Backup[index]);
        //折弯
        formBendedData(bendnum,false,pBend_Backup[index]);
        formBendedData(bendnum,true,pBend_Backup[index]);
        formBendSideSurface(bendnum,pBend_Backup[index]);
    }
    bendFlag = true;
}

void fileOperate::updateVisibleState(bool bBody, bool bPunch, bool bDie, bool bX, bool bR, bool bZ)
{
    bShow_body = bBody;
    bShow_punch = bPunch;
    bShow_die = bDie;
    bShow_X = bX;
    bShow_R = bR;
    bShow_Z = bZ;
}

void fileOperate::drawEntity(trimmed_surface_type *pTrimmed, float color[],bool flag)
{
    //绘制填充实体
    trimmed_surface_type *pTmpCur = pTrimmed;
    while (pTmpCur) {
        int typeNum = pTmpCur->surfaceType;
        if(typeNum == 128 || typeNum == 120)//程序暂时还未考虑其他类型的曲面，有待增加
        {
            nurbsSurface_type *pSurface = (nurbsSurface_type *)pTmpCur->pSurface;
            pNurb = gluNewNurbsRenderer();  //创建NURBS对象
            gluNurbsProperty(pNurb,GLU_DISPLAY_MODE,(GLfloat)GLU_FILL);
            int knot_count1 = pSurface->K1_num+pSurface->M1_num+2;
            int knot_count2 = pSurface->K2_num+pSurface->M2_num+2;
            glColor4fv(color);
            gluBeginSurface(pNurb);
            if(typeNum == 128)
            {
                gluNurbsSurface(pNurb,knot_count1,pSurface->pKnot1,knot_count2,\
                    pSurface->pKnot2,(pSurface->K1_num+1)*3,3,pSurface->pCtlarray,\
                    pSurface->M1_num+1,pSurface->M2_num+1,GL_MAP2_VERTEX_3);
                for(int i = 0; i < pTmpCur->curve_num+1; i++)
                {
                    gluBeginTrim(pNurb);
                    for(int j = 0; j < pTmpCur->curveCount[i]; j++)
                    {
                        if(pTmpCur->curveType[i][j] == 100)     //圆弧实体
                        {
                            //暂时保留，因为基本上不会出现此种情况
                        }
                        else if(pTmpCur->curveType[i][j] == 110)//直线实体
                        {
                            line_type *pLine = (line_type *)pTmpCur->pCurves[i][j];
                            float linepoints[2][3];
                            linepoints[0][0] = pLine->start.x;
                            linepoints[0][1] = pLine->start.y;
                            linepoints[0][2] = pLine->start.z;
                            linepoints[1][0] = pLine->end.x;
                            linepoints[1][1] = pLine->end.y;
                            linepoints[1][2] = pLine->end.z;
                            gluPwlCurve(pNurb,2,&linepoints[0][0],3,GLU_MAP1_TRIM_2);
                        }
                        else if(pTmpCur->curveType[i][j] == 126)//B样条曲线实体
                        {
                            nurbsCurve_type *pNurbsCurve = (nurbsCurve_type *)pTmpCur->pCurves[i][j];
                            int knot_count = pNurbsCurve->K_num+pNurbsCurve->M_num+2;
                            gluNurbsCurve(pNurb,knot_count,pNurbsCurve->pKnot,3,pNurbsCurve->pCtlarray,\
                                          pNurbsCurve->M_num+1,GLU_MAP1_TRIM_2);
                        }
                    }
                    gluEndTrim(pNurb);
                }
            }
            else if(typeNum == 120)
            {
                //glColor4fv(SelectColor);
                gluNurbsSurface(pNurb,knot_count2,pSurface->pKnot2,knot_count1,\
                    pSurface->pKnot1,3,(pSurface->K2_num+1)*3,pSurface->pCtlarray,\
                    pSurface->M1_num+1,pSurface->M2_num+1,GL_MAP2_VERTEX_3);
                gluBeginTrim(pNurb);
                gluPwlCurve(pNurb,5,&trimmed_data[0][0],2,GLU_MAP1_TRIM_2);
                gluEndTrim(pNurb);
            }
            gluEndSurface(pNurb);
            if(pNurb)           //如果对象已经建立，则删除
            {
                gluDeleteNurbsRenderer(pNurb);
                pNurb = NULL;
            }
            //绘制外轮廓
            if(flag == true){
                for(int j = 0; j < pTmpCur->outlineCount[0]; j++)
                {
                    int type = pTmpCur->outlineType[0][j];
                    if(type == 110)
                    {
                        glLineWidth(2);
                        glColor3f(1.0,1.0,1.0);
                        line_type *pline = (line_type *)pTmpCur->pOutlines[0][j];
                        glBegin(GL_LINES);
                        glEnable(GL_LINE_SMOOTH);
                        glVertex3f(pline->start.x,pline->start.y,pline->start.z);
                        glVertex3f(pline->end.x,pline->end.y,pline->end.z);
                        glEnd();
                     }
                 }
            }
        }
        pTmpCur = pTmpCur->pNext;
    }
}

void fileOperate::drawWorkPiece()
{
    glLineWidth(1); //设置线宽为1
    bend_surface *pBend = pBendHead;
    resetVisitFlag(pBendHead);
    while (pBend) {
        trimmed_surface_type *ptmp;
        glColor3fv(bendColor);
        //绘制下面的其中一个基面
        if(pBend->pLeftBase->isVisited == 0)
        {
            glColor4fv(bac_workpieceColor);
            ptmp = pBend->pLeftBase->pSurface;
            drawBaseSurface(ptmp);
            pBend->pLeftBase->isVisited = 1;
        }
        //绘制上面的其中一个基面
        if(pBend->pParallel->pLeftBase->isVisited == 0)
        {
            glColor4fv(workpieceColor);
            ptmp = pBend->pParallel->pLeftBase->pSurface;
            drawBaseSurface(ptmp);
            pBend->pLeftBase->isVisited = 1;
        }
        //绘制下面的另一个基面
        if(pBend->pRightBase->isVisited == 0)
        {
            glColor4fv(bac_workpieceColor);
            ptmp = pBend->pRightBase->pSurface;
            drawBaseSurface(ptmp);
            pBend->pRightBase->isVisited = 1;
        }
        //绘制上面的另一个基面
        if(pBend->pParallel->pRightBase->isVisited == 0)
        {
            glColor4fv(workpieceColor);
            ptmp = pBend->pParallel->pRightBase->pSurface;
            drawBaseSurface(ptmp);
            pBend->pParallel->pRightBase->isVisited = 1;
        }
        //绘制折弯面,分两种情况，已折弯的绘制旋转曲面，未折弯的绘制平面
        if(pBend->isBended == 1)
        {
            //绘制下面的旋转曲面折弯面
            nurbsSurface_type *pSurface = pBend->pBendSurface;
            pNurb = gluNewNurbsRenderer();  //创建NURBS对象
            gluNurbsProperty(pNurb,GLU_DISPLAY_MODE,(GLfloat)GLU_FILL);
            int knot_count1 = pSurface->K1_num+pSurface->M1_num+2;
            int knot_count2 = pSurface->K2_num+pSurface->M2_num+2;
            gluBeginSurface(pNurb);
            glColor3fv(bendColor);
            //glColor3f(0.0,0.0,0.0);
            gluNurbsSurface(pNurb,knot_count2,pSurface->pKnot2,knot_count1,\
                            pSurface->pKnot1,3,(pSurface->K2_num+1)*3,pSurface->pCtlarray,\
                            pSurface->M1_num+1,pSurface->M2_num+1,GL_MAP2_VERTEX_3);
            gluEndSurface(pNurb);
            if(pNurb)           //如果对象已经建立，则删除
            {
                gluDeleteNurbsRenderer(pNurb);
                pNurb = NULL;
            }
            //绘制上面的旋转曲面折弯面
            nurbsSurface_type *pSurface2 = pBend->pParallel->pBendSurface;
            pNurb = gluNewNurbsRenderer();  //创建NURBS对象
            gluNurbsProperty(pNurb,GLU_DISPLAY_MODE,(GLfloat)GLU_FILL);
            int knot_count12 = pSurface2->K1_num+pSurface2->M1_num+2;
            int knot_count22 = pSurface2->K2_num+pSurface2->M2_num+2;
            gluBeginSurface(pNurb);
            glColor3fv(bendColor);
            gluNurbsSurface(pNurb,knot_count22,pSurface2->pKnot2,knot_count12,\
                            pSurface2->pKnot1,3,(pSurface2->K2_num+1)*3,pSurface2->pCtlarray,\
                            pSurface2->M1_num+1,pSurface2->M2_num+1,GL_MAP2_VERTEX_3);
            gluEndSurface(pNurb);
            if(pNurb)           //如果对象已经建立，则删除
            {
                gluDeleteNurbsRenderer(pNurb);
                pNurb = NULL;
            }
            //绘制折弯面的侧面
            nurbsSurface_type *pSideBack = pBend->pBendSideBack;
            pNurb = gluNewNurbsRenderer();  //创建NURBS对象
            gluNurbsProperty(pNurb,GLU_DISPLAY_MODE,(GLfloat)GLU_FILL);
            int knot_count13 = pSideBack->K1_num+pSideBack->M1_num+2;
            int knot_count23 = pSideBack->K2_num+pSideBack->M2_num+2;
            gluBeginSurface(pNurb);
            glColor3fv(bendColor);
            gluNurbsSurface(pNurb,knot_count23,pSideBack->pKnot2,knot_count13,\
                            pSideBack->pKnot1,3,(pSideBack->K2_num+1)*3,pSideBack->pCtlarray,\
                            pSideBack->M1_num+1,pSideBack->M2_num+1,GL_MAP2_VERTEX_3);
            gluEndSurface(pNurb);
            if(pNurb)           //如果对象已经建立，则删除
            {
                gluDeleteNurbsRenderer(pNurb);
                pNurb = NULL;
            }
            nurbsSurface_type *pSideFont = pBend->pBendSideFont;
            pNurb = gluNewNurbsRenderer();  //创建NURBS对象
            gluNurbsProperty(pNurb,GLU_DISPLAY_MODE,(GLfloat)GLU_FILL);
            int knot_count14 = pSideFont->K1_num+pSideFont->M1_num+2;
            int knot_count24 = pSideFont->K2_num+pSideFont->M2_num+2;
            gluBeginSurface(pNurb);
            glColor3fv(bendColor);
            gluNurbsSurface(pNurb,knot_count24,pSideFont->pKnot2,knot_count14,\
                            pSideFont->pKnot1,3,(pSideFont->K2_num+1)*3,pSideFont->pCtlarray,\
                            pSideFont->M1_num+1,pSideFont->M2_num+1,GL_MAP2_VERTEX_3);
            gluEndSurface(pNurb);
            if(pNurb)           //如果对象已经建立，则删除
            {
                gluDeleteNurbsRenderer(pNurb);
                pNurb = NULL;
            }
        }
        else        //未折弯的绘制平面的同时，还需要绘制折弯线
        {
            //绘制下面的平面折弯面
            glBegin(GL_POLYGON);
            glColor3fv(bendColor);
            int len = 0;
            if(pBend->pDeploy)
            {
                len = pBend->pDeploy->point.size();
            }
            for(int i = 0; i < len; i++)
            {
                stu_polygon *pSurface = pBend->pDeploy;
                glVertex3f(pSurface->point[i].x,pSurface->point[i].y,pSurface->point[i].z);
            }
            glEnd();
            //绘制上面的平面折弯面
            glBegin(GL_POLYGON);
            glColor3fv(bendColor);
            int len_pa = 0;
            if(pBend->pParallel->pDeploy)
            {
                len_pa = pBend->pParallel->pDeploy->point.size();
            }
            for(int i = 0; i < len_pa; i++)
            {
                stu_polygon *pSurface = pBend->pParallel->pDeploy;
                glVertex3f(pSurface->point[i].x,pSurface->point[i].y,pSurface->point[i].z);
            }
            glEnd();
            //绘制折弯面展开时的侧面
            stu_polygon *pBack = pBend->pSideBack;
            stu_polygon *pFont = pBend->pSideFont;
            if(pBack && pFont)
            {
                glBegin(GL_POLYGON);
                glColor3fv(bendColor);
                for(int i = 0; i < pFont->point.size(); i++)
                    glVertex3f(pFont->point[i].x,pFont->point[i].y,pFont->point[i].z);
                glEnd();
                glBegin(GL_POLYGON);
                glColor3fv(bendColor);
                for(int i = 0; i < pBack->point.size(); i++)
                    glVertex3f(pBack->point[i].x,pBack->point[i].y,pBack->point[i].z);
                glEnd();
            }
        }
        //绘制侧面
        for(int i = 0; i < pBend->pLeftBase->pSideSurface.size(); i++)
        {
            trimmed_surface_type *pside = pBend->pLeftBase->pSideSurface[i];
            drawBaseSurface(pside);
        }
        for(int i = 0; i < pBend->pRightBase->pSideSurface.size(); i++)
        {
            trimmed_surface_type *pside = pBend->pRightBase->pSideSurface[i];
            drawBaseSurface(pside);
        }

//        glBegin(GL_LINES);
//        glColor3f(1.0,0.0,0.0);
//        glVertex3f(pBend->generatrix.start.x,pBend->generatrix.start.y,pBend->generatrix.start.z);
//        glVertex3f(pBend->generatrix.end.x,pBend->generatrix.end.y,pBend->generatrix.end.z);
//        glEnd();
        pBend = pBend->pNext;
    }
    resetVisitFlag(pBendHead);

//    float knot1_prepare[12] = {0.0,0.0,0.0,0.25,0.25,0.5,0.5,0.75,0.75,1.0,1.0,1.0};
//    float knot2_prepare[6] = {0.0,0.0,0.0,1.0,1.0,1.0};
//    //构造nurbs曲面
//    nurbsSurface_type *pNurbsSurface = new nurbsSurface_type;
//    memset(pNurbsSurface,0,sizeof(nurbsSurface_type));
//    pNurbsSurface->K1_num = 8;
//    pNurbsSurface->K2_num = 2;
//    pNurbsSurface->M1_num = 2;
//    pNurbsSurface->M2_num = 2;
//    int N1_num = 1+pNurbsSurface->K1_num-pNurbsSurface->M1_num;
//    int N2_num = 1+pNurbsSurface->K2_num-pNurbsSurface->M2_num;
//    int A_num = N1_num+2*pNurbsSurface->M1_num+1;
//    int B_num = N2_num+2*pNurbsSurface->M2_num+1;
//    int C_num = (1+pNurbsSurface->K1_num)*(1+pNurbsSurface->K2_num);
//    float *knot1 = new float[A_num];
//    float *knot2 = new float[B_num];
//    for(int i = 0; i < A_num; i++)
//        knot1[i] = knot1_prepare[i];
//    for(int i = 0; i < B_num; i++)
//        knot2[i] = knot2_prepare[i];
//    pNurbsSurface->pKnot1 = knot1;
//    pNurbsSurface->pKnot2 = knot2;
//    float *ctldata = new float[C_num*4];

//    ctldata[0] = 20;
//    ctldata[1] = 0;
//    ctldata[2] = 0;
//    ctldata[3] = 1;
//    ctldata[4] = 20;
//    ctldata[5] = 1;
//    ctldata[6] = 0;
//    ctldata[7] = 1;
//    ctldata[8] = 20;
//    ctldata[9] = 2;
//    ctldata[10] = 0;
//    ctldata[11] = 1;

//    ctldata[12] = 20/1.414;
//    ctldata[13] = 0;
//    ctldata[14] = 20/1.414;
//    ctldata[15] = 0.707;
//    ctldata[16] = 20/1.414;
//    ctldata[17] = 1;
//    ctldata[18] = 20/1.414;
//    ctldata[19] = 0.707;
//    ctldata[20] = 20/1.414;;
//    ctldata[21] = 2;
//    ctldata[22] = 20/1.414;;
//    ctldata[23] = 0.707;

//    ctldata[24] = 0;
//    ctldata[25] = 0;
//    ctldata[26] = 20;
//    ctldata[27] = 1;
//    ctldata[28] = 0;
//    ctldata[29] = 1;
//    ctldata[30] = 20;
//    ctldata[31] = 1;
//    ctldata[32] = 0;
//    ctldata[33] = 2;
//    ctldata[34] = 20;
//    ctldata[35] = 1;

//    ctldata[36] = -20/1.414;
//    ctldata[37] = 0;
//    ctldata[38] = 20/1.414;
//    ctldata[39] = 0.707;
//    ctldata[40] = -20/1.414;
//    ctldata[41] = 1;
//    ctldata[42] = 20/1.414;
//    ctldata[43] = 0.707;
//    ctldata[44] = -20/1.414;
//    ctldata[45] = 2;
//    ctldata[46] = 20/1.414;
//    ctldata[47] = 0.707;

//    ctldata[48] = -20;
//    ctldata[49] = 0;
//    ctldata[50] = 0;
//    ctldata[51] = 1;
//    ctldata[52] = -20;
//    ctldata[53] = 1;
//    ctldata[54] = 0;
//    ctldata[55] = 1;
//    ctldata[56] = -20;
//    ctldata[57] = 2;
//    ctldata[58] = 0;
//    ctldata[59] = 1;

//    ctldata[60] = -20/1.414;
//    ctldata[61] = 0;
//    ctldata[62] = -20/1.414;
//    ctldata[63] = 0.707;
//    ctldata[64] = -20/1.414;
//    ctldata[65] = 1;
//    ctldata[66] = -20/1.414;
//    ctldata[67] = 0.707;
//    ctldata[68] = -20/1.414;
//    ctldata[69] = 2;
//    ctldata[70] = -20/1.414;
//    ctldata[71] = 0.707;

//    ctldata[72] = 0;
//    ctldata[73] = 0;
//    ctldata[74] = -20;
//    ctldata[75] = 1;
//    ctldata[76] = 0;
//    ctldata[77] = 1;
//    ctldata[78] = -20;
//    ctldata[79] = 1;
//    ctldata[80] = 0;
//    ctldata[81] = 2;
//    ctldata[82] = -20;
//    ctldata[83] = 1;

//    ctldata[84] = 20/1.414;
//    ctldata[85] = 0;
//    ctldata[86] = -20/1.414;
//    ctldata[87] = 0.707;
//    ctldata[88] = 20/1.414;
//    ctldata[89] = 1;
//    ctldata[90] = -20/1.414;
//    ctldata[91] = 0.707;
//    ctldata[92] = 20/1.414;
//    ctldata[93] = 2;
//    ctldata[94] = -20/1.414;
//    ctldata[95] = 0.707;

//    ctldata[96] = 20;
//    ctldata[97] = 0;
//    ctldata[98] = 0;
//    ctldata[99] = 1;
//    ctldata[100] = 20;
//    ctldata[101] = 1;
//    ctldata[102] = 0;
//    ctldata[103] = 1;
//    ctldata[104] = 20;
//    ctldata[105] = 2;
//    ctldata[106] = 0;
//    ctldata[107] = 1;

//    pNurbsSurface->pCtlarray = ctldata;

//    //绘制下面的旋转曲面折弯面
//    pNurb = gluNewNurbsRenderer();  //创建NURBS对象
//    gluNurbsProperty(pNurb,GLU_DISPLAY_MODE,(GLfloat)GLU_FILL);
//    int knot_count1 = pNurbsSurface->K1_num+pNurbsSurface->M1_num+2;
//    int knot_count2 = pNurbsSurface->K2_num+pNurbsSurface->M2_num+2;
//    gluBeginSurface(pNurb);
//    glColor3f(0.0,0.0,1.0);
//    //glColor3f(0.0,0.0,0.0);
//    gluNurbsSurface(pNurb,knot_count2,pNurbsSurface->pKnot2,knot_count1,\
//                    pNurbsSurface->pKnot1,4,(pNurbsSurface->K2_num+1)*4,pNurbsSurface->pCtlarray,\
//                    pNurbsSurface->M1_num+1,pNurbsSurface->M2_num+1,GL_MAP2_VERTEX_4);
//    gluEndSurface(pNurb);
//    if(pNurb)           //如果对象已经建立，则删除
//    {
//        gluDeleteNurbsRenderer(pNurb);
//        pNurb = NULL;
//    }
}

GLUtesselator *fileOperate::tessor()
{
    GLUtesselator *tess;
    tess = gluNewTess();
    gluTessCallback(tess,GLU_TESS_BEGIN,(void (CALLBACK*)())&beginCallback);
    gluTessCallback(tess,GLU_TESS_VERTEX,(void (CALLBACK*)())&vertexCallback);
    gluTessCallback(tess,GLU_TESS_END,(void (CALLBACK*)())&endCallback);
    return tess;
}

void fileOperate::drawBaseSurface(trimmed_surface_type *pTmpCur)
{
//    GLUtesselator *tess = tessor();
//    if(!tess)
//        return;
//    gluTessBeginPolygon(tess,NULL);
//    int n = pSurface->curve_num+1;
//    GLdouble (*array[n])[3];
//    for(int i = 0; i < pSurface->curve_num+1; i++)
//    {
//        int tmp_count = 0;
//        int num = pSurface->curveCount[i];
//        int count = num;
//        for(int j = 0; j < num; j++)
//        {
//            int type = pSurface->curveType[i][j];
//            if(type == 126) //B样条曲线实体
//            {
//                nurbsCurve_type *pNurbsCurve = (nurbsCurve_type *)pSurface->pCurves[i][j];
//                count += pNurbsCurve->K_num;
//            }
//        }
//        array[i] = new GLdouble[count][3];
//        for(int j = 0; j < num; j++)
//        {
//            int type = pSurface->curveType[i][j];
//            if(type == 110) //直线实体
//            {
//                line_type *pline = (line_type *)pSurface->pCurves[i][j];
//                array[i][tmp_count][0] = pline->start.x;
//                array[i][tmp_count][1] = pline->start.y;
//                array[i][tmp_count][2] = pline->start.z;
//                tmp_count++;
//            }
//            if(type == 126) //B样条曲线实体
//            {
//                nurbsCurve_type *pNurbsCurve = (nurbsCurve_type *)pSurface->pCurves[i][j];
//                int number = pNurbsCurve->K_num+1;
//                for(int m = 0; m < number; m++)
//                {
//                    array[i][tmp_count][0] = pNurbsCurve->pCtlarray[m*3];
//                    array[i][tmp_count][1] = pNurbsCurve->pCtlarray[m*3+1];
//                    array[i][tmp_count][2] = pNurbsCurve->pCtlarray[m*3+2];
//                    tmp_count++;
//                }
//            }
//        }
//        gluTessBeginContour(tess);
//        for(int j = 0; j < count; j++)
//            gluTessVertex(tess,array[i][j],array[i][j]);
//        gluTessEndContour(tess);
//    }
//    gluTessEndPolygon(tess);
//    gluDeleteTess(tess);
//    //析构之前申请的内存
//    for(int i = 0; i < pSurface->curve_num+1; i++)
//        delete []array[i];

//    int typeNum = pTmpCur->surfaceType;
//    if(typeNum == 128 || typeNum == 120)//程序暂时还未考虑其他类型的曲面，有待增加
//    {

//        for(int i = 0; i < pTmpCur->curve_num+1; i++){
//            for(int j = 0; j < pTmpCur->outlineCount[i]; j++){
//                int type = pTmpCur->outlineType[i][j];
//                if(type == 110){
//                    line_type *pline = (line_type *)pTmpCur->pOutlines[i][j];
//                    glBegin(GL_LINES);
//                    glVertex3f(pline->start.x,pline->start.y,pline->start.z);
//                    glVertex3f(pline->end.x,pline->end.y,pline->end.z);
//                    glEnd();
//                }
//            }
//        }
//    }


    int typeNum = pTmpCur->surfaceType;
    if(typeNum == 128 || typeNum == 120)//程序暂时还未考虑其他类型的曲面，有待增加
    {
        nurbsSurface_type *pSurface = (nurbsSurface_type *)pTmpCur->pSurface;
        pNurb = gluNewNurbsRenderer();  //创建NURBS对象
        gluNurbsProperty(pNurb,GLU_DISPLAY_MODE,(GLfloat)GLU_FILL);
        int knot_count1 = pSurface->K1_num+pSurface->M1_num+2;
        int knot_count2 = pSurface->K2_num+pSurface->M2_num+2;
//        glColor4fv(workpieceColor);
        gluBeginSurface(pNurb);
        if(typeNum == 128)
        {
            gluNurbsSurface(pNurb,knot_count1,pSurface->pKnot1,knot_count2,\
                pSurface->pKnot2,(pSurface->K1_num+1)*3,3,pSurface->pCtlarray,\
                pSurface->M1_num+1,pSurface->M2_num+1,GL_MAP2_VERTEX_3);
            for(int i = 0; i < pTmpCur->curve_num+1; i++)
            {
                gluBeginTrim(pNurb);
                for(int j = 0; j < pTmpCur->curveCount[i]; j++)
                {
                    if(pTmpCur->curveType[i][j] == 100)     //圆弧实体
                    {
                        //暂时保留，因为基本上不会出现此种情况
                    }
                    else if(pTmpCur->curveType[i][j] == 110)//直线实体
                    {
                        line_type *pLine = (line_type *)pTmpCur->pCurves[i][j];
                        float linepoints[2][3];
                        linepoints[0][0] = pLine->start.x;
                        linepoints[0][1] = pLine->start.y;
                        linepoints[0][2] = pLine->start.z;
                        linepoints[1][0] = pLine->end.x;
                        linepoints[1][1] = pLine->end.y;
                        linepoints[1][2] = pLine->end.z;
                        gluPwlCurve(pNurb,2,&linepoints[0][0],3,GLU_MAP1_TRIM_2);
                    }
                    else if(pTmpCur->curveType[i][j] == 126)//B样条曲线实体
                    {
                        nurbsCurve_type *pNurbsCurve = (nurbsCurve_type *)pTmpCur->pCurves[i][j];
                        int knot_count = pNurbsCurve->K_num+pNurbsCurve->M_num+2;
                        gluNurbsCurve(pNurb,knot_count,pNurbsCurve->pKnot,3,pNurbsCurve->pCtlarray,\
                                      pNurbsCurve->M_num+1,GLU_MAP1_TRIM_2);
                    }
                }
                gluEndTrim(pNurb);
            }
        }
        else if(typeNum == 120)
        {
            //glColor4fv(SelectColor);
            gluNurbsSurface(pNurb,knot_count2,pSurface->pKnot2,knot_count1,\
                pSurface->pKnot1,3,(pSurface->K2_num+1)*3,pSurface->pCtlarray,\
                pSurface->M1_num+1,pSurface->M2_num+1,GL_MAP2_VERTEX_3);
            for(int i = 0; i < pTmpCur->curve_num+1; i++)
            {
                gluBeginTrim(pNurb);
                for(int j = pTmpCur->curveCount[i]-1; j >= 0; j--)
                {
                    if(pTmpCur->curveType[i][j] == 100)     //圆弧实体
                    {
                        //暂时保留，因为基本上不会出现此种情况
                    }
                    else if(pTmpCur->curveType[i][j] == 110)//直线实体
                    {
                    }
                    else if(pTmpCur->curveType[i][j] == 126)//B样条曲线实体
                    {
                        nurbsCurve_type *pNurbsCurve = (nurbsCurve_type *)pTmpCur->pCurves[i][j];
                        int num = pNurbsCurve->K_num;
                        float linepoints[2][3];
                        for(int m = num; m >= 0; m--){
                            linepoints[num-m][0] = pNurbsCurve->pCtlarray[3*m];
                            linepoints[num-m][1] = pNurbsCurve->pCtlarray[3*m+1];
                            linepoints[num-m][2] = pNurbsCurve->pCtlarray[3*m+2];
                        }
                        gluPwlCurve(pNurb,2,&linepoints[0][0],3,GLU_MAP1_TRIM_2);
                    }
                }
                gluEndTrim(pNurb);
            }
        }
        gluEndSurface(pNurb);
        if(pNurb)           //如果对象已经建立，则删除
        {
            gluDeleteNurbsRenderer(pNurb);
            pNurb = NULL;
        }
        for(int i = 0; i < pTmpCur->curve_num+1; i++)
        {
            for(int j = 0; j < pTmpCur->outlineCount[i]; j++)
            {
                int type = pTmpCur->outlineType[i][j];
                if(type == 110)
                {
                    glLineWidth(0.25);
                    glColor3f(0.0,0.0,0.0);
                    line_type *pline = (line_type *)pTmpCur->pOutlines[i][j];
                    glBegin(GL_LINES);
                    glEnable(GL_LINE_SMOOTH);
                    glVertex3f(pline->start.x,pline->start.y,pline->start.z);
                    glVertex3f(pline->end.x,pline->end.y,pline->end.z);
                    glEnd();
                 }
             }
        }
    }
}

void fileOperate::drawMachine()
{
    if(pMachine && bShow_body)    //绘制机床本体
    {
        if(zoom > 0.6)
            zoom = 0.6;
        drawEntity(pMachine,machineColor);
    }
    if(pUpper && bShow_body)      //绘制上滑块
        drawEntity(pUpper,upperColor);
    if(pLower && bShow_body)      //绘制上滑块
        drawEntity(pLower,lowerColor);
    if(pPunch.size() && bShow_punch)//绘制上模
    {
        if(zoom > 1.3)
            zoom =1.3;
        for(int i = 0; i < pPunch.size(); i++)
            drawEntity(pPunch[i],punchColor,true);
    }
    if(pDie.size() && bShow_die)    //绘制下模
    {
        if(zoom > 1.3)
            zoom =1.3;
        for(int i = 0; i < pDie.size(); i++)
            drawEntity(pDie[i],dieColor,true);
    }
    //绘制后挡料,分别绘制左右两侧，分为X，R，Z轴
    if(pGaung_RL && bShow_R)
        drawEntity(pGaung_RL,backgaung_rColor);
    if(pGaung_XL && bShow_X)
        drawEntity(pGaung_XL,backgaung_xColor);
    if(pGaung_ZL && bShow_Z)
        drawEntity(pGaung_ZL,backgaung_zColor);
    if(pGaung_RR && bShow_R)
        drawEntity(pGaung_RR,backgaung_rColor);
    if(pGaung_XR && bShow_X)
        drawEntity(pGaung_XR,backgaung_xColor);
    if(pGaung_ZR && bShow_Z)
        drawEntity(pGaung_ZR,backgaung_zColor);
}

/********************************************
*function:偏移结构体数据
 *adding:
 *author: xu
 *date: 2015/12/31
 *******************************************/
void fileOperate::offToolData(trimmed_surface_type *pTrimmed,point3f off)
{
    if(pTrimmed == NULL)
        return;
    resetToolFlag(pTrimmed);
    trimmed_surface_type *pTmpCur = pTrimmed;
    while (pTmpCur) {
        int typeNum = pTmpCur->surfaceType;
        if(typeNum == 128 || typeNum == 120)//程序暂时还未考虑其他类型的曲面，有待增加
        {
            nurbsSurface_type *pSurface = (nurbsSurface_type *)pTmpCur->pSurface;
            int num = (pSurface->K1_num+1)*(pSurface->K2_num+1);
            for(int i = 0; i < num; i++)
            {
                pSurface->pCtlarray[3*i] += off.x;
                pSurface->pCtlarray[3*i+1] += off.y;
                pSurface->pCtlarray[3*i+2] += off.z;
            }
        }
        if(typeNum == 128)  //暂时先不考虑曲面
        {
            for(int i = 0; i < pTrimmed->curve_num+1; i++)
            {
                for(int j = 0; j < pTrimmed->outlineCount[i]; j++)
                {
                    int type = pTmpCur->outlineType[i][j];
                    if(type == 110) //为直线
                    {
                        line_type *pline = (line_type *)pTmpCur->pOutlines[i][j];
                        if(pline->isVisited == 0)
                        {
                            pline->start.x += off.x;
                            pline->end.x += off.x;
                            pline->start.y += off.y;
                            pline->end.y += off.y;
                            pline->start.z += off.z;
                            pline->end.z += off.z;
                            pline->isVisited = 1;
                        }
                    }
                    else if(type == 126)
                    {
                        nurbsCurve_type *pcurve = (nurbsCurve_type *)pTmpCur->pOutlines[i][j];
                        if(pcurve->isVisited == 0)
                        {
                            int num = pcurve->K_num+1;
                            for(int m = 0; m < num; m++)
                            {
                                pcurve->pCtlarray[3*m] += off.x;
                                pcurve->pCtlarray[3*m+1] += off.y;
                                pcurve->pCtlarray[3*m+2] += off.z;
                            }
                            pcurve->isVisited = 1;
                        }
                    }
                }
            }
        }
        pTmpCur = pTmpCur->pNext;
    }
    resetToolFlag(pTrimmed);
}

void fileOperate::offToolData(trimmed_surface_type *pTrimmed, float x, float y, float z)
{
    if(pTrimmed == NULL)
        return;
    resetToolFlag(pTrimmed);
    trimmed_surface_type *pTmpCur = pTrimmed;
    while (pTmpCur) {
        int typeNum = pTmpCur->surfaceType;
        if(typeNum == 128 || typeNum == 120)//程序暂时还未考虑其他类型的曲面，有待增加
        {
            nurbsSurface_type *pSurface = (nurbsSurface_type *)pTmpCur->pSurface;
            int num = (pSurface->K1_num+1)*(pSurface->K2_num+1);
            for(int i = 0; i < num; i++)
            {
                pSurface->pCtlarray[3*i] += x;
                pSurface->pCtlarray[3*i+1] += y;
                pSurface->pCtlarray[3*i+2] += z;
            }
        }
        if(typeNum == 128)  //暂时先不考虑曲面
        {
            for(int i = 0; i < pTrimmed->curve_num+1; i++)
            {
                for(int j = 0; j < pTrimmed->outlineCount[i]; j++)
                {
                    int type = pTmpCur->outlineType[i][j];
                    if(type == 110) //为直线
                    {
                        line_type *pline = (line_type *)pTmpCur->pOutlines[i][j];
                        if(pline->isVisited == 0)
                        {
                            pline->start.x += x;
                            pline->end.x += x;
                            pline->start.y += y;
                            pline->end.y += y;
                            pline->start.z += z;
                            pline->end.z += z;
                            pline->isVisited = 1;
                        }
                    }
                    else if(type == 126)
                    {
                        nurbsCurve_type *pcurve = (nurbsCurve_type *)pTmpCur->pOutlines[i][j];
                        if(pcurve->isVisited == 0)
                        {
                            int num = pcurve->K_num+1;
                            for(int m = 0; m < num; m++)
                            {
                                pcurve->pCtlarray[3*m] += x;
                                pcurve->pCtlarray[3*m+1] += y;
                                pcurve->pCtlarray[3*m+2] += z;
                            }
                            pcurve->isVisited = 1;
                        }
                    }
                }
            }
        }
        pTmpCur = pTmpCur->pNext;
    }
    resetToolFlag(pTrimmed);
}

/********************************************
 *function:转换工件的坐标到指定的折弯位置
 *adding: isBend 表示折弯后展开,1为折弯,0为展开
 *author: xu
 *date: 2015/10/29
 *******************************************/
void fileOperate::convertToBendPos(bend_order *pOrder, bend_surface *pHead)
{
    int bendnum = pOrder->bendpoint;
    //首先根据实际情况生成转换矩阵
    if(bendnum > bendNum || bendnum < 1)
        return;
    bend_surface *pbend_find = findBendSurface(pOrder->bendpoint,pHead);
    float tool_off = 0.0;
    if(tool_x_off.size() > 0)
        tool_off = tool_x_off[pOrder->tool_id-1];
    line_type *pbendline = pbend_find->pBendLine;
    //*********将钣金工件的当前折弯线与上部外形上表面中线重合***********************
    //生成向坐标原点平移的矩阵
    point3f off;
    off.x = -(pbendline->start.x+pbendline->end.x)/2;
    off.y = -(pbendline->start.y+pbendline->end.y)/2;
    off.z = -(pbendline->start.z+pbendline->end.z)/2;
    xform_type *pTranslation1 = formTranslationMatrix(off);
    //生成所需的旋转矩阵
    point3f source,dest;
    source.x = pbendline->end.x + off.x;
    source.y = pbendline->end.y + off.y;
    source.z = pbendline->end.z + off.z;
    dest.x = 1.0;
    dest.y = 0.0;
    dest.z = 0.0;
    //计算所需的旋转矩阵
    xform_type *pRote1 = computeMatrix(source,dest);
    //将平移和旋转矩阵相乘，得到变换矩阵
    xform_type *pForm1 = MatrixMultiply(pTranslation1,pRote1);
    delete pTranslation1;
    delete pRote1;
    //实现钣金工件的坐标变换
    MatrixTotalConvert(pHead,pForm1,false);
    MatrixTotalConvert(pHead,pForm1,true);
    //***********将钣金工件当前的折弯面平行于上部外形上表面************************
    source.x = pbend_find->pDeploy->point[0].x-pbendline->start.x;
    source.y = pbend_find->pDeploy->point[0].y-pbendline->start.y;
    source.z = pbend_find->pDeploy->point[0].z-pbendline->start.z;
    dest.x = 0.0;
    dest.y = 0.0;
    dest.z = 1.0;
    xform_type *pRote2 = computeMatrix(source,dest);
    MatrixTotalConvert(pHead,pRote2,false);
    MatrixTotalConvert(pHead,pRote2,true);
    //需要根据折弯角度判断是否需要翻转,角度为正,则上面的y值大,否则下面的y值大
    //取当前折弯上面和下面的任意轮廓y轴的值
    float y_top = pbend_find->pParallel->pBendLine->start.y;
    if((pbend_find->bendAngle > 0) && (y_top < EPSINON))
    {
        //说明当前折弯需要翻转
        xform_type *pRote3 = new xform_type;
        pRote3->R11 = -1.0;
        pRote3->R12 = 0.0;
        pRote3->R13 = 0.0;
        pRote3->R21 = 0.0;
        pRote3->R22 = -1.0;
        pRote3->R23 = 0.0;
        pRote3->R31 = 0.0;
        pRote3->R32 = 0.0;
        pRote3->R33 = 1.0;
        pRote3->T1 = 0.0;
        pRote3->T2 = 0.0;
        pRote3->T3 = 0.0;
        MatrixTotalConvert(pHead,pRote3,false);
        MatrixTotalConvert(pHead,pRote3,true);
        xform_type *ptmp = MatrixMultiply(pRote2,pRote3);
        delete pRote2;
        delete pRote3;
        pRote2 = ptmp;
    }
    else if(pbend_find->bendAngle < 0)
    {
        xform_type *pRote3 = new xform_type;
        if(y_top > EPSINON)     //说明当前折弯需要翻转
        {
            pRote3->R11 = -1.0;
            pRote3->R12 = 0.0;
            pRote3->R13 = 0.0;
            pRote3->R21 = 0.0;
            pRote3->R22 = -1.0;
            pRote3->R23 = 0.0;
            pRote3->R31 = 0.0;
            pRote3->R32 = 0.0;
            pRote3->R33 = 1.0;
        }
        else                  //说明当前折弯不需要翻转
        {
            pRote3->R11 = 1.0;
            pRote3->R12 = 0.0;
            pRote3->R13 = 0.0;
            pRote3->R21 = 0.0;
            pRote3->R22 = 1.0;
            pRote3->R23 = 0.0;
            pRote3->R31 = 0.0;
            pRote3->R32 = 0.0;
            pRote3->R33 = 1.0;
        }
        pRote3->T1 = 0.0;
        pRote3->T2 = thickness;
        pRote3->T3 = 0.0;
        MatrixTotalConvert(pHead,pRote3,false);
        MatrixTotalConvert(pHead,pRote3,true);
        xform_type *ptmp = MatrixMultiply(pRote2,pRote3);
        delete pRote2;
        delete pRote3;
        pRote2 = ptmp;
    }
    //*****限定折弯线在机床上的方位,折弯线的起点必须在左侧,终点必须在右侧*******/
    if(pbendline->start.x > 0 && pbendline->end.x < 0)
    {
        xform_type matrix;
        matrix.R11 = -1.0;
        matrix.R12 = 0.0;
        matrix.R13 = 0.0;
        matrix.R21 = 0.0;
        matrix.R22 = 1.0;
        matrix.R23 = 0.0;
        matrix.R31 = 0.0;
        matrix.R32 = 0.0;
        matrix.R33 = -1.0;
        matrix.T1 = 0.0;
        matrix.T2 = 0.0;
        matrix.T3 = 0.0;
        MatrixTotalConvert(pHead,&matrix,false);
        MatrixTotalConvert(pHead,&matrix,true);  //调头
        xform_type *ptmp = MatrixMultiply(pRote2,&matrix);
        delete pRote2;
        pRote2 = ptmp;
    }
    //*****************************************************************/
    if(pOrder->direction_h == 1)//说明当前折弯需要调头
    {
        xform_type matrix;
        matrix.R11 = -1.0;
        matrix.R12 = 0.0;
        matrix.R13 = 0.0;
        matrix.R21 = 0.0;
        matrix.R22 = 1.0;
        matrix.R23 = 0.0;
        matrix.R31 = 0.0;
        matrix.R32 = 0.0;
        matrix.R33 = -1.0;
        matrix.T1 = 0.0;
        matrix.T2 = 0.0;
        matrix.T3 = 0.0;
        MatrixTotalConvert(pHead,&matrix,false);
        MatrixTotalConvert(pHead,&matrix,true);  //调头
        xform_type *ptmp = MatrixMultiply(pRote2,&matrix);
        delete pRote2;
        pRote2 = ptmp;
    }
    //此时的钣金工件还需要沿Y轴上移下模高度的距离,实现板料在上模上表面的中线位置
    off.x = tool_off;
    off.y = dieHeight;
    off.z = 0.0;
    xform_type *pTranslation2 = formTranslationMatrix(off);
    MatrixTotalConvert(pHead,pTranslation2,false);
    MatrixTotalConvert(pHead,pTranslation2,true);
    xform_type *pForm2 = MatrixMultiply(pRote2,pTranslation2);
//    MatrixTotalConvert(pForm2,false);
    //每次重新新的矩阵时需要进行判断,防止内存泄露
    if(pbend_find->pMatrix)
    {
        delete pbend_find->pMatrix;
        pbend_find->pMatrix = NULL;
    }
    if(pbend_find->pInverse)
    {
        delete pbend_find->pInverse;
        pbend_find->pInverse = NULL;
    }
    pbend_find->pMatrix = MatrixMultiply(pForm1,pForm2);
    pbend_find->pInverse = computeInverseMatrix(pbend_find->pMatrix);
    //MatrixTotalConvert(pbend_find->pMatrix,true);
    delete pForm1;
    delete pForm2;
    delete pTranslation2;
    delete pRote2;
}

/********************************************
*function:回退到全部展开的状态
 *adding:
 *author: xu
 *date: 2016/01/02
 *******************************************/
void fileOperate::resetToDeploy()
{
    if(bendFlag)   //折弯状态下，执行回程操作0
    {
        formPuchBendData(Y_Pos-Y_open);
        //bendFlag = false;
    }
    //将折弯数据返回到初始化的数据
    while (pOrderCur) {
        if(bendFlag == false)
        {
            bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
            MatrixTotalConvert(pBendHead,pbend->pInverse,false);
            MatrixTotalConvert(pBendHead,pbend->pInverse,true);
            pOrderCur = pOrderCur->pBefore;
            bendFlag = true;    //重置为折弯状态
        }
        else
        {
            bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
            pbend->isBended = 0;
            pbend->pParallel->isBended = 0;
            resetVisitFlag(pBendHead);
            MatrixPartConvert(pbend->pLeftInverse,pBendHead,pbend,pbend->pLeftBase,false);
            MatrixPartConvert(pbend->pParallel->pLeftInverse,pBendHead,pbend->pParallel,pbend->pParallel->pLeftBase,true);
            MatrixPartConvert(pbend->pRightInverse,pBendHead,pbend,pbend->pRightBase,false);
            MatrixPartConvert(pbend->pParallel->pRightInverse,pBendHead,pbend->pParallel,pbend->pParallel->pRightBase,true);
            resetVisitFlag(pBendHead);
            bendFlag = false;   //重置为回程状态
        }
    }
    bendFlag = false;
}

void fileOperate::resetToBend()
{
    resetToDeploy();
    pOrderCur = pOrderHead; //当前工序为链表的头结点
    //工件的位置
    convertToBendPos(pOrderCur,pBendHead);
    bendFlag = false;
    //首先让板料折一遍然后再恢复到原始位置
    while(1){
        if(bendFlag == false)   //回程状态下,执行折弯操作
        {
            if(pOrderCur == NULL)
                break;
            //生成上模和滑块的折弯模型数据
            formBendedData(pOrderCur->bendpoint,false,pBendHead);
            formBendedData(pOrderCur->bendpoint,true,pBendHead);
            formBendSideSurface(pOrderCur->bendpoint,pBendHead);
            bend_surface *pbend = findBendSurface(pOrderCur->bendpoint,pBendHead);
            pOrderCur->position_y1 = Y_bendDepth(pbend->bendAngle);
            pOrderCur->position_y2 = pOrderCur->position_y1;
            formPuchBendData(pOrderCur->position_y1);
            bendFlag = true;    //重置为折弯状态
        }
        else                    //折弯状态下，执行回程操作
        {
            if(pOrderCur->pNext == NULL)
                break;
            //生成上模和滑块的回程模型数据
            pOrderCur = pOrderCur->pNext;//转到下一道折弯
            convertToBendPos(pOrderCur,pBendHead);
            computeBackGaungPos(pOrderCur,pBendHead);
            bendFlag = false;   //重置为回程状态
        }
    }
}

/********************************************
*function:生成折弯后的上模和滑块的模型数据
 *adding:
 *author: xu
 *date: 2015/12/30
 *******************************************/
void fileOperate::formPuchBendData(float depth)
{
    Y_Pos = depth-Y_Pos;
    point3f off;
    off.x = 0.0;
    off.y = -Y_Pos;
    off.z = 0.0;
    offToolData(pUpper,off);            //上滑块
    for(int m = 0; m < pPunch.size(); m++)//上模
    {
        offToolData(pPunch[m],off);
    }
    Y_Pos = depth;
}
