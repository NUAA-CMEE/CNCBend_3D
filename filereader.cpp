#include "filereader.h"
#include <QDebug>
#include <QMessageBox>
//float knot1_prepare[25] = {0.0,0.0,0.0,0.05,0.1,0.15,0.2,0.25,0.3,0.35,0.4,0.45,0.5,\
//                             0.55,0.6,0.65,0.7,0.75,0.8,0.85,0.9,0.95,1.0,1.0,1.0};
//float knot2_prepare[6] = {0.0,0.0,0.0,1.0,1.0,1.0};
//float knot1_prepare[12] = {0.0,0.0,0.0,0.25,0.25,0.5,0.5,0.75,0.75,1.0,1.0,1.0};
//float knot2_prepare[6] = {0.0,0.0,0.0,1.0,1.0,1.0};

float knot1_prepare[44] = {0.00,0.00,0.00,0.05,0.05,0.10,0.10,0.15,0.15,0.20,0.20,\
                           0.25,0.25,0.30,0.30,0.35,0.35,0.40,0.40,0.45,0.45,0.50,\
                           0.50,0.55,0.55,0.60,0.60,0.65,0.65,0.70,0.70,0.75,0.75,\
                           0.80,0.80,0.85,0.85,0.90,0.90,0.95,0.95,1.00,1.00,1.00};
float knot2_prepare[6] = {0.0,0.0,0.0,1.0,1.0,1.0};

fileReader::fileReader()
{
    fileType = 0;   //0为Pro/e,1为SolidWorks
    bendNum = 0;    //折弯总道数
    thickness = 0;
}

fileReader::~fileReader()
{

}

/********************************************
 *function:读取原始文件
 *adding:
 *author: xu
 *date: 2016/06/16
 *******************************************/
GLData *fileReader::readOriginFile(QString path)
{
    gl_data *pGLDataHead = NULL;
    gl_data *pGLDataCur = NULL;
    gl_data *pGLDataLast = NULL;
    QFile file(path);
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug()<<"打开原始文件失败!";
        return NULL;
    }
    QTextStream data(&file);
    //读取文件的最后一行内容
    QString lastline;
    while (!data.atEnd()) {
       lastline = data.readLine();
    }
    //处理最后一行的数据
    int s_row,g_row,d_row,p_row;    //各个数据块的行数0.0f
    s_row = lastline.mid(1,7).trimmed().toInt();    //开始段的行数
    g_row = lastline.mid(9,7).trimmed().toInt();    //全局参数段的行数
    d_row = lastline.mid(17,7).trimmed().toInt();   //目录条目段的行数
    p_row = lastline.mid(25,7).trimmed().toInt();   //参数数据段的行数
    data.seek(0);           //文件指针转到开头
    int count_row = 1;      //记录当前读取的行数
    QString strTmp;         //读取的临时数据
    while (!data.atEnd()) {
        strTmp = data.readLine();
        if(count_row >= 1 && count_row <= s_row)
        {
            QStringList list = strTmp.split(" ");
            if(list.isEmpty())
            {
                file.close();
                return NULL;
            }
            if(list.at(0) == "SolidWorks")
                fileType = 1;
            else
                fileType = 0;
        }
        else if((count_row >= s_row+g_row+1) && (count_row <= s_row+g_row+d_row))//文件读到目录条目段
        {
            gl_data *pTmp = new gl_data;
            memset(pTmp,0,sizeof(gl_data));
            pTmp->typeNum = strTmp.mid(0,8).trimmed().toInt();  //实体类型号
            pTmp->data_pos = strTmp.mid(8,8).trimmed().toInt(); //数据区位置
            pTmp->xform_pos = strTmp.mid(48,8).trimmed().toInt();//变换矩阵位置
            //状态号中的形成特性位置编号，0为几何特征，2为定义特征，5为二维特征
            pTmp->state = strTmp.mid(68,2).trimmed().toInt();
            pTmp->type_pos = strTmp.mid(73,8).trimmed().toInt(); //该条记录所在行标号
            strTmp = data.readLine();
            count_row++;
            pTmp->data_row = strTmp.mid(24,8).trimmed().toInt(); //数据区行数
            //将读取的数据形成数据链表
            if(pGLDataHead == NULL)
            {
                pGLDataLast = pTmp;
                pGLDataLast->pNext = NULL;
                pGLDataLast->pBefore = NULL;
                pGLDataHead = pGLDataLast;
                pGLDataCur = pGLDataLast;
            }
            else
            {
                pGLDataLast->pNext = pTmp;
                pTmp->pBefore = pGLDataLast;
                pGLDataLast = pTmp;
                pGLDataLast->pNext = NULL;
            }
        }
        //文件读到参数数据段
        else if((count_row >= s_row+g_row+d_row+1) && (count_row <= s_row+g_row+d_row+p_row))
        {
            strTmp = strTmp.left(64);       //截取一行中的前64个字符
            int typeNum = pGLDataCur->typeNum;
            int row = pGLDataCur->data_row;
            int xform = pGLDataCur->xform_pos;
            for(int i = 1; i < row; i++)
            {
                strTmp.append(data.readLine().left(64));
                count_row++;
            }
            switch (typeNum) {
            case 100:   //圆弧实体
            {
                arc_type *pArcData = new arc_type;
                memset(pArcData,0,sizeof(arc_type));
                QStringList list,list2;
                list = strTmp.split(",");
                int size = list.size();
                if(size == 0)   //防止文件出现错误而出现段错误
                    break;
                pArcData->typeNum = list.at(0).toInt();
                for(int i = 1; i < size; i++)
                {
                    QString str = list.at(i);
                    if(str.contains(";"))
                        str = str.split(";").at(0);
                    if(fileType == 0)   //Pro/E文件
                    {
                        list2 = str.split("D");
                        if(i == 1)
                            pArcData->ZT = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 2)
                            pArcData->X1 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 3)
                            pArcData->Y1 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 4)
                            pArcData->X2 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 5)
                            pArcData->Y2 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 6)
                            pArcData->X3 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 7)
                            pArcData->Y3 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else        //SolidWorks文件
                    {
                        if(i == 1)
                            pArcData->ZT = str.toFloat();
                        else if(i == 2)
                            pArcData->X1 = str.toFloat();
                        else if(i == 3)
                            pArcData->Y1 = str.toFloat();
                        else if(i == 4)
                            pArcData->X2 = str.toFloat();
                        else if(i == 5)
                            pArcData->Y2 = str.toFloat();
                        else if(i == 6)
                            pArcData->X3 = str.toFloat();
                        if(i == 7)
                            pArcData->Y3 = str.toFloat();
                    }
                }
                int xform = pGLDataCur->xform_pos;
                if(xform != 0)
                {
                    gl_data *pgldata = findGLData(xform,pGLDataHead);
                    pArcData->pForm = *(xform_type *)pgldata->pData;
                    pgldata->useCount++;    //变换矩阵被使用
                }
                pGLDataCur->pData = pArcData;
                break;
            }
            case 102:   //组合曲线实体
            {
                composite_curve_type *pCurve = new composite_curve_type;
                memset(pCurve,0,sizeof(composite_curve_type));
                QStringList list;
                list = strTmp.split(",");
                int size = list.size();
                if(size == 0)   //防止文件出现错误而出现段错误
                    break;
                pCurve->typeNum = list.at(0).toInt();
                pCurve->curveCount = list.at(1).toInt();
                for(int i = 0; i < pCurve->curveCount; i++)
                {
                    QString str = list.at(i+2);
                    if(str.contains(";"))
                        str = str.split(";").at(0);
                    pCurve->curvePos[i] = str.toInt();
                }
                pGLDataCur->pData = pCurve;
                break;
            }
            case 110:   //直线实体
            {
                line_type *pLineData = new line_type;
                memset(pLineData,0,sizeof(line_type));
                QStringList list,list2;
                list = strTmp.split(",");
                int size = list.size();
                if(size == 0)   //防止文件出现错误而出现段错误
                    break;
                pLineData->typeNum = list.at(0).toInt();
                for(int i = 1; i < size; i++)
                {
                    QString str = list.at(i);
                    if(str.contains(";"))
                        str = str.split(";").at(0);
                    if(fileType == 0)   //Pro/E文件
                    {
                        list2 = str.split("D");
                        if(i == 1)
                            pLineData->start.x = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 2)
                            pLineData->start.y = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 3)
                            pLineData->start.z = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 4)
                            pLineData->end.x = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 5)
                            pLineData->end.y = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 6)
                            pLineData->end.z = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else        //SolidWorks文件
                    {
                        if(i == 1)
                            pLineData->start.x = str.toFloat();
                        else if(i == 2)
                            pLineData->start.y = str.toFloat();
                        else if(i == 3)
                            pLineData->start.z = str.toFloat();
                        else if(i == 4)
                            pLineData->end.x = str.toFloat();
                        else if(i == 5)
                            pLineData->end.y = str.toFloat();
                        else if(i == 6)
                            pLineData->end.z = str.toFloat();
                    }
                }
                if(xform != 0)  //需要进行矩阵变换
                {
                    gl_data *pTmp = pGLDataCur;
                    while (pTmp) {
                        if(pTmp->type_pos == xform)
                            break;
                        pTmp = pTmp->pBefore;
                    }
                    xform_type *pTmpForm = (xform_type *)pTmp->pData;
                    xform_matrix(pTmpForm,pLineData->start);
                    xform_matrix(pTmpForm,pLineData->end);
                }
                pGLDataCur->pData = pLineData;
                break;
            }
            case 120:   //旋转曲面
            {
                revolution_type *pRevolution = new revolution_type;
                memset(pRevolution,0,sizeof(revolution_type));
                QStringList list,list2;
                list = strTmp.split(",");
                int size = list.size();
                if(size == 0)   //防止文件出现错误而出现段错误
                    break;
                pRevolution->typeNum = list.at(0).toInt();
                pRevolution->axis = list.at(1).toInt();
                pRevolution->generatrix = list.at(2).toInt();
                if(fileType == 0)   //Pro/E文件
                {
                    list2 = list.at(3).split("D");
                    pRevolution->start_angle = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                }
                else
                {
                    pRevolution->start_angle = list.at(3).toFloat();
                }
                QString str = list.at(4);
                str = str.split(";").at(0);
                if(fileType == 0)   //Pro/E文件
                {
                    list2 = str.split("D");
                    pRevolution->end_angle = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                }
                else
                {
                    pRevolution->end_angle = str.toFloat();
                }
                pGLDataCur->pData = pRevolution;
                break;
            }
            case 124:   //变换矩阵
            {
                xform_type *pXform = new xform_type;
                memset(pXform,0,sizeof(xform_type));
                QStringList list,list2;
                list = strTmp.split(",");
                int size = list.size();
                if(size == 0)   //防止文件出现错误而出现段错误
                    break;
                pXform->typeNum = list.at(0).toInt();
                for(int i = 1; i < size; i++)
                {
                    QString str = list.at(i);
                    if(str.contains(";"))
                        str = str.split(";").at(0);
                    if(fileType == 0)   //Pro/E文件
                    {
                        list2 = str.split("D");
                        if(i == 1)
                            pXform->R11 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 2)
                            pXform->R12 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 3)
                            pXform->R13 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 4)
                            pXform->T1 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 5)
                            pXform->R21 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 6)
                            pXform->R22 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 7)
                            pXform->R23 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 8)
                            pXform->T2 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 9)
                            pXform->R31 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 10)
                            pXform->R32 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 11)
                            pXform->R33 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 12)
                            pXform->T3 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else
                    {
                        if(i == 1)
                            pXform->R11 = str.toFloat();
                        else if(i == 2)
                            pXform->R12 = str.toFloat();
                        else if(i == 3)
                            pXform->R13 = str.toFloat();
                        else if(i == 4)
                            pXform->T1 = str.toFloat();
                        else if(i == 5)
                            pXform->R21 = str.toFloat();
                        else if(i == 6)
                            pXform->R22 = str.toFloat();
                        else if(i == 7)
                            pXform->R23 = str.toFloat();
                        else if(i == 8)
                            pXform->T2 = str.toFloat();
                        else if(i == 9)
                            pXform->R31 = str.toFloat();
                        else if(i == 10)
                            pXform->R32 = str.toFloat();
                        else if(i == 11)
                            pXform->R33 = str.toFloat();
                        else if(i == 12)
                            pXform->T3 = str.toFloat();
                    }
                }
                pGLDataCur->pData = pXform;
                break;
            }
            //暂时未考虑变换矩阵的影响
            case 126:       //B样条曲线实体
            {
                nurbsCurve_type *pNurbsCurve = new nurbsCurve_type;
                memset(pNurbsCurve,0,sizeof(nurbsCurve_type));
                QStringList list;
                list = strTmp.split(",");
                int size = list.size();
                if(size == 0)   //防止文件出现错误而出现段错误
                    break;
                pNurbsCurve->typeNum = list.at(0).toInt();
                pNurbsCurve->K_num = list.at(1).toInt();
                pNurbsCurve->M_num = list.at(2).toInt();
                pNurbsCurve->prop_plane = list.at(3).toInt();
                pNurbsCurve->prop_closed = list.at(4).toInt();
                pNurbsCurve->prop_poly = list.at(5).toInt();
                pNurbsCurve->prop_periodic = list.at(6).toInt();
                int N_num = 1+pNurbsCurve->K_num-pNurbsCurve->M_num;
                int A_num = N_num+2*pNurbsCurve->M_num+1;
                //为结点序列数组赋值
                float *pKnot = new float[A_num];
                for(int i = 0; i < A_num; i++)
                {
                    QString str = list.at(i+7);
                    if(fileType == 0)   //Pro/E文件
                    {
                        QStringList list2 = str.split("D");
                        pKnot[i] = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else
                    {
                        pKnot[i] = str.toFloat();
                    }
                }
                pNurbsCurve->pKnot = pKnot;
                //为权重值数组赋值
                float *pWeight = new float[pNurbsCurve->K_num+1];
                for(int i = 0; i <= pNurbsCurve->K_num; i++)
                {
                    QString str = list.at(i+A_num+7);
                    if(fileType == 0)   //Pro/E文件
                    {
                        QStringList list2 = str.split("D");
                        pWeight[i] = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else
                    {
                        pWeight[i] = str.toFloat();
                    }
                }
                pNurbsCurve->pWeight = pWeight;
                //为控制点数组赋值
                float *pCtlData = new float[pNurbsCurve->K_num*3+3];
                for(int i = 0; i < pNurbsCurve->K_num*3+3; i++)
                {
                    QString str = list.at(i+A_num+pNurbsCurve->K_num+8);
                    if(fileType == 0)   //Pro/E文件
                    {
                        QStringList list2 = str.split("D");
                        pCtlData[i] = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else
                    {
                        pCtlData[i] = str.toFloat();
                    }
                }
                pNurbsCurve->pCtlarray = pCtlData;
                //最后的五个点赋值
                for(int i = 0; i < 5; i++)
                {
                    QString str = list.at(i+A_num+pNurbsCurve->K_num*4+11);
                    if(str.contains(";"))
                        str = str.split(";").at(0);
                    if(fileType == 0)   //Pro/E文件
                    {
                        QStringList list2 = str.split("D");
                        if(i == 0)
                            pNurbsCurve->V0 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 1)
                            pNurbsCurve->V1 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 2)
                            pNurbsCurve->xnorm = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 3)
                            pNurbsCurve->ynorm = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 4)
                            pNurbsCurve->znorm = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else
                    {
                        if(i == 0)
                            pNurbsCurve->V0 = str.toFloat();
                        else if(i == 1)
                            pNurbsCurve->V1 = str.toFloat();
                        else if(i == 2)
                            pNurbsCurve->xnorm = str.toFloat();
                        else if(i == 3)
                            pNurbsCurve->ynorm = str.toFloat();
                        else if(i == 4)
                            pNurbsCurve->znorm = str.toFloat();
                    }
                }
                pGLDataCur->pData = pNurbsCurve;
                break;
            }
            case 128:       //B样条曲面实体
            {
                nurbsSurface_type *pNurbsSurface = new nurbsSurface_type;
                memset(pNurbsSurface,0,sizeof(nurbsSurface_type));
                QStringList list;
                list = strTmp.split(",");
                int size = list.size();
                if(size == 0)   //防止文件出现错误而出现段错误
                    break;
                pNurbsSurface->typeNum = list.at(0).toInt();
                pNurbsSurface->K1_num = list.at(1).toInt();
                pNurbsSurface->K2_num = list.at(2).toInt();
                pNurbsSurface->M1_num = list.at(3).toInt();
                pNurbsSurface->M2_num = list.at(4).toInt();
                pNurbsSurface->prop1_closed = list.at(5).toInt();
                pNurbsSurface->prop2_closed = list.at(6).toInt();
                pNurbsSurface->prop_poly = list.at(7).toInt();
                pNurbsSurface->prop1_periodic = list.at(8).toInt();
                pNurbsSurface->prop2_periodic = list.at(9).toInt();
                int N1_num = 1+pNurbsSurface->K1_num-pNurbsSurface->M1_num;
                int N2_num = 1+pNurbsSurface->K2_num-pNurbsSurface->M2_num;
                int A_num = N1_num+2*pNurbsSurface->M1_num+1;
                int B_num = N2_num+2*pNurbsSurface->M2_num+1;
                int C_num = (1+pNurbsSurface->K1_num)*(1+pNurbsSurface->K2_num);
                //为结点序列数组1赋值
                float *pKnot1 = new float[A_num];
                for(int i = 0; i < A_num; i++)
                {
                    QString str = list.at(i+10);
                    if(fileType == 0)   //Pro/E文件
                    {
                        QStringList list2 = str.split("D");
                        pKnot1[i] = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else
                    {
                        pKnot1[i] = str.toFloat();
                    }
                }
                pNurbsSurface->pKnot1 = pKnot1;
                //为结点序列数组2赋值
                float *pKnot2 = new float[B_num];
                for(int i = 0; i < B_num; i++)
                {
                    QString str = list.at(i+A_num+10);
                    if(fileType == 0)   //Pro/E文件
                    {
                        QStringList list2 = str.split("D");
                        pKnot2[i] = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else
                    {
                        pKnot2[i] = str.toFloat();
                    }
                }
                pNurbsSurface->pKnot2 = pKnot2;
                //为权重值数组赋值
                float *pWeight = new float[C_num];
                for(int i = 0; i < C_num; i++)
                {
                    QString str = list.at(i+A_num+B_num+10);
                    if(fileType == 0)   //Pro/E文件
                    {
                        QStringList list2 = str.split("D");
                        pWeight[i] = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else
                    {
                        pWeight[i] = str.toFloat();
                    }
                }
                pNurbsSurface->pWeight = pWeight;
                //为控制点数组赋值
                float *pCtl = new float[C_num*3];
                float pCtlData[C_num][3];
                float ptmp[C_num][3];
                for(int i = 0; i < C_num; i++)
                {
                    QString str = list.at(i*3+A_num+B_num+C_num+10);
                    if(fileType == 0)   //Pro/E文件
                    {
                        QStringList list2 = str.split("D");
                        pCtlData[i][0] = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        ptmp[i][0] = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else
                    {
                        pCtlData[i][0] = str.toFloat();
                        ptmp[i][0] = str.toFloat();
                    }
                    str = list.at(i*3+A_num+B_num+C_num+11);
                    if(fileType == 0)   //Pro/E文件
                    {
                        QStringList list2 = str.split("D");
                        pCtlData[i][1] = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        ptmp[i][1] = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else
                    {
                        pCtlData[i][1] = str.toFloat();
                        ptmp[i][1] = str.toFloat();
                    }
                    str = list.at(i*3+A_num+B_num+C_num+12);
                    if(fileType == 0)   //Pro/E文件
                    {
                        QStringList list2 = str.split("D");
                        pCtlData[i][2] = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        ptmp[i][2] = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else
                    {
                        pCtlData[i][2] = str.toFloat();
                        ptmp[i][2] = str.toFloat();
                    }
                }
                for(int i = 0; i < pNurbsSurface->K1_num+1; i++)
                {
                    for(int j = 0; j < pNurbsSurface->K2_num+1; j++)
                    {
                        pCtlData[i*(pNurbsSurface->K2_num+1)+j][0] = ptmp[j*(pNurbsSurface->K1_num+1)+i][0];
                        pCtlData[i*(pNurbsSurface->K2_num+1)+j][1] = ptmp[j*(pNurbsSurface->K1_num+1)+i][1];
                        pCtlData[i*(pNurbsSurface->K2_num+1)+j][2] = ptmp[j*(pNurbsSurface->K1_num+1)+i][2];
                    }
                }
                for(int i = 0; i < C_num; i++)
                {
                    pCtl[i*3] = pCtlData[i][0];
                    pCtl[i*3+1] = pCtlData[i][1];
                    pCtl[i*3+2] = pCtlData[i][2];
                }
                pNurbsSurface->pCtlarray = pCtl;
                //最后的四个点赋值
                for(int i = 0; i < 4; i++)
                {
                    QString str = list.at(i+A_num+B_num+C_num*4+10);
                    if(str.contains(";"))
                        str = str.split(";").at(0);
                    if(fileType == 0)   //Pro/E文件
                    {
                        QStringList list2 = str.split("D");
                        if(i == 0)
                            pNurbsSurface->U0 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 1)
                            pNurbsSurface->U1 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 2)
                            pNurbsSurface->V0 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                        else if(i == 3)
                            pNurbsSurface->V1 = list2.at(0).toFloat()*pow(10,list2.at(1).toInt());
                    }
                    else
                    {
                        if(i == 0)
                            pNurbsSurface->U0 = str.toFloat();
                        else if(i == 1)
                            pNurbsSurface->U1 = str.toFloat();
                        else if(i == 2)
                            pNurbsSurface->V0 = str.toFloat();
                        else if(i == 3)
                            pNurbsSurface->V1 = str.toFloat();
                    }
                }
                pGLDataCur->pData = pNurbsSurface;
                break;
            }
            case 142:           //参数曲面上的曲线实体
            {
                para_surface_type *pParaSurface = new para_surface_type;
                memset(pParaSurface,0,sizeof(para_surface_type));
                QStringList list;
                list = strTmp.split(",");
                int size = list.size();
                if(size == 0)   //防止文件出现错误而出现段错误
                    break;
                pParaSurface->typeNum = list.at(0).toInt();
                pParaSurface->curve_way = list.at(1).toInt();
                pParaSurface->pt_surface = list.at(2).toInt();
                pParaSurface->pt_curve = list.at(3).toInt();
                pParaSurface->cptr = list.at(4).toInt();
                QString str = list.at(5);
                if(str.contains(";"))
                    str = str.split(";").at(0);
                pParaSurface->pref = str.toInt();
                pGLDataCur->pData = pParaSurface;
                break;
            }
            case 144:           //裁剪曲面实体
            {
                trimmed_surface_type *pTrimmed = new trimmed_surface_type;
                memset(pTrimmed,0,sizeof(trimmed_surface_type));
                QStringList list;
                list = strTmp.split(",");
                int size = list.size();
                if(size == 0)   //防止文件出现错误而出现段错误
                    break;
                pTrimmed->typeNum = list.at(0).toInt();
                pTrimmed->pt_surface = list.at(1).toInt();
                pTrimmed->boundary = list.at(2).toInt();
                pTrimmed->curve_num = list.at(3).toInt();
                if(pTrimmed->curve_num == 0)
                {
                    QString str = list.at(4);
                    if(str.contains(";"))
                        str = str.split(";").at(0);
                    pTrimmed->pt_para = str.toInt();
                }
                else
                {
                    pTrimmed->pt_para = list.at(4).toInt();
                    for(int i = 0; i < pTrimmed->curve_num; i++)
                    {
                        QString str = list.at(5+i);
                        if(str.contains(";"))
                            str = str.split(";").at(0);
                        pTrimmed->pt_curves[i] = str.toInt();
                    }
                }
                pGLDataCur->pData = pTrimmed;
                break;
            }
            default:    //其他未考虑到的实体编号
                break;
            }
            pGLDataCur = pGLDataCur->pNext;
        }
        count_row++;
    }
    file.close();
    return pGLDataHead;
}

