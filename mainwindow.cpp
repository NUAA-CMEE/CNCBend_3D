#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "sysglobalvars.h"
#include "SysKeyboard.h"
#include <QTextCodec>
#include <QPainter>
#include <algorithm>
#include <QDir>

QColor normal_color(0,0,0);
QColor select_color(255,0,0);
QColor disable_color(220,220,220);

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui_MainWindow(new Ui::MainWindow)
{
    dbName = dbPath + "machineconstants.db";
//    QDir tmpDir(dbName);
//    dbName = tmpDir.absolutePath();
    pDeploy = NULL;
    pTools = NULL;
    pModel = NULL;
    bShow_body = false;
    bShow_punch = false;
    bShow_die = false;
    bShow_x = false;
    bShow_r = false;
    bShow_z = false;
    bManualSelect = false;
    mainView_flg = true;
    topView_flg = true;
    sideView_flg = true;
    axisSide_flg = true;
    ui_MainWindow->setupUi(this);
    this->setWindowTitle(tr("折弯机钣金加工仿真软件"));
    this->setWindowIcon(QIcon(":/images/resource/Micon.PNG"));
    //setWindowFlags(/*Qt::Tool |*/ Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    //this->showFullScreen();

    pStatus = ui_MainWindow->statusBar;
    pGLWidget = ui_MainWindow->widget;
    pTree = ui_MainWindow->treeWidget;
    pMachineArea = ui_MainWindow->widget_3;
    pPartArea = ui_MainWindow->widget_4;
    pManualArea = ui_MainWindow->widget_2;
    pManualArea_slider = ui_MainWindow->widget_5;
    pTool_btn = ui_MainWindow->widget_6;
    pSlider = ui_MainWindow->slider;
    pYAxis = ui_MainWindow->y_btn;
    pX1Axis = ui_MainWindow->x1_btn;
    pX2Axis = ui_MainWindow->x2_btn;
    pX1Axis_Back = ui_MainWindow->x1_back_btn;
    pX2Axis_Back = ui_MainWindow->x2_back_btn;
    pR1Axis = ui_MainWindow->r1_btn;
    pR2Axis = ui_MainWindow->r2_btn;
    pZ1Axis = ui_MainWindow->z1_btn;
    pZ2Axis = ui_MainWindow->z2_btn;
    pCurPos = ui_MainWindow->pos_edit;
    pInterfere = ui_MainWindow->inter_tag;
    pInterText = ui_MainWindow->inter_label;
    pInterText->setVisible(pInterfere->checkState());
    pButton1 = ui_MainWindow->before_btn;
    pButton2 = ui_MainWindow->next_btn;
    pManualList = ui_MainWindow->listWidget;
    pModelSelect = ui_MainWindow->comboBox;
    pTree->setColumnCount(1);
    pTree->setHeaderLabel(tr("项目"));

    count_num = 0;
    bendNum = 0;
    rote_x = 0.0;
    rote_y = 0.0;
    curNum = 1;

    timeLabel = new QLabel();
    pStatus->addPermanentWidget(timeLabel);
    pTimer = new QTimer(this);
    connect(pTimer,SIGNAL(timeout()),this,SLOT(timeOutSlot()));
    pTimer->start(1);

    pNetWork = NULL;
    pSelect = NULL;
    pConfigDlg = NULL;
    pMachineConstant = NULL;
    pAxisConfig = NULL;

    initPopMenuContent();
    initMenuState();

    QString strSQL = "select y_max,y_min,x_max,x_min,r_max,r_min,z_max,z_min,bendRate from AxisSet where id = 1";
    QStringList list = ReadSqlData(dbName,strSQL);
    if(!list.isEmpty())
    {
        y_max = list.at(0).toFloat();
        y_min = list.at(1).toFloat();
        x_max = list.at(2).toFloat();
        x_min = list.at(3).toFloat();
        r_max = list.at(4).toFloat();
        r_min = list.at(5).toFloat();
        z_max = list.at(6).toFloat();
        z_min = list.at(7).toFloat();
    }
    Y_factor = list.at(8).toFloat();
    updateRadioButtonState();

    //设置窗体初始化的相对尺寸
    QList<int> width;
    width<<400<<100;
    ui_MainWindow->splitter->setSizes(width);
    ui_MainWindow->splitter->setStretchFactor(0,1);
    ui_MainWindow->splitter->setStretchFactor(1,1);
    QList<int> width2;
    width2<<100<<100;
    ui_MainWindow->splitter_2->setSizes(width2);
    ui_MainWindow->splitter_2->setStretchFactor(0,1);
    ui_MainWindow->splitter_2->setStretchFactor(1,1);
    QList<int> width3;
    width3<<150<<500<<50<<50;
    ui_MainWindow->splitter_3->setSizes(width3);
    ui_MainWindow->splitter_3->setStretchFactor(0,1);
    ui_MainWindow->splitter_3->setStretchFactor(1,1);
    ui_MainWindow->splitter_3->setStretchFactor(2,1);
    ui_MainWindow->splitter_3->setStretchFactor(3,1);
    QList<int> width4;
    width4<<100<<500;
    ui_MainWindow->splitter_4->setSizes(width4);
    ui_MainWindow->splitter_4->setStretchFactor(0,1);
    ui_MainWindow->splitter_4->setStretchFactor(1,1);

    pOperate = fileOperate::getInstance();
    pBendGL = new bendGLWidget(pGLWidget);
    pBendGL->show();
    pDeploy = new uiDeployPage(pPartArea);
    connect(pDeploy,SIGNAL(sendSelectInfo(int)),this,SLOT(getSelectInfo(int)));
    pDeploy->show();
    this->showMaximized();

    //centralWidget是QMainWindow类的子类,同时需要开启
    ui_MainWindow->centralWidget->setMouseTracking(true);
    //ui_MainWindow->splitter_5->setMouseTracking(true);
    //ui_MainWindow->splitter_4->widget(0)->setMouseTracking(true);
    ui_MainWindow->treeWidget->setMouseTracking(true);
    setMouseTracking(true);
    pManualArea->hide();
    pManualArea_slider->hide();
    pTool_btn->hide();
    pInterText->setText("");
}

MainWindow::~MainWindow()
{
    if(pNetWork)
    {
        delete pNetWork;
        pNetWork = NULL;
    }
    if(pSelect)
    {
        delete pSelect;
        pSelect = NULL;
    }
    if(pDeploy)
    {
        delete pDeploy;
        pDeploy = NULL;
    }
    if(pModel)
    {
        delete pModel;
        pModel = NULL;
    }
    if(pBendGL)
    {
        delete pBendGL;
        pBendGL = NULL;
    }
    if(pOperate)
    {
        pOperate->destroyClass();
    }
    delete ui_MainWindow;
}

/********************************************
 *function:窗口大小改变触发该函数
 *adding:
 *author: xu
 *date: 2015/9/18
 *******************************************/
void MainWindow::resizeEvent(QResizeEvent *event)
{
    event = event;
    updateShowRect();
}

/********************************************
 *function:更新显示窗口大小
 *adding:
 *author: xu
 *date: 2016/04/15
 *******************************************/
void MainWindow::updateShowRect()
{
    if(pBendGL)
    {
        int width = pGLWidget->width();
        int height = pGLWidget->height();
        pBendGL->resizeWindow(width,height);
    }
    if(pTools)
    {
        int width = pMachineArea->width();
        int height = pMachineArea->height();
        pTools->resizeWindow(width,height);
    }
    if(pDeploy)
    {
        int width = pPartArea->width();
        int height = pPartArea->height();
        pDeploy->resizeWindow(width,height);
    }
}

/********************************************
 *function:键盘事件触发
 *adding:
 *author: xu
 *date: 2015/9/21
 *******************************************/
void MainWindow::keyPressEvent(QKeyEvent *eKey)
{
    int iKey = eKey->key();
    switch (iKey) {
    case Key_F1:        //图形复位
    {
        if(pBendGL)
            pBendGL->resetGraph();
        break;
    }
    case Key_Left:
    {
        rote_y += 2.0;
        pBendGL->ex = circle_radius*sin(rote_y/180.0*PI);
        pBendGL->ez = circle_radius*cos(rote_y/180.0*PI);
        //pBendGL->ey = CIRCLE_RADIUS*sin(rote_x/180.0*PI);
        if(pBendGL)
            pBendGL->convertGraph(-0.8,CONVERT_Y);
        break;
    }
    case Key_Right:
    {
        rote_y -= 2.0;
        pBendGL->ex = circle_radius*sin(rote_y/180.0*PI);
        pBendGL->ez = circle_radius*cos(rote_y/180.0*PI);
        //pBendGL->ey = CIRCLE_RADIUS*sin(rote_x/180.0*PI);
        if(pBendGL)
            pBendGL->convertGraph(0.8,CONVERT_Y);
        break;
    }
    case Key_Up:
    {
        rote_x += 2.0;
        //pBendGL->ex = CIRCLE_RADIUS*cos(rote_x/180.0*PI)*sin(rote_y/180.0*PI);
        //pBendGL->ez = CIRCLE_RADIUS*cos(rote_x/180.0*PI)*cos(rote_y/180.0*PI);
        pBendGL->ey = circle_radius*sin(rote_x/180.0*PI);
        if(pBendGL)
            pBendGL->convertGraph(-0.8,CONVERT_X);
        break;
    }
    case Key_Down:
    {
        rote_x -= 2.0;
        //pBendGL->ex = CIRCLE_RADIUS*cos(rote_x/180.0*PI)*sin(rote_y/180.0*PI);
        //pBendGL->ez = CIRCLE_RADIUS*cos(rote_x/180.0*PI)*cos(rote_y/180.0*PI);
        pBendGL->ey = circle_radius*sin(rote_x/180.0*PI);
        if(pBendGL)
            pBendGL->convertGraph(0.8,CONVERT_X);
        break;
    }
    case Key_F2:
    {
        if(pBendGL)
            pBendGL->convertGraph(0.8,CONVERT_Z);
        break;
    }
    case Key_F3:
    {
        if(pBendGL)
            pBendGL->convertGraph(-0.8,CONVERT_Z);
        updatePosData();
        break;
    }
    case Key_F4:
    {
        bendAction();
        break;
    }
    case Key_F5:
    {
        pOperate->drawDeployAction();
        updatePosData();
        updateSelectState();
        pBendGL->updateGL();
        break;
    }
    case Key_F6:
    {
        if(pBendGL)
        {
            count_num++;
            //pBendGL->formBendOrder(count_num);
        }
        //pBendGL->num_count++;

        break;
    }
    case Key_F7:
    {

        count_num++;
        if(count_num >= 20)
            count_num = 0;
//        pOperate->converToNum(count_num);
//        updateShowRect();
//        if(pBendGL)
//            pBendGL->test(count_num);
        break;
    }
    case Key_F8:
    {
        //pOperate->deploy_num(count_num);
//        bend_surface *pbend = pBend_Backup[count_num];
//        pBendGL->setWorkPieceModel(pbend);
//        updateShowRect();
        break;
    }
    case Key_Return:
    case Key_Enter:
    {
        enterSlot();
        break;
    }
    default:
        break;
    }
}

/********************************************
 *function:鼠标滚轮事件触发
 *adding:
 *author: xu
 *date: 2015/9/21
 *******************************************/
