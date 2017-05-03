#include "uirealtimepage.h"
#include "ui_uirealtimepage.h"
#include "SysKeyboard.h"
#include "sysglobalvars.h"

uiRealTimePage::uiRealTimePage(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::uiRealTimePage)
{
    Qt::WindowFlags flags=Qt::Dialog;
    flags |=Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint;
    setWindowFlags(flags);
    ui->setupUi(this);

    setWindowTitle(tr("折弯实时仿真"));
    pGLWidget = ui->widget;
    pLabel[0] = ui->label_2;
    pLabel[1] = ui->label_3;
    pLabel[2] = ui->label_4;
    pLabel[3] = ui->label_5;
    pLabel[4] = ui->label_6;
    pLabel[5] = ui->label_8;
    pLabel[6] = ui->label_9;
    pLabel[7] = ui->label_10;

    pShowTimer = new QTimer(this);
    connect(pShowTimer,SIGNAL(timeout()),this,SLOT(showTimeOutSlot()));
    pShowTimer->start(10);
}

uiRealTimePage::~uiRealTimePage()
{
//    delete ui;
//    if(pBendGL)
//    {
//        delete pBendGL;
//        pBendGL = NULL;
//    }
}

void uiRealTimePage::resizeEvent(QResizeEvent *event)
{
//    event = event;
//    if(pBendGL)
//    {
//        int width = pGLWidget->width();
//        int height = pGLWidget->height();
//        pBendGL->resizeWindow(width,height);
//    }
}

void uiRealTimePage::keyPressEvent(QKeyEvent *eKey)
{
//    int iKey = eKey->key();
//    switch (iKey) {
//    case Key_Left:
//    {
//        if(pBendGL)
//            pBendGL->convertGraph(-0.8,CONVERT_Y);
//        break;
//    }
//    case Key_Right:
//    {
//        if(pBendGL)
//            pBendGL->convertGraph(0.8,CONVERT_Y);
//        break;
//    }
//    case Key_Up:
//    {
//        if(pBendGL)
//            pBendGL->convertGraph(-0.8,CONVERT_X);
//        break;
//    }
//    case Key_Down:
//    {
//        if(pBendGL)
//            pBendGL->convertGraph(0.8,CONVERT_X);
//        break;
//    }
//    default:
//        break;
//    }
}

void uiRealTimePage::wheelEvent(QWheelEvent *event)
{
//    int numDegress = event->delta()/8;  //滚动的角度，*8就是鼠标滚动的距离
//    int numSteps = numDegress/15;       //滚动的步数，*15就是鼠标滚动的角度
//    if(pBendGL)
//    {
//        float zoom = numSteps*0.1;
//        pBendGL->convertGraph(zoom,CONVERT_ZOOM);
//    }
//    event->accept();                    //接收该事件
}

void uiRealTimePage::mouseMoveEvent(QMouseEvent *event)
{
//    int pos_x = event->x();
//    int pos_y = event->y();
//    if(event->buttons() & Qt::RightButton)      //右键被按下
//    {
//        if(pBendGL)
//        {
//            if(abs(pos_x-mouse_x) > abs(pos_y-mouse_y))
//                pBendGL->convertGraph((pos_x-mouse_x)/2,CONVERT_HORIZONTAL);
//            else
//                pBendGL->convertGraph((mouse_y-pos_y)/2,CONVERT_VERTICAL);
//        }
//    }
//    else if(event->buttons() & Qt::MiddleButton) //中键被按下
//    {
//        float rote = 4.0;
//        if(pBendGL)
//        {
//            float x_off = pos_x-mouse_x;
//            float y_off = pos_y-mouse_y;
//            if(x_off > 100)
//                pBendGL->convertGraph(rote,CONVERT_Y);
//            else if(x_off < -100)
//                pBendGL->convertGraph(-rote,CONVERT_Y);
//            if(y_off > 100)
//                pBendGL->convertGraph(rote,CONVERT_X);
//            else if(y_off < -100)
//                pBendGL->convertGraph(-rote,CONVERT_X);
//            if(x_off > 100 && y_off > 100)
//                pBendGL->rotateGraph(1.0,1.0,0.0);
//            else if(x_off < -100 && y_off < -100)
//                pBendGL->rotateGraph(-1.0,-1.0,0.0);
//            else if(x_off > 100 && y_off < 100)
//                pBendGL->rotateGraph(0.0,1.0,0.0);
//        }
//    }
//    else if(event->buttons() & Qt::LeftButton)  //左键被按下
//    {

//    }
}

void uiRealTimePage::mousePressEvent(QMouseEvent *event)
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
        setCursor(Qt::IBeamCursor);
        break;
    }
    default:
        break;
    }
}

void uiRealTimePage::mouseReleaseEvent(QMouseEvent *event)
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
        break;
    }
    default:
        break;
    }
}

