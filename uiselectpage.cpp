#include "uiselectpage.h"
#include "ui_uiselectpage.h"
#include "sysglobalvars.h"

uiSelectPage::uiSelectPage(QWidget *parent, QString path) :
    QDialog(parent),
    ui(new Ui::uiSelectPage)
{
//    Qt::WindowFlags flags=Qt::Dialog;
//    flags |=Qt::WindowMinMaxButtonsHint | Qt::WindowCloseButtonHint;
//    setWindowFlags(flags);
//    ui->setupUi(this);
//    setWindowTitle(tr("选择折弯线"));
//    pGLWidget = ui->widget;
//    //pList = ui->listWidget;
//    pButton[0] = ui->btn_cancel;
//    pButton[1] = ui->btn_sure;
//    workpice_path = path;

//    if(pBendGL == NULL)
//        pBendGL = new bendGLWidget(pGLWidget);
//    pBendGL->readOriginFile(path,WORK_PIECE);
//    pBendGL->setShowMode(true);     //选择模式
}

uiSelectPage::~uiSelectPage()
{
//    if(pBendGL)
//    {
//        delete pBendGL;
//        pBendGL = NULL;
//    }
//    delete ui;
}

void uiSelectPage::resizeEvent(QResizeEvent *event)
{
//    event = event;
//    if(pBendGL)
//    {
//        int width = pGLWidget->width();
//        int height = pGLWidget->height();
//        pBendGL->resizeWindow(width,height);
//    }
}

void uiSelectPage::wheelEvent(QWheelEvent *event)
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

void uiSelectPage::mouseMoveEvent(QMouseEvent *event)
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

void uiSelectPage::mousePressEvent(QMouseEvent *event)
{
//    mouse_x = event->x();
//    mouse_y = event->y();
//    int button = event->button();
//    switch (button) {
//    case Qt::RightButton:
//    {
//        break;
//    }
//    case Qt::LeftButton:
//    {
//        if(pBendGL && pBendGL->getShowMode())
//        {
//            int pos_x = pGLWidget->x();
//            int pos_y = pGLWidget->y();
//            pBendGL->SelectedObject(mouse_x-pos_x,mouse_y-pos_y);
//        }
//        break;
//    }
//    case Qt::MiddleButton:
//    {
//        setCursor(Qt::IBeamCursor);
//        break;
//    }
//    default:
//        break;
//    }
}

void uiSelectPage::mouseReleaseEvent(QMouseEvent *event)
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

void uiSelectPage::on_btn_cancel_clicked()
{
//    if(pButton[0]->text() == tr("取消"))
//    {
//        this->releaseKeyboard();
//        this->reject();
//    }
//    else if(pButton[1]->text() == tr("上一步"))
//    {
//        pBendGL->setShowMode(true);
//        pButton[0]->setText(tr("取消"));
//        pButton[1]->setText(tr("预览"));
//    }
}

void uiSelectPage::on_btn_sure_clicked()
{
//    if(pButton[1]->text() == tr("预览"))
//    {
//        pBendGL->formBendImage("D:/deploy.bmp");
//        pBendGL->setShowMode(false); //在非选择模式下显示
//        pBendGL->filterBendSurface();//筛选出折弯面并获取板料的信息
//        //pBendGL->getToolInfo();
//        pBendGL->showBendedPiece(); //显示全部折弯后的工件模型
//        pButton[0]->setText(tr("上一步"));
//        pButton[1]->setText(tr("完成"));
//    }
//    else if(pButton[1]->text() == tr("完成"))
//    {
//        this->releaseKeyboard();
//        this->accept();
//    }
}