void MainWindow::wheelEvent(QWheelEvent *event)
{
//    int pos_x = event->x();
//    int pos_y = event->y();
    int numDegress = event->delta()/8;  //滚动的角度，*8就是鼠标滚动的距离
    int numSteps = numDegress/15;       //滚动的步数，*15就是鼠标滚动的角度
//    int part_x_start = pPartArea->x();
//    int part_x_end = pPartArea->x()+pPartArea->width();
//    int part_y_start = pPartArea->y();
//    int part_y_end = pPartArea->y()+pPartArea->height();
//    int bend_x_start = pGLWidget->x();
//    int bend_x_end = pGLWidget->x()+pGLWidget->width();
//    int bend_y_start = pGLWidget->y();
//    int bend_y_end = pGLWidget->y()+pGLWidget->height();
//    if((pos_x >= part_x_start) && (pos_x <= part_x_end) && \
//        (pos_y >= part_y_start) && (pos_y <= part_y_end))
//    {
//        //说明鼠标当前在折弯展开图
//        if(pDeploy)
//        {
//            float zoom = numSteps*0.1;
//            pDeploy->convertGraph(zoom,CONVERT_ZOOM);
//            updateShowRect();
//        }

//    }
//    if((pos_x >= bend_x_start) && (pos_x <= bend_x_end) && \
//        (pos_y >= bend_y_start) && (pos_y <= bend_y_end))
//    {
        //说明鼠标当前在折弯仿真区域
        if(pBendGL)
        {
            float zoom = numSteps*0.1;
            pBendGL->convertGraph(zoom,CONVERT_ZOOM);
//            updateShowRect();
        }
//    }
    event->accept();                    //接收该事件
}

/********************************************
 *function:鼠标移动事件触发
 *adding:
 *author: xu
 *date: 2015/9/21
 *******************************************/
void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    int pos_x = event->x();
    int pos_y = event->y(); //去掉
//    pButton1->setText(QString("%1").arg(pos_x));
//    pButton2->setText(QString("%1").arg(pos_y));
    //qDebug()<<pos_x<<pos_y<<endl;

//    int part_x_start = pPartArea->x();
//    int part_x_end = pPartArea->x()+pPartArea->width();
//    int part_y_start = pPartArea->y();
//    int part_y_end = pPartArea->y()+pPartArea->height();
//    int x = ui_MainWindow->splitter->widget(1)->x();
//    int y = ui_MainWindow->splitter->y();
//    int x2 = ui_MainWindow->splitter_2->x();
//    int y2 = ui_MainWindow->splitter_2->y();
//    int bend_x_start = ui_MainWindow->splitter->widget(0)->x()+ui_MainWindow->splitter_4->width();
//    int bend_x_end = bend_x_start+ui_MainWindow->splitter->widget(0)->width();
//    int bend_y_start = ui_MainWindow->splitter->widget(0)->y()+ui_MainWindow->splitter_2->height();
//    int bend_y_end = bend_y_start+ui_MainWindow->splitter->widget(0)->height();
    //qDebug()<<bend_x_start<<bend_y_start<<endl;
    if(event->buttons() & Qt::LeftButton)      //左键被按下
    {
//        if((pos_x >= part_x_start) && (pos_x <= part_x_end) && \
//            (pos_y >= part_y_start) && (pos_y <= part_y_end))
//        {
//            //说明鼠标当前在折弯展开图
//            if(pDeploy)
//            {
//                if(fabs(pos_x-mouse_x) > fabs(pos_y-mouse_y))
//                    pDeploy->convertGraph((pos_x-mouse_x)/2,CONVERT_HORIZONTAL);
//                else
//                    pDeploy->convertGraph((mouse_y-pos_y)/2,CONVERT_VERTICAL);
//                updateShowRect();
//            }
//        }
//        else if((pos_x >= bend_x_start) && (pos_x <= bend_x_end) && \
//            (pos_y >= bend_y_start) && (pos_y <= bend_y_end))
//        {
            //说明鼠标当前在折弯仿真区域
            if(pBendGL)
            {
                if(fabs(pos_x-mouse_x) > fabs(pos_y-mouse_y))
                    pBendGL->convertGraph((pos_x-mouse_x)/2,CONVERT_HORIZONTAL);
                else
                    pBendGL->convertGraph((mouse_y-pos_y)/2,CONVERT_VERTICAL);
                updateShowRect();
            }
//        }
    }
    else if(event->buttons() & Qt::MiddleButton) //中键被按下
    {
        float rote = 4.0;
        if(pBendGL)
        {
            float x_off = pos_x-mouse_x;
            float y_off = pos_y-mouse_y;
            if(x_off > 100)
                pBendGL->convertGraph(rote,CONVERT_Y);
            else if(x_off < -100)
                pBendGL->convertGraph(-rote,CONVERT_Y);
            if(y_off > 100)
                pBendGL->convertGraph(rote,CONVERT_X);
            else if(y_off < -100)
                pBendGL->convertGraph(-rote,CONVERT_X);
            if(x_off > 100 && y_off > 100)
                pBendGL->rotateGraph(1.0,1.0,0.0);
            else if(x_off < -100 && y_off < -100)
                pBendGL->rotateGraph(-1.0,-1.0,0.0);
            else if(x_off > 100 && y_off < 100)
                pBendGL->rotateGraph(0.0,1.0,0.0);
            updateShowRect();
        }
    }
    else if(event->buttons() & Qt::RightButton)  //右键被按下
    {

    }
}

/********************************************
 *function:鼠标按下事件触发
 *adding:
 *author: xu
 *date: 2015/9/21
 *******************************************/
void MainWindow::mousePressEvent(QMouseEvent *event)
{
    mouse_x = event->x();
    mouse_y = event->y();
    int button = event->button();
    switch (button) {
    case Qt::RightButton:
    {
        break;
    }
    case Qt::LeftButton:
    {
        if(bManualState == true)   //说明进入了手动工序调整阶段
        {
            //处理鼠标的位置,以折弯展开图的左上角（0,0）
            int pos_x = mouse_x-(pTree->width()+ui_MainWindow->splitter_2->widget(0)->width()+21);
            int pos_y = mouse_y - 68;
            int end_x = ui_MainWindow->splitter_2->widget(1)->width();
            int end_y = ui_MainWindow->splitter_2->widget(1)->height();
            if(pos_x >= 0 && pos_x <= end_x && pos_y >= 0 && pos_y <= end_y){
//                pButton1->setText(QString("%1").arg(pos_x));
//                pButton2->setText(QString("%1").arg(pos_y));
                //在折弯展开图中,需要进行折弯段选取工作
                pDeploy->selectBendLine(pos_x,pos_y);
            }
        }
        break;
    }
    case Qt::MiddleButton:
    {
        setCursor(Qt::IBeamCursor);
        break;
    }
    default:
        break;
    }
}

/********************************************
 *function:鼠标松开事件触发
 *adding:
 *author: xu
 *date: 2015/10/10
 *******************************************/
void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    mouse_x = event->x();
    mouse_y = event->y();
    int button = event->button();
    switch (button) {
    case Qt::RightButton:
    {
        break;
    }
    case Qt::LeftButton:
    {
        break;
    }
    case Qt::MiddleButton:
    {
        setCursor(Qt::ArrowCursor);
        updateShowRect();
        break;
    }
    default:
        break;
    }
}

/********************************************
 *function:确认键响应函数
 *adding:
 *author: xu
 *date: 2016/06/11
 *******************************************/
void MainWindow::enterSlot()
{
    float curpos = pCurPos->text().toFloat();
    if(pYAxis->isChecked())
    {
        if(pOperate->getBendedState())   //处于折弯状态
        {
            pOperate->formAxisData(Y_AXIS,curpos);
            updatePosData();
        }
        else
        {
            QMessageBox::information(this,tr("程序提示"),tr("当前为展开状态,无法设置Y轴开口"),QMessageBox::Yes);
            return;
        }
    }
    else if(pX1Axis->isChecked())
    {
        pOperate->formAxisData(X1_AXIS,curpos);
        updatePosData();
    }
    else if(pX1Axis_Back->isChecked())
    {
        if(pOperate->getBendedState())   //处于折弯状态
        {
            pOperate->formAxisData(X1_AXIS_BACK,curpos);
            updatePosData();
        }
        else
        {
            QMessageBox::information(this,tr("程序提示"),tr("当前为展开状态,无法设置退让距离"),QMessageBox::Yes);
            return;
        }
    }
    else if(pX2Axis->isChecked())
    {
        pOperate->formAxisData(X2_AXIS,curpos);
        updatePosData();
    }
    else if(pX2Axis_Back->isChecked())
    {
        if(pOperate->getBendedState())   //处于折弯状态
        {
            pOperate->formAxisData(X2_AXIS_BACK,curpos);
            updatePosData();
        }
        else
        {
            QMessageBox::information(this,tr("程序提示"),tr("当前为展开状态,无法设置退让距离"),QMessageBox::Yes);
            return;
        }
    }
    else if(pR1Axis->isChecked())
    {
        pOperate->formAxisData(R1_AXIS,curpos);
        updatePosData();
    }
    else if(pR2Axis->isChecked())
    {
        pOperate->formAxisData(R2_AXIS,curpos);
        updatePosData();
    }
    else if(pZ1Axis->isChecked())
    {
        pOperate->formAxisData(Z1_AXIS,curpos);
        updatePosData();
    }
    else if(pZ2Axis->isChecked())
    {
        pOperate->formAxisData(Z2_AXIS,curpos);
        updatePosData();
    }
    pBendGL->updateGL();
}

/********************************************
 *function:点击新建的槽函数
 *adding:
 *author: xu
 *date: 2015/9/18
 *******************************************/
void MainWindow::on_New_triggered()
{

}

/********************************************
 *function:点击打开的槽函数
 *adding:
 *author: xu
 *date: 2015/9/18
 *******************************************/
void MainWindow::on_Open_triggered()
{
    //使用打开文件标准对话框
    QFileDialog *fd = new QFileDialog(this);
    fd->setWindowTitle(tr("打开板料模型"));
    fd->setDirectory(filePath);             //选取默认路径
    QString filters = tr("IGES files(*.igs)");
    fd->setFilter(QDir::Files | QDir::NoSymLinks);//设置类型过滤器，只为文件格式
    fd->setNameFilter(filters); //设置过滤条件 
    if(fd->exec() == QDialog::Accepted)
    {
        pOperate->destroyWorkPiece();
        select_filepath = fd->selectedFiles()[0];
        int size = fd->directory().path().size()+1;
        int path_size = select_filepath.size();
        fileName = select_filepath.right(path_size-size).toUpper();
        QString tmp = fileName.split(".").at(0);
        picPath += tmp;
        makeDir(picPath);
        //显示折弯仿真区域
        bend_surface *pbend = pOperate->getWorkPieceModel(select_filepath,false);
        if(character_flag == true)
        {
             character_flag = false;
            return;
        }
        bend_order *porder = pOperate->getBendOrder(pbend);
        pBendGL->setShowModel(pOperate);
        //显示展开图
        bend_surface *pdeploy = pOperate->getWorkPieceModel(select_filepath,true);
        pdeploy = pBendGL->adjustDeployWorkpiece(pOperate);
        float maxlength = pOperate->findBsicSurfaceMaxLength();
        pDeploy->getMaxLengthOfBasicsurface(maxlength);
        pDeploy->setShowData(pdeploy,porder);
        updateShowRect();
        //首先将树形图清空
        pTree->clear();
        //构建树形图数据
        QString name = tr("工件 ")+fileName;
        pRoot_part = new QTreeWidgetItem(pTree,QStringList(name));
        QVariant var1(0);
        pRoot_part->setData(0,Qt::UserRole,var1);
        float thickness = pOperate->getThickness();
        QString text = tr("厚度=%1").arg(thickness,0,'f',2);
        QTreeWidgetItem *item1 = new QTreeWidgetItem(pRoot_part,QStringList(text));
        pRoot_part->addChild(item1);
        pRoot_machine = new QTreeWidgetItem(pTree,QStringList(QString("机床")));
        QVariant var2(1);
        pRoot_machine->setData(0,Qt::UserRole,var2);
        pRoot_bend = new QTreeWidgetItem(pTree,QStringList(QString("折弯")));
        QVariant var3(2);
        pRoot_bend->setData(0,Qt::UserRole,var3);
        bendNum = pOperate->getBendNum();
        for(int i = 0; i < bendNum; i++)
        {
            float angle = fabs(pbend->bendAngle)/PI*180;
            float radius = pbend->bendRadius;
            float width = pbend->bendWidth;
            QTreeWidgetItem *item = new QTreeWidgetItem(pRoot_bend,\
                    QStringList(QString("BEND_%1 [A=%2,R=%3,W=%4]")\
                    .arg(i+1).arg(angle,0,'f',2).arg(radius,0,'f',2).arg(width,0,'f',2)));
            QVariant var(3);    //折弯段的明细标识
            item->setData(0,Qt::UserRole,var);
            pRoot_bend->addChild(item);
            pbend = pbend->pNext;
        }

        pTree->expandAll();
        pRoot_bend->child(curNum-1)->setSelected(true);
        pRoot_bend->child(curNum-1)->setTextColor(0,select_color);
        pTool_btn->show();
    }
    delete fd;
    ui_MainWindow->Auto->setEnabled(true);
    ui_MainWindow->ToolConfig->setEnabled(true);
    ui_MainWindow->body->setEnabled(true);
    ui_MainWindow->Transfer->setEnabled(true);
    ui_MainWindow->before_btn->setEnabled(true);
    ui_MainWindow->next_btn->setEnabled(true);
    ui_MainWindow->Bend->setEnabled(true);
    ui_MainWindow->Deploy->setEnabled(true);
}