/********************************************
 *function:设置显示的内容
 *adding:
 *author: xu
 *date: 2016/03/11
 *******************************************/
void uiRealTimePage::setShowContent()
{
//    if(pBendGL == NULL)
//        pBendGL = new bendGLWidget(pGLWidget);
//    QString maPath,upperPath,lowerPath,backgaungPath;
//    maPath = machinePath + "MB8-100X4200.igs";
//    upperPath = machinePath + "upper.igs";
//    lowerPath = machinePath + "lower.igs";
//    pBendGL->readOriginFile(maPath,BODY);
//    pBendGL->readOriginFile(upperPath,UPPER);
//    pBendGL->readOriginFile(lowerPath,LOWER);
//    backgaungPath = machinePath + "backgaung_r.igs";
//    pBendGL->readOriginFile(backgaungPath,BACK_GAUNG_RL);
//    pBendGL->readOriginFile(backgaungPath,BACK_GAUNG_RR);
//    backgaungPath = machinePath + "backgaung_x.igs";
//    pBendGL->readOriginFile(backgaungPath,BACK_GAUNG_XL);
//    pBendGL->readOriginFile(backgaungPath,BACK_GAUNG_XR);
//    backgaungPath = machinePath + "backgaung_z.igs";
//    pBendGL->readOriginFile(backgaungPath,BACK_GAUNG_ZL);
//    pBendGL->readOriginFile(backgaungPath,BACK_GAUNG_ZR);
//    pBendGL->drawTranslation(X1_AXIS,-100.0,0.0,0.0);
//    pBendGL->drawTranslation(R1_AXIS,-100.0,dieHeight,0.0);
//    pBendGL->drawTranslation(Z1_AXIS,-100.0,0.0,0.0);
//    pBendGL->drawTranslation(X2_AXIS,100.0,0.0,0.0);
//    pBendGL->drawTranslation(R2_AXIS,100.0,dieHeight,0.0);
//    pBendGL->drawTranslation(Z2_AXIS,100.0,0.0,0.0);
//    QString punchPath = machinePath + "punch.igs";
//    pBendGL->readOriginFile(punchPath,PUNCH);
//    QString diePath = machinePath + "die.igs";
//    pBendGL->readOriginFile(diePath,DIE);
//    pBendGL->show();
}

void uiRealTimePage::showTimeOutSlot()
{
//    pShowTimer->stop();
//    stu_stateData state;
//    mutex.lock();
//    memcpy(&state,&stateData,sizeof(stu_stateData));
//    mutex.unlock();
//    float y_pos = ((float)state.Y1_data + (float)state.Y2_data)/(2*200.0);
//    float x_pos = (float)state.X1_data/320.0;
//    float r_pos = (float)state.R_data/160.0;
//    int udp = (int)(state.plc_M & 0x0001);
//    int mute = (int)((state.plc_M >> 1) & 0x0001);
//    int clamp = (int)((state.plc_M >> 2) & 0x0001);
//    int ldp = (int)((state.plc_M >> 3) & 0x0001);
//    int er = (int)((state.plc_M >> 4) & 0x0001);
//    int y_close = (int)((state.plc_M >> 5) & 0x0001);
//    int y_press = (int)((state.plc_M >> 6) & 0x0001);
//    int y_open = (int)((state.plc_M >> 7) & 0x0001);
//    if(udp)
//        pLabel[0]->setStyleSheet(UDP_COLOR);
//    else
//        pLabel[0]->setStyleSheet(NORMAL_COLOR);
//    if(er)
//        pLabel[1]->setStyleSheet(ER_COLOR);
//    else
//        pLabel[1]->setStyleSheet(NORMAL_COLOR);
//    if(mute)
//        pLabel[2]->setStyleSheet(MUTE_COLOR);
//    else
//        pLabel[2]->setStyleSheet(NORMAL_COLOR);
//    if(clamp)
//        pLabel[3]->setStyleSheet(CLAMP_COLOR);
//    else
//        pLabel[3]->setStyleSheet(NORMAL_COLOR);
//    if(ldp)
//        pLabel[4]->setStyleSheet(LDP_COLOR);
//    else
//        pLabel[4]->setStyleSheet(NORMAL_COLOR);
//    if(y_close)
//        pLabel[5]->setStyleSheet(CLOSING_COLOR);
//    else
//        pLabel[5]->setStyleSheet(NORMAL_COLOR);
//    if(y_press)
//        pLabel[6]->setStyleSheet(PRESSING_COLOR);
//    else
//        pLabel[6]->setStyleSheet(NORMAL_COLOR);
//    if(y_open)
//        pLabel[7]->setStyleSheet(OPENING_COLOR);
//    else
//        pLabel[7]->setStyleSheet(NORMAL_COLOR);
//    pBendGL->drawAnimateModel(y_pos,x_pos,x_pos,r_pos,r_pos);
//    pShowTimer->start(100);
}
