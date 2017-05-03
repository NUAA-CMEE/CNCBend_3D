#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtPrintSupport/QtPrintSupport>
#include "bendglwidget.h"
#include "drawtools.h"
#include <stdlib.h>
#include <QMutex>
#include "SysGLData.h"
#include "network.h"
#include "uirealtimepage.h"
#include "uiselectpage.h"
#include "toolconfigdialog.h"
#include "uideploypage.h"
#include "uimachineconstant.h"
#include "CNCGlobalFuncs.h"
#include "uiaxisconfigdialog.h"
#include "uibendorderpage.h"
#include <QSlider>
#include <QMenu>
#include "fileoperate.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
protected:
    void resizeEvent(QResizeEvent *event);
    void keyPressEvent(QKeyEvent *eKey);
    void wheelEvent(QWheelEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void enterSlot();
    void updateShowRect();
    QPixmap *drawSummaryPage();
    void updatePosData();
    void updateTreeData();
    void updateSelectState();
    void initPopMenuContent();
    void initMenuState();
    void updateRadioButtonState();
    void bendAction();
    void InterfereInfo(QVector<int> res);
    void makeDir(QString path);
    
private slots:
    void on_New_triggered();
    void on_Open_triggered();
    void on_Quit_triggered();
    void on_Punch_triggered();
    void on_Die_triggered();
    void on_Bend_triggered();
    void on_Deploy_triggered();
    void on_Print_triggered();
    void on_MB8_100X4200_triggered();
    void on_Auto_triggered();
    void on_enLarge_triggered();
    void on_shrink_triggered();
    void on_reset_triggered();
    void on_Return_triggered();
    void on_NC_triggered();
    void on_Link_triggered();
    void on_Transfer_triggered();
    void on_RealTime_triggered();
    void on_Body_Show_triggered();
    void on_Punch_Show_triggered();
    void on_Die_Show_triggered();
    void on_X_Show_triggered();
    void on_R_Show_triggered();
    void on_Z_Show_triggered();
    void on_ToolConfig_triggered();
    void timeOutSlot();
    void on_y_btn_clicked(bool checked);
    void on_x1_btn_clicked(bool checked);
    void on_x1_back_btn_clicked(bool checked);
    void on_r1_btn_clicked(bool checked);
    void on_z1_btn_clicked(bool checked);
    void on_z2_btn_clicked(bool checked);
    void on_inter_tag_clicked(bool checked);
    void on_Constant_triggered();
    void on_slider_sliderMoved(int position);
    void on_before_btn_clicked();
    void on_next_btn_clicked();
    void action_up_slot();
    void action_down_slot();
    void action_delete_slot();
    void on_splitter_4_splitterMoved(int pos, int index);
    void on_splitter_3_splitterMoved(int pos, int index);
    void on_splitter_splitterMoved(int pos, int index);
    void on_splitter_2_splitterMoved(int pos, int index);
    void on_treeWidget_customContextMenuRequested(const QPoint &pos);
    void on_BackGaung_triggered();
    void on_x2_btn_clicked(bool checked);
    void on_x2_back_btn_clicked(bool checked);
    void on_r2_btn_clicked(bool checked);
    void on_bend_order_triggered();

    void on_close_window_triggered();
    void getSelectInfo(int bendnum);

    void on_bend_btn_clicked();

    void on_deploy_btn_clicked();

    void on_reverse_btn_clicked();

    void on_del_btn_clicked();

    void on_cancel_btn_clicked();

    void on_sure_btn_clicked();

    void on_reset_btn_clicked();

    void on_comboBox_currentIndexChanged(int index);

    void on_up_btn_clicked();

    void on_down_btn_clicked();

    void on_listWidget_currentRowChanged(int currentRow);

    void on_bend_all_clicked();

    void on_deploy_all_clicked();

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_listWidget_indexesMoved(const QModelIndexList &indexes);

    void on_axis_beside_btn_clicked();

    void on_show_or_not_triggered();

    void on_mainView_btn_clicked();

    void on_sideView_btn_clicked();

    void on_topView_btn_clicked();

    void on_axisSide_btn_clicked();

private:
    Ui::MainWindow *ui_MainWindow;
    QWidget *pGLWidget;     //折弯仿真显示区
    QWidget *pMachineArea;  //上下模选择显示区
    QWidget *pPartArea;     //零件展开图显示区
    QWidget *pManualArea;     //手动调整显示区域
    QWidget *pManualArea_slider;//手动调整显示区域
    QWidget *pTool_btn;     //工具按钮区域
    QGridLayout *pGrid;     //栅格布局
    QTreeWidget *pTree;     //项目信息显示区
    QListWidget *pManualList;//手动调整显示区
    int cur_list_row;
    QComboBox *pModelSelect;
    bool bManualSelect;
    QStatusBar *pStatus;    //状态栏
    QLabel *timeLabel;
    QTimer *pTimer;
    QMutex mutex;
    QTreeWidgetItem *pRoot_part;
    QTreeWidgetItem *pRoot_machine;
    QTreeWidgetItem *pRoot_bend;
    bendGLWidget *pModel;
    uiDeployPage *pDeploy;
    DrawTools *pTools;
    QString fileName;
    int mouse_x,mouse_y;
    int count_num;
    int bendNum;
    int curNum;

    bool bShow_body;
    bool bShow_punch;
    bool bShow_die;
    bool bShow_x;
    bool bShow_r;
    bool bShow_z;

    float rote_y,rote_x;
    QString manual_bend_info;

    QString dbName;
    QSlider *pSlider;
    QRadioButton *pYAxis;
    QRadioButton *pX1Axis;
    QRadioButton *pX2Axis;
    QRadioButton *pX1Axis_Back;
    QRadioButton *pX2Axis_Back;
    QRadioButton *pR1Axis;
    QRadioButton *pR2Axis;
    QRadioButton *pZ1Axis;
    QRadioButton *pZ2Axis;
    QLineEdit *pCurPos;
    QCheckBox *pInterfere;
    QLabel *pInterText;
    QPushButton *pButton1;
    QPushButton *pButton2;

    QVector<bend_surface *>pBendData;

    NetWork *pNetWork;
    uiRealTimePage *pRealTimePage;
    uiSelectPage *pSelect;
    ToolConfigDialog *pConfigDlg;
    uiMachineConstant *pMachineConstant;
    uiAxisConfigDialog *pAxisConfig;
    uiBendOrderPage *pOrderPage;

    //右键菜单项目
    QAction *pAction_Up;    //上移选项
    QAction *pAction_Down;  //下移项目
    QAction *pAction_Delete;//删除折弯

    fileOperate *pOperate;
    bendGLWidget *pBendGL;

    bool mainView_flg;
    bool topView_flg;
    bool sideView_flg;
    bool axisSide_flg;

};

#endif // MAINWINDOW_H