/********************************************
 *function:点击退出的槽函数
 *adding:
 *author: xu
 *date: 2015/9/18
 *******************************************/
void MainWindow::on_Quit_triggered()
{
    qApp->closeAllWindows();
}

/********************************************
 *function:点击导入上模的槽函数
 *adding:
 *author: xu
 *date: 2015/10/11
 *******************************************/
void MainWindow::on_Punch_triggered()
{
//    if(pBendGL == NULL)
//        pBendGL = new bendGLWidget(pGLWidget);
//    QString punchPath;
//    punchPath = machinePath+QString("punch_600.igs");
//    pBendGL->readOriginFile(punchPath,PUNCH);
//    pBendGL->show();
////    bShow_punch = true;
////    QIcon icon;
////    icon.addFile(QStringLiteral(":/images/check"), QSize(), QIcon::Normal, QIcon::Off);
////    ui_MainWindow->Punch_Show->setIcon(icon);
////    pBendGL->updateVisibleState(bShow_body,bShow_punch,bShow_die,bShow_gaung);
////    if(pModel == NULL)
////        pModel = new bendGLWidget(pModelArea);
////    pModel->readOriginFile(punchPath,PUNCH);
////    pModel->show();
//    updateShowRect();
}

/********************************************
 *function:点击导入下模的槽函数
 *adding:
 *author: xu
 *date: 2015/10/11
 *******************************************/
void MainWindow::on_Die_triggered()
{
//    QString backgaungPath = machinePath + "backgaung_r.igs";
//    if(x1_enable)
//    {
//        pOperate->getFileModel(backgaungPath,BACK_GAUNG_RL);
//    }
//    point3f off;
//    off.x = 0.0;
//    off.y = 500;
//    off.z = 0.0;
//    pOperate->initToolPos(BACK_GAUNG_RL,off);
//    pBendGL->setShowModel(pOperate);
//    bShow_body = true;
//    bShow_x = true;
//    bShow_r = true;
//    bShow_z = true;
//    pOperate->updateVisibleState(bShow_body,bShow_punch,bShow_die,bShow_x,bShow_r,bShow_z);
}

void MainWindow::on_Bend_triggered()
{
//    if(pBendGL)
//        pBendGL->drawBendAction();
}

void MainWindow::on_Deploy_triggered()
{
//    if(pBendGL)
//        pBendGL->showOriginPiece();
}

/********************************************
 *function:打印生成的折弯工序
 *adding:
 *author: xu
 *date: 2016/01/07
 *******************************************/
void MainWindow::on_Print_triggered()
{
//    if(pBendGL)
//        pBendGL->print();
    QPrinter printer(QPrinter::HighResolution);
    printer.setPageSize(QPrinter::A4);
    printer.setOutputFormat(QPrinter::PdfFormat);
    printer.setOutputFileName(QString("%1/%2.pdf").arg(picPath).arg(fileName));
    //获取界面的图片

    //QPixmap pixmap = QPixmap::grabWindow(pGLWidget,pGLWidget->rect());
    //QPixmap pixmap = ui_MainWindow->widget->grab(ui_MainWindow->widget->rect());
    //QPixmap pixmap = pBendGL->grab(pBendGL->rect());

    QPixmap *pixmap = drawSummaryPage();
    QPainter painter;
    painter.begin(&printer);

    QRect rect = painter.viewport();
    int multiple = rect.width()/pixmap->width();
    //将图像在PDF上放大multiple-1倍
    painter.scale(multiple-1,multiple-1);
    painter.drawPixmap(0,0,*pixmap);//画图

    //绘制板料展开图形
    printer.newPage();
    QPixmap pixmap_model = pPartArea->grab(pPartArea->rect());
    painter.drawPixmap(PAGE_X_MARGIN,PAGE_Y_MARGIN,PAGE_WIDTH,PAGE_HEIGHT/6,pixmap_model);

    int pageNum;
    if(bendNum%4 == 0)
        pageNum = bendNum/4;
    else
        pageNum = bendNum/4 + 1;
    for(int i = 0; i < pageNum; i++)
    {
        printer.newPage();
        QPen pen(Qt::black,1,Qt::SolidLine);
        painter.setPen(pen);
        painter.setFont(QFont("微软雅黑",8));
        //painter.drawText(QRectF(10,10,200,20),tr("折弯——%1").arg(1));
        int count = 4;
        if(i == pageNum-1)
            count = bendNum-4*i;
        for(int j = 0; j < count; j++)
        {
            float image_width = (PAGE_WIDTH-3*PAGE_X_MARGIN);
            float image_height = (PAGE_HEIGHT-5*PAGE_X_MARGIN)/4.0;
            painter.drawLine(QPointF(PAGE_X_MARGIN,PAGE_Y_MARGIN/2+j*(image_height+PAGE_Y_MARGIN)),\
                QPointF(PAGE_WIDTH+PAGE_X_MARGIN,PAGE_Y_MARGIN/2+j*(image_height+PAGE_Y_MARGIN)));
//            painter.drawText(QRectF(PAGE_X_MARGIN,j*(image_height+PAGE_Y_MARGIN),200,20),\
//                             tr("折弯——%1").arg(j+1));
            QImage image1(QString("%1/%2.bmp").arg(picPath).arg(j+1+i*4));
            painter.drawImage(QRectF(PAGE_X_MARGIN+25,PAGE_Y_MARGIN+\
                j*(image_height+PAGE_Y_MARGIN),image_width,image_height),image1);
//            QImage image2(QString("D:/%1_2.bmp").arg(j+1));
//            painter.drawImage(QRectF(PAGE_X_MARGIN*2+image_width,PAGE_Y_MARGIN+\
//                j*(image_height+PAGE_Y_MARGIN),image_width,image_height),image2);
        }
    }


    //painter.drawPixmap(0,0,*pixmap);
    painter.end();
    delete pixmap;
    pixmap = NULL;

//    painter.begin(&printer);
//    rect = painter.viewport();
//    multiple = rect.width()/pixmap->width();
//    //将图像在PDF上放大multiple-1倍
//    painter.scale(multiple,multiple);
//    pixmap = drawPix();
//    painter.drawPixmap(0,0,*pixmap);//画图
//    painter.end();
//    delete pixmap;
//    pixmap = NULL;
     //pBendGL->print();
}

/********************************************
 *function:绘制打印内容
 *adding:
 *author: xu
 *date: 2016/02/28
 *******************************************/