/********************************************
 *function:将读取文件后的结构体重新进行分配,以供画图使用
 *adding:
 *author: xu
 *date: 2015/10/05
 *******************************************/
trimmed_surface_type *fileReader::transform_Brep(gl_data *pHead)
{
    trimmed_surface_type *pTrimmedHead = NULL;
    trimmed_surface_type *pTrimmedLast = NULL;
    gl_data *pTmp = pHead;
    while(pTmp){
        if(pTmp->typeNum == 144)    //裁剪实体
        {
            pTmp->useCount++;       //该结点访问的次数
            trimmed_surface_type *pTrimmed = (trimmed_surface_type *)pTmp->pData;
            gl_data *pgldata = findGLData(pTrimmed->pt_surface,pHead);
            int typeNum = pgldata->typeNum;
            pTrimmed->surfaceType = typeNum;
            pTrimmed->bBendSurface = false; //初始化时设置初始值
            revolution_type *pRevolution = NULL;
            if(typeNum == 120)//旋转曲面实体，需要进行数据的转换,转换成nurbs曲面
            {
                pRevolution = (revolution_type *)pgldata->pData;
                gl_data *ptmp = findGLData(pRevolution->generatrix,pHead);
                line_type *generatrix,*axis;
                if(ptmp->typeNum == 110) //暂只考虑母线为直线的情况
                    generatrix = (line_type *)ptmp->pData;
                ptmp = findGLData(pRevolution->axis,pHead);
                if(ptmp->typeNum == 110)
                    axis = (line_type *)ptmp->pData;
                float x = generatrix->start.x-generatrix->end.x;
                float y = generatrix->start.y-generatrix->end.y;
                float z = generatrix->start.z-generatrix->end.z;
                float width = sqrt(x*x+y*y+z*z);
                pTrimmed->pSurface = formRevolution(*generatrix,*axis,\
                    pRevolution->start_angle,pRevolution->end_angle);
                memcpy(&pTrimmed->axis,axis,sizeof(line_type));
                memcpy(&pTrimmed->generatrix,generatrix,sizeof(line_type));
                pTrimmed->angleFlag = false;
                pTrimmed->start_angle = pRevolution->start_angle;
                pTrimmed->end_angle = pRevolution->end_angle;
//                point3f normal;
//                normal.x = x/width;
//                normal.y = y/width;
//                normal.z = z/width;
//                if(fabs(fabs(normal.z)-1) > EPSINON)//根据母线的法向量进行判断
//                    pTrimmed->bBendSurface = true;
                if(width > BEND_SURFACE_DIFFER)   //此处暂时这样处理,母线长度大于10.00认为是折弯面
                    pTrimmed->bBendSurface = true;
            }
            else if(typeNum == 128)//将128实体赋值到裁剪实体结构体中
            {
                pgldata->useCount++;//组成裁剪曲面的内存需要保留
                pTrimmed->pSurface = (nurbsSurface_type *)pgldata->pData;
            }
            else                //其他实体类型需要其他算法，此处暂不考虑
                qDebug()<<"it is not 120/128 entity";
            //裁剪实体先指向外边界的曲线组合
            for(int i = 0; i < pTrimmed->curve_num+1; i++)
            {
                if(i == 0)      //第一个首先指向外边界
                    pgldata = findGLData(pTrimmed->pt_para,pHead);
                else            //存在内边界的需要重新查找
                    pgldata = findGLData(pTrimmed->pt_curves[i-1],pHead);
                if(pgldata->typeNum == 142)
                {
                    para_surface_type *para = (para_surface_type *)pgldata->pData;
                    //首先写入裁剪轮廓
                    pgldata = findGLData(para->pt_curve,pHead);
                    if(pgldata->typeNum == 102) //组合曲线
                    {
                        composite_curve_type *pCurves = (composite_curve_type *)pgldata->pData;
                        for(int j = 0; j < pCurves->curveCount; j++)
                        {
                            pgldata = findGLData(pCurves->curvePos[j],pHead);
                            pgldata->useCount++;//组成闭合曲线的内存需要保留
                            pTrimmed->pCurves[i][j] = pgldata->pData;
                            pTrimmed->curveType[i][j] = pgldata->typeNum;
                        }
                        pTrimmed->curveCount[i] = pCurves->curveCount;
                    }
                    //然后写入三维实体轮廓线
                    pgldata = findGLData(para->cptr,pHead);
                    if(pgldata->typeNum == 102) //组合曲线
                    {
                        composite_curve_type *pCurves = (composite_curve_type *)pgldata->pData;
                        for(int j = 0; j < pCurves->curveCount; j++)
                        {
                            pgldata = findGLData(pCurves->curvePos[j],pHead);
                            pTrimmed->outlineType[i][j] = pgldata->typeNum;
                            if(pgldata->typeNum == 100){//圆弧实体
                                arc_type *pData = (arc_type *)pgldata->pData;
                                arc_type *ptmp = new arc_type;
                                memcpy(ptmp,pData,sizeof(arc_type));
                                pTrimmed->pOutlines[i][j] = ptmp;
                            }
                            else if(pgldata->typeNum == 110){//直线实体
                                line_type *pData = (line_type *)pgldata->pData;
                                line_type *ptmp = new line_type;
                                memcpy(ptmp,pData,sizeof(line_type));
                                pTrimmed->pOutlines[i][j] = ptmp;
                            }
                            else if(pgldata->typeNum == 126){//nurbs曲线
                                pTrimmed->pOutlines[i][j] = pgldata->pData;
                                //nurbsCurve_type *pData = (nurbsCurve_type *)pgldata->pData;
                            }
                            pgldata->useCount++;//组成闭合曲线的内存需要保留

                        }
                        pTrimmed->outlineCount[i] = pCurves->curveCount;
                    }
                }
            }
            //设置折弯面的折弯角度的正负号
            if(typeNum == 120)  //为折弯面
            {
                float start_angle = pRevolution->start_angle;
                float end_angle = pRevolution->end_angle;
                float angle = end_angle-start_angle;
                if(fileType == 0)   //pro/e生成的文件
                {
                    if(start_angle > 0)
                        pTrimmed->angleFlag = true;
                    else
                        pTrimmed->angleFlag = false;
                    pTrimmed->angle = fabs(start_angle+end_angle);
                    for(int i = 0; i < pTrimmed->curve_num+1; i++)
                    {
                        for(int j = 0; j < pTrimmed->curveCount[i]; j++)
                        {
                            int type = pTrimmed->curveType[i][j];
                            if(type == 110) //NurbsCurve实体
                            {
                               line_type *pline = (line_type *)pTrimmed->pCurves[i][j];
                               pline->start.y = (pline->start.y-start_angle)/angle;
                               pline->end.y = (pline->end.y-start_angle)/angle;
                            }
                        }
                    }
                }
                else        //SolidWorks生成的文件
                {
                    float max = 0.0;
                    float min = 1.0;
                    for(int i = 0; i < pTrimmed->curve_num+1; i++)
                    {
                        for(int j = 0; j < pTrimmed->curveCount[i]; j++)
                        {
                            int type = pTrimmed->curveType[i][j];
                            if(type == 126) //NurbsCurve实体
                            {
                               nurbsCurve_type *pCurve = (nurbsCurve_type *)pTrimmed->pCurves[i][j];
                               int num = pCurve->K_num+1;
                               for(int m = 0; m < num; m++)
                               {
                                   pCurve->pCtlarray[3*m+1] /= angle;
                                   if(pCurve->pCtlarray[3*m+1] > max)
                                       max = pCurve->pCtlarray[3*m+1];
                                   if(pCurve->pCtlarray[3*m+1] < min)
                                       min = pCurve->pCtlarray[3*m+1];
                               }
                            }
                        }
                    }
                    if(min > EPSINON)
                        pTrimmed->angleFlag = false;
                    else
                        pTrimmed->angleFlag = true;
                }
            }
            if(pTrimmedHead == NULL)
            {
                pTrimmedHead = pTrimmed;
                pTrimmedLast = pTrimmed;
                pTrimmed->pNext = NULL;
            }
            else
            {
                pTrimmedLast->pNext = pTrimmed;
                pTrimmedLast = pTrimmed;
                pTrimmedLast->pNext = NULL;
            }
        }
        pTmp = pTmp->pNext;
    }
    return pTrimmedHead;
}

/********************************************
 *function:修改当前折弯的母线,并返回与该母线重合的左右基面
 *adding:  返回-1说明是左侧基面,返回1说明为右侧基面,
 *         返回0说明计算错误
 *author: xu
 *date: 2016/06/26
 *******************************************/
int fileReader::modifyGeneratrix(bend_surface *pbend)
{
    basic_surface *pleft = pbend->pLeftBase;
    basic_surface *pright = pbend->pRightBase;
    trimmed_surface_type *psurface = pbend->pSurface;
    //由于SolidWorks生成的iges文件中折弯面的母线与左右基面不相同,所以需要进行处理
    line_type generatrix = pbend->generatrix;
    line_type generatrix_tmp;
    line_type axis = pbend->axis;
    if(fileType != 0){
        //solidworks生成的文件
        float min = 1;
        float max = 0;
        //折弯面正常情况下只有一个外轮廓,并且此处只考虑外轮廓
        for(int j = 0; j < psurface->curveCount[0]; j++)
        {
            int type = psurface->curveType[0][j];
            if(126 == type)
            {
                nurbsCurve_type *pcurve = (nurbsCurve_type *)psurface->pCurves[0][j];
                int num = pcurve->K_num+1;
                for(int m = 0; m < num; m++)
                {
                    if(pcurve->pCtlarray[3*m+1] < min)
                        min = pcurve->pCtlarray[3*m+1];
                    if(pcurve->pCtlarray[3*m+1] > max)
                        max = pcurve->pCtlarray[3*m+1];
                }
            }
        }
        float angle = max*2*PI;
        point3f axis_point;
        axis_point.x = axis.end.x-axis.start.x;
        axis_point.y = axis.end.y-axis.start.y;
        axis_point.z = axis.end.z-axis.start.z;
        generatrix.start.x -= axis.start.x;
        generatrix.start.y -= axis.start.y;
        generatrix.start.z -= axis.start.z;
        generatrix.end.x -= axis.start.x;
        generatrix.end.y -= axis.start.y;
        generatrix.end.z -= axis.start.z;
        //对目前进行统一的变化
        xform_type *matrix = formRevolutinMatrix(axis_point,angle,axis.start);
        xform_matrix(matrix,generatrix.start);
        xform_matrix(matrix,generatrix.end);
        delete matrix;
        generatrix_tmp = generatrix;
    }
    else{
        //pro/e生成的文件
        float min = 1;
        float max = 0;
        point3f normal;
        normal.x = generatrix.end.x-generatrix.start.x;
        normal.y = generatrix.end.y-generatrix.start.y;
        normal.z = generatrix.end.z-generatrix.start.z;
        float len = sqrt(normal.x*normal.x+normal.y*normal.y+normal.z*normal.z);
        normal.x /= len;
        normal.y /= len;
        normal.z /= len;
        //折弯面正常情况下只有一个外轮廓,并且此处只考虑外轮廓
        for(int j = 0; j < psurface->curveCount[0]; j++)
        {
            int type = psurface->curveType[0][j];
            if(110 == type)
            {
                line_type *pline = (line_type *)psurface->pCurves[0][j];
                if(pline->start.x > max)
                    max = pline->start.x;
                if(pline->start.x < min)
                    min = pline->start.x;
            }
        }
        float x0 = generatrix.start.x;
        float y0 = generatrix.start.y;
        float z0 = generatrix.start.z;
        generatrix_tmp.start.x = x0+normal.x*len*min;
        generatrix_tmp.start.y = y0+normal.y*len*min;
        generatrix_tmp.start.z = z0+normal.z*len*min;
        generatrix_tmp.end.x = x0+normal.x*len*max;
        generatrix_tmp.end.y = y0+normal.y*len*max;
        generatrix_tmp.end.z = z0+normal.z*len*max;

    }
    //寻找与母线重合的基面
    for(int i = 0; i < pleft->pSurface->curve_num+1; i++)
    {
        for(int j = 0; j < pleft->pSurface->outlineCount[i]; j++)
        {
            int type = pleft->pSurface->outlineType[i][j];
            if(110 == type)
            {
                line_type *pline = (line_type *)pleft->pSurface->pOutlines[i][j];
                if(judgeLineEqual(pline,&generatrix_tmp)){
                    if(fileType == 0)
                        return 1;
                    else
                        return -1;
                }
            }
        }
    }
    for(int i = 0; i < pright->pSurface->curve_num+1; i++)
    {
        for(int j = 0; j < pright->pSurface->outlineCount[i]; j++)
        {
            int type = pright->pSurface->outlineType[i][j];
            if(110 == type)
            {
                line_type *pline = (line_type *)pright->pSurface->pOutlines[i][j];
                if(judgeLineEqual(pline,&generatrix_tmp)){
                    if(fileType == 0)
                        return -1;
                    else
                        return 1;
                }
            }
        }
    }
    qDebug()<<"not find basic surface!";
    return 0;
}

/********************************************
 *function:将原始文件转换到xz平面内
 *adding:
 *author: xu
 *date: 2015/10/23
 *******************************************/
void fileReader::convertOriginToXY(bend_surface *pHead)
{
    bend_surface *pbend = pHead;
    float max_len = 0;
    bend_surface *ptmp_bend = NULL;
    while(pbend){
        line_type *pline = pbend->pBendLine;
        float x = pline->start.x-pline->end.x;
        float y = pline->start.y-pline->end.y;
        float z = pline->start.z-pline->end.z;
        float len = sqrt(x*x+y*y+z*z);
        if(len > max_len){
            ptmp_bend = pbend;
            max_len = len;
        }
        pbend = pbend->pNext;
    }
    pbend = ptmp_bend;
    line_type *pbendline = pbend->pBendLine;
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
//    off.x = -(pbend->pDeploy->point[0].x+pbend->pDeploy->point[2].x)/2;
//    off.y = -(pbend->pDeploy->point[0].y+pbend->pDeploy->point[2].y)/2;
//    off.z = -(pbend->pDeploy->point[0].z+pbend->pDeploy->point[2].z)/2;
//    source.x = pbend->pDeploy->point[0].x+off.x;
//    source.y = pbend->pDeploy->point[0].y+off.y;
//    source.z = pbend->pDeploy->point[0].z+off.z;
//    dest.x = 0.0;
//    dest.y = 0.0;
//    dest.z = 1.0;
//    xform_type *pRote2 = computeMatrix(source,dest);
//    MatrixTotalConvert(pHead,pRote2,false);
//    MatrixTotalConvert(pHead,pRote2,true);
//    delete pRote2;

    //将板料移到中心位置
    pbend = pHead;
    trimmed_surface_type *ptmp = NULL;
    int max = 0;
    while (pbend) {
        if(pbend->pLeftBase->pBendSurface.size() > max){
            max = pbend->pLeftBase->pBendSurface.size();
            ptmp = pbend->pLeftBase->pSurface;
        }
        if(pbend->pRightBase->pBendSurface.size() > max){
            max = pbend->pRightBase->pBendSurface.size();
            ptmp = pbend->pRightBase->pSurface;
        }
        pbend = pbend->pNext;
    }
    off.x = 0;
    off.y = 0;
    off.z = 0;
    int count = 0;
    for(int i = 0; i < ptmp->outlineCount[0]; i++){
        int type = ptmp->outlineType[0][i];
        if(type == 110){
            line_type *pline = (line_type *)ptmp->pOutlines[0][i];
            off.x += pline->start.x;
            off.y += pline->start.y;
            off.z += pline->start.z;
            count++;
        }
        else if(type == 126){
            //nurbsCurve_type *parc = (arc_type *)ptmp->pOutlines[0][i];
        }
    }
    off.x = -off.x/count;
    off.y = -off.y/count;
    off.z = -off.z/count;
    xform_type *matrix = formTranslationMatrix(off);
    MatrixTotalConvert(pHead,matrix,false);
    MatrixTotalConvert(pHead,matrix,true);
    off.x = 0;
    off.y = 0;
    off.z = 0;
    for(int i = 0; i < ptmp->outlineCount[0]; i++){
        int type = ptmp->outlineType[0][i];
        if(type == 110){
            line_type *pline = (line_type *)ptmp->pOutlines[0][i];
            off.x += pline->start.x;
            off.y += pline->start.y;
            off.z += pline->start.z;
        }
        else if(type == 126){
            //nurbsCurve_type *parc = (arc_type *)ptmp->pOutlines[0][i];
        }
    }
    delete matrix;
}