QPixmap *MainWindow::drawSummaryPage()
{
    QPixmap *pixMap = new QPixmap(PAGE_WIDTH,PAGE_HEIGHT);
    pixMap->fill(Qt::white);
    QPainter painter;
    painter.begin(pixMap);
    //************************绘制标题栏*************************************************************/
    QPen pen(Qt::black,1,Qt::SolidLine);
    painter.setPen(pen);
    //标题栏的边框
    painter.drawLine(QPointF(PAGE_X_MARGIN,PAGE_Y_MARGIN),QPointF(PAGE_WIDTH-PAGE_X_MARGIN,PAGE_Y_MARGIN));
    painter.drawLine(QPointF(PAGE_X_MARGIN,PAGE_Y_MARGIN),QPointF(PAGE_X_MARGIN,PAGE_HEIGHT/4));
    painter.drawLine(QPointF(PAGE_WIDTH-PAGE_X_MARGIN,PAGE_Y_MARGIN),QPointF(PAGE_WIDTH-PAGE_X_MARGIN,PAGE_HEIGHT/4));
    painter.drawLine(QPointF(PAGE_X_MARGIN,PAGE_HEIGHT/4),QPointF(PAGE_WIDTH-PAGE_X_MARGIN,PAGE_HEIGHT/4));
    //标题栏显示的内容
    painter.setFont(QFont("微软雅黑",10));
    painter.drawText(QRect(200,20,200,30),tr("MiconBendCNC"));
    painter.setFont(QFont("微软雅黑",8));
    painter.drawText(QRect(PAGE_X_MARGIN*2,50,100,20),tr("程序号:"));
    painter.drawText(QRect(PAGE_X_MARGIN*2+50,50,100,20),fileName);
    painter.drawText(QRect(250,50,200,20),tr("材质:"));
    painter.drawText(QRect(PAGE_X_MARGIN*2,85,100,20),tr("程序名称:"));
    painter.drawText(QRect(PAGE_X_MARGIN*2+50,85,100,20),fileName);
    painter.drawText(QRect(250,85,200,20),tr("材料等级:"));
    painter.drawText(QRect(PAGE_X_MARGIN*2,120,200,20),tr("图形名称:"));
    painter.drawText(QRect(250,120,200,20),tr("厚度:"));
    painter.drawText(QRect(300,120,200,20),tr("%1 mm").arg(pOperate->getThickness()));
    painter.drawText(QRect(PAGE_X_MARGIN*2,155,200,20),tr("代码文件名称:"));
    painter.drawText(QRect(250,155,100,20),tr("日期:"));
    QDateTime time = QDateTime::currentDateTime();
    QString date = time.toString("yyyy-MM-dd");
    painter.drawText(QRect(300,155,100,20),date);
    painter.drawText(QRect(PAGE_X_MARGIN*2,190,100,20),tr("机床:"));
    if(pRoot_machine->childCount() != 0)
    {
        QString machineName = pRoot_machine->child(0)->text(0);
        painter.drawText(QRect(PAGE_X_MARGIN*2+50,190,100,20),machineName);
    }
    painter.drawText(QRect(250,190,200,20),tr("程序员:"));
    painter.drawText(QRect(300,190,200,20),tr("admin"));

    //模具栏
    painter.drawLine(QPointF(PAGE_X_MARGIN,PAGE_HEIGHT/4+30),QPointF(PAGE_WIDTH-PAGE_X_MARGIN,PAGE_HEIGHT/4+30));
    QPixmap pixmap_model = pMachineArea->grab(pMachineArea->rect());
    painter.drawPixmap(PAGE_X_MARGIN,PAGE_HEIGHT/4+40,PAGE_WIDTH-2*PAGE_X_MARGIN,150,pixmap_model);
    painter.drawText(QRect(PAGE_X_MARGIN,420,110,30),tr("上模号"));
    painter.drawText(QRect(120,420,110,30),tr("起点位置"));
    painter.drawText(QRect(230,420,110,30),tr("长度"));
    painter.drawText(QRect(310,420,110,30),tr("下模号"));
    painter.drawText(QRect(420,420,110,30),tr("起点位置"));
    painter.drawText(QRect(530,420,110,30),tr("长度"));

    int size = toolPunch.size();
    for(int i = 0; i < size; i++)
    {
        painter.drawText(QRect(PAGE_X_MARGIN,440+20*i,120,30),tr("%1").arg(toolPunch[i]));
        painter.drawText(QRect(120,440+20*i,110,30),tr("%1").arg(toolPosition[i]));
        painter.drawText(QRect(230,440+20*i,110,30),tr("%1").arg(toolWidth[i]));

        painter.drawText(QRect(310,440+20*i,110,30),tr("%1").arg(toolDie[i]));
        painter.drawText(QRect(420,440+20*i,110,30),tr("%1").arg(toolPosition[i]));
        painter.drawText(QRect(530,440+20*i,110,30),tr("%1").arg(toolWidth[i]));
    }

    painter.drawLine(QPointF(PAGE_X_MARGIN,460+20*size),QPointF(PAGE_WIDTH-PAGE_X_MARGIN,460+20*size));
    //绘制表格的数据
    int bendnum = pOperate->getBendNum();
    painter.drawLine(QPointF(PAGE_X_MARGIN,600),QPointF(PAGE_WIDTH-PAGE_X_MARGIN,600));
    //painter.drawLine(QPointF(PAGE_X_MARGIN,600),QPointF(PAGE_X_MARGIN,600+20*(bendnum+1)));
    painter.drawLine(QPointF(PAGE_WIDTH-PAGE_X_MARGIN,600),QPointF(PAGE_WIDTH-PAGE_X_MARGIN,600+20*(bendnum+1)));
    painter.drawLine(QPointF(PAGE_X_MARGIN,600+20*(bendnum+1)),QPointF(PAGE_WIDTH-PAGE_X_MARGIN,600+20*(bendnum+1)));
    float margin = (PAGE_WIDTH-2*PAGE_X_MARGIN)/11.0;
    bend_order *ptmp_order = pOperate->getBendOrder();
    for(int i = 0; i < bendnum; i++)
    {
        float width = ptmp_order->bendWidth;
        int m = 0;
        for(m = 0; m < toolWidth.size(); m++)
        {
            if(fabs(toolWidth[m]-width) < toolMargin[m])
                break;
        }
        painter.drawLine(QPointF(PAGE_X_MARGIN,620+20*i),QPointF(PAGE_WIDTH-PAGE_X_MARGIN,620+20*i));
        painter.drawText(QRectF(PAGE_X_MARGIN+20,623+20*i,margin,20),tr("%1").arg(i+1));
        painter.drawText(QRectF(PAGE_X_MARGIN+20+margin,623+20*i,margin,20),tr("%1/%2").arg(toolPunch[m]).arg(toolDie[m]));
        painter.drawText(QRectF(PAGE_X_MARGIN+15+margin*2,623+20*i,margin,20),tr("%1").arg(fabs(ptmp_order->bendAngle)/PI*180.0,0,'f',2));
        painter.drawText(QRectF(PAGE_X_MARGIN+10+margin*3,623+20*i,margin,20),tr("%1").arg(ptmp_order->bendWidth,0,'f',2));
        painter.drawText(QRectF(PAGE_X_MARGIN+20+margin*4,623+20*i,margin,20),tr("%1").arg(ptmp_order->bendRadius,0,'f',2));
        painter.drawText(QRectF(PAGE_X_MARGIN+15+margin*5,623+20*i,margin,20),tr("%1").arg(ptmp_order->position_x1,0,'f',2));
        painter.drawText(QRectF(PAGE_X_MARGIN+15+margin*6,623+20*i,margin,20),tr("%1").arg(ptmp_order->position_x2,0,'f',2));
        painter.drawText(QRectF(PAGE_X_MARGIN+15+margin*7,623+20*i,margin,20),tr("%1").arg(ptmp_order->position_r1,0,'f',2));
        painter.drawText(QRectF(PAGE_X_MARGIN+15+margin*8,623+20*i,margin,20),tr("%1").arg(ptmp_order->position_r2,0,'f',2));
        painter.drawText(QRectF(PAGE_X_MARGIN+5+margin*9,623+20*i,margin,20),tr("%1").arg(ptmp_order->position_z1,0,'f',2));
        painter.drawText(QRectF(PAGE_X_MARGIN+5+margin*10,623+20*i,margin,20),tr("%1").arg(ptmp_order->position_z2,0,'f',2));
        ptmp_order = ptmp_order->pNext;
    }
    for(int i = 0; i < 11; i++)
    {
        painter.drawLine(QPointF(PAGE_X_MARGIN+i*margin,600),QPointF(PAGE_X_MARGIN+i*margin,600+20*(bendNum+1)));
    }

    painter.drawText(QRectF(PAGE_X_MARGIN+20,603,margin,20),tr("折弯"));
    painter.drawText(QRectF(PAGE_X_MARGIN+margin+20,603,margin,20),tr("模具"));
    painter.drawText(QRectF(PAGE_X_MARGIN+margin*2+20,603,margin,20),tr("角度"));
    painter.drawText(QRectF(PAGE_X_MARGIN+margin*3+20,603,margin,20),tr("长度"));
    painter.drawText(QRectF(PAGE_X_MARGIN+margin*4+20,603,margin,20),tr("半径"));
    painter.drawText(QRectF(PAGE_X_MARGIN+margin*5+20,603,margin,20),tr("X1"));
    painter.drawText(QRectF(PAGE_X_MARGIN+margin*6+20,603,margin,20),tr("X2"));
    painter.drawText(QRectF(PAGE_X_MARGIN+margin*7+20,603,margin,20),tr("R1"));
    painter.drawText(QRectF(PAGE_X_MARGIN+margin*8+20,603,margin,20),tr("R2"));
    painter.drawText(QRectF(PAGE_X_MARGIN+margin*9+20,603,margin,20),tr("Z1"));
    painter.drawText(QRectF(PAGE_X_MARGIN+margin*10+20,603,margin,20),tr("Z2"));
    //pOperate->drawDataTable(&painter);

    painter.end();
    return pixMap;
}

/********************************************
 *function:实时更新位置和滑块数据
 *adding:
 *author: xu
 *date: 2016/01/19
 *******************************************/
void MainWindow::updatePosData()
{
    if(pYAxis->isChecked())
    {
        bend_order *pcur = pOperate->getCurOrder();
        if(pcur == NULL)
            return;
        float curpos = pcur->y_open;
        float max = pcur->position_y1;
        float min = 0.00;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        int value = (curpos-min)*100/(max-min);
        pSlider->setValue(value);
    }
    else if(pX1Axis->isChecked())
    {
        float curpos = pOperate->X1_Pos;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        int value = (curpos-x_min)*100/(x_max-x_min);
        pSlider->setValue(value);
    }
    else if(pX2Axis->isChecked())
    {
        float curpos = pOperate->X2_Pos;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        int value = (curpos-x_min)*100/(x_max-x_min);
        pSlider->setValue(value);
    }
    else if(pX1Axis_Back->isChecked())
    {
        bend_order *pcur = pOperate->getCurOrder();
        if(pcur == NULL)
            return;
        float curpos = pcur->dX1Retract;
        float cur_x_pos = pOperate->X1_Pos;
        float retract_max = x_max-cur_x_pos;
        float retract_min = 0.00;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        int value = (curpos-retract_min)*100/(retract_max-retract_min);
        pSlider->setValue(value);
    }
    else if(pX2Axis_Back->isChecked())
    {
        bend_order *pcur = pOperate->getCurOrder();
        if(pcur == NULL)
            return;
        float curpos = pcur->dX2Retract;
        float cur_x_pos = pOperate->X2_Pos;
        float retract_max = x_max-cur_x_pos;
        float retract_min = 0.00;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        int value = (curpos-retract_min)*100/(retract_max-retract_min);
        pSlider->setValue(value);
    }
    else if(pR1Axis->isChecked())
    {
        float curpos = pOperate->R1_Pos;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        int value = (curpos-r_min)*100/(r_max-r_min);
        pSlider->setValue(value);
    }
    else if(pR2Axis->isChecked())
    {
        float curpos = pOperate->R2_Pos;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        int value = (curpos-r_min)*100/(r_max-r_min);
        pSlider->setValue(value);
    }
    else if(pZ1Axis->isChecked())
    {
        float curpos = pOperate->Z1_Pos;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        int value = (curpos-z_min)*100/(z_max-z_min);
        pSlider->setValue(value);
    }
    else if(pZ2Axis->isChecked())
    {
        float curpos = pOperate->Z2_Pos;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        int value = (curpos-z_min)*100/(z_max-z_min);
        pSlider->setValue(value);
    }
}

void MainWindow::updateTreeData()
{
    pRoot_bend->child(curNum-1)->setSelected(false);
    pRoot_bend->child(curNum-1)->setTextColor(0,normal_color);
    bend_order *porder = pOperate->getBendOrder();
    int count = 0;
    while (porder) {
        float angle = fabs(porder->bendAngle)/PI*180;
        float radius = porder->bendRadius;
        float width = porder->bendWidth;
        int tool_id = porder->tool_id;
        QString text = tr("BEND_%1 [A=%2,R=%3,W=%4 刀具组=%5]").arg(count+1).\
            arg(angle,0,'f',2).arg(radius,0,'f',2).arg(width,0,'f',2).arg(tool_id);
        pRoot_bend->child(count)->setText(0,text);
        count++;
        porder = porder->pNext;
    }
    curNum = 1;
    pRoot_bend->child(curNum-1)->setSelected(true);
    pRoot_bend->child(curNum-1)->setTextColor(0,select_color);
}

void MainWindow::updateSelectState()
{
    pRoot_bend->child(curNum-1)->setSelected(false);
    pRoot_bend->child(curNum-1)->setTextColor(0,normal_color);
    curNum = pOperate->getCurOrder()->bend_id;
    pRoot_bend->child(curNum-1)->setSelected(true);
    pRoot_bend->child(curNum-1)->setTextColor(0,select_color);
}

void MainWindow::initPopMenuContent()
{
    pAction_Up = new QAction(tr("上移"),this);
    pAction_Down = new QAction(tr("下移"),this);
    pAction_Delete = new QAction(tr("删除折弯"),this);

    connect(pAction_Up,&QAction::triggered,this,&MainWindow::action_up_slot);
    connect(pAction_Down,&QAction::triggered,this,&MainWindow::action_down_slot);
    connect(pAction_Delete,&QAction::triggered,this,&MainWindow::action_delete_slot);
}

void MainWindow::initMenuState()
{
    //菜单栏初始化状态
    ui_MainWindow->Auto->setEnabled(false);
    ui_MainWindow->bend_order->setEnabled(false);
    ui_MainWindow->ToolConfig->setEnabled(false);
    ui_MainWindow->Transfer->setEnabled(false);
    ui_MainWindow->Print->setEnabled(false);
    ui_MainWindow->body->setEnabled(false);
    ui_MainWindow->before_btn->setEnabled(false);
    ui_MainWindow->next_btn->setEnabled(false);
    ui_MainWindow->Bend->setEnabled(false);
    ui_MainWindow->Deploy->setEnabled(false);
    ui_MainWindow->bend_order->setEnabled(false);
    pButton1->hide();
    pButton2->hide();
}

void MainWindow::updateRadioButtonState()
{
    QString strSQL = "select x1_enable,x2_enable,r1_enable,r2_enable,\
            z1_enable,z2_enable,type from AxisSet where id = 1";
    QStringList strList = ReadSqlData(dbName,strSQL);
    if(strList.isEmpty())
        return;
    x1_enable = strList.at(0).toInt();
    x2_enable = strList.at(1).toInt();
    r1_enable = strList.at(2).toInt();
    r2_enable = strList.at(3).toInt();
    z1_enable = strList.at(4).toInt();
    z2_enable = strList.at(5).toInt();
    pX1Axis->setEnabled(x1_enable);
    pX1Axis_Back->setEnabled(x1_enable);
    pX2Axis->setEnabled(x2_enable);
    pX2Axis_Back->setEnabled(x2_enable);
    pR1Axis->setEnabled(r1_enable);
    pR2Axis->setEnabled(r2_enable);
    pZ1Axis->setEnabled(z1_enable);
    pZ2Axis->setEnabled(z2_enable);
}

/********************************************
 *function:折弯操作
 *adding:
 *author: xu
 *date: 2016/01/19
 *******************************************/
void MainWindow::bendAction()
{
    QVector<int> res = pOperate->drawBendAction();
    InterfereInfo(res);
    updatePosData();
    updateSelectState();
    bend_order *pCur = pOperate->getCurOrder();
    if(pOperate->bendFlag == false)
    {
        pBendGL->setPrintMode(true,pCur->bend_id);
    }
    else
    {
        pBendGL->setPrintMode(false,pCur->bend_id);
    }
    pBendGL->updateGL();
    pInterText->setVisible(pInterfere->checkState());
}

void MainWindow::InterfereInfo(QVector<int> res)
{
    QString showText;
    int size = res.size();
    if(size == 0)   //没有干涉
    {
        showText = tr("当前工件无干涉！");
        pInterText->setStyleSheet("color:black");
    }
    else
    {
        if(res[0] == -1)//折弯到头的标志
            return;
        showText = tr("当前工件与");
        for(int i = 0; i < size; i++)
        {
            if(res[i] == 1)
                showText += tr(" 上模 ");
            else if(res[i] == 2)
                showText += tr(" 下模 ");
            else if(res[i] == 3)
                showText += tr(" 上部外形 ");
            else if(res[i] == 4)
                showText += tr(" 下部外形 ");
        }
        showText += tr("干涉！");
        pInterText->setStyleSheet("color:red");
    }
    pInterText->setText(showText);
}

void MainWindow::makeDir(QString path)
{
    QDir *temp = new QDir;
    bool exist = temp->exists(path);
    if(exist)
        return;
    temp->mkdir(path);
    delete temp;
}

void MainWindow::on_MB8_100X4200_triggered()
{
    QString maPath,upperPath,lowerPath,backgaungPath;
    maPath = machinePath + "MB8-100X4200.igs";
    upperPath = machinePath + "upper.igs";
    lowerPath = machinePath + "lower.igs";
    pOperate->getFileModel(maPath,BODY);
    pOperate->getFileModel(upperPath,UPPER);
    pOperate->getFileModel(lowerPath,LOWER);
    backgaungPath = machinePath + "backgaung_r.igs";
    if(x1_enable)
    {
        pOperate->getFileModel(backgaungPath,BACK_GAUNG_RL);
    }
    if(x2_enable)
    {
        pOperate->getFileModel(backgaungPath,BACK_GAUNG_RR);
    }
    backgaungPath = machinePath + "backgaung_x.igs";
    if(r1_enable)
    {
        pOperate->getFileModel(backgaungPath,BACK_GAUNG_XL);
    }
    if(r2_enable)
    {
        pOperate->getFileModel(backgaungPath,BACK_GAUNG_XR);
    }
    backgaungPath = machinePath + "backgaung_z.igs";
    float z1_off = 0.00;
    float z2_off = 0.00;
    if(z1_enable || z2_enable)
    {
        if(z1_enable && z2_enable)
        {
            pOperate->getFileModel(backgaungPath,BACK_GAUNG_ZL);
            pOperate->getFileModel(backgaungPath,BACK_GAUNG_ZR);
            z1_off = -100.00;
            z2_off = 100.00;
        }
        else if(z1_enable)
        {
            pOperate->getFileModel(backgaungPath,BACK_GAUNG_ZL);
            z1_off = 0.00;
        }
        else if(z2_enable)
        {
            pOperate->getFileModel(backgaungPath,BACK_GAUNG_ZR);
            z2_off = 0.00;
        }
    }
    point3f off;
    off.x = z1_off;
    off.y = 0.0;
    off.z = 0.0;
    pOperate->initToolPos(BACK_GAUNG_XL,off);
    pOperate->initToolPos(BACK_GAUNG_ZL,off);
    off.y = dieHeight;
    pOperate->initToolPos(BACK_GAUNG_RL,off);
    off.x = z2_off;
    pOperate->initToolPos(BACK_GAUNG_RR,off);
    off.y = 0.0;
    pOperate->initToolPos(BACK_GAUNG_XR,off);
    pOperate->initToolPos(BACK_GAUNG_ZR,off);
    bShow_body = true;
    bShow_x = true;
    bShow_r = true;
    bShow_z = true;
    QIcon icon;
    icon.addFile(QStringLiteral(":/images/check"), QSize(), QIcon::Normal, QIcon::Off);
    ui_MainWindow->Body_Show->setIcon(icon);
    ui_MainWindow->X_Show->setIcon(icon);
    ui_MainWindow->R_Show->setIcon(icon);
    ui_MainWindow->Z_Show->setIcon(icon);
    pOperate->updateVisibleState(bShow_body,bShow_punch,bShow_die,bShow_x,bShow_r,bShow_z);
    QTreeWidgetItem *item = new QTreeWidgetItem(pRoot_machine,QStringList("MB8-100X4200"));
    pRoot_machine->addChild(item);
    pTree->expandAll();
    updateShowRect();
    //一旦将模具或机床等环境导入后将不允许重新导入工件数据,只能将当前窗口关闭,重新导入,这样可以很好的保护数据
    ui_MainWindow->Open->setEnabled(false);
}

void MainWindow::on_Auto_triggered()
{
    int res = pOperate->bendorderResult(this);
    pOperate->formBendorderFile();
    if(res == -2)
    {
        QMessageBox::warning(this,tr("程序提示"),\
        tr("当前工作环境未导入,请首先导入工作环境"),QMessageBox::Yes);
    }
    else if(res == -1)
    {
        QMessageBox::information(this,tr("程序提示"),tr("未找到合适的折弯工序"),\
            QMessageBox::Yes|QMessageBox::No);
    }
    updateTreeData();
    pBendGL->setPrintMode(true,pOperate->getCurOrder()->bend_id);
    pDeploy->updateBendOrder(pOperate->getBendOrder());
    pBendGL->setShowModel(pOperate);
    updateShowRect();
    ui_MainWindow->Print->setEnabled(true);
}

/********************************************
 *function:放大图形
 *adding:
 *author: xu
 *date: 2016/01/19
 *******************************************/
void MainWindow::on_enLarge_triggered()
{

}

/********************************************
 *function:缩小图形
 *adding:
 *author: xu
 *date: 2016/01/19
 *******************************************/
void MainWindow::on_shrink_triggered()
{

}

/********************************************
 *function:图形恢复到原始大小
 *adding:
 *author: xu
 *date: 2016/01/19
 *******************************************/
void MainWindow::on_reset_triggered()
{
    pBendGL->resetGraph();
}


void MainWindow::on_Return_triggered()
{
    bend_order *pcur = pOperate->getCurOrder();
    pOperate->turnAboutPiece(pcur);
    updatePosData();
    pBendGL->updateGL();
}

/********************************************
 *function:时间到达槽函数
 *adding:
 *author: xu
 *date: 2016/01/20
 *******************************************/
void MainWindow::timeOutSlot()
{
    pTimer->stop();
    QDateTime time = QDateTime::currentDateTime();
    QString strTime = time.toString("yyyy-MM-dd hh:mm:ss ddd");
    timeLabel->setText(strTime);
    pTimer->start(1000);
}

void MainWindow::on_NC_triggered()
{
    QString path = QFileDialog::getSaveFileName(this,tr("生成NC程序"),savePath,tr("NC Files (*.data)"));
    if(!path.isNull())
    {
        pOperate->formBendFile(path);
    }
}

void MainWindow::on_Link_triggered()
{
    pNetWork = new NetWork(this);
    pNetWork->netConnection();
}

void MainWindow::on_Transfer_triggered()
{
    if(pNetWork)
    {
        //使用打开文件标准对话框
        QFileDialog *fd = new QFileDialog(this);
        fd->setWindowTitle(tr("选择需要发送的折弯程序"));
        fd->setDirectory(savePath); //选取默认路径
        QString filters = tr("Bend files(*.data)");
        fd->setFilter(QDir::Files | QDir::NoSymLinks);//设置类型过滤器，只为文件格式
        fd->setNameFilter(filters); //设置过滤条件
        if(fd->exec() == QDialog::Accepted)
        {
            QString path = fd->selectedFiles()[0];
//            int size = fd->directory().path().size()+1;
//            int path_size = path.size();
//            QString name = path.right(path_size-size).toUpper();
//            int id = name.split(".").at(0).toInt();
            pNetWork->sendBendFile(path);
            pNetWork->sendBendPic(picPath);
        }
        delete fd;
    }
    else
    {
        QMessageBox::information(this,tr("程序提示"),tr("当前未连接服务器"),\
            QMessageBox::Yes|QMessageBox::No);
    }
}

/********************************************
 *function:开始进行实时状态仿真
 *adding:
 *author: xu
 *date: 2016/03/11
 *******************************************/
void MainWindow::on_RealTime_triggered()
{
    if(pNetWork)
    {
        pRealTimePage = new uiRealTimePage(this);
        pRealTimePage->grabKeyboard();
        pRealTimePage->setShowContent();
        pRealTimePage->exec();
        delete pRealTimePage;
    }
    else
    {
        QMessageBox::information(this,tr("程序提示"),tr("当前未连接服务器"),\
            QMessageBox::Yes|QMessageBox::No);
    }
}

void MainWindow::on_Body_Show_triggered()
{
    QIcon icon;
    if(bShow_body)
    {
        bShow_body = false;
        ui_MainWindow->Body_Show->setIcon(icon);
    }
    else
    {
        bShow_body = true;
        icon.addFile(QStringLiteral(":/images/check"), QSize(), QIcon::Normal, QIcon::Off);
        ui_MainWindow->Body_Show->setIcon(icon);
    }
    pOperate->updateVisibleState(bShow_body,bShow_punch,bShow_die,bShow_x,bShow_r,bShow_z);
    updateShowRect();
}