/********************************************
 *function:获取工件的特征信息
 *adding:根据filetype的取值进行不同的操作方式
 * pro/e的
 *author: xu
 *date: 2015/10/23
 *******************************************/
bend_surface *fileReader::getWorkPieceInfo(trimmed_surface_type *pTrimmed)
{
    bend_surface *pHead = NULL;
    bend_surface *pLast = NULL;
    trimmed_surface_type *pWork = pTrimmed;
    bend_surface *pBendTmp = NULL;
    //首先可得到2倍的折弯点数
    while (pWork) {
        int typenum = pWork->surfaceType;
        if(typenum == 120 && pWork->bBendSurface)  //为折弯面
        {
            bend_surface *pbend = new bend_surface;
            memset(pbend,0,sizeof(bend_surface));
            pbend->pSurface = pWork;
            pbend->axis = pWork->axis;
            pbend->generatrix = pWork->generatrix;
            //pbend->bendAngle = getBendAngle(pWork);
            QVector<trimmed_surface_type *> pResult = findTrimmedSurface(pWork,pTrimmed);
            if(pResult.size() == 2)//正常情况下都应该是一个折弯曲面相邻两个基面
            {
                basic_surface *pLeft = new basic_surface;
                pLeft->isTop = 0;
                pLeft->isVisited = 0;
                pLeft->pSurface = pResult[0];
                pLeft->pBendSurface = findTrimmedSurface(pResult[0],pTrimmed);
                pbend->pLeftBase = pLeft;
                basic_surface *pRight = new basic_surface;
                pRight->isTop = 0;
                pRight->isVisited = 0;
                pRight->pSurface = pResult[1];
                pRight->pBendSurface = findTrimmedSurface(pResult[1],pTrimmed);
                pbend->pRightBase = pRight;
                pbend->isBended = 0;
                //析构重复的基面内存
                bend_surface *ptmp = pHead;
                while (ptmp) {
                    if(ptmp->pLeftBase->pSurface == pResult[0])
                    {
                        pbend->pLeftBase = ptmp->pLeftBase;
                        if(pLeft)
                            delete pLeft;
                        pLeft = NULL;
                    }
                    if(ptmp->pLeftBase->pSurface == pResult[1])
                    {
                        pbend->pRightBase = ptmp->pLeftBase;
                        if(pRight)
                            delete pRight;
                        pRight = NULL;
                    }
                    if(ptmp->pRightBase->pSurface == pResult[0])
                    {
                        pbend->pLeftBase = ptmp->pRightBase;
                        if(pLeft)
                            delete pLeft;
                        pLeft = NULL;
                    }
                    if(ptmp->pRightBase->pSurface == pResult[1])
                    {
                        pbend->pRightBase = ptmp->pRightBase;
                        if(pRight)
                            delete pRight;
                        pRight = NULL;
                    }
                    ptmp = ptmp->pNext;
                }
            }
            //构造折弯面的展开平面
            //掌握两个基面的相连折弯面数，以相连折弯面数多的基面为基准，另一个基面需要做旋转变换
            if(pHead == NULL)
            {
                pHead = pbend;
                pLast = pbend;
                pLast->pNext = NULL;
                pLast->pParallel = NULL;
            }
            else
            {
                pLast->pNext = pbend;
                pLast = pbend;
                pLast->pNext = NULL;
                pLast->pParallel = NULL;
            }
        }
        pWork = pWork->pNext;
    }
    //设置上下面
    setBendTopFlag(pHead,pHead);
    //因为钣金工件的厚度是一致的，所以上下面的是完全相同的，
    //因而通过这一个性质可以得到钣金工件相互平行的各个面`
    pBendTmp = pHead;
    while (pBendTmp) {
        if(pBendTmp->pParallel != NULL)
        {
            pBendTmp = pBendTmp->pNext;
            continue;
        }
        bend_surface *ptmp = pHead;
        while(ptmp){
            if(pBendTmp == ptmp || ptmp->pParallel != NULL || \
                    !judgeLineEqual(&pBendTmp->axis,&ptmp->axis))
            {
                ptmp = ptmp->pNext;
                continue;
            }
            //进行到这一步,说明该两个节点是平行的
            trimmed_surface_type *pLeft1 = pBendTmp->pLeftBase->pSurface;
            trimmed_surface_type *pRight1 = pBendTmp->pRightBase->pSurface;
            trimmed_surface_type *pLeft2 = ptmp->pLeftBase->pSurface;
            trimmed_surface_type *pRight2 = ptmp->pRightBase->pSurface;
            point3f normal1,normal2,normal3,normal4;
            computeTrimmedNormal(pLeft1,normal1);
            computeTrimmedNormal(pRight1,normal2);
            computeTrimmedNormal(pLeft2,normal3);
            computeTrimmedNormal(pRight2,normal4);
            if(fabs(fabs(normal1.x)-fabs(normal4.x))<EPSINON && \
               fabs(fabs(normal1.y)-fabs(normal4.y))<EPSINON && \
               fabs(fabs(normal1.z)-fabs(normal4.z))<EPSINON)
            {
                basic_surface *pleftbase = ptmp->pLeftBase;
                basic_surface *prightbase = ptmp->pRightBase;
                ptmp->pLeftBase = prightbase;
                ptmp->pRightBase = pleftbase;
            }
            pBendTmp->pParallel = ptmp;
            ptmp->pParallel = pBendTmp;
            break;
        }
        pBendTmp = pBendTmp->pNext;
    }
    //为折弯面做顺序标记,并且重新定位折弯数据指针，结点数量为折弯点个数
    bend_surface *phead_tmp = NULL;
    bend_surface *plast_tmp = NULL;
    int count = 0;
    pBendTmp = pHead;
    while (pBendTmp) {
        if(pBendTmp->isTop == false)
        {
            pBendTmp = pBendTmp->pNext;
            continue;
        }
        count++;
        pBendTmp->bendNum = count;
        pBendTmp->pParallel->bendNum = count;
        if(phead_tmp == NULL)
        {
            phead_tmp = pBendTmp;
            plast_tmp = pBendTmp;
        }
        else
        {
            plast_tmp->pNext = pBendTmp;
            plast_tmp = pBendTmp;
        }
        pBendTmp = pBendTmp->pNext;
    }
    if(plast_tmp == NULL)
    {
        QMessageBox message(QMessageBox::Warning,"Error","当前工件不存在折弯特征，请重新选择工件！",QMessageBox::Yes);
        if(message.exec() == QMessageBox::Yes)
        {
            character_flag = true;
            pHead = NULL;
            return pHead;
        }
    }
    else
        plast_tmp->pNext = NULL;
    pHead = phead_tmp;
    bendNum = count;    //记录折弯总数
    //获取工件的厚度和折弯半径
    getThickAndRadiusAndAngle(pHead);
    //************寻找各个基面的侧面********************************************/
    pBendTmp = pHead;
    resetVisitFlag(pHead);
    while (pBendTmp) {
        basic_surface *pleft = pBendTmp->pLeftBase;
        basic_surface *pright = pBendTmp->pRightBase;
        pBendTmp->isBended = false;
        pBendTmp->pParallel->isBended = false;
        if(pleft->isVisited == 0){
            pleft->pSideSurface = findSideSurface(pleft->pSurface,pTrimmed);
            pleft->isVisited = 1;
        }
        if(pright->isVisited == 0){
            pright->pSideSurface = findSideSurface(pright->pSurface,pTrimmed);
            pright->isVisited = 1;
        }
        pBendTmp = pBendTmp->pNext;
    }
    resetVisitFlag(pHead);
    //**************************************************************************
    //将原始数据转换到展开数据,本函数仅用于SolidWorks生成的IGES文件
    convertToDeploy(pHead);
    //生成板料的轮廓数据
    resetVisitFlag(pHead);
    pBendTmp = pHead;
    while (pBendTmp) {
        pBendTmp->pDeploy = formDeploySurface(pBendTmp);
        pBendTmp->pParallel->pDeploy = formDeploySurface(pBendTmp->pParallel);
        formSideSurface(pBendTmp);
        pBendTmp = pBendTmp->pNext;
    }
    resetVisitFlag(pHead);       //重置访问标志位
    //将原始数据转换到XZ轴平面内
    convertOriginToXY(pHead);
    return pHead;
}

/********************************************
 *function:获取折弯面的折弯角度
 *adding:
 *author: xu
 *date: 2015/10/26
 *******************************************/
float fileReader::getBendAngle(trimmed_surface_type *pSurface)
{
    float angle = 0.0;
    if(pSurface == NULL || pSurface->surfaceType != 120)
    {
        qDebug()<<"getBendAngle--the surface is null or surface type is incorrect!";
        return angle;
    }
    for(int i = 0; i < pSurface->outlineCount[0]; i++)
    {
        int typenum = pSurface->outlineType[0][i];
        if(typenum == 100)
        {
            arc_type *pArc = (arc_type *)pSurface->pOutlines[0][i];
            float X1 = pArc->X1;
            float X2 = pArc->X2;
            float X3 = pArc->X3;
            float Y1 = pArc->Y1;
            float Y2 = pArc->Y2;
            float Y3 = pArc->Y3;
            //首先求得圆弧的半径
            float radius = sqrt((X2-X1)*(X2-X1)+(Y2-Y1)*(Y2-Y1));
            //求得圆弧的弧度
            angle = PI - acos(((X2-X1)*(X3-X1)+(Y2-Y1)*(Y3-Y1))/(radius*radius));
            if(pSurface->angleFlag == false)
                return -angle;
            else
                return angle;
        }
    }
    qDebug()<<"getBendAngle--error!";
    return angle;   //查询出现问题，同样返回0
}

/********************************************
 *function:查找符合条件的裁剪曲面
 *adding:
 *author: xu
 *date: 2015/10/25
 *******************************************/