void MainWindow::on_Punch_Show_triggered()
{
    QIcon icon;
    if(bShow_punch)
    {
        bShow_punch = false;
        ui_MainWindow->Punch_Show->setIcon(icon);
    }
    else
    {
        bShow_punch = true;
        icon.addFile(QStringLiteral(":/images/check"), QSize(), QIcon::Normal, QIcon::Off);
        ui_MainWindow->Punch_Show->setIcon(icon);
    }
    pOperate->updateVisibleState(bShow_body,bShow_punch,bShow_die,bShow_x,bShow_r,bShow_z);
    updateShowRect();
}

void MainWindow::on_Die_Show_triggered()
{
    QIcon icon;
    if(bShow_die)
    {
        bShow_die = false;
        ui_MainWindow->Die_Show->setIcon(icon);
    }
    else
    {
        bShow_die = true;
        icon.addFile(QStringLiteral(":/images/check"), QSize(), QIcon::Normal, QIcon::Off);
        ui_MainWindow->Die_Show->setIcon(icon);
    }
    pOperate->updateVisibleState(bShow_body,bShow_punch,bShow_die,bShow_x,bShow_r,bShow_z);
    updateShowRect();
}

/********************************************
 *function:模具配置槽函数
 *adding:
 *author: xu
 *date: 2016/04/13
 *******************************************/
void MainWindow::on_ToolConfig_triggered()
{
    pConfigDlg = new ToolConfigDialog(this);
    int res = pConfigDlg->exec();
    if(res == QDialog::Accepted)
    {
        pTools = new DrawTools(pMachineArea);
        pTools->show();
        pOperate->clearToolData();
        pOperate->resetToDeploy();
        //显示上下模图形
        QString punchPath,diePath;
        QVector<trimmed_surface_type *> punch,die;
        for(int i = 0; i < toolWidth.size(); i++)
        {
            punchPath = machinePath+QString("punch_%1.igs").arg(toolWidth[i]);
            diePath = machinePath+QString("die_%1.igs").arg(toolWidth[i]);
            punch.push_back(pOperate->getFileModel(punchPath,PUNCH));
            die.push_back(pOperate->getFileModel(diePath,DIE));
        }
        point3f off;
        off.x = 0.0;
        off.y = 0.0;
        off.z = 0.0;
        pOperate->initToolPos(PUNCH,off); //根据输入的位置调整模具的位置
        pOperate->bendorder_toolconfig();//根据模具的选择,默认一种模具选配方式
        pOperate->initWorkSpacePos();
//        for(int i = 0; i < toolWidth.size(); i++)
//        {
//            pBendGL->setShowModel(punch[i],PUNCH);
//            pBendGL->setMachineModel(die[i],DIE);
//        }
        pBendGL->setShowModel(pOperate);
        bShow_punch = true;
        bShow_die = true;
        QIcon icon1,icon2;
        icon1.addFile(QStringLiteral(":/images/check"), QSize(), QIcon::Normal, QIcon::Off);
        ui_MainWindow->Punch_Show->setIcon(icon1);
        icon2.addFile(QStringLiteral(":/images/check"), QSize(), QIcon::Normal, QIcon::Off);
        ui_MainWindow->Die_Show->setIcon(icon2);
        pOperate->updateVisibleState(bShow_body,bShow_punch,bShow_die,bShow_x,bShow_r,bShow_z);
//        punchPath = machinePath+QString("punch_1.igs");
//        diePath = machinePath+QString("die_1.igs");
//        if(pModel == NULL)
//            pModel = new bendGLWidget(pModelArea);
//        pModel->readOriginFile(punchPath,PUNCH);
//        pModel->readOriginFile(diePath,DIE);
//        pModel->updateVisibleState(bShow_body,bShow_punch,bShow_die,bShow_x,bShow_r,bShow_z);
//        pModel->show();
    }
    delete pConfigDlg;
    pConfigDlg = NULL; 
    pModelSelect->clear();
    for(int i = 0; i < toolWidth.size(); i++){
        pModelSelect->insertItem(i,QString("%1号模具组").arg(i+1));
    }
    //一旦将模具或机床等环境导入后将不允许重新导入工件数据,只能将当前窗口关闭,重新导入,这样可以很好的保护数据
    ui_MainWindow->Open->setEnabled(false);
    ui_MainWindow->bend_order->setEnabled(true);
    ui_MainWindow->splitter_2->refresh();
    updateShowRect();
}

void MainWindow::on_X_Show_triggered()
{
    QIcon icon;
    if(bShow_x)
    {
        bShow_x = false;
        ui_MainWindow->X_Show->setIcon(icon);
    }
    else
    {
        bShow_x = true;
        icon.addFile(QStringLiteral(":/images/check"), QSize(), QIcon::Normal, QIcon::Off);
        ui_MainWindow->X_Show->setIcon(icon);
    }
    pOperate->updateVisibleState(bShow_body,bShow_punch,bShow_die,bShow_x,bShow_r,bShow_z);
    updateShowRect();
}

void MainWindow::on_R_Show_triggered()
{
    QIcon icon;
    if(bShow_r)
    {
        bShow_r = false;
        ui_MainWindow->R_Show->setIcon(icon);
    }
    else
    {
        bShow_r = true;
        icon.addFile(QStringLiteral(":/images/check"), QSize(), QIcon::Normal, QIcon::Off);
        ui_MainWindow->R_Show->setIcon(icon);
    }
    pOperate->updateVisibleState(bShow_body,bShow_punch,bShow_die,bShow_x,bShow_r,bShow_z);
    updateShowRect();
}

void MainWindow::on_Z_Show_triggered()
{
    QIcon icon;
    if(bShow_z)
    {
        bShow_z = false;
        ui_MainWindow->Z_Show->setIcon(icon);
    }
    else
    {
        bShow_z = true;
        icon.addFile(QStringLiteral(":/images/check"), QSize(), QIcon::Normal, QIcon::Off);
        ui_MainWindow->Z_Show->setIcon(icon);
    }
    pOperate->updateVisibleState(bShow_body,bShow_punch,bShow_die,bShow_x,bShow_r,bShow_z);
    updateShowRect();
}

void MainWindow::on_y_btn_clicked(bool checked)
{
    if(checked) //改按钮被选中
    {
        bend_order *pcur= pOperate->getCurOrder();
        if(pcur == NULL)
            return;
        float curpos = pcur->y_open;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        float max = pcur->position_y1;
        float min = 0.00;
        if(curpos < min)
            curpos = min;
        if(curpos > max)
            curpos = max;
        int value = (curpos-min)*100/(max-min);
        pSlider->setValue(value);
    }
}

void MainWindow::on_x1_btn_clicked(bool checked)
{
    if(checked) //改按钮被选中
    {
        float curpos = pOperate->X1_Pos+pOperate->X1_Retract;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        if(curpos < x_min)
            curpos = x_min;
        if(curpos > x_max)
            curpos = x_max;
        int value = (curpos-x_min)*100/(x_max-x_min);
        pSlider->setValue(value);
    }
}

void MainWindow::on_x2_btn_clicked(bool checked)
{
    if(checked) //改按钮被选中
    {
        float curpos = pOperate->X2_Pos+pOperate->X2_Retract;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        if(curpos < x_min)
            curpos = x_min;
        if(curpos > x_max)
            curpos = x_max;
        int value = (curpos-x_min)*100/(x_max-x_min);
        pSlider->setValue(value);
    }
}

void MainWindow::on_x1_back_btn_clicked(bool checked)
{
    if(checked) //改按钮被选中
    {
        float curpos = pOperate->X1_Retract;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        float max = x_max-pOperate->X1_Pos;
        float min = 0.0;
        if(curpos < min)
            curpos = min;
        if(curpos > max)
            curpos = max;
        int value = (curpos-min)*100/(max-min);
        pSlider->setValue(value);
    }
}

void MainWindow::on_x2_back_btn_clicked(bool checked)
{
    if(checked) //改按钮被选中
    {
        float curpos = pOperate->X2_Retract;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        float max = x_max-pOperate->X2_Pos;
        float min = 0.0;
        if(curpos < min)
            curpos = min;
        if(curpos > max)
            curpos = max;
        int value = (curpos-min)*100/(max-min);
        pSlider->setValue(value);
    }
}

void MainWindow::on_r1_btn_clicked(bool checked)
{
    if(checked) //改按钮被选中
    {
        float curpos = pOperate->R1_Pos;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        if(curpos < r_min)
            curpos = r_min;
        if(curpos > r_max)
            curpos = r_max;
        int value = (curpos-r_min)*100/(r_max-r_min);
        pSlider->setValue(value);
    }
}

void MainWindow::on_r2_btn_clicked(bool checked)
{
    if(checked) //改按钮被选中
    {
        float curpos = pOperate->R2_Pos;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        if(curpos < r_min)
            curpos = r_min;
        if(curpos > r_max)
            curpos = r_max;
        int value = (curpos-r_min)*100/(r_max-r_min);
        pSlider->setValue(value);
    }
}

void MainWindow::on_z1_btn_clicked(bool checked)
{
    if(checked) //改按钮被选中
    {
        float curpos = pOperate->Z1_Pos;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        if(curpos < z_min)
            curpos = z_min;
        if(curpos > z_max)
            curpos = z_max;
        int value = (curpos-z_min)*100/(z_max-z_min);
        pSlider->setValue(value);
    }
}

void MainWindow::on_z2_btn_clicked(bool checked)
{
    if(checked) //改按钮被选中
    {
        float curpos = pOperate->Z2_Pos;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        if(curpos < z_min)
            curpos = z_min;
        if(curpos > z_max)
            curpos = z_max;
        int value = (curpos-z_min)*100/(z_max-z_min);
        pSlider->setValue(value);
    }
}

void MainWindow::on_inter_tag_clicked(bool checked)
{
    pInterText->setVisible(checked);
}

void MainWindow::on_slider_sliderMoved(int position)
{
    if(pYAxis->isChecked())
    {
        if(pOperate->getBendedState())   //处于折弯状态
        {
            bend_order *pcur = pOperate->getCurOrder();
            if(pcur == NULL)
                return;
            float max = pcur->position_y1;
            float min = 0.00;
            float curpos = (max-min)*position/100.0+min;
            pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
            pOperate->formAxisData(Y_AXIS,curpos);
        }
        else
        {
            QMessageBox::information(this,tr("程序提示"),tr("当前为展开状态,无法设置Y轴开口"),QMessageBox::Yes);
            return;
        }
    }
    else if(pX1Axis->isChecked())
    {
        float curpos = (x_max-x_min)*position/100.0+x_min;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        pOperate->formAxisData(X1_AXIS,curpos);
    }
    else if(pX1Axis_Back->isChecked())
    {
        if(pOperate->getBendedState())   //处于折弯状态
        {
            float x_pos = pOperate->X1_Pos;
            float max = x_max-x_pos;
            float min = 0.00;
            float curpos = (max-min)*position/100.0+min;
            pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
            pOperate->formAxisData(X1_AXIS_BACK,curpos);
        }
        else
        {
            QMessageBox::information(this,tr("程序提示"),tr("当前为展开状态,无法设置退让距离"),QMessageBox::Yes);
            return;
        }
    }
    else if(pX2Axis->isChecked())
    {
        float curpos = (x_max-x_min)*position/100.0+x_min;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        pOperate->formAxisData(X2_AXIS,curpos);
    }
    else if(pX2Axis_Back->isChecked())
    {
        if(pOperate->getBendedState())   //处于折弯状态
        {
            float x_pos = pOperate->X2_Pos;
            float max = x_max-x_pos;
            float min = 0.00;
            float curpos = (max-min)*position/100.0+min;
            pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
            pOperate->formAxisData(X2_AXIS_BACK,curpos);
        }
        else
        {
            QMessageBox::information(this,tr("程序提示"),tr("当前为展开状态,无法设置退让距离"),QMessageBox::Yes);
            return;
        }
    }
    else if(pR1Axis->isChecked())
    {
        float curpos = (r_max-r_min)*position/100.0+r_min;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        pOperate->formAxisData(R1_AXIS,curpos);
    }
    else if(pR2Axis->isChecked())
    {
        float curpos = (r_max-r_min)*position/100.0+r_min;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        pOperate->formAxisData(R2_AXIS,curpos);
    }
    else if(pZ1Axis->isChecked())
    {
        float curpos = (z_max-z_min)*position/100.0+z_min;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        pOperate->formAxisData(Z1_AXIS,curpos);
    }
    else if(pZ2Axis->isChecked())
    {
        float curpos = (z_max-z_min)*position/100.0+z_min;
        pCurPos->setText(QString("%1").arg(curpos,0,'f',2));
        pOperate->formAxisData(Z2_AXIS,curpos);
    }
    pBendGL->updateGL();
}

/********************************************
 *function:转到机床参数设置界面
 *adding:
 *author: xu
 *date: 2016/05/26
 *******************************************/
void MainWindow::on_Constant_triggered()
{
    pMachineConstant = new uiMachineConstant(this);
    pMachineConstant->exec();
    delete pMachineConstant;
    pMachineConstant = NULL;
}

void MainWindow::on_before_btn_clicked()
{
    pOperate->drawDeployAction();
    updatePosData();
    updateSelectState();
    pBendGL->updateGL();
}

void MainWindow::on_next_btn_clicked()
{
    bendAction();
}

void MainWindow::action_up_slot()
{
    //curNum = pRoot_bend->
}

void MainWindow::action_down_slot()
{

}

void MainWindow::action_delete_slot()
{

}

void MainWindow::on_splitter_4_splitterMoved(int pos, int index)
{
    pos = pos;
    index = index;
    updateShowRect();
}

void MainWindow::on_splitter_3_splitterMoved(int pos, int index)
{
    pos = pos;
    index = index;
    updateShowRect();
}

void MainWindow::on_splitter_splitterMoved(int pos, int index)
{
    pos = pos;
    index = index;
    updateShowRect();
}

void MainWindow::on_splitter_2_splitterMoved(int pos, int index)
{
    pos = pos;
    index = index;
    updateShowRect();
}

void MainWindow::on_treeWidget_customContextMenuRequested(const QPoint &pos)
{
    QTreeWidgetItem *item = ui_MainWindow->treeWidget->itemAt(pos);//获取当前被点击的节点
    if(item == NULL)//这种情况是右键的位置不在treeItem的范围内，即在空白位置右击
        return;
    QVariant var = item->data(0,Qt::UserRole);
    if(var == 3)
    {
        QMenu *popMenu = new QMenu(this);
        popMenu->addAction(ui_MainWindow->bend_order);
        popMenu->addAction(pAction_Delete);
        popMenu->addAction(ui_MainWindow->Bend);
        popMenu->addAction(ui_MainWindow->Deploy);
        popMenu->addAction(ui_MainWindow->ToolConfig);
        popMenu->exec(QCursor::pos());  //弹出右键菜单,菜单位置为光标位置
        delete popMenu;
    }
}

void MainWindow::on_BackGaung_triggered()
{
    pAxisConfig = new uiAxisConfigDialog(this);
    pAxisConfig->exec();
    delete pAxisConfig;
    pAxisConfig = NULL;
    updateRadioButtonState();
    updateShowRect();
}

void MainWindow::on_bend_order_triggered()
{
//    bend_surface *pdeploy = pOperate->getDeployHead();
//    bend_order *porder = pOperate->getBendOrder();
//    int bendnum = pOperate->getBendNum();
//    pOrderPage = new uiBendOrderPage(this,pdeploy,porder,bendnum);
//    int res = pOrderPage->exec();
//    if(res == QDialog::Accepted)
//    {
//        //首先将板料返回到原始状态
//        pOperate->showOriginPiece();
//        pOrderPage->copyData();
//        pDeploy->updateBendOrder(porder);
//        pOperate->initBendAxisData();
//        updateTreeData();
//    }
//    delete pOrderPage;

    //换用其他方法
    pInterText->setText("");
    pOperate->resetToBend();//完全折弯状态
    bManualState = true;
    //将当前的折弯工序直接显示在手动区域
    bend_order *pCur = pOperate->getBendOrder();
    int count = 1;
    while(pCur){
        ManualOrder.push_back(pCur->bendpoint);
        ManualTag.push_back(pCur->direction_h);
        ManualTool.push_back(pCur->tool_id);
        float angle = fabs(pCur->bendAngle)/PI*180;
        float radius = pCur->bendRadius;
        float width = pCur->bendWidth;
        int tool_id = pCur->tool_id;
        manual_bend_info = QString("%1 [A=%2  R=%3  W=%4  Model=%5]").arg(count).\
                arg(angle,0,'f',2).arg(radius,0,'f',2).arg(width,0,'f',2).arg(tool_id);
//        manual_bend_info = QString("%1 [A=%2  R=%3  W=%4]").arg(count).\
//                arg(angle,0,'f',2).arg(radius,0,'f',2).arg(width,0,'f',2);
        pManualList->addItem(manual_bend_info);
        pManualList->item(pManualList->count()-1)->setTextAlignment(Qt::AlignCenter);
//        for(int i = 0; i < pManualList->count()-1; i++){
//            pManualList->item(i)->setTextColor(normal_color);
//        }

//        pManualList->currentItem()->setTextColor(select_color);
//        pManualList->currentItem()->setTextAlignment(Qt::AlignCenter);
        pCur = pCur->pNext;
        count++;
    }
    cur_list_row = pManualList->count()-1;  //初始化当前选中行标号
    pManualList->setCurrentRow(pManualList->count()-1);


    ui_MainWindow->cancel_btn->setEnabled(true);
    ui_MainWindow->sure_btn->setEnabled(true);
    ui_MainWindow->bend_btn->setEnabled(false);
    ui_MainWindow->deploy_btn->setEnabled(true);
    ui_MainWindow->del_btn->setEnabled(true);
    ui_MainWindow->up_btn->setEnabled(true);
    ui_MainWindow->down_btn->setEnabled(true);
    ui_MainWindow->reset_btn->setEnabled(true);
    ui_MainWindow->reverse_btn->setEnabled(false);
    ui_MainWindow->comboBox->setEnabled(false);
    pManualArea->show();
    pManualArea_slider->show();
    pTool_btn->hide();
    updateShowRect();
}

void MainWindow::on_close_window_triggered()
{
    pTree->clear();
    if(pBendGL)
    {
        delete pBendGL;
        pBendGL = NULL;
        pBendGL = new bendGLWidget(pGLWidget);
        pBendGL->show();
    }
    if(pDeploy)
    {
        delete pDeploy;
        pDeploy = NULL;
        pDeploy = new uiDeployPage(pPartArea);
        pDeploy->show();
    }
    if(pTools){
        delete pTools;
        pTools = NULL;
        pTools = new DrawTools(pMachineArea);
        pTools->show();
    }
    if(pManualArea){
        delete pManualArea;
        pManualArea = NULL;
    }
    ui_MainWindow->Open->setEnabled(true);
}

void MainWindow::getSelectInfo(int bendnum)
{
    int num = ManualOrder.size();
    //qDebug()<<num;
    if(pOperate->bendFlag == false && num > 1) //当前为回程状态,需要折弯后才能进行下一道折弯
    {
        //QMessageBox::information(this,tr("程序提示"),tr("当前为展开状态,请折弯后继续"),QMessageBox::Yes);
        pOperate->drawToBend(ManualOrder[num-2]);
    }
    ui_MainWindow->bend_btn->setEnabled(true);
    ui_MainWindow->deploy_btn->setEnabled(false);
    ui_MainWindow->cancel_btn->setEnabled(true);
    ui_MainWindow->sure_btn->setEnabled(true);
    ui_MainWindow->del_btn->setEnabled(true);
    ui_MainWindow->reset_btn->setEnabled(true);
    ui_MainWindow->reverse_btn->setEnabled(true);
    ui_MainWindow->comboBox->setEnabled(true);
    ui_MainWindow->up_btn->setEnabled(true);
    ui_MainWindow->down_btn->setEnabled(true);

    bend_order *phead = pOperate->getBendOrder();
    bend_order *pCur = pOperate->findBendOrder(bendnum,phead);
    float angle = fabs(pCur->bendAngle)/PI*180;
    float radius = pCur->bendRadius;
    float width = pCur->bendWidth;
    int tool_id = pCur->tool_id;
    manual_bend_info = QString("%1 [A=%2  R=%3  W=%4  Model=%5]").arg(ManualOrder.size()).\
            arg(angle,0,'f',2).arg(radius,0,'f',2).arg(width,0,'f',2).arg(tool_id);
    pManualList->addItem(manual_bend_info);
    ManualTag.push_back(pCur->direction_h);
    ManualTool.push_back(tool_id);
    cur_list_row = pManualList->count()-1;
    pManualList->setCurrentRow(cur_list_row);
    pManualList->currentItem()->setTextAlignment(Qt::AlignCenter);
    bManualSelect = true;
    pModelSelect->setCurrentIndex(tool_id-1);
    QVector<int> res = pOperate->drawToMachine(bendnum);
    InterfereInfo(res);
    pBendGL->updateGL();
}

void MainWindow::on_bend_btn_clicked()
{
    QVector<int> res = pOperate->drawToBend(ManualOrder[cur_list_row]);
    InterfereInfo(res);
    ui_MainWindow->bend_btn->setEnabled(false);
    ui_MainWindow->deploy_btn->setEnabled(true);
    ui_MainWindow->reverse_btn->setEnabled(false);
    ui_MainWindow->comboBox->setEnabled(false);
    pBendGL->updateGL();
}

void MainWindow::on_deploy_btn_clicked()
{
    int row = pManualList->currentRow();
    QVector<int> res = pOperate->drawToDeploy(ManualOrder[row]);
    InterfereInfo(res);
    ui_MainWindow->bend_btn->setEnabled(true);
    ui_MainWindow->deploy_btn->setEnabled(false);
    ui_MainWindow->reverse_btn->setEnabled(true);
    ui_MainWindow->comboBox->setEnabled(true);
    pBendGL->updateGL();
}

void MainWindow::on_reverse_btn_clicked()
{
    int row = pManualList->currentRow();
    bend_order *phead = pOperate->getBendOrder();
    bend_order *pCur = pOperate->findBendOrder(ManualOrder[row],phead);
    pOperate->turnAboutPiece(pCur);
    if(ManualTag[row] == 0)
    {
        //pCur->direction_h = 1;
        ManualTag[row] = 1;
    }
    else if(ManualTag[row] == 1)
    {
        //pCur->direction_h = 0;
        ManualTag[row] = 0;
    }
    updatePosData();
    pBendGL->updateGL();
}

void MainWindow::on_del_btn_clicked()
{
    if(ManualOrder.size() <= 1)
    {
        on_reset_btn_clicked();
        return;
    }
    int count = ManualOrder.size();
    int row = pManualList->currentRow();
    pManualList->setCurrentRow(row-1);
    for(int i = count-1; i >= row; i--){
        ManualOrder.pop_back();
        ManualTag.pop_back();
        ManualTool.pop_back();
        QListWidgetItem *item = pManualList->item(i);
        pManualList->removeItemWidget(item);
        delete item;
    }
    //清空上次鼠标位置数值
    pDeploy->resetMousePos();
    updateShowRect();
    pBendGL->updateGL();
    ui_MainWindow->bend_btn->setEnabled(true);
    ui_MainWindow->deploy_btn->setEnabled(false);
}