QVector<trimmed_surface_type *> fileReader::findTrimmedSurface(trimmed_surface_type *pTrimmed,trimmed_surface_type *pHead)
{
    QVector<trimmed_surface_type *> pResult;
    if(pTrimmed == NULL)
    {
        qDebug()<<"findTrimmedSurface--the surface is null!";
        return pResult;
    }
    trimmed_surface_type *ptmp = pHead;
    while(ptmp){
        if((ptmp == pTrimmed) || (ptmp->surfaceType == pTrimmed->surfaceType))
        {
            //裁剪曲面为本身或者裁剪曲面和形参的曲面类型相同,直接跳过
            ptmp = ptmp->pNext;
            continue;
        }
        bool flag = false;
        for(int m = 0; m < ptmp->curve_num+1; m++)
        {
            if(flag == true)
                break;
            for(int n = 0; n < ptmp->outlineCount[m]; n++)
            {
                if(flag == true)
                    break;
                int num = ptmp->outlineType[m][n];
                if(num == 110)//直线实体
                {
                    line_type *pLinetmp = (line_type *)ptmp->pOutlines[m][n];
                    for(int i = 0; i < pTrimmed->curve_num+1; i++)
                    {
                        if(flag == true)
                            break;
                        for(int j = 0; j < pTrimmed->outlineCount[i]; j++)
                        {
                            if(flag == true)
                                break;
                            int typenum = pTrimmed->outlineType[i][j];
                            if(typenum == 110)  //直线实体
                            {
                                line_type *pLine = (line_type *)pTrimmed->pOutlines[i][j];
                                if(judgeLineEqual(pLine,pLinetmp))
                                {
                                    pResult.push_back(ptmp);
                                    flag = true;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
        ptmp = ptmp->pNext;
    }
    return pResult;
}

/********************************************
 *function:查找基面侧面的裁剪曲面
 *adding:
 *author: xu
 *date: 2017/03/01
 *******************************************/
QVector<trimmed_surface_type *> fileReader::findSideSurface(trimmed_surface_type *pCur, trimmed_surface_type *pHead)
{
    QVector<trimmed_surface_type *> pResult;
    if(pCur == NULL)
    {
        qDebug()<<"findTrimmedSurface--the surface is null!";
        return pResult;
    }
    trimmed_surface_type *ptmp = pHead;
    while(ptmp){
        if(ptmp == pCur || ptmp->bBendSurface)
        {
            //裁剪曲面为本身或者裁剪曲面和折弯面,直接跳过
            ptmp = ptmp->pNext;
            continue;
        }
        bool flag = false;
        for(int m = 0; m < ptmp->curve_num+1; m++)
        {
            if(flag == true)
                break;
            for(int n = 0; n < ptmp->outlineCount[m]; n++)
            {
                if(flag == true)
                    break;
                int num = ptmp->outlineType[m][n];
                if(num == 110){
                    //直线实体
                    line_type *pLinetmp = (line_type *)ptmp->pOutlines[m][n];
                    for(int i = 0; i < pCur->curve_num+1; i++){
                        if(flag == true)
                            break;
                        for(int j = 0; j < pCur->outlineCount[i]; j++){
                            if(flag == true)
                                break;
                            int typenum = pCur->outlineType[i][j];
                            if(typenum == 110){
                                  //直线实体
                                line_type *pLine = (line_type *)pCur->pOutlines[i][j];
                                if(judgeLineEqual(pLine,pLinetmp)){
                                    pResult.push_back(ptmp);
                                    flag = true;
                                    break;
                                }
                            }
                        }
                    }
                }
                else if(num == 100){ //圆弧实体
                    arc_type *pArc_tmp = (arc_type *)ptmp->pOutlines[m][n];
                    for(int i = 0; i < pCur->curve_num+1; i++){
                        if(flag == true)
                            break;
                        for(int j = 0; j < pCur->outlineCount[i]; j++){
                            if(flag == true)
                                break;
                            int typenum = pCur->outlineType[i][j];
                            if(typenum == 100){ //圆弧实体
                                arc_type *pArc = (arc_type *)pCur->pOutlines[i][j];
                                if(fabs(pArc_tmp->X1-pArc->X1) < EPSINON && fabs(pArc_tmp->Y1-pArc->Y1) < EPSINON &&\
                                   fabs(pArc_tmp->ZT-pArc->ZT) < EPSINON){
                                    //首先判断圆心和深度是否相同
                                    if((fabs(pArc_tmp->X2-pArc->X2) < EPSINON && fabs(pArc_tmp->X3-pArc->X3) < EPSINON &&\
                                    fabs(pArc_tmp->Y2-pArc->Y2) < EPSINON && fabs(pArc_tmp->Y3-pArc->Y3) < EPSINON) ||\
                                    (fabs(pArc_tmp->X2-pArc->X3) < EPSINON && fabs(pArc_tmp->X3-pArc->X2) < EPSINON &&\
                                    fabs(pArc_tmp->Y2-pArc->Y3) < EPSINON && fabs(pArc_tmp->Y3-pArc->Y2) < EPSINON)){
                                        //然后判断圆弧的另外两个点
                                        if(fileType == 0){//pro/e的文件需要加上对旋转矩阵进行的判断
                                            xform_type form = pArc->pForm;
                                            xform_type form_tmp = pArc_tmp->pForm;
                                            if(judgeXformEqual(form,form_tmp)){
                                                pResult.push_back(ptmp);
                                                flag = true;
                                                break;
                                            }
                                        }
                                        else{
                                            pResult.push_back(ptmp);
                                            flag = true;
                                            break;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        ptmp = ptmp->pNext;
    }
    return pResult;
}

/********************************************
 *function:判断两条直线是否相同
 *adding:
 *author: xu
 *date: 2015/9/18
 *******************************************/
bool fileReader::judgeLineEqual(line_type *pline1, line_type *pline2)
{
    if(pline1 == NULL || pline2 == NULL)
    {
        qDebug()<<"judgeLineEqual--line is null!";
        return false;
    }
    float x_start1 = pline1->start.x;
    float x_end1 = pline1->end.x;
    float y_start1 = pline1->start.y;
    float y_end1 = pline1->end.y;
    float z_start1 = pline1->start.z;
    float z_end1 = pline1->end.z;
    float x_start2 = pline2->start.x;
    float x_end2 = pline2->end.x;
    float y_start2 = pline2->start.y;
    float y_end2 = pline2->end.y;
    float z_start2 = pline2->start.z;
    float z_end2 = pline2->end.z;
    if(((fabs(x_start1-x_end2) < EPSINON) && (fabs(x_end1-x_start2) < EPSINON) &&\
       (fabs(y_start1-y_end2) < EPSINON) && (fabs(y_end1-y_start2) < EPSINON) &&\
       (fabs(z_start1-z_end2) < EPSINON) && (fabs(z_end1-z_start2) < EPSINON)) ||\
       ((fabs(x_start1-x_start2) < EPSINON) && (fabs(x_end1-x_end2) < EPSINON) &&\
       (fabs(y_start1-y_start2) < EPSINON) && (fabs(y_end1-y_end2) < EPSINON) &&\
       (fabs(z_start1-z_start2) < EPSINON) && (fabs(z_end1-z_end2) < EPSINON)))
        return true;
    return false;
}

bool fileReader::judgeXformEqual(xform_type form1, xform_type form2)
{
    float R11 = form1.R11;
    float R12 = form1.R12;
    float R13 = form1.R13;
    float T1 = form1.T1;
    float R21 = form1.R21;
    float R22 = form1.R22;
    float R23 = form1.R23;
    float T2 = form1.T2;
    float R31 = form1.R31;
    float R32 = form1.R32;
    float R33 = form1.R33;
    float T3 = form1.T3;

    float R11_2 = form2.R11;
    float R12_2 = form2.R12;
    float R13_2 = form2.R13;
    float T1_2 = form2.T1;
    float R21_2 = form2.R21;
    float R22_2 = form2.R22;
    float R23_2 = form2.R23;
    float T2_2 = form2.T2;
    float R31_2 = form2.R31;
    float R32_2 = form2.R32;
    float R33_2 = form2.R33;
    float T3_2 = form2.T3;

    if(fabs(R11-R11_2) < EPSINON && fabs(R12-R12_2) < EPSINON && fabs(R13-R13_2) < EPSINON &&\
       fabs(R21-R21_2) < EPSINON && fabs(R22-R22_2) < EPSINON && fabs(R23-R23_2) < EPSINON &&\
       fabs(R31-R31_2) < EPSINON && fabs(R32-R32_2) < EPSINON && fabs(R33-R33_2) < EPSINON &&\
       fabs(T1-T1_2) < EPSINON && fabs(T2-T2_2) < EPSINON && fabs(T3-T3_2) < EPSINON){
        return true;
    }
    return false;
}

/********************************************
 *function:计算平面的法向量
 *adding:  干涉返回true,不干涉返回false
 *author: xu
 *date: 2015/11/19
 *******************************************/
void fileReader::computeSurfaceNormal(stu_polygon *pPolygon, point3f &normal)
{
    int num = pPolygon->point.size();
    point3f point1,point2,point3;
    point3f vector1,vector2;
    point1 = pPolygon->point[0];
    point2 = pPolygon->point[1];
    vector1.x = point2.x-point1.x;
    vector1.y = point2.y-point1.y;
    vector1.z = point2.z-point1.z;
    //求得vector1的单位向量
    float unit1 = sqrt((vector1.x*vector1.x)+(vector1.y*vector1.y)+(vector1.z*vector1.z));
    vector1.x = vector1.x/unit1;
    vector1.y = vector1.y/unit1;
    vector1.z = vector1.z/unit1;
    //point3的选择很重要,需要验证,三点不能再一条直线上
    for(int i = 2; i < num; i++)
    {
        point3 = pPolygon->point[i];
        vector2.x = point3.x-point1.x;
        vector2.y = point3.y-point1.y;
        vector2.z = point3.z-point1.z;
        //求得vector2的单位向量
        float unit2 = sqrt((vector2.x*vector2.x)+(vector2.y*vector2.y)+(vector2.z*vector2.z));
        vector2.x = vector2.x/unit2;
        vector2.y = vector2.y/unit2;
        vector2.z = vector2.z/unit2;
        if(fabs(vector2.x-vector1.x)<EPSINON && fabs(vector2.y-vector1.y)<EPSINON\
            && fabs(vector2.z-vector1.z)<EPSINON)
        {
            //说明三个空间点共线
            continue;
        }
        break;
    }
    float x = (vector1.y*vector2.z)-(vector2.y*vector1.z);
    float y = (vector2.x*vector1.z)-(vector1.x*vector2.z);
    float z = (vector1.x*vector2.y)-(vector2.x*vector1.y);
    float n = sqrt(x*x+y*y+z*z);
    normal.x = x/n;
    normal.y = y/n;
    normal.z = z/n;
}

void fileReader::computeTrimmedNormal(trimmed_surface_type *ptrimmed, point3f &normal)
{
    stu_polygon polygon;
    for(int j = 0; j < ptrimmed->outlineCount[0]; j++)
    {
        int type = ptrimmed->outlineType[0][j];
        if(type == 110)
        {
            line_type *pline = (line_type *)ptrimmed->pOutlines[0][j];
            polygon.point.push_back(pline->start);
        }
    }
    computeSurfaceNormal(&polygon,normal);
}

/********************************************
 *function:获取工件的厚度和折弯半径
 *adding:
 *author: xu
 *date: 2016/04/01
 *******************************************/
void fileReader::getThickAndRadiusAndAngle(bend_surface *pHead)
{
    bend_surface *pbend = pHead;
    float min_radius,max_radius;
    float angle = 0.0;
    while (pbend) {
        trimmed_surface_type *ptmp = pbend->pSurface;
        trimmed_surface_type *ptmp_parallel = pbend->pParallel->pSurface;
        for(int i = 0; i < ptmp->outlineCount[0]; i++)
        {
            int typenum = ptmp->outlineType[0][i];
            if(typenum == 100)
            {
                arc_type *pArc = (arc_type *)ptmp->pOutlines[0][i];
                float X1 = pArc->X1;
                float X2 = pArc->X2;
                float X3 = pArc->X3;
                float Y1 = pArc->Y1;
                float Y2 = pArc->Y2;
                float Y3 = pArc->Y3;
                //首先求得圆弧的半径
                min_radius = sqrt((X2-X1)*(X2-X1)+(Y2-Y1)*(Y2-Y1));
                angle = PI - acos(((X2-X1)*(X3-X1)+(Y2-Y1)*(Y3-Y1))/(min_radius*min_radius));
                break;
            }
//            else if(typenum == 126)
//            {
//                nurbsCurve_type *pNurbsCurve = (nurbsCurve_type *)ptmp->pOutlines[0][i];
//                int num = pNurbsCurve->K_num+1;
//                for(int m = 0; m < num; m++){
//                    int x = pNurbsCurve->pCtlarray[3*m];
//                    int y = pNurbsCurve->pCtlarray[3*m+1];
//                    int z = pNurbsCurve->pCtlarray[3*m+2];
//                    int a = 0;
//                }
//            }
        }
        for(int i = 0; i < ptmp_parallel->outlineCount[0]; i++)
        {
            int typenum = ptmp_parallel->outlineType[0][i];
            if(typenum == 100)
            {
                arc_type *pArc = (arc_type *)ptmp_parallel->pOutlines[0][i];
                float X1 = pArc->X1;
                float X2 = pArc->X2;
                float X3 = pArc->X3;
                float Y1 = pArc->Y1;
                float Y2 = pArc->Y2;
                float Y3 = pArc->Y3;
                //首先求得圆弧的半径
                max_radius = sqrt((X2-X1)*(X2-X1)+(Y2-Y1)*(Y2-Y1));
                angle = PI - acos(((X2-X1)*(X3-X1)+(Y2-Y1)*(Y3-Y1))/(max_radius*max_radius));
                break;
            }
        }
        if(max_radius > min_radius)
        {
            thickness = max_radius-min_radius;
            workpiece_thickness = thickness;
            pbend->bendRadius = min_radius;
            pbend->pParallel->bendRadius = min_radius;
            pbend->bendAngle = angle;
            pbend->pParallel->bendAngle = angle;
        }
        else
        {
            thickness = min_radius-max_radius;
            workpiece_thickness = thickness;
            pbend->bendRadius = max_radius;
            pbend->pParallel->bendRadius = max_radius;
            pbend->bendAngle = -angle;
            pbend->pParallel->bendAngle = -angle;
        }
        pbend = pbend->pNext;
    }
}

/********************************************
 *function:获取展开后基面应当向外平移的数值
 *adding:
 *author: xu
 *date: 2016/06/17
 *******************************************/
point3f fileReader::getDeploy_off(bend_surface *pbend,basic_surface *pBasic1,basic_surface *pBasic2)
{
    float angle = PI-fabs(pbend->bendAngle);
    //展开长度,根据pro/e归纳的经验公式
    float length = (pbend->bendRadius+thickness*2*Y_factor/PI)*angle;
    line_type *pBoard_line = NULL;
    QVector<line_type *> board;
    bool flag = false;
    trimmed_surface_type *pSurface1 = pBasic1->pSurface;
    trimmed_surface_type *pSurface2 = pBasic2->pSurface;
    for(int i = 0; i < pSurface1->curve_num+1; i++)
    {
        if(flag == true)
            break;
        for(int j = 0; j < pSurface1->outlineCount[i]; j++)
        {
            if(flag == true)
                break;
            board.clear();
            int type = pSurface1->outlineType[i][j];
            if(110 == type)
            {
                line_type *pline1 = (line_type *)pSurface1->pOutlines[i][j];
                for(int m = 0; m < pSurface2->curve_num+1; m++)
                {
                    if(flag == true)
                        break;
                    for(int n = 0; n < pSurface2->outlineCount[m]; n++)
                    {
                        type = pSurface2->outlineType[m][n];
                        if(110 == type)
                        {
                            line_type *pline2 = (line_type *)pSurface2->pOutlines[m][n];
                            if(judgeLineEqual(pline1,pline2))
                            {
                                pBoard_line = pline2;
                                flag = true;
                            }
                            else
                            {
                                board.push_back(pline2);
                            }
                        }
                    }
                }
            }
        }
    }
    if(pBoard_line == NULL)
        qDebug()<<"not find board_line!";
    //此处以后还需要进行修改,求取边界线的垂线的方向向量
    point3f normal,normal2,off,point1,point2;
    point1 = pBoard_line->start;
    point2 = pBoard_line->end;
    normal.x = point2.x-point1.x;
    normal.y = point2.y-point1.y;
    normal.z = point2.z-point1.z;
    for(int i = 0; i < board.size(); i++)
    {
        if(fabs(board[i]->start.x-point1.x) < EPSINON && \
           fabs(board[i]->start.y-point1.y) < EPSINON && \
           fabs(board[i]->start.z-point1.z) < EPSINON)
        {
            normal2.x = board[i]->end.x-point1.x;
            normal2.y = board[i]->end.y-point1.y;
            normal2.z = board[i]->end.z-point1.z;
        }
        else if(fabs(board[i]->end.x-point1.x) < EPSINON && \
                fabs(board[i]->end.y-point1.y) < EPSINON && \
                fabs(board[i]->end.z-point1.z) < EPSINON)

        {
            normal2.x = board[i]->start.x-point1.x;
            normal2.y = board[i]->start.y-point1.y;
            normal2.z = board[i]->start.z-point1.z;
        }
        else
        {
            continue;
        }
        if(fabs(normal2.x)<EPSINON && fabs(normal2.y)<EPSINON && fabs(normal2.z)<EPSINON)
            continue;
        if(fabs(normal.x*normal2.x+normal.y*normal2.y+normal.z*normal2.z)<EPSINON)
        {
            float n = sqrt(normal2.x*normal2.x+normal2.y*normal2.y+normal2.z*normal2.z);
            normal2.x /= n;
            normal2.y /= n;
            normal2.z /= n;
            off.x = length*normal2.x;
            off.y = length*normal2.y;
            off.z = length*normal2.z;
            return off;
        }
//        normal3.x = board[i].x-point2.x;
//        normal3.y = board[i].y-point2.y;
//        normal3.z = board[i].z-point2.z;
//        if(fabs(normal3.x)<EPSINON && fabs(normal3.y)<EPSINON && fabs(normal3.z)<EPSINON)
//            continue;
//        if(fabs(normal.x*normal3.x+normal.y*normal3.y+normal.z*normal3.z)<EPSINON)
//        {
//            float n = sqrt(normal3.x*normal3.x+normal3.y*normal3.y+normal3.z*normal3.z);
//            normal3.x /= n;
//            normal3.y /= n;
//            normal3.z /= n;
//            off.x = length*normal3.x;
//            off.y = length*normal3.y;
//            off.z = length*normal3.z;
//            return off;
//        }
    }
    qDebug()<<QObject::tr("don't find normal deploy off!");
    return off;
}

/********************************************
 *function:生成平移矩阵
 *adding:
 *author: xu
 *date: 2015/11/04
 *******************************************/
xform_type *fileReader::formTranslationMatrix(point3f off)
{
    xform_type *pForm = new xform_type;
    pForm->R11 = 1.0;
    pForm->R12 = 0.0;
    pForm->R13 = 0.0;
    pForm->R21 = 0.0;
    pForm->R22 = 1.0;
    pForm->R23 = 0.0;
    pForm->R31 = 0.0;
    pForm->R32 = 0.0;
    pForm->R33 = 1.0;
    pForm->T1 = off.x;
    pForm->T2 = off.y;
    pForm->T3 = off.z;
    return pForm;
}

/********************************************
 *function:计算两个矩阵相乘,数值正常顺序带入
 *adding:  利用左乘原理
 *         R11  R12  R13  T1
 *         R21  R22  R23  T2
 *         R31  R32  R33  T3
 *         0    0    0    1
 *author: xu
 *date: 2015/11/03
 *******************************************/
xform_type *fileReader::MatrixMultiply(xform_type *pForm1, xform_type *pForm2)
{
    xform_type *pForm = new xform_type;
    pForm->R11 = pForm2->R11*pForm1->R11+pForm2->R12*pForm1->R21+pForm2->R13*pForm1->R31;
    pForm->R12 = pForm2->R11*pForm1->R12+pForm2->R12*pForm1->R22+pForm2->R13*pForm1->R32;
    pForm->R13 = pForm2->R11*pForm1->R13+pForm2->R12*pForm1->R23+pForm2->R13*pForm1->R33;
    pForm->T1 = pForm2->R11*pForm1->T1+pForm2->R12*pForm1->T2+pForm2->R13*pForm1->T3+pForm2->T1;
    pForm->R21 = pForm2->R21*pForm1->R11+pForm2->R22*pForm1->R21+pForm2->R23*pForm1->R31;
    pForm->R22 = pForm2->R21*pForm1->R12+pForm2->R22*pForm1->R22+pForm2->R23*pForm1->R32;
    pForm->R23 = pForm2->R21*pForm1->R13+pForm2->R22*pForm1->R23+pForm2->R23*pForm1->R33;
    pForm->T2 = pForm2->R21*pForm1->T1+pForm2->R22*pForm1->T2+pForm2->R23*pForm1->T3+pForm2->T2;
    pForm->R31 = pForm2->R31*pForm1->R11+pForm2->R32*pForm1->R21+pForm2->R33*pForm1->R31;
    pForm->R32 = pForm2->R31*pForm1->R12+pForm2->R32*pForm1->R22+pForm2->R33*pForm1->R32;
    pForm->R33 = pForm2->R31*pForm1->R13+pForm2->R32*pForm1->R23+pForm2->R33*pForm1->R33;
    pForm->T3 = pForm2->R31*pForm1->T1+pForm2->R32*pForm1->T2+pForm2->R33*pForm1->T3+pForm2->T3;
    return pForm;
}

/********************************************
 *function:传入一条经过原点的直线以及一条
 *         过原点的目标直线
 *adding:
 *author: xu
 *date: 2015/11/01
 *******************************************/
xform_type *fileReader::computeMatrix(point3f source, point3f dest)
{
    point3f axis,off;
    float x1 = source.x;
    float y1 = source.y;
    float z1 = source.z;
    float x2 = dest.x;
    float y2 = dest.y;
    float z2 = dest.z;
    float a = sqrt(x1*x1+y1*y1+z1*z1);
    float b = sqrt(x2*x2+y2*y2+z2*z2);
    float angle = acos((x1*x2+y1*y2+z1*z2)/(a*b));
    //找到旋转轴和旋转角度的对应关系，需要进行判断，否则容易出现问题
    if(x2 == 1.0 && y2 == 0.0 && z2 == 0.0)
    {
        axis.x = 0.0;
        if(y1 > EPSINON)
        {
            axis.z = -sqrt((y1*y1)/(y1*y1+z1*z1));
            axis.y = -(axis.z*z1)/y1;
        }
        else if(fabs(y1) < EPSINON) //y1=0的情况比较复杂
        {
            if(fabs(z1) < EPSINON)
            {
                if(x1 < 0)
                    angle = 0;
                axis.z = 0.0;
                axis.y = 1.0;
            }
            else
            {
                axis.z = 0.0;
                axis.y = z1/fabs(z1);
            }
        }
        else
        {
            axis.z = sqrt((y1*y1)/(y1*y1+z1*z1));
            axis.y = -(axis.z*z1)/y1;
        }
    }
    else if(x2 == 0.0 && y2 == 0.0 && z2 == 1.0)
    {
        if(x1 > EPSINON)    //x1大于
        {
            axis.y = -sqrt((x1*x1)/(x1*x1+y1*y1));
            axis.x = -(axis.y*y1)/x1;
            if(angle > PI/2)
            {
                axis.y = -axis.y;
                axis.x = -axis.x;
                angle = PI-angle;
            }
        }
        else if(fabs(x1) < EPSINON)//x1=0的情况比较复杂
        {
            if(fabs(y1) < EPSINON)
            {
                if(z1 < 0)
                    angle = 0.0;
                axis.y = 0.0;
                axis.x = 1.0;
            }
            else
            {
                axis.y = 0.0;
                axis.x = y1/fabs(y1);
            }
            if(angle > PI/2)
            {
                axis.y = -axis.y;
                axis.x = -axis.x;
                angle = PI-angle;
            }
        }
        else
        {
            axis.y = sqrt((x1*x1)/(x1*x1+y1*y1));
            axis.x = -(axis.y*y1)/x1;
            if(angle > PI/2)
            {
                axis.y = -axis.y;
                axis.x = -axis.x;
                angle = PI-angle;
            }
        }
        axis.z = 0.0;
    }
    else if(x2 == 0.0 && y2 == 1.0 && z2 == 0.0){
        axis.y = 0.0;
        if(z1 > EPSINON)
        {
            axis.x = -sqrt((z1*z1)/(z1*z1+x1*x1));
            axis.z = -(axis.x*x1)/z1;
        }
        else if(fabs(z1) < EPSINON) //z1=0的情况比较复杂
        {
            if(fabs(x1) < EPSINON)
            {
                if(x1 < 0)
                    angle = 0;
                axis.x = 0.0;
                axis.z = 1.0;
            }
            else
            {
                axis.x = 0.0;
                axis.z = x1/fabs(x1);
            }
        }
        else
        {
            axis.x = sqrt((z1*z1)/(x1*x1+z1*z1));
            axis.z = -(axis.x*x1)/z1;
        }
    }
    off.x = 0.0;
    off.y = 0.0;
    off.z = 0.0;
    xform_type *pform = formRevolutinMatrix(axis,angle,off);
    return pform;
}

/********************************************
 *function:计算给定矩阵的逆矩阵
 *adding:  R11  R12  R13  T1
 *         R21  R22  R23  T2
 *         R31  R32  R33  T3
 *         0    0    0    1
 *author: xu
 *date: 2015/11/05
 *******************************************/
xform_type *fileReader::computeInverseMatrix(xform_type *pForm)
{
    //首先求得该矩阵行列式的值
    float R11 = pForm->R11;
    float R12 = pForm->R12;
    float R13 = pForm->R13;
    float R21 = pForm->R21;
    float R22 = pForm->R22;
    float R23 = pForm->R23;
    float R31 = pForm->R31;
    float R32 = pForm->R32;
    float R33 = pForm->R33;
    float T1 = pForm->T1;
    float T2 = pForm->T2;
    float T3 = pForm->T3;
    float det[3][3] = {{R11,R12,R13},{R21,R22,R23},{R31,R32,R33}};
    float determinant = computeDeterminant(det);
    if(determinant == 0)
        return NULL;
    xform_type *pInvers = new xform_type;
    float array1[3][3] = {{R22,R23,T2},{R32,R33,T3},{0,0,1}};
    float array2[3][3] = {{R12,R13,T1},{R32,R33,T3},{0,0,1}};
    float array3[3][3] = {{R12,R13,T1},{R22,R23,T2},{0,0,1}};
    float array4[3][3] = {{R12,R13,T1},{R22,R23,T2},{R32,R33,T3}};
    float array5[3][3] = {{R21,R23,T2},{R31,R33,T3},{0,0,1}};
    float array6[3][3] = {{R11,R13,T1},{R31,R33,T3},{0,0,1}};
    float array7[3][3] = {{R11,R13,T1},{R21,R23,T2},{0,0,1}};
    float array8[3][3] = {{R11,R13,T1},{R21,R23,T2},{R31,R33,T3}};
    float array9[3][3] = {{R21,R22,T2},{R31,R32,T3},{0,0,1}};
    float array10[3][3] = {{R11,R12,T1},{R31,R32,T3},{0,0,1}};
    float array11[3][3] = {{R11,R12,T1},{R21,R22,T2},{0,0,1}};
    float array12[3][3] = {{R11,R12,T1},{R21,R22,T2},{R31,R32,T3}};
    pInvers->R11 = computeDeterminant(array1)/determinant;
    pInvers->R12 = -computeDeterminant(array2)/determinant;
    pInvers->R13 = computeDeterminant(array3)/determinant;
    pInvers->T1 = -computeDeterminant(array4)/determinant;
    pInvers->R21 = -computeDeterminant(array5)/determinant;
    pInvers->R22 = computeDeterminant(array6)/determinant;
    pInvers->R23 = -computeDeterminant(array7)/determinant;
    pInvers->T2 = computeDeterminant(array8)/determinant;
    pInvers->R31 = computeDeterminant(array9)/determinant;
    pInvers->R32 = -computeDeterminant(array10)/determinant;
    pInvers->R33 = computeDeterminant(array11)/determinant;
    pInvers->T3 = -computeDeterminant(array12)/determinant;
    return pInvers;
}

/********************************************
 *function:生成调头变换矩阵和逆矩阵
 *adding:
 *author: xu
 *date: 2015/12/28
 *******************************************/
xform_type *fileReader::formTurnAboutMatrix(int bendnum, bend_surface *pHead)
{
    bend_surface *pbend = findBendSurface(bendnum,pHead);
    line_type *pbendline = pbend->pBendLine;
    //构造前后调头的矩阵
    //point3f center;
    float x_off = (pbendline->start.x+pbendline->end.x)/2.0;
    //center.y = (pBendLine->start.y+pBendLine->end.y)/2.0;
    //center.z = (pBendLine->start.z+pBendLine->end.z)/2.0;
    xform_type *pForm1 = new xform_type;
    pForm1->R11 = 1.0;
    pForm1->R12 = 0.0;
    pForm1->R13 = 0.0;
    pForm1->R21 = 0.0;
    pForm1->R22 = 1.0;
    pForm1->R23 = 0.0;
    pForm1->R31 = 0.0;
    pForm1->R32 = 0.0;
    pForm1->R33 = 1.0;
    pForm1->T1 = -x_off;
    pForm1->T2 = 0.0;
    pForm1->T3 = 0.0;
    xform_type *pForm2 = new xform_type;
    pForm2->R11 = -1.0;
    pForm2->R12 = 0.0;
    pForm2->R13 = 0.0;
    pForm2->R21 = 0.0;
    pForm2->R22 = 1.0;
    pForm2->R23 = 0.0;
    pForm2->R31 = 0.0;
    pForm2->R32 = 0.0;
    pForm2->R33 = -1.0;
    pForm2->T1 = x_off;
    pForm2->T2 = 0.0;
    pForm2->T3 = 0.0;
    xform_type *result = MatrixMultiply(pForm1,pForm2);
    delete pForm1;
    delete pForm2;
    return result;
}

/********************************************
 *function:计算3X3行列式的值
 *adding:
 *author: xu
 *date: 2015/11/05
 *******************************************/
float fileReader::computeDeterminant(float a[3][3])
{
    float line1 = a[0][0]*(a[1][1]*a[2][2]-a[1][2]*a[2][1]);
    float line2 = -a[0][1]*(a[1][0]*a[2][2]-a[1][2]*a[2][0]);
    float line3 = a[0][2]*(a[1][0]*a[2][1]-a[1][1]*a[2][0]);
    return (line1+line2+line3);
}

/********************************************
 *function:基面的访问有可能是重复的，访问之前
 *         需要对访问标志位清零
 *adding:
 *author: xu
 *date: 2015/10/29
 *******************************************/
void fileReader::resetToolFlag(trimmed_surface_type *pTrimmed)
{
    trimmed_surface_type *pTmpCur = pTrimmed;
    while (pTmpCur) {
        int typeNum = pTmpCur->surfaceType;
        if(typeNum == 128)  //暂时先不考虑曲面
        {
            for(int i = 0; i < pTmpCur->curve_num+1; i++)
            {
                for(int j = 0; j < pTmpCur->outlineCount[i]; j++)
                {
                    int type = pTmpCur->outlineType[i][j];
                    if(type == 110) //为直线
                    {
                        line_type *pline = (line_type *)pTmpCur->pOutlines[i][j];
                        pline->isVisited = 0;
                    }
                    else if(126 == type)//nurbs曲线
                    {
                        nurbsCurve_type *pcurve = (nurbsCurve_type *)pTmpCur->pOutlines[i][j];
                        pcurve->isVisited = 0;
                    }
                }
            }
        }
        pTmpCur = pTmpCur->pNext;
    }
}

/********************************************
 *function:基面的访问有可能是重复的，访问之前
 *         需要对访问标志位清零
 *adding:
 *author: xu
 *date: 2015/10/29
 *******************************************/
void fileReader::resetVisitFlag(bend_surface *pHead)
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

/********************************************
 *function:将原始数据转换到展开数据,本函数仅用于
 * SolidWorks生成的IGES文件
 *adding:
 *author: xu
 *date: 2015/10/29
 *******************************************/
void fileReader::convertToDeploy(bend_surface *pHead)
{
    bend_surface *pBendTmp = pHead;
    while (pBendTmp) {
        int result = modifyGeneratrix(pBendTmp); //修改当前折弯的母线,并返回与该母线重合的基面
        line_type axis = pBendTmp->axis;
        point3f off,axis_point;
        off.x = -axis.start.x;
        off.y = -axis.start.y;
        off.z = -axis.start.z;
        axis_point.x = axis.end.x-axis.start.x;
        axis_point.y = axis.end.y-axis.start.y;
        axis_point.z = axis.end.z-axis.start.z;
        xform_type *translation = formTranslationMatrix(off);//平移矩阵
        float angle = -PI+fabs(pBendTmp->bendAngle);
        xform_type *rote = formRevolutinMatrix(axis_point,angle,axis.start);
        xform_type *pMatrix = MatrixMultiply(translation,rote);
        if(result == -1)    //需要旋转左侧基面
        {
            resetVisitFlag(pHead);
            convertWork_Part(pMatrix,pHead,pBendTmp,pBendTmp->pLeftBase,false);
            convertWork_Part(pMatrix,pHead,pBendTmp->pParallel,pBendTmp->pParallel->pLeftBase,true);
            resetVisitFlag(pHead);
            point3f off_tmp = getDeploy_off(pBendTmp,pBendTmp->pRightBase,pBendTmp->pLeftBase);
            xform_type *tmp = formTranslationMatrix(off_tmp);
            convertWork_Part(tmp,pHead,pBendTmp,pBendTmp->pLeftBase,false);
            convertWork_Part(tmp,pHead,pBendTmp->pParallel,pBendTmp->pParallel->pLeftBase,true);
            resetVisitFlag(pHead);
            delete tmp;
        }
        else if(result == 1)//需要旋转右侧基面
        {
            resetVisitFlag(pHead);
            convertWork_Part(pMatrix,pHead,pBendTmp,pBendTmp->pRightBase,false);
            convertWork_Part(pMatrix,pHead,pBendTmp->pParallel,pBendTmp->pParallel->pRightBase,true);
            resetVisitFlag(pHead);
            point3f off_tmp = getDeploy_off(pBendTmp,pBendTmp->pLeftBase,pBendTmp->pRightBase);
            xform_type *tmp = formTranslationMatrix(off_tmp);
            convertWork_Part(tmp,pHead,pBendTmp,pBendTmp->pRightBase,false);
            convertWork_Part(tmp,pHead,pBendTmp->pParallel,pBendTmp->pParallel->pRightBase,true);
            resetVisitFlag(pHead);
            delete tmp;
        }
        //析构申请的内存资源
        delete translation;
        delete rote;
        delete pMatrix;
        pBendTmp = pBendTmp->pNext;
    }
}

/********************************************
 *function:对各个基面的轮廓数据进行处理,SolidWorks
 * 生成的文件和pro/e生成的文件处理方式不同
 *adding:
 *author: xu
 *date: 2016/06/17
 *******************************************/
void fileReader::setTrimmedData(trimmed_surface_type *pSurface,trimmed_surface_type *pSurface_pa)
{
    if(fileType != 0)   //SolidWorks生成的文件
    {
        //根据nurbs曲面控制点的数值求得x,z轴的最小最大值
        float x_start,z_start,x_end,z_end;
        float x1,z1,x2,z2,x3,z3;
        float *data = pSurface->pSurface->pCtlarray;
        int num1 = pSurface->pSurface->K1_num+1;
        int num2 = pSurface->pSurface->K2_num+1;
        int num = num1*num2;
        x_start = x1 = data[0];
        z_start = z1 = data[1];
        x2 = data[(num1-1)*3];
        z2 = data[(num1-1)*3+1];
        x3 = data[num1*(num2-1)*3];
        z3 = data[num1*(num2-1)*3+1];
        float len_x = sqrt((x2-x1)*(x2-x1)+(z2-z1)*(z2-z1));
        float len_z = sqrt((x3-x1)*(x3-x1)+(z3-z1)*(z3-z1));
        x_end = data[(num-1)*3];
        z_end = data[(num-1)*3+1];
        if(x_end-x_start < EPSINON)
            len_x = -len_x;
        if(z_end-z_start < EPSINON)
            len_z = -len_z;
        float angle = atan((z1-z2)/(x1-x2));

        for(int i = 0; i < pSurface->curve_num+1; i++)
        {
            for(int j = 0; j < pSurface->curveCount[i]; j++)
            {
                int type = pSurface->curveType[i][j];
                if(type == 110)
                {
                }
                else if(type == 126)
                {
                    nurbsCurve_type *pCurve = (nurbsCurve_type *)pSurface->pCurves[i][j];
                    int count = pCurve->K_num+1;
                    for(int m = 0; m < count; m++)
                    {
                        float a = pCurve->pCtlarray[3*m];
                        float b = pCurve->pCtlarray[3*m+1];
                        float z = pCurve->pCtlarray[3*m+2];
                        float x = b*len_x*cos(angle)-a*len_z*sin(angle);
                        float y = b*len_x*sin(angle)+a*len_z*cos(angle);
                        pCurve->pCtlarray[3*m] = x_start+x;
                        pCurve->pCtlarray[3*m+1] = z;
                        pCurve->pCtlarray[3*m+2] = z_start+y;
                        x = pCurve->pCtlarray[3*m];
                        y = pCurve->pCtlarray[3*m+2];
                        int c = 0;
                    }
                }
            }
        }
//        float x_start,z_start,x_end,z_end;
//        float *data = pSurface->pSurface->pCtlarray;
//        int num1 = pSurface->pSurface->K1_num+1;
//        int num2 = pSurface->pSurface->K2_num+1;
//        int num = num1*num2;
//        x_start = data[0];
//        z_start = data[1];
//        x_end = data[(num*3)-3];
//        z_end = data[(num*3)-2];
//        for(int i = 0; i < pSurface->curve_num+1; i++)
//        {
//            for(int j = 0; j < pSurface->curveCount[i]; j++)
//            {
//                int type = pSurface->curveType[i][j];
//                if(type == 110)
//                {
//                }
//                else if(type == 126)
//                {
//                    nurbsCurve_type *pCurve = (nurbsCurve_type *)pSurface->pCurves[i][j];
//                    int num = pCurve->K_num+1;
//                    for(int m = 0; m < num; m++)
//                    {
//                        float x = pCurve->pCtlarray[3*m];
//                        float y = pCurve->pCtlarray[3*m+1];
//                        float z = pCurve->pCtlarray[3*m+2];
//                        pCurve->pCtlarray[3*m] = x_start+y*(x_end-x_start);
//                        pCurve->pCtlarray[3*m+1] = z;
//                        pCurve->pCtlarray[3*m+2] = z_start+x*(z_end-z_start);
//                    }
//                }
//            }
//        }
    }
    else        //pro/e生成的文件
    {
        for(int i = 0; i < pSurface->curve_num+1; i++)
        {
            for(int j = 0; j < pSurface->curveCount[i]; j++)
            {
                int type = pSurface->curveType[i][j];
                if(type == 110) //此处有两种情况，需要分开考虑，110和126
                {
                    line_type *pline = (line_type *)pSurface->pCurves[i][j];
                    float y_pos = pline->start.y;
                    pline->start.y = pline->start.z;
                    pline->start.z = -y_pos;
                    y_pos = pline->end.y;
                    pline->end.y = pline->end.z;
                    pline->end.z = -y_pos;
                }
                if(type == 126) //B样条曲线
                {
                    nurbsCurve_type *pNurbsCurve = (nurbsCurve_type *)pSurface->pCurves[i][j];
                    int num = pNurbsCurve->K_num+1;
                    for(int m = 0; m < num; m++)
                    {
                        float tmp = pNurbsCurve->pCtlarray[m*3+1];
                        pNurbsCurve->pCtlarray[m*3+1] = pNurbsCurve->pCtlarray[m*3+2];
                        pNurbsCurve->pCtlarray[m*3+2] = -tmp;
                    }
                }
            }
        }
    }
    //将上下面的数据保持一致
    for(int i = 0; i < pSurface_pa->curve_num+1; i++)
    {
        for(int j = 0; j < pSurface_pa->curveCount[i]; j++)
        {
            pSurface_pa->curveType[i][j] = pSurface->curveType[i][j];
            int type = pSurface_pa->curveType[i][j];
            if(110 == type)
            {
                line_type *pline = (line_type *)pSurface->pCurves[i][j];
                //此处申请的内存,最后需要析构
                line_type *ptmp = new line_type;
                memcpy(ptmp,pline,sizeof(line_type));
                ptmp->start.y += thickness;
                ptmp->end.y += thickness;
                pSurface_pa->pCurves[i][j] = ptmp;
            }
            if(126 == type)
            {
                nurbsCurve_type *pcurve = (nurbsCurve_type *)pSurface->pCurves[i][j];
                nurbsCurve_type *ptmp = new nurbsCurve_type;
                memcpy(ptmp,pcurve,sizeof(nurbsCurve_type));
                int N_num = 1+pcurve->K_num-pcurve->M_num;
                int A_num = N_num+2*pcurve->M_num+1;
                //为结点序列数组赋值
                float *pKnot = new float[A_num];
                for(int m = 0; m < A_num; m++)
                {
                    pKnot[m] = pcurve->pKnot[m];
                }
                ptmp->pKnot = pKnot;
                //为权重值数组赋值
                float *pWeight = new float[pcurve->K_num+1];
                for(int m = 0; m <= pcurve->K_num; m++)
                {
                    pWeight[m] = pcurve->pWeight[m];
                }
                ptmp->pWeight = pWeight;
                //为控制点数组赋值
                float *pCtlData = new float[pcurve->K_num*3+3];
                for(int m = 0; m < pcurve->K_num+1; m++)
                {
                    float x = pcurve->pCtlarray[3*m];
                    float y = pcurve->pCtlarray[3*m+2];
                    pCtlData[3*m] = pcurve->pCtlarray[3*m];
                    pCtlData[3*m+1] = pcurve->pCtlarray[3*m+1]+thickness;
                    pCtlData[3*m+2] = pcurve->pCtlarray[3*m+2];
                }
                ptmp->pCtlarray = pCtlData;
                pSurface_pa->pCurves[i][j] = ptmp;
            }
        }
    }
}

/********************************************
 *function:生成当前折弯面的展开平面轮廓
 *adding: 生成展开轮廓的同时求得折弯线
 *author: xu
 *date: 2015/10/28
 *******************************************/
stu_polygon *fileReader::formDeploySurface(bend_surface *pbend)
{
    stu_polygon *pDeploy = NULL;
    trimmed_surface_type *pLeft = pbend->pLeftBase->pSurface;
    trimmed_surface_type *pRight = pbend->pRightBase->pSurface;
    float angle = PI - fabs(pbend->bendAngle);
    float length = (pbend->bendRadius+thickness*2*Y_factor/PI)*angle;//展开长度,根据pro/e归纳的经验公式
    for(int i = 0; i < pLeft->curve_num+1; i++)
    {
        for(int j = 0; j < pLeft->outlineCount[i]; j++)
        {
            int type1 = pLeft->outlineType[i][j];
            if(type1 == 110)
            {
                line_type *pline1 = (line_type *)pLeft->pOutlines[i][j];
                for(int m = 0; m < pRight->curve_num+1; m++)
                {
                    for(int n = 0; n < pRight->outlineCount[m]; n++)
                    {
                        int type2 = pRight->outlineType[m][n];
                        if(type2 == 110)
                        {
                            line_type *pline2 = (line_type *)pRight->pOutlines[m][n];
                            float x_off = (pline1->start.x+pline1->end.x)/2-(pline2->start.x+pline2->end.x)/2;
                            float y_off = (pline1->start.y+pline1->end.y)/2-(pline2->start.y+pline2->end.y)/2;
                            float z_off = (pline1->start.z+pline1->end.z)/2-(pline2->start.z+pline2->end.z)/2;
                            float distance = sqrt(x_off*x_off+y_off*y_off+z_off*z_off);
                            if(fabs(distance-length) < EPSINON) //认为这两条直线就是要找的折弯展开面的轮廓线
                            {
                                pline1->isBendBound = true;
                                pline2->isBendBound = true;
                                pDeploy = new stu_polygon;
                                if(pbend->pParallel->pDeploy)
                                {
                                    //上下两个面需要保持一致
                                    float x0 = pbend->pParallel->pDeploy->point[0].x;
                                    float y0 = pbend->pParallel->pDeploy->point[0].y;
                                    float z0 = pbend->pParallel->pDeploy->point[0].z;
                                    float x1 = pline1->start.x;
                                    float y1 = pline1->start.y;
                                    float z1 = pline1->start.z;
                                    float len = sqrt((x1-x0)*(x1-x0)+(y1-y0)*(y1-y0)+(z1-z0)*(z1-z0));
                                    if(fabs(len-thickness) < EPSINON){
                                        pDeploy->point.push_back(pline1->start);
                                        pDeploy->point.push_back(pline1->end);
                                        pDeploy->point.push_back(pline2->start);
                                        pDeploy->point.push_back(pline2->end);
                                    }
                                    else{
                                        pDeploy->point.push_back(pline2->start);
                                        pDeploy->point.push_back(pline2->end);
                                        pDeploy->point.push_back(pline1->start);
                                        pDeploy->point.push_back(pline1->end);
                                    }
                                }
                                else
                                {
                                    pDeploy->point.push_back(pline1->start);
                                    pDeploy->point.push_back(pline1->end);
                                    pDeploy->point.push_back(pline2->start);
                                    pDeploy->point.push_back(pline2->end);
                                }
                                line_type *pBendLine = new line_type;
                                pBendLine->start.x = (pline1->start.x+pline2->end.x)/2;
                                pBendLine->start.y = (pline1->start.y+pline2->end.y)/2;
                                pBendLine->start.z = (pline1->start.z+pline2->end.z)/2;
                                pBendLine->end.x = (pline2->start.x+pline1->end.x)/2;
                                pBendLine->end.y = (pline2->start.y+pline1->end.y)/2;
                                pBendLine->end.z = (pline2->start.z+pline1->end.z)/2;
                                float x = fabs(pBendLine->start.x-pBendLine->end.x);
                                float y = fabs(pBendLine->start.y-pBendLine->end.y);
                                float z = fabs(pBendLine->start.z-pBendLine->end.z);
                                float width = sqrt(x*x+y*y+z*z);
                                pbend->bendWidth = width;
                                pbend->pBendLine = pBendLine;
                                pbend->deployLength = distance;
                                return pDeploy;
                            }
                        }
                    }
                }
            }
            if(type1 == 126)    //这种情况一般是Solidworks导出的文件
            {
                int a = 0;
            }
        }
    }
    if(pDeploy == NULL){
        qDebug()<<"not find deploy surface!";
    }
    return pDeploy;
}

/********************************************
 *function:生成各个基面相连的侧面
 *adding:
 *author: xu
 *date: 2015/11/07
 *******************************************/
void fileReader::formSideSurface(bend_surface *pBend)
{
    //生成折弯面展开时的侧面
    stu_polygon *pFont = new stu_polygon;
    pFont->point.push_back(pBend->pDeploy->point[0]);
    pFont->point.push_back(pBend->pDeploy->point[3]);
    pFont->point.push_back(pBend->pParallel->pDeploy->point[3]);
    pFont->point.push_back(pBend->pParallel->pDeploy->point[0]);
    //生成折弯面展开另一测的侧面
    stu_polygon *pBack = new stu_polygon;
    pBack->point.push_back(pBend->pDeploy->point[1]);
    pBack->point.push_back(pBend->pDeploy->point[2]);
    pBack->point.push_back(pBend->pParallel->pDeploy->point[2]);
    pBack->point.push_back(pBend->pParallel->pDeploy->point[1]);

    pBend->pSideBack = pBack;
    pBend->pSideFont = pFont;
}

/********************************************
 *function:根据折弯点的位置转换折弯两侧的板料数据
 *adding:  属于局部的坐标变换
 *author: xu
 *date: 2015/10/31
 *******************************************/
void fileReader::convertWork_Part(xform_type *pForm,bend_surface *pHead, bend_surface *pBend, basic_surface *pBasic, bool isTop)
{
    if(pForm == NULL)
        return;
    int size = pBasic->pBendSurface.size();
    if(pBasic->isVisited == 0)
    {
        //说明只需要转换该基面的数据
        convertOutLineSurface(pForm,pBasic->pSurface);
        pBasic->isVisited = 1;
    }
    for(int i = 0; i < pBasic->pSideSurface.size(); i++){
        convertOutLineSurface(pForm,pBasic->pSideSurface[i]);
    }
    if(size == 1)//如果无其他相连的折弯面，则直接返回
        return;
    for(int i = 0; i < size; i++)
    {
        trimmed_surface_type *ptrimmed = pBasic->pBendSurface[i];
        //查找相连的折弯面
        bend_surface *pbendtmp = pHead;
        while (pbendtmp) {
            if(isTop)
                pbendtmp = pbendtmp->pParallel;
            if(pbendtmp->pSurface == ptrimmed)
                break;
            if(pbendtmp->pParallel->pSurface == ptrimmed)
                break;
            if(isTop)
                pbendtmp = pbendtmp->pParallel;
            pbendtmp = pbendtmp->pNext;
        }
        if(pbendtmp == pBend)
            continue;
        //对折弯面,旋转轴和母线进行变换
        xform_matrix(pForm,pbendtmp->axis.start);
        xform_matrix(pForm,pbendtmp->axis.end);
        xform_matrix(pForm,pbendtmp->generatrix.start);
        xform_matrix(pForm,pbendtmp->generatrix.end);
        convertOutLineSurface(pForm,pbendtmp->pSurface);
        //利用递归算法，实现一侧折弯数据的转换
        if(pbendtmp->pLeftBase->pSurface == pBasic->pSurface)
            convertWork_Part(pForm,pHead,pbendtmp,pbendtmp->pRightBase,isTop);
        else
            convertWork_Part(pForm,pHead,pbendtmp,pbendtmp->pLeftBase,isTop);
    }
}

/********************************************
 *function:根据折弯点的位置转换折弯两侧的板料数据
 *adding:  属于局部的坐标变换
 *author: xu
 *date: 2015/10/31
 *******************************************/
void fileReader::MatrixPartConvert(xform_type *pForm, bend_surface *pHead, bend_surface *pBend, basic_surface *pBasic, bool isTop)
{
    if(pForm == NULL)
        return;
    int size = pBasic->pBendSurface.size();
    if(pBasic->isVisited == 0)
    {
        //说明只需要转换该基面的数据
        trimmed_surface_type *pSurface = pBasic->pSurface;
        convertOutLineSurface(pForm,pSurface);
        for(int i = 0; i < pBasic->pSideSurface.size(); i++)
        {
            trimmed_surface_type *pside = pBasic->pSideSurface[i];
            convertOutLineSurface(pForm,pside);
        }
        pBasic->isVisited = 1;
    }
    if(size == 1)//如果无其他相连的折弯面，则直接返回
        return;
    for(int i = 0; i < size; i++)
    {
        trimmed_surface_type *ptrimmed = pBasic->pBendSurface[i];
        //查找相连的折弯面
        bend_surface *pbendtmp = pHead;
        while (pbendtmp) {
            if(isTop)
                pbendtmp = pbendtmp->pParallel;
            if(pbendtmp->pSurface == ptrimmed)
                break;
            if(isTop)
                pbendtmp = pbendtmp->pParallel;
            pbendtmp = pbendtmp->pNext;
        }
        if(pbendtmp == pBend)
            continue;
//        if(pbendtmp->isBended == 0) //还未折弯，则需要转换的是折弯面的展开平面
//        {
            for(int i = 0; i < pbendtmp->pDeploy->point.size(); i++)
                xform_matrix(pForm,pbendtmp->pDeploy->point[i]);
            xform_matrix(pForm,pbendtmp->pBendLine->start);
            xform_matrix(pForm,pbendtmp->pBendLine->end);
            stu_polygon *pBack = pbendtmp->pSideBack;
            stu_polygon *pFont = pbendtmp->pSideFont;
            if(pBack && pFont)
            {
                for(int i = 0; i < pBack->point.size(); i++)
                    xform_matrix(pForm,pBack->point[i]);
                for(int i = 0; i < pFont->point.size(); i++)
                    xform_matrix(pForm,pFont->point[i]);
            }
//        }
        if(pbendtmp->isBended == 1)      //已经折弯，则需要转换的是折弯面的旋转曲面
        {
            nurbsSurface_type *pNurbs = pbendtmp->pBendSurface;
            int num = pNurbs->K1_num+1;
            for(int i = 0; i < num*3; i++)//对折弯面的控制点进行坐标变换
                xform_matrix(pForm,pNurbs->pCtlarray[3*i],pNurbs->pCtlarray[3*i+1],pNurbs->pCtlarray[3*i+2]);
            //对折弯面的两个侧面的控制点进行坐标变换
            nurbsSurface_type *pSideBack = pbendtmp->pBendSideBack;
            nurbsSurface_type *pSideFont = pbendtmp->pBendSideFont;
            if(pSideFont && pSideBack)
            {
                num = pSideBack->K1_num+1;
                for(int i = 0; i < num*3; i++)
                    xform_matrix(pForm,pSideBack->pCtlarray[3*i],pSideBack->pCtlarray[3*i+1],pSideBack->pCtlarray[3*i+2]);
                num = pSideFont->K1_num+1;
                for(int i = 0; i < num*3; i++)
                    xform_matrix(pForm,pSideFont->pCtlarray[3*i],pSideFont->pCtlarray[3*i+1],pSideFont->pCtlarray[3*i+2]);
            }
        }
        //利用递归算法，实现一侧折弯数据的转换
        if(pbendtmp->pLeftBase->pSurface == pBasic->pSurface)
            MatrixPartConvert(pForm,pHead,pbendtmp,pbendtmp->pRightBase,isTop);
        else
            MatrixPartConvert(pForm,pHead,pbendtmp,pbendtmp->pLeftBase,isTop);
    }
}

/********************************************
 *function:求出变换矩阵后进行矩阵坐标变换
 *adding:  属于整体的坐标变换
 *author: xu
 *date: 2015/11/01
 *******************************************/
void fileReader::MatrixTotalConvert(bend_surface *pHead, xform_type *pForm, bool isTop)
{
    resetVisitFlag(pHead);   //重置访问标志位
    bend_surface *pbend = pHead;
    while (pbend) {
        if(isTop)       //上面的平面
            pbend = pbend->pParallel;
        line_type *plinetmp = pbend->pBendLine;
        stu_polygon *pSurface = pbend->pDeploy;      //折弯面的展开平面
        nurbsSurface_type *pNurbs = pbend->pBendSurface;//折弯面的旋转曲面
        nurbsSurface_type *pSideBack = pbend->pBendSideBack;
        nurbsSurface_type *pSideFont = pbend->pBendSideFont;
        trimmed_surface_type *pLeft = pbend->pLeftBase->pSurface;
        trimmed_surface_type *pRight = pbend->pRightBase->pSurface;
        stu_polygon *pBack = pbend->pSideBack;
        stu_polygon *pFont = pbend->pSideFont;
        //首先平移钣金工件，使其指定折弯线经过原点
        //旋转变换折弯线
        if(plinetmp)
        {
            xform_matrix(pForm,plinetmp->start);
            xform_matrix(pForm,plinetmp->end);
        }
        //旋转变换展开平面
        if(pSurface)
        {
            for(int i = 0; i < pSurface->point.size(); i++)
                xform_matrix(pForm,pSurface->point[i]);
        }
        //旋转变换展开平面的侧面
        if(pBack && pFont)
        {
            for(int i = 0; i < pBack->point.size(); i++)
                xform_matrix(pForm,pBack->point[i]);
            for(int i = 0; i < pFont->point.size(); i++)
                xform_matrix(pForm,pFont->point[i]);
        }
        if(pbend->isBended == 1)//此折弯线处已经折弯
        {
            int num = pNurbs->K1_num+1;
            for(int i = 0; i < num*3; i++)//对折弯面的控制点进行坐标变换
                xform_matrix(pForm,pNurbs->pCtlarray[3*i],pNurbs->pCtlarray[3*i+1],pNurbs->pCtlarray[3*i+2]);
            if(pSideBack && pSideFont)
            {
                num = pSideBack->K1_num+1;
                for(int i = 0; i < num*3; i++)//对折弯面后侧面的控制点进行坐标变换
                    xform_matrix(pForm,pSideBack->pCtlarray[3*i],pSideBack->pCtlarray[3*i+1],pSideBack->pCtlarray[3*i+2]);
                num = pSideFont->K1_num+1;
                for(int i = 0; i < num*3; i++)//对折弯面前侧面的控制点进行坐标变换
                    xform_matrix(pForm,pSideFont->pCtlarray[3*i],pSideFont->pCtlarray[3*i+1],pSideFont->pCtlarray[3*i+2]);
            }
        }
        //旋转变换一个基面
        if(pbend->pLeftBase->isVisited == 0)
        {
            convertOutLineSurface(pForm,pLeft);
            for(int i = 0; i < pbend->pLeftBase->pSideSurface.size(); i++)
            {
                trimmed_surface_type *pside = pbend->pLeftBase->pSideSurface[i];
                convertOutLineSurface(pForm,pside);
            }
            pbend->pLeftBase->isVisited = 1;
        }
        //旋转变换另一个基面
        if(pbend->pRightBase->isVisited == 0)
        {
            convertOutLineSurface(pForm,pRight);
            for(int i = 0; i < pbend->pRightBase->pSideSurface.size(); i++)
            {
                trimmed_surface_type *pside = pbend->pRightBase->pSideSurface[i];
                convertOutLineSurface(pForm,pside);
            }
            pbend->pRightBase->isVisited = 1;
        }
        if(isTop)       //上面的平面
            pbend = pbend->pParallel;
        pbend = pbend->pNext;
    }
    resetVisitFlag(pHead);   //重置访问标志位
}

/********************************************
 *function:对裁剪曲面的实际轮廓线进行转换
 *adding:
 *author: xu
 *date: 2015/10/30
 *******************************************/
void fileReader::convertOutLineSurface(xform_type *pForm, trimmed_surface_type *pSurface)
{
    int num = (pSurface->pSurface->K1_num+1)*(pSurface->pSurface->K2_num+1);
    if(pSurface->pSurface->typeNum == 128 || pSurface->pSurface->typeNum == 120){
        for(int i = 0; i < num; i++){
            xform_matrix(pForm,pSurface->pSurface->pCtlarray[3*i],\
                pSurface->pSurface->pCtlarray[3*i+1],pSurface->pSurface->pCtlarray[3*i+2]);
        }
    }
    for(int i = 0; i < pSurface->curve_num+1; i++)
    {
        for(int j = 0; j < pSurface->outlineCount[i]; j++)
        {
            int type = pSurface->outlineType[i][j];
            if(110 == type)  //直线实体
            {
                line_type *pline = (line_type *)pSurface->pOutlines[i][j];
                xform_matrix(pForm,pline->start);
                xform_matrix(pForm,pline->end);
            }
            else if(126 == type)    //此处可能不会出现126实体
            {
                nurbsCurve_type *pNurbsCurve = (nurbsCurve_type *)pSurface->pOutlines[i][j];
                int num = pNurbsCurve->K_num+1;
                for(int m = 0; m < num; m++)
                {
                    xform_matrix(pForm,pNurbsCurve->pCtlarray[3*m],\
                    pNurbsCurve->pCtlarray[3*m+1],pNurbsCurve->pCtlarray[3*m+2]);
                }
            }
        }
    }
}

/********************************************
 *function:生成折弯后的数据
 *adding:
 *author: xu
 *date: 2015/10/30
 *******************************************/
void fileReader::formBendedData(int bendnum, bool isTop, bend_surface *pHead)
{
    bend_surface *pbend = findBendSurface(bendnum,pHead);
    if(isTop)
        pbend = pbend->pParallel;
    resetVisitFlag(pHead);   //重置折弯数据访问标志
    line_type *pbendline = pbend->pBendLine;
    float angle = fabs(pbend->bendAngle);//该折弯线处的折弯角度
    float deployLength = pbend->deployLength;//展开长度
    //此处获取左右两侧基面轮廓中Z的值，以确定在折弯线两侧的位置
    trimmed_surface_type *pleft = pbend->pLeftBase->pSurface;
    trimmed_surface_type *pright = pbend->pRightBase->pSurface;
    float left_z_pos = 0;
    float right_z_pos = 0;
    bool flag = false;
    for(int i = 0; i < pleft->curve_num+1; i++)
    {
        if(flag == true)
            break;
        for(int j = 0; j < pleft->outlineCount[i]; j++)
        {
            int type = pleft->outlineType[i][j];
            if(110 == type)
            {
                line_type *pline = (line_type *)pleft->pOutlines[i][j];
                if(pline->isBendBound)
                {
                    float x1 = (pbendline->start.x+pbendline->end.x)/2.0;
                    float y1 = (pbendline->start.y+pbendline->end.y)/2.0;
                    float z1 = (pbendline->start.z+pbendline->end.z)/2.0;
                    float x2 = (pline->start.x+pline->end.x)/2.0;
                    float y2 = (pline->start.y+pline->end.y)/2.0;
                    float z2 = (pline->start.z+pline->end.z)/2.0;
                    float len = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
                    if(fabs(len-deployLength/2.0) < EPSINON)
                    {
                        left_z_pos = pline->start.z;
                        flag = true;
                        break;
                    }
                }
            }
            else if(126 == type)
            {
//                nurbsCurve_type *pcurve = (nurbsCurve_type *)pleft->pOutlines[i][j];
//                int num = pcurve->K_num+1;
//                for(int m = 0; m < num; m++)
//                {
//                    float x = pcurve->pCtlarray[3*m];
//                    float y = pcurve->pCtlarray[3*m+1];
//                    float z = pcurve->pCtlarray[3*m+2];
//                    int a = 0;
//                }
//                if(pcurve->isBendBound)
//                {
//                    float x1 = (pbendline->start.x+pbendline->end.x)/2.0;
//                    float y1 = (pbendline->start.y+pbendline->end.y)/2.0;
//                    float z1 = (pbendline->start.z+pbendline->end.z)/2.0;
//                    float x2 = (pcurve->pCtlarray[0]+pcurve->pCtlarray[3])/2.0;
//                    float y2 = (pcurve->pCtlarray[1]+pcurve->pCtlarray[4])/2.0;
//                    float z2 = (pcurve->pCtlarray[2]+pcurve->pCtlarray[5])/2.0;
//                    float len = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
//                    if(fabs(len-deployLength/2.0) < EPSINON)
//                    {
//                        left_z_pos = pcurve->pCtlarray[2];
//                        flag = true;
//                        break;
//                    }
//                }
            }
            else
            {
                qDebug()<<QObject::tr("组成裁剪曲面的外轮廓中包含除了110,126以外的实体");
            }
        }
    }
    flag = false;
    for(int i = 0; i < pright->curve_num+1; i++)
    {
        if(flag == true)
            break;
        for(int j = 0; j < pright->outlineCount[i]; j++)
        {
            int type = pright->outlineType[i][j];
            if(110 == type)
            {
                line_type *pline = (line_type *)pright->pOutlines[i][j];
                if(pline->isBendBound)
                {
                    float x1 = (pbendline->start.x+pbendline->end.x)/2.0;
                    float y1 = (pbendline->start.y+pbendline->end.y)/2.0;
                    float z1 = (pbendline->start.z+pbendline->end.z)/2.0;
                    float x2 = (pline->start.x+pline->end.x)/2.0;
                    float y2 = (pline->start.y+pline->end.y)/2.0;
                    float z2 = (pline->start.z+pline->end.z)/2.0;
                    float len = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
                    if(fabs(len-deployLength/2.0) < EPSINON)
                    {
                        right_z_pos = pline->start.z;
                        flag = true;
                        break;
                    }
                }
            }
            else if(126 == type)
            {
//                nurbsCurve_type *pcurve = (nurbsCurve_type *)pright->pCurves[i][j];
//                int num = pcurve->K_num+1;
//                for(int m = 0; m < num; m++)
//                {
//                    float x = pcurve->pCtlarray[3*m];
//                    float y = pcurve->pCtlarray[3*m+1];
//                    float z = pcurve->pCtlarray[3*m+2];
//                    int a = 0;
//                }
//                if(pcurve->isBendBound)
//                {
//                    float x1 = (pbendline->start.x+pbendline->end.x)/2.0;
//                    float y1 = (pbendline->start.y+pbendline->end.y)/2.0;
//                    float z1 = (pbendline->start.z+pbendline->end.z)/2.0;
//                    float x2 = (pcurve->pCtlarray[0]+pcurve->pCtlarray[3])/2.0;
//                    float y2 = (pcurve->pCtlarray[1]+pcurve->pCtlarray[4])/2.0;
//                    float z2 = (pcurve->pCtlarray[2]+pcurve->pCtlarray[5])/2.0;
//                    float len = sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)+(z1-z2)*(z1-z2));
//                    if(fabs(len-deployLength/2.0) < EPSINON)
//                    {
//                        right_z_pos = pcurve->pCtlarray[2];
//                        flag = true;
//                        break;
//                    }
//                }
            }
            else
            {
                qDebug()<<QObject::tr("组成裁剪曲面的外轮廓中包含除了110,126以外的实体");
            }
        }
    }

    point3f off;
    //float radius = 1/**thickness*/;
    float length = 0.0;// = pbend->deployLength;
    float off_y = 0.0;
    float cur_min_y = deployLength/2.0*tan(angle/2.0)-(thickness+pbend->bendRadius)/sin(angle/2.0);
    float dest_min_y = -die_width/2.0/tan(angle/2.0);
    float relative_y = cur_min_y-dest_min_y;
    //if((isTop && pbend->bendAngle < 0) || (!isTop && pbend->bendAngle > 0))
    if((isTop && pbend->bendAngle > 0) || (!isTop && pbend->bendAngle < 0))
    {
        //求出小折弯面
        length = 2*pbend->bendRadius*sin((PI-angle)/2);
        off_y = thickness*sin(angle/2);
    }
    else
    {
        //求出大折弯面
        length = 2*(pbend->bendRadius+thickness)*sin((PI-angle)/2);
        off_y = thickness;
    }

    //需要先将板料沿Y轴下移到下部外形的上表面
    off.x = 0.0;
    off.y = -pbendline->start.y;
    off.z = -deployLength/2;
    xform_type *pTranslation1 = formTranslationMatrix(off);
    //生成转换折弯的旋转矩阵
    point3f axis;
    float rote_angle = (PI-angle)/2;
    axis.x = -1.0;
    axis.y = 0.0;
    axis.z = 0.0;
    off.x = 0.0;
    off.y = pbendline->start.y-thickness+off_y-relative_y;
    off.z = length/2;
    //生成左侧轮廓的旋转矩阵
    xform_type *pRote1 = formRevolutinMatrix(axis,rote_angle,off);
    xform_type *pForm1 = MatrixMultiply(pTranslation1,pRote1);
    //生成左侧折弯后的轮廓数据
    if(left_z_pos >= right_z_pos)
    {
        MatrixPartConvert(pForm1,pHead,pbend,pbend->pLeftBase,isTop);
        if(pbend->pLeftMatrix)
        {
            delete pbend->pLeftMatrix;
            pbend->pLeftMatrix = NULL;
        }
        if(pbend->pLeftInverse)
        {
            delete pbend->pLeftInverse;
            pbend->pLeftInverse = NULL;
        }
        pbend->pLeftMatrix = pForm1;
        pbend->pLeftInverse = computeInverseMatrix(pForm1);
    }
    else
    {
        MatrixPartConvert(pForm1,pHead,pbend,pbend->pRightBase,isTop);
        if(pbend->pRightMatrix)
        {
            delete pbend->pRightMatrix;
            pbend->pRightMatrix = NULL;
        }
        if(pbend->pRightInverse)
        {
            delete pbend->pRightInverse;
            pbend->pRightInverse = NULL;
        }
        pbend->pRightMatrix = pForm1;
        pbend->pRightInverse = computeInverseMatrix(pForm1);
    }
    off.x = 0.0;
    off.y = -pbendline->start.y;
    off.z = deployLength/2;
    xform_type *pTranslation2 = formTranslationMatrix(off);
    off.x = 0.0;
    off.y = pbendline->start.y-thickness+off_y-relative_y;
    off.z = -length/2;
    //生成右侧轮廓的旋转矩阵
    xform_type *pRote2 = formRevolutinMatrix(axis,-rote_angle,off);
    xform_type *pForm2 = MatrixMultiply(pTranslation2,pRote2);
    //生成右侧折弯后的轮廓数据
    if(left_z_pos >= right_z_pos)
    {
        MatrixPartConvert(pForm2,pHead,pbend,pbend->pRightBase,isTop);
        if(pbend->pRightMatrix)
        {
            delete pbend->pRightMatrix;
            pbend->pRightMatrix = NULL;
        }
        if(pbend->pRightInverse)
        {
            delete pbend->pRightInverse;
            pbend->pRightInverse = NULL;
        }
        pbend->pRightMatrix = pForm2;
        pbend->pRightInverse = computeInverseMatrix(pForm2);
    }
    else
    {
        MatrixPartConvert(pForm2,pHead,pbend,pbend->pLeftBase,isTop);
        if(pbend->pLeftMatrix)
        {
            delete pbend->pLeftMatrix;
            pbend->pLeftMatrix = NULL;
        }
        if(pbend->pLeftInverse)
        {
            delete pbend->pLeftInverse;
            pbend->pLeftInverse = NULL;
        }
        pbend->pLeftMatrix = pForm2;
        pbend->pLeftInverse = computeInverseMatrix(pForm2);
    }
    //使用矩阵后应当析构内存
    delete pTranslation1;
    delete pTranslation2;
    delete pRote1;
    delete pRote2;
    //**************生成折弯面的轮廓数据************************/
    line_type bend_generatrix,bend_axis;//母线
    //折弯展开平面的轮廓都是从左侧到右侧的顺序
    bend_generatrix.start.x = pbend->pDeploy->point[0].x;
    bend_generatrix.start.y = pbend->pDeploy->point[0].y-thickness+off_y-relative_y;
    bend_generatrix.start.z = length/2;
    bend_generatrix.end.x = pbend->pDeploy->point[1].x;
    bend_generatrix.end.y = pbend->pDeploy->point[1].y-thickness+off_y-relative_y;
    bend_generatrix.end.z = length/2;
    float start_angle = 0.0;
    float end_angle = 2*rote_angle;
    float interval = length/(2*tan(rote_angle));
    float Y1 = pbendline->start.y-thickness+off_y-relative_y+interval;
    bend_axis.start.x = 0.0;
    bend_axis.start.y = Y1;
    bend_axis.start.z = 0.0;
    bend_axis.end.x = 1.0;
    bend_axis.end.y = Y1;
    bend_axis.end.z = 0.0;
    pbend->pBendSurface = formRevolution(bend_generatrix,bend_axis,start_angle,end_angle);
    pbend->isBended = 1;    //折弯状态为1
    resetVisitFlag(pHead);   //重置折弯数据访问标志
}

/********************************************
 *function:生成折弯面的两个侧面
 *adding:
 *author: xu
 *date: 2015/11/08
 *******************************************/
void fileReader::formBendSideSurface(int bendnum,bend_surface *pHead)
{
    bend_surface *pbend = findBendSurface(bendnum,pHead);
    nurbsSurface_type *pSurface = pbend->pBendSurface;
    nurbsSurface_type *pSurface_pa = pbend->pParallel->pBendSurface;
    //首先应当析构之前申请的内存
    if(pbend->pBendSideBack)
    {
        delete pbend->pBendSideBack->pKnot1;
        delete pbend->pBendSideBack->pKnot2;
        delete pbend->pBendSideBack->pCtlarray;
        delete pbend->pBendSideBack;
        pbend->pBendSideBack = NULL;
    }
    if(pbend->pBendSideFont)
    {
        delete pbend->pBendSideFont->pKnot1;
        delete pbend->pBendSideFont->pKnot2;
        delete pbend->pBendSideFont->pCtlarray;
        delete pbend->pBendSideFont;
        pbend->pBendSideFont = NULL;
    }
    //构建其中的一个折弯侧面
    pbend->pBendSideFont = formSideNurbsSurface(pSurface,pSurface_pa,true);
    //构建另一个折弯侧面
    pbend->pBendSideBack = formSideNurbsSurface(pSurface,pSurface_pa,false);
}

/********************************************
 *function:根据上下两个折弯面生成折弯侧面
 *adding:
 *author: xu
 *date: 2015/11/08
 *******************************************/
nurbsSurface_type *fileReader::formSideNurbsSurface(nurbsSurface_type *pSurface, nurbsSurface_type *pSurface_pa,bool bFont)
{
    nurbsSurface_type *pSide = new nurbsSurface_type;
    memset(pSide,0,sizeof(nurbsSurface_type));
    pSide->typeNum = 120;
    pSide->K1_num = 40;
    pSide->K2_num = 2;
    pSide->M1_num = 2;
    pSide->M2_num = 2;
    int N1_num = 1+pSide->K1_num-pSide->M1_num;
    int N2_num = 1+pSide->K2_num-pSide->M2_num;
    int A_num = N1_num+2*pSide->M1_num+1;
    int B_num = N2_num+2*pSide->M2_num+1;
    int C_num = (1+pSide->K1_num)*(1+pSide->K2_num);
    float *knot12 = new float[A_num];
    float *knot22 = new float[B_num];
    for(int i = 0; i < A_num; i++)
        knot12[i] = knot1_prepare[i];
    for(int i = 0; i < B_num; i++)
        knot22[i] = knot2_prepare[i];
    pSide->pKnot1 = knot12;
    pSide->pKnot2 = knot22;
    int num = pSide->K1_num+1;
    float *ctldata = new float[C_num*3];
    for(int i = 0; i < num; i++)
    {
        if(bFont == false)
        {
            ctldata[9*i] = pSurface->pCtlarray[9*i+6];
            ctldata[9*i+1] = pSurface->pCtlarray[9*i+7];
            ctldata[9*i+2] = pSurface->pCtlarray[9*i+8];
            ctldata[9*i+3] = (pSurface->pCtlarray[9*i+6]+pSurface_pa->pCtlarray[9*i+6])/2;
            ctldata[9*i+4] = (pSurface->pCtlarray[9*i+7]+pSurface_pa->pCtlarray[9*i+7])/2;
            ctldata[9*i+5] = (pSurface->pCtlarray[9*i+8]+pSurface_pa->pCtlarray[9*i+8])/2;
            ctldata[9*i+6] = pSurface_pa->pCtlarray[9*i+6];
            ctldata[9*i+7] = pSurface_pa->pCtlarray[9*i+7];
            ctldata[9*i+8] = pSurface_pa->pCtlarray[9*i+8];
        }
        else
        {
            ctldata[9*i] = pSurface->pCtlarray[9*i];
            ctldata[9*i+1] = pSurface->pCtlarray[9*i+1];
            ctldata[9*i+2] = pSurface->pCtlarray[9*i+2];
            ctldata[9*i+3] = (pSurface->pCtlarray[9*i]+pSurface_pa->pCtlarray[9*i])/2;
            ctldata[9*i+4] = (pSurface->pCtlarray[9*i+1]+pSurface_pa->pCtlarray[9*i+1])/2;
            ctldata[9*i+5] = (pSurface->pCtlarray[9*i+2]+pSurface_pa->pCtlarray[9*i+2])/2;
            ctldata[9*i+6] = pSurface_pa->pCtlarray[9*i];
            ctldata[9*i+7] = pSurface_pa->pCtlarray[9*i+1];
            ctldata[9*i+8] = pSurface_pa->pCtlarray[9*i+2];
        }
    }
    pSide->pCtlarray = ctldata;
    return pSide;
}

/********************************************
 *function:构建旋转曲面的nurbs表示
 *adding:
 *author: xu
 *date: 2015/10/10
 *******************************************/
nurbsSurface_type *fileReader::formRevolution(line_type generatrix,line_type axis,float start_angle,float end_angle)
{
    float angle = end_angle-start_angle;
    //构造nurbs曲面
    nurbsSurface_type *pNurbsSurface = new nurbsSurface_type;
    memset(pNurbsSurface,0,sizeof(nurbsSurface_type));
    pNurbsSurface->typeNum = 120;
    pNurbsSurface->K1_num = 40;
    pNurbsSurface->K2_num = 2;
    pNurbsSurface->M1_num = 2;
    pNurbsSurface->M2_num = 2;
    int N1_num = 1+pNurbsSurface->K1_num-pNurbsSurface->M1_num;
    int N2_num = 1+pNurbsSurface->K2_num-pNurbsSurface->M2_num;
    int A_num = N1_num+2*pNurbsSurface->M1_num+1;
    int B_num = N2_num+2*pNurbsSurface->M2_num+1;
    int C_num = (1+pNurbsSurface->K1_num)*(1+pNurbsSurface->K2_num);
    float *knot1 = new float[A_num];
    float *knot2 = new float[B_num];
    for(int i = 0; i < A_num; i++)
        knot1[i] = knot1_prepare[i];
    for(int i = 0; i < B_num; i++)
        knot2[i] = knot2_prepare[i];
    pNurbsSurface->pKnot1 = knot1;
    pNurbsSurface->pKnot2 = knot2;
    point3f axis_point;
    axis_point.x = axis.end.x-axis.start.x;
    axis_point.y = axis.end.y-axis.start.y;
    axis_point.z = axis.end.z-axis.start.z;
    float old_x1,old_y1,old_z1,old_x2,old_y2,old_z2,old_midx,old_midy,old_midz;
    old_x1 = generatrix.start.x-axis.start.x;
    old_y1 = generatrix.start.y-axis.start.y;
    old_z1 = generatrix.start.z-axis.start.z;
    old_x2 = generatrix.end.x-axis.start.x;
    old_y2 = generatrix.end.y-axis.start.y;
    old_z2 = generatrix.end.z-axis.start.z;
    old_midx = (old_x1+old_x2)/2.0;
    old_midy = (old_y1+old_y2)/2.0;
    old_midz = (old_z1+old_z2)/2.0;
    int num = pNurbsSurface->K1_num+1;
    float *ctldata = new float[C_num*3];
    float angle_divide = 0;
    for(int i = 0; i < num; i++)
    {
        angle_divide = angle/pNurbsSurface->K1_num*i+start_angle;
        xform_type *matrix = formRevolutinMatrix(axis_point,angle_divide,axis.start);
        ctldata[9*i] = old_x1;
        ctldata[9*i+1] = old_y1;
        ctldata[9*i+2] = old_z1;
        ctldata[9*i+3] = old_midx;
        ctldata[9*i+4] = old_midy;
        ctldata[9*i+5] = old_midz;
        ctldata[9*i+6] = old_x2;
        ctldata[9*i+7] = old_y2;
        ctldata[9*i+8] = old_z2;
        xform_matrix(matrix,ctldata[9*i],ctldata[9*i+1],ctldata[9*i+2]);
        xform_matrix(matrix,ctldata[9*i+3],ctldata[9*i+4],ctldata[9*i+5]);
        xform_matrix(matrix,ctldata[9*i+6],ctldata[9*i+7],ctldata[9*i+8]);
        delete matrix;
    }
    pNurbsSurface->pCtlarray = ctldata;
    return pNurbsSurface;
}

/********************************************
 *function:查找索引目录的位置
 *adding:
 *author: xu
 *date: 2015/9/26
 *******************************************/
gl_data *fileReader::findGLData(int pos, gl_data *pHead)
{
    gl_data *pTmp = pHead;
    while (pTmp) {
        if(pTmp->type_pos == pos)
            break;
        pTmp = pTmp->pNext;
    }
    return pTmp;
}

/********************************************
 *function:查找当前折弯点对应的数据
 *adding:
 *author: xu
 *date: 2015/10/30
 *******************************************/
bend_surface *fileReader::findBendSurface(int bendnum, bend_surface *pHead)
{
    if(bendnum < 1 || bendnum > bendNum)
        return NULL;
    bend_surface *pbend = pHead;
    while (pbend) {
        if(pbend->bendNum == bendnum)
            break;
        pbend = pbend->pNext;
    }
    return pbend;
}

/********************************************
 *function:设置折弯数据链表的上下面的标志位
 *adding:
 *author: xu
 *date: 2015/10/30
 *******************************************/
void fileReader::setBendTopFlag(bend_surface *pbend,bend_surface *pHead)
{
    if(pbend == NULL)
        return;
    basic_surface *pleft = pbend->pLeftBase;
    basic_surface *pright = pbend->pRightBase;
    pbend->isTop = true;
    pbend->isVisited = true;
    pleft->isTop = true;
    pright->isTop = true;
    pleft->isVisited = 1;
    pright->isVisited = 1;
    int count1 = pleft->pBendSurface.size();
    if(count1 > 1)
    {
        for(int i = 0; i < count1; i++)
        {
            if(pleft->pBendSurface[i] == pbend->pSurface)
                continue;
            bend_surface *ptmp = pHead;
            while (ptmp) {
                if(ptmp->pSurface == pleft->pBendSurface[i] && ptmp->isVisited == false)
                {
                    setBendTopFlag(ptmp,pHead);
                }
                ptmp = ptmp->pNext;
            }
        }
    }
    int count2 = pright->pBendSurface.size();
    if(count2 > 1)
    {
        for(int i = 0; i < count2; i++)
        {
            if(pright->pBendSurface[i] == pbend->pSurface)
                continue;
            bend_surface *ptmp = pHead;
            while (ptmp) {
                if(ptmp->pSurface == pright->pBendSurface[i] && ptmp->isVisited == false)
                {
                    setBendTopFlag(ptmp,pHead);
                }
                ptmp = ptmp->pNext;
            }
        }
    }
}

/********************************************
*function:计算Y轴(滑块下压)的相关参数
 *adding:
 *author: xu
 *date: 2015/12/30
 *******************************************/
float fileReader::Y_bendDepth(float bendAngle)
{
    float y,y1,y2,y3;
    //float A = float((3*Tensile_Strength)/Emodulus);       //工件材料的简化系数
    //float x = float((0.65+0.5*log10(Kr*die_width/2))/2);  //中性层系数
    float angle = (float)fabs(bendAngle)/2;
    //y1理论值
    y1=(float)(0.5*die_width*tan(PI/2-angle));
    //y2变薄量
    float bianbo;
    float aa;
    aa=Kr*die_width/thickness;
    if(aa<=0.1)
        bianbo=0.82;
    else if(aa>0.1 && aa<=0.25)
        bianbo=0.82+((aa-0.1)/0.15)*0.05;
    else if(aa>0.25 && aa<=0.5)
        bianbo=0.87+((aa-0.25)/0.25)*0.05;
    else if(aa>0.5 && aa<=1)
        bianbo=0.92+((aa-0.5)/0.5)*0.04;
    else if(aa>1 && aa<=2)
        bianbo=0.96+(aa-1)*0.025;
    else if(aa>2 && aa<=3)
        bianbo=0.985+(aa-2)*0.007;
    else if(aa>3 && aa<=4)
        bianbo=0.992+(aa-3)*0.003;
    else if(aa>4 && aa<=5)
        bianbo=0.995+(aa-4)*0.003;
    else
        bianbo=1;
    y2=(float)bianbo*thickness;
    //y3圆角过渡修正量
    y3=float((Kr*die_width+bianbo*thickness)/sin(angle)-(Kr*die_width+bianbo*thickness));

    y=y1-y2-y3;
    y = 168-thickness+y;
    return y;
}

/********************************************
 *function:求解三元一次方程组的解
 *adding:
 *author: xu
 *date: 2016/06/23
 *******************************************/
void fileReader::EquationGroup(point3f &off, xform_type matrix)
{
    float a1 = matrix.R11;
    float b1 = matrix.R12;
    float c1 = matrix.R13;
    float d1 = matrix.T1;
    float a2 = matrix.R21;
    float b2 = matrix.R22;
    float c2 = matrix.R23;
    float d2 = matrix.T2;
    float a3 = matrix.R31;
    float b3 = matrix.R32;
    float c3 = matrix.R33;
    float d3 = matrix.T3;
    float k1 = a1 * c2 - a2 * c1;
    float m1 = b1 * c2 - b2 * c1;
    float n1 = d1 * c2 - d2 * c1;
    float k2 = a1 * c3 - a3 * c1;
    float m2 = b1 * c3 - b3 * c1;
    float n2 = d1 * c3 - d3 * c1;
    off.x = (n1 * m2 - n2 * m1) / (k1 * m2 - k2 * m1);
    off.y = (n1 - k1 * off.x) / m1;
    off.z = (d1 - a1 * off.x - b1 * off.y) / c1;
}

/********************************************
 *function:对坐标值进行矩阵变换
 *adding:
 *author: xu
 *date: 2015/9/20
 *******************************************/
void fileReader::xform_matrix(xform_type *pForm, point3f &point)
{
    if(pForm == NULL)
        return;
    float x,y,z;
    x = point.x;
    y = point.y;
    z = point.z;
    point.x = pForm->R11*x+pForm->R12*y+pForm->R13*z+pForm->T1;
    point.y = pForm->R21*x+pForm->R22*y+pForm->R23*z+pForm->T2;
    point.z = pForm->R31*x+pForm->R32*y+pForm->R33*z+pForm->T3;
}

void fileReader::xform_matrix(xform_type *pForm, float &x_pos, float &y_pos, float &z_pos)
{
    if(pForm == NULL)
        return;
    float x,y,z;
    x = x_pos;
    y = y_pos;
    z = z_pos;
    x_pos = pForm->R11*x+pForm->R12*y+pForm->R13*z+pForm->T1;
    y_pos = pForm->R21*x+pForm->R22*y+pForm->R23*z+pForm->T2;
    z_pos = pForm->R31*x+pForm->R32*y+pForm->R33*z+pForm->T3;
}

/********************************************
 *function:生成旋转矩阵
 *adding:  axis:旋转轴，angle:旋转角度
 *author: xu
 *date: 2015/11/04
 *******************************************/
xform_type *fileReader::formRevolutinMatrix(point3f axis, float angle,point3f off)
{
    xform_type *pform = new xform_type;
    float x = axis.x;
    float y = axis.y;
    float z = axis.z;
    float normal = sqrt(x*x+y*y+z*z);
    //求出旋转轴的单位向量
    x = x/normal;
    y = y/normal;
    z = z/normal;
    float c = cos(angle);
    float s = sin(angle);
    pform->R11 = x*x*(1-c)+c;
    pform->R12 = x*y*(1-c)-z*s;
    pform->R13 = x*z*(1-c)+y*s;
    pform->R21 = y*x*(1-c)+z*s;
    pform->R22 = y*y*(1-c)+c;
    pform->R23 = y*z*(1-c)-x*s;
    pform->R31 = x*z*(1-c)-y*s;
    pform->R32 = y*z*(1-c)+x*s;
    pform->R33 = z*z*(1-c)+c;
    pform->T1 = off.x;
    pform->T2 = off.y;
    pform->T3 = off.z;
    return pform;
}

/********************************************
 *function:析构裁剪曲面实体内存
 *adding:
 *author: xu
 *date: 2015/10/12
 *******************************************/
void fileReader::destroyTrimmed(trimmed_surface_type *pTrimmed)
{
    trimmed_surface_type *pCur = pTrimmed;
    while (pCur) {
        pTrimmed = pCur->pNext;
        if(pCur->typeNum == 120)    //因为只有旋转曲面才重新申请内存
        {
            destroyNurbsSurface(pCur->pSurface);
        }
        pCur = pTrimmed;
    }
    pTrimmed = NULL;
}

/********************************************
 *function:析构B样条曲面实体内存
 *adding:
 *author: xu
 *date: 2016/06/04
 *******************************************/
void fileReader::destroyNurbsSurface(nurbsSurface_type *pNurbs)
{
    if(pNurbs == NULL)
        return;
    if(pNurbs->pKnot1)
        delete pNurbs->pKnot1;
    if(pNurbs->pKnot2)
        delete pNurbs->pKnot2;
    if(pNurbs->pWeight)
        delete pNurbs->pWeight;
    if(pNurbs->pCtlarray)
        delete pNurbs->pCtlarray;
    delete pNurbs;
    pNurbs = NULL;
}

/********************************************
 *function:析构实体内存
 *adding:
 *author: xu
 *date: 2015/10/12
 *******************************************/
void fileReader::destroyEntity(gl_data *pHead)
{
    gl_data *pgldata = pHead;
    while (pgldata) {
        pHead = pgldata->pNext;
        //B样条曲线/面实体中包含一些动态申请的数组,需要析构
        int type = pgldata->typeNum;
        if(type == 100)
        {
            arc_type *ptmp = (arc_type *)pgldata->pData;
            if(ptmp)
            {
                delete ptmp;
                ptmp = NULL;
            }
        }
        else if(type == 102)
        {
            composite_curve_type *ptmp = (composite_curve_type *)pgldata->pData;
            if(ptmp)
            {
                delete ptmp;
                ptmp = NULL;
            }
        }
        else if(type == 110)
        {
            line_type *ptmp = (line_type *)pgldata->pData;
            if(ptmp)
            {
                delete ptmp;
                ptmp = NULL;
            }
        }
        else if(type == 120)
        {
            revolution_type *ptmp = (revolution_type *)pgldata->pData;
            if(ptmp)
            {
                delete ptmp;
                ptmp = NULL;
            }
        }
        else if(type == 124)
        {
            xform_type *ptmp = (xform_type *)pgldata->pData;
            if(ptmp)
            {
                delete ptmp;
                ptmp = NULL;
            }
        }
        else if(type == 126)
        {
            nurbsCurve_type *ptmp = (nurbsCurve_type *)pgldata->pData;
            if(ptmp)
            {
                delete ptmp->pKnot;
                delete ptmp->pWeight;
                delete ptmp->pCtlarray;
                delete ptmp;
                ptmp = NULL;
            }
        }
        else if(type == 128)
        {
            nurbsSurface_type *ptmp = (nurbsSurface_type *)pgldata->pData;
            if(ptmp)
            {
                delete ptmp->pKnot1;
                delete ptmp->pKnot2;
                delete ptmp->pWeight;
                delete ptmp->pCtlarray;
                delete ptmp;
                ptmp = NULL;
            }
        }
        else if(type == 142)
        {
            para_surface_type *ptmp = (para_surface_type *)pgldata->pData;
            if(ptmp)
            {
                delete ptmp;
                ptmp = NULL;
            }
        }
        else if(type == 144)
        {
            trimmed_surface_type *ptmp = (trimmed_surface_type *)pgldata->pData;
            if(ptmp)
            {
                delete ptmp;
                ptmp = NULL;
            }
        }
        delete pgldata;
        pgldata = pHead;
    }
    pHead = NULL;
}

/********************************************
 *function:对工件与机床各个部分是否干涉进行判断
 *adding:  干涉返回true,不干涉返回false
 *author: xu
 *date: 2015/11/19
 *******************************************/
bool fileReader::InterFereWithTools(trimmed_surface_type *pPart,bend_surface *pHead)
{
    if(pPart == NULL)
        return false;
    stu_polygon ptmp_surface;
    while (pPart) {
        ptmp_surface.point.clear();
        int typeNum = pPart->surfaceType;
        if(typeNum == 128)  //暂时先不考虑曲面,此处也未考虑机床各部分的内轮廓
        {
            for(int i = 0; i < pPart->outlineCount[0]; i++)
            {
                int type = pPart->outlineType[0][i];
                if(type == 110) //为直线
                {
                    line_type *pline = (line_type *)pPart->pOutlines[0][i];
                    ptmp_surface.point.push_back(pline->start);
                }
            }
            ptmp_surface.point_num = ptmp_surface.point.size();
            bend_surface *pBend = pHead;
            resetVisitFlag(pHead);
            while (pBend) {
                if(pBend->isBended == 0)//此处为展开平面
                {
                    //判断展开平面和侧面是否与工件碰撞
                    bool btag = InterFere_Face_With_Face(pBend->pDeploy,&ptmp_surface);
                    if(btag)
                        return true;
                    btag = InterFere_Face_With_Face(pBend->pParallel->pDeploy,&ptmp_surface);
                    if(btag)
                        return true;
                    if(pBend->pSideBack)
                        btag = InterFere_Face_With_Face(pBend->pSideBack,&ptmp_surface);
                    else
                        btag = InterFere_Face_With_Face(pBend->pParallel->pSideBack,&ptmp_surface);
                    if(btag)
                        return true;
                    if(pBend->pSideFont)
                        btag = InterFere_Face_With_Face(pBend->pSideFont,&ptmp_surface);
                    else
                        btag = InterFere_Face_With_Face(pBend->pParallel->pSideFont,&ptmp_surface);
                    if(btag)
                        return true;
                }
                if(pBend->pLeftBase->isVisited == 0)
                {
                    //判断左侧基面是否与工件发生碰撞 /*该处可能出现错误*/
                    bool btag = InterFere_trimmed_surface_With_Face(pBend->pLeftBase->pSurface,&ptmp_surface);
                    if(btag == true)
                        return btag;
                    pBend->pLeftBase->isVisited = 1;
                }
                if(pBend->pParallel->pLeftBase->isVisited == 0)
                {
                    //判断左侧基面是否与工件发生碰撞
                    bool btag = InterFere_trimmed_surface_With_Face(pBend->pParallel->pLeftBase->pSurface,&ptmp_surface);
                    if(btag == true)
                        return btag;
                    pBend->pParallel->pLeftBase->isVisited = 1;
                }
                if(pBend->pRightBase->isVisited == 0)
                {
                    //判断右侧基面是否与工件发生碰撞
                    bool btag = InterFere_trimmed_surface_With_Face(pBend->pRightBase->pSurface,&ptmp_surface);
                    if(btag == true)
                        return btag;
                    pBend->pRightBase->isVisited = 1;
                }
                if(pBend->pParallel->pRightBase->isVisited == 0)
                {
                    //判断右侧基面是否与工件发生碰撞
                    bool btag = InterFere_trimmed_surface_With_Face(pBend->pParallel->pRightBase->pSurface,&ptmp_surface);
                    if(btag == true)
                        return btag;
                    pBend->pParallel->pRightBase->isVisited = 1;
                }
                for(int i = 0; i < pBend->pLeftBase->pSideSurface.size(); i++)
                {
                    //判断左侧侧面是否与工件碰撞
                    trimmed_surface_type *pface = pBend->pLeftBase->pSideSurface[i];
                    bool btag = InterFere_trimmed_surface_With_Face(pface,&ptmp_surface);
                    if(btag == true)
                        return btag;
                }
                for(int i = 0; i < pBend->pRightBase->pSideSurface.size(); i++)
                {
                    //判断右侧侧面是否与工件碰撞
                    trimmed_surface_type *pface = pBend->pRightBase->pSideSurface[i];
                    bool btag = InterFere_trimmed_surface_With_Face(pface,&ptmp_surface);
                    if(btag == true)
                        return btag;
                }
                pBend = pBend->pNext;
            }
            resetVisitFlag(pHead);
        }
        pPart = pPart->pNext;
    }
    return false;   //没有发生碰撞
}

/********************************************
 *function:判断基面与平面的干涉情况
 *adding:  干涉返回true,不干涉返回false
 *author: xu
 *date: 2015/11/19
 *******************************************/
bool fileReader::InterFere_trimmed_surface_With_Face(trimmed_surface_type *pTrimmed, stu_polygon *pPolygon)
{
    //判断左侧基面是否与工件发生碰撞 /*该处可能出现错误*/
    stu_polygon pface;
    trimmed_surface_type *ptmp = pTrimmed;
    for(int i = 0; i < ptmp->outlineCount[0]; i++)
    {
        int type = ptmp->outlineType[0][i];
        if(type == 110)
        {
            line_type *pline = (line_type *)ptmp->pOutlines[0][i];
            pface.point.push_back(pline->start);
        }
        else if(126 == type)
        {
            nurbsCurve_type *pcurve = (nurbsCurve_type *)ptmp->pOutlines[0][i];
            point3f point;
            point.x = pcurve->pCtlarray[0];
            point.y = pcurve->pCtlarray[1];
            point.z = pcurve->pCtlarray[2];
            pface.point.push_back(point);
        }
    }
    pface.point_num = pface.point.size();
    bool btag = InterFere_Face_With_Face(&pface,pPolygon);
    if(btag)   //初步判断此处有干涉,但还要考虑板料中间的孔洞(也就是内轮廓)
    {
        //跳过外轮廓,直接判断内轮廓
        QVector<stu_polygon> pInner;//获取所有的内轮廓
        for(int i = 1; i < ptmp->curve_num+1; i++)
        {
            stu_polygon psur;
            for(int j = 0; j < ptmp->outlineCount[i]; j++)
            {
                int type = ptmp->outlineType[i][j];
                if(type == 110)
                {
                    line_type *pline = (line_type *)ptmp->pOutlines[i][j];
                    psur.point.push_back(pline->start);
                }
                else if(126 == type)
                {
                    nurbsCurve_type *pcurve = (nurbsCurve_type *)ptmp->pOutlines[i][j];
                    point3f point;
                    point.x = pcurve->pCtlarray[0];
                    point.y = pcurve->pCtlarray[1];
                    point.z = pcurve->pCtlarray[2];
                    psur.point.push_back(point);
                }
            }
            pInner.push_back(psur);
        }
        //首先必须判断哪个内轮廓平面与检测的平面干涉,舍弃那些与该检测平面不干涉的内轮廓
        int count = 0; //内轮廓与该检测平面不干涉的个数
        for(int n = 0; n < pInner.size(); n++)
        {
            bool tmp_tag = InterFere_Face_With_Face(&pInner[n],pPolygon);
            if(tmp_tag) //如果与该检测平面有干涉,说明需要对其进行深度判断
            {
                //判断内轮廓的每一条线段是否与该检测平面是否干涉,如果有任何一条线段与之有干涉,则干涉
                //如果任何一条线段都与该检测平面不干涉,则说明该检测平面与内轮廓不干涉
                QVector<point3f> point = pInner[n].point;
                int num = point.size();
                //首先判断第一个平面的轮廓线段与第二个平面的碰撞情况,只要有碰撞就返回
                line_type line;
                for(int i = 0; i < num; i++)
                {
                    line.start = point[i];
                    if(i == num-1)
                        line.end = point[0];
                    else
                        line.end = point[i+1];
                    bool inner_tag = InterFere_Line_With_Surface(&line,pPolygon);
                    if(inner_tag)    //说明内轮廓与该检测平面有干涉
                        return inner_tag;
                }
            }
            else
                count++;
        }
        if(count == pInner.size())  //说明内轮廓与该检测平面都没有干涉,说明确实存在干涉
            return true;
    }
    return false;
}

/********************************************
 *function:判断平面与平面的干涉情况
 *adding:  干涉返回true,不干涉返回false
 *author: xu
 *date: 2015/11/19
 *******************************************/
bool fileReader::InterFere_Face_With_Face(stu_polygon *pPolygon1, stu_polygon *pPolygon2)
{
    int num1 = pPolygon1->point.size();
    int num2 = pPolygon2->point.size();
    //首先判断第一个平面的轮廓线段与第二个平面的碰撞情况,只要有碰撞就返回
    line_type line;
    for(int i = 0; i < num1; i++)
    {
        line.start = pPolygon1->point[i];
        if(i == num1-1)
            line.end = pPolygon1->point[0];
        else
            line.end = pPolygon1->point[i+1];
        bool btag = InterFere_Line_With_Surface(&line,pPolygon2);
        if(btag)
            return true;
    }
    //然后判断第二个平面的轮廓线与第一个平面的碰转情况,只要碰撞就返回
    for(int i = 0; i < num2; i++)
    {
        line.start = pPolygon2->point[i];
        if(i == num2-1)
            line.end = pPolygon2->point[0];
        else
            line.end = pPolygon2->point[i+1];
        bool btag = InterFere_Line_With_Surface(&line,pPolygon1);
        if(btag)
            return true;
    }
    return false;
}

/********************************************
 *function:判断直线与平面的干涉情况
 *adding:  干涉返回true,不干涉返回false
 *author: xu
 *date: 2015/11/19
 *******************************************/
bool fileReader::InterFere_Line_With_Surface(line_type *pLine, stu_polygon *pPolygon)
{
    point3f line1,line2,dir;
    point3f normal,face;
    if(pPolygon->point.size() == 0)
        return false;
    face = pPolygon->point[0];
    //计算求得平面的法向量
    computeSurfaceNormal(pPolygon,normal);
    //求得直线的方向向量
    line1 = pLine->start;
    line2 = pLine->end;
    dir.x = pLine->end.x-pLine->start.x;
    dir.y = pLine->end.y-pLine->start.y;
    dir.z = pLine->end.z-pLine->start.z;
    float vpt = normal.x*dir.x+normal.y*dir.y+normal.z*dir.z;
    if(fabs(vpt) < EPSINON) //分母为0,则直线与平面平行,不会发生干涉
        return false;
    float t = ((face.x-line1.x)*normal.x+(face.y-line1.y)*normal.y+(face.z-line1.z)*normal.z)/vpt;
    if(t < 0) //t小于0，则为反方向
        return false;
    //求得直线与平面的交点
    point3f point;
    point.x = line1.x+dir.x*t;
    point.y = line1.y+dir.y*t;
    point.z = line1.z+dir.z*t;
    //判断交点是否在线段上
    //只需要判断一个坐标轴上的点就可以了
    if(fabs(dir.x) > EPSINON)
    {
        if(dir.x > 0)
        {
            //方向向量X坐标大于0,交点坐标大于终点坐标，则不会干涉
            if(point.x > line2.x)
                return false;
        }
        else
        {
            //方向向量X坐标小于0,交点坐标小于终点坐标，则不会干涉
            if(point.x < line2.x)
                return false;
        }
    }
    else if(fabs(dir.y) > EPSINON)
    {
        if(dir.y > 0)
        {
            //方向向量Y坐标大于0,交点坐标大于终点坐标，则不会干涉
            if(point.y > line2.y)
                return false;
        }
        else
        {
            //方向向量Y坐标小于0,交点坐标小于终点坐标，则不会干涉
            if(point.y < line2.y)
                return false;
        }
    }
    else if(fabs(dir.z) > EPSINON)
    {
        if(dir.z > 0)
        {
            //方向向量Z坐标大于0,交点坐标大于终点坐标，则不会干涉
            if(point.z > line2.z)
                return false;
        }
        else
        {
            //方向向量Z坐标小于0,交点坐标小于终点坐标，则不会干涉
            if(point.z < line2.z)
                return false;
        }
    }
    //判断交点是否在平面上
    return judgePointOnSurface(point,pPolygon,normal);
}

/********************************************
 *function:判断点是否在平面上
 *adding:
 *author: xu
 *date: 2015/11/20
 *******************************************/
bool fileReader::judgePointOnSurface(point3f point, stu_polygon *pPolygon, point3f normal)
{
    //float number = 1;
   //本函数采用引射线法判断点是否在多边形内部,
   //从目标出发引一条射线，看这条射线和多边形所有边的交点数目
   //如果有奇数个点则说明在内部，如果有偶数个点则说明在外部
   //首先根据法向量来判断向那个方向投影
   int num = pPolygon->point.size();
   float x_array[num];
   float y_array[num];
   float x_pos,y_pos;
   //这个地方判断很重要,此处验证是只要法线XYZ其中小于0.1即为0;
   float x_num = fabs(normal.x);
   float y_num = fabs(normal.y);
   float z_num = fabs(normal.z);
   int flag = 0;
   if(x_num > y_num)
   {
       if(x_num > z_num)   //x轴数值最大
           flag = 1;
       else                //z轴数值最大
           flag = 3;
   }
   else
   {
       if(y_num > z_num)   //y轴数值最大
           flag = 2;
       else                //z轴数值最大
           flag = 3;
   }

   if(flag == 1)
   {
       //法向量的Z,Y轴坐标为0
       for(int i = 0; i < num; i++)
       {
           x_array[i] = pPolygon->point[i].z;
           y_array[i] = pPolygon->point[i].y;
       }
       x_pos = point.z;
       y_pos = point.y;
   }
   if(flag == 2)
   {
       //法向量的X,Z轴坐标为0
       for(int i = 0; i < num; i++)
       {
           x_array[i] = pPolygon->point[i].x;
           y_array[i] = pPolygon->point[i].z;
       }
       x_pos = point.x;
       y_pos = point.z;
   }
   if(flag == 3)
   {
       //法向量的X,Y轴坐标为0
       for(int i = 0; i < num; i++)
       {
           x_array[i] = pPolygon->point[i].x;
           y_array[i] = pPolygon->point[i].y;
       }
       x_pos = point.x;
       y_pos = point.y;
   }
//    if(fabs(normal.x) < EPSINON && fabs(normal.y) < EPSINON)
//    {
//        //法向量的X,Y轴坐标为0
//        for(int i = 0; i < num; i++)
//        {
//            x_array[i] = pPolygon->point[i].x;
//            y_array[i] = pPolygon->point[i].y;
//        }
//        x_pos = point.x;
//        y_pos = point.y;
//    }
//    else if(fabs(normal.x) < EPSINON && fabs(normal.z) < EPSINON)
//    {
//        //法向量的X,Z轴坐标为0
//        for(int i = 0; i < num; i++)
//        {
//            x_array[i] = pPolygon->point[i].x;
//            y_array[i] = pPolygon->point[i].z;
//        }
//        x_pos = point.x;
//        y_pos = point.z;
//    }
//    else if(fabs(normal.y) < EPSINON && fabs(normal.z) < EPSINON)
//    {
//        //法向量的Z,Y轴坐标为0
//        for(int i = 0; i < num; i++)
//        {
//            x_array[i] = pPolygon->point[i].z;
//            y_array[i] = pPolygon->point[i].y;
//        }
//        x_pos = point.z;
//        y_pos = point.y;
//    }
//    else
//    {
//        for(int i = 0; i < num; i++)
//        {
//            x_array[i] = pPolygon->point[i].x;
//            y_array[i] = pPolygon->point[i].y;
//        }
//        x_pos = point.x;
//        y_pos = point.y;
//    }
   int i,j,count = 0;
   for(i = 0,j = num-1; i < num; j = i++)
   {
       if(((y_array[i] > y_pos) != (y_array[j] > y_pos)) &&\
          (x_pos < (x_array[j]-x_array[i])*(y_pos-y_array[i])\
           /(y_array[j]-y_array[i])+x_array[i]))
           count = !count;
   }
   if(count%2 == 0)    //count为偶数
       return false;   //不在多边形内部，没有碰撞
   else                //count为奇数
       return true;    //在多边形内部，有碰撞
}

/********************************************
 *function:恢复指定折弯点的折弯数据
 *adding:
 *author: xu
 *date: 2015/12/28
 *******************************************/
void fileReader::recoverBendData(int bendnum, bend_surface *pHead, int tag)
{
    bend_surface *pbend = findBendSurface(bendnum,pHead);
    pbend->isBended = 0;
    pbend->pParallel->isBended = 0;
    resetVisitFlag(pHead);
    MatrixPartConvert(pbend->pLeftInverse,pHead,pbend,pbend->pLeftBase,false);
    MatrixPartConvert(pbend->pParallel->pLeftInverse,pHead,pbend->pParallel,pbend->pParallel->pLeftBase,true);
    MatrixPartConvert(pbend->pRightInverse,pHead,pbend,pbend->pRightBase,false);
    MatrixPartConvert(pbend->pParallel->pRightInverse,pHead,pbend->pParallel,pbend->pParallel->pRightBase,true);
    resetVisitFlag(pHead);
    if(tag == 1)
    {
        xform_type *matrix = formTurnAboutMatrix(bendnum,pHead);
        MatrixTotalConvert(pHead,matrix,false);
        MatrixTotalConvert(pHead,matrix,true); //调头
        delete matrix;
    }
    MatrixTotalConvert(pHead,pbend->pInverse,false);
    MatrixTotalConvert(pHead,pbend->pInverse,true);
}