void MainWindow::on_cancel_btn_clicked()
{
    bManualState = false;
    pManualArea->hide();
    ui_MainWindow->splitter->refresh();
    pOperate->ManualCancel(cur_list_row);
    pOperate->initWorkSpacePos();
    ManualOrder.clear();
    ManualTag.clear();
    ManualTool.clear();
    pManualList->clear();
    //清空上次鼠标位置数值
    pDeploy->resetMousePos();
    updateShowRect();
    pInterText->setText("");
}

void MainWindow::on_sure_btn_clicked()
{
    int num = ManualOrder.size();
    if(num != pOperate->getBendNum()){
        QMessageBox::information(this,tr("程序提示"),tr("未完整规划折弯工序!"),QMessageBox::Yes);
        return;
    }
    bManualState = false;
    pManualArea->hide();
    pManualArea_slider->hide();
    pTool_btn->show();
    ui_MainWindow->splitter->refresh();
    pOperate->ManualSure(cur_list_row);
    ManualOrder.clear();
    ManualTag.clear();
    ManualTool.clear();
    pManualList->clear();
    //清空上次鼠标位置数值
    pDeploy->resetMousePos();
    updateShowRect();
    pBendGL->updateGL();
    pInterText->setText("");
}

void MainWindow::on_reset_btn_clicked()
{
    ui_MainWindow->cancel_btn->setEnabled(true);
    ui_MainWindow->sure_btn->setEnabled(false);
    ui_MainWindow->bend_btn->setEnabled(false);
    ui_MainWindow->deploy_btn->setEnabled(false);
    ui_MainWindow->del_btn->setEnabled(false);
    ui_MainWindow->reset_btn->setEnabled(false);
    ui_MainWindow->reverse_btn->setEnabled(false);
    ui_MainWindow->comboBox->setEnabled(false);
    ui_MainWindow->up_btn->setEnabled(false);
    ui_MainWindow->down_btn->setEnabled(false);
    //首先钣金回退到初始状态
    pOperate->ManualCancel(cur_list_row);
    ManualOrder.clear();
    ManualTag.clear();
    ManualTool.clear();
    pManualList->clear();
    //清空上次鼠标位置数值
    pDeploy->resetMousePos();
    updateShowRect();
    pBendGL->updateGL();
    pInterText->setText("");
}

void MainWindow::on_comboBox_currentIndexChanged(int index)
{
    if(bManualState == false)
        return;
    if(bManualSelect){
        bManualSelect = false;
        return;
    }
    manual_bend_info = pManualList->currentItem()->text();
    int len = manual_bend_info.size();
    manual_bend_info.replace(len-2,1,QString("%1").arg(index+1));
    pManualList->currentItem()->setText(manual_bend_info);
    int row = pManualList->currentRow();
    bend_order *phead = pOperate->getBendOrder();
    bend_order *pCur = pOperate->findBendOrder(ManualOrder[row],phead);
    if(pCur->tool_id == index+1)
        return;
    ManualTool[row] = index+1;
    pOperate->ModelChange(pCur,index+1);
    updateShowRect();
    pInterText->setText("");
}

void MainWindow::on_up_btn_clicked()
{
    if(cur_list_row <= 0)
        return;
    int row = cur_list_row;
    //首先回退到最初的状态
    pOperate->ManualCancel(row);
    //然后修改折弯顺序
    ManualOrder.swap(row,row-1);
    ManualTag.swap(row,row-1);
    ManualTool.swap(row,row-1);
    //然后折弯到当前折弯的上一道
    pOperate->resetWorkPos();
    pOperate->convertToPoint(0,row-1);
    //首先为cur_list_row赋值,避免槽函数再去运行
    cur_list_row = row-1;
    pManualList->setCurrentRow(cur_list_row);
    //改变list中的内容
    bend_order *pHead = pOperate->getBendOrder();
    bend_order *pCur = pOperate->findBendOrder(ManualOrder[cur_list_row],pHead);
    float angle = fabs(pCur->bendAngle)/PI*180;
    float radius = pCur->bendRadius;
    float width = pCur->bendWidth;
    int tool_id = pCur->tool_id;
    manual_bend_info = QString("%1 [A=%2  R=%3  W=%4  Model=%5]").arg(cur_list_row+1).\
            arg(angle,0,'f',2).arg(radius,0,'f',2).arg(width,0,'f',2).arg(tool_id);
    pManualList->item(cur_list_row)->setText(manual_bend_info);
    bManualSelect = true;
    pModelSelect->setCurrentIndex(tool_id-1);

    pCur = pOperate->findBendOrder(ManualOrder[cur_list_row+1],pHead);
    angle = fabs(pCur->bendAngle)/PI*180;
    radius = pCur->bendRadius;
    width = pCur->bendWidth;
    tool_id = pCur->tool_id;
    manual_bend_info = QString("%1 [A=%2  R=%3  W=%4  Model=%5]").arg(cur_list_row+2).\
            arg(angle,0,'f',2).arg(radius,0,'f',2).arg(width,0,'f',2).arg(tool_id);
    pManualList->item(cur_list_row+1)->setText(manual_bend_info);

    updateShowRect();

//    for(int i = row+1; i < len; i++){
//        //pManualList->item(i)->setHidden(true);
//        pManualList->item(i)->setFlags((Qt::ItemFlag)0);
//        pManualList->item(i)->setBackgroundColor(disable_color);
//    }
}

void MainWindow::on_down_btn_clicked()
{
    if(cur_list_row >= pManualList->count()-1)
        return;
    int row = cur_list_row;
    //首先回退到最初的状态
    pOperate->ManualCancel(row);
    //然后修改折弯顺序
    ManualOrder.swap(row,row+1);
    ManualTag.swap(row,row+1);
    ManualTool.swap(row,row+1);
    //然后折弯到当前折弯的上一道
    pOperate->resetWorkPos();
    pOperate->convertToPoint(0,row+1);
    //首先为cur_list_row赋值,避免槽函数再去运行
    cur_list_row = row+1;
    pManualList->setCurrentRow(cur_list_row);
    //改变list中的内容
    bend_order *pHead = pOperate->getBendOrder();
    bend_order *pCur = pOperate->findBendOrder(ManualOrder[cur_list_row],pHead);
    float angle = fabs(pCur->bendAngle)/PI*180;
    float radius = pCur->bendRadius;
    float width = pCur->bendWidth;
    int tool_id = pCur->tool_id;
    manual_bend_info = QString("%1 [A=%2  R=%3  W=%4  Model=%5]").arg(cur_list_row+1).\
            arg(angle,0,'f',2).arg(radius,0,'f',2).arg(width,0,'f',2).arg(tool_id);
    pManualList->item(cur_list_row)->setText(manual_bend_info);
    bManualSelect = true;
    pModelSelect->setCurrentIndex(tool_id-1);

    pCur = pOperate->findBendOrder(ManualOrder[cur_list_row-1],pHead);
    angle = fabs(pCur->bendAngle)/PI*180;
    radius = pCur->bendRadius;
    width = pCur->bendWidth;
    tool_id = pCur->tool_id;
    manual_bend_info = QString("%1 [A=%2  R=%3  W=%4  Model=%5]").arg(cur_list_row).\
            arg(angle,0,'f',2).arg(radius,0,'f',2).arg(width,0,'f',2).arg(tool_id);
    pManualList->item(cur_list_row-1)->setText(manual_bend_info);

    updateShowRect();

}

void MainWindow::on_listWidget_currentRowChanged(int currentRow)
{
    if(currentRow < 0){
        cur_list_row = currentRow;
        return;
    }
    for(int i = 0; i < pManualList->count(); i++){
        if(i == currentRow)
        {
            pManualList->item(i)->setTextColor(select_color);
        }
        else
        {
            pManualList->item(i)->setTextColor(normal_color);
        }
    }
    if(currentRow == cur_list_row)
        return;
    pOperate->convertToPoint(cur_list_row,currentRow);
    bManualSelect = true;
    pModelSelect->setCurrentIndex(ManualTool[currentRow]-1);
    cur_list_row = currentRow;
    ui_MainWindow->bend_btn->setEnabled(true);
    ui_MainWindow->deploy_btn->setEnabled(false);
    ui_MainWindow->comboBox->setEnabled(true);
    ui_MainWindow->reverse_btn->setEnabled(true);
    updateShowRect();
}

void MainWindow::on_bend_all_clicked()
{
    pOperate->resetToBend();
    pBendGL->setShowModel(pOperate);
}

void MainWindow::on_deploy_all_clicked()
{
    pOperate->resetToDeploy();
    pBendGL->setShowModel(pOperate);
}

void MainWindow::on_listWidget_itemDoubleClicked(QListWidgetItem *item)
{
    item = item;
    if(ui_MainWindow->bend_btn->isEnabled())
    {
        QVector<int> res = pOperate->drawToBend(ManualOrder[cur_list_row]);
        InterfereInfo(res);
        ui_MainWindow->bend_btn->setEnabled(false);
        ui_MainWindow->deploy_btn->setEnabled(true);
        ui_MainWindow->reverse_btn->setEnabled(false);
        ui_MainWindow->comboBox->setEnabled(false);
    }
    else
    {
        int row = pManualList->currentRow();
        QVector<int> res = pOperate->drawToDeploy(ManualOrder[row]);
        InterfereInfo(res);
        ui_MainWindow->bend_btn->setEnabled(true);
        ui_MainWindow->deploy_btn->setEnabled(false);
        ui_MainWindow->reverse_btn->setEnabled(true);
        ui_MainWindow->comboBox->setEnabled(true);
    }
    pBendGL->updateGL();
}

void MainWindow::on_listWidget_indexesMoved(const QModelIndexList &indexes)
{

}

/********************************************
 *function:轴侧按钮槽函数
 *input:
 *output:
 *adding:
 *author: wang
 *date: 2016/07/06
 *******************************************/
void MainWindow::on_axis_beside_btn_clicked()
{
    point3f temp= pOperate->findBasicData(true);
    qDebug()<<temp.x;
    qDebug()<<temp.y;
    qDebug()<<temp.z;
    pBendGL->axisBesideView(temp);
}

void MainWindow::on_show_or_not_triggered()
{
    if(bShow_body)
    {
        bShow_body = false;
    }
    else
    {
        bShow_body = true;
    }
    pOperate->updateVisibleState(bShow_body,bShow_punch,bShow_die,bShow_x,bShow_r,bShow_z);
    updateShowRect();
}


void MainWindow::on_mainView_btn_clicked()
{
    if(mainView_flg ==true)
    {
        pBendGL->mainView();
        mainView_flg = false;
    }
    else
    {
        pBendGL->anti_mainview();
        mainView_flg = true;
    }
}

void MainWindow::on_sideView_btn_clicked()
{
    if(sideView_flg == true)
    {
        pBendGL->sideView();
        sideView_flg = false;
    }
    else
    {
        pBendGL->anti_sideView();
        sideView_flg = true;
    }
}

void MainWindow::on_topView_btn_clicked()
{
    if(topView_flg == true)
    {
        pBendGL->topView();
        topView_flg = false;
    }
    else
    {
        pBendGL->anti_topView();
        topView_flg = true;
    }
}

void MainWindow::on_axisSide_btn_clicked()
{
    if(axisSide_flg == true)
    {
        pBendGL->axisSide();
        axisSide_flg = false;
    }
    else
    {
        pBendGL->anti_axisside();
        axisSide_flg = true;
    }

}
