/**
 * @file /src/main_window.cpp
 *
 * @brief Implementation for the qt gui.
 *
 * @date February 2011
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui>
#include <QTreeWidgetItem>
#include <QMessageBox>
#include <iostream>
#include "../include/qt_panel/main_window.hpp"

/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace qt_panel {

using namespace Qt;

/*****************************************************************************
** Implementation [MainWindow]
*****************************************************************************/

MainWindow::MainWindow(int argc, char** argv, QWidget *parent)
	: QMainWindow(parent)
	, qnode(argc,argv)
{
	ui.setupUi(this); // Calling this incidentally connects all ui's triggers to on_...() callbacks in this class.
    QObject::connect(ui.actionAbout_Qt, SIGNAL(triggered(bool)), qApp, SLOT(aboutQt())); // qApp is a global variable for the application

    ReadSettings();
	setWindowIcon(QIcon(":/images/icon.png"));
	ui.tab_manager->setCurrentIndex(0); // ensure the first tab is showing - qt-designer should have this already hardwired, but often loses it (settings?).
    QObject::connect(&qnode, SIGNAL(rosShutdown()), this, SLOT(close()));

	/*********************
	** Logging
	**********************/
	ui.view_logging->setModel(qnode.loggingModel());
    QObject::connect(&qnode, SIGNAL(loggingUpdated()), this, SLOT(updateLoggingView()));

    /*********************
    ** Auto Start
    **********************/
    if ( ui.checkbox_remember_settings->isChecked() ) {
        on_button_connect_clicked(true);
    }
    // connect(ui.horizontalSlider_linera,SIGNAL(valueChanged(int)),this,SLOT(slot_linera_value_change(int)));
    // connect(ui.horizontalSlider_raw,SIGNAL(valueChanged(int)),this,SLOT(slot_raw_value_change(int)));
    // connect(ui.pushButton_i,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    // connect(ui.pushButton_j,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    // connect(ui.pushButton_l,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    // connect(ui.pushButton_n,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    // connect(ui.pushButton_m,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    // connect(ui.pushButton_br,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    // connect(ui.pushButton_u,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    // connect(ui.pushButton_o,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));


    //init ui
    speed_x_dashBoard=new CCtrlDashBoard(ui.widget_speed_x);
    speed_y_dashBoard=new CCtrlDashBoard(ui.widget_speed_y);
    speed_x_dashBoard->setGeometry(ui.widget_speed_x->rect());
    speed_y_dashBoard->setGeometry(ui.widget_speed_y->rect());
    speed_x_dashBoard->setValue(0);
    speed_y_dashBoard->setValue(0);
    // ui.horizontalSlider_linera->setValue(50);
    // ui.horizontalSlider_raw->setValue(50);

//    ui.treeWidget->setWindowTitle("Display");
//    ui.treeWidget->setWindowIcon(QIcon("://images/classes/Displays.svg"));

    //header
    ui.treeWidget->setHeaderLabels(QStringList()<<"key"<<"value");
    ui.treeWidget->setHeaderHidden(true);

    //GLobal Options
    QTreeWidgetItem* Global=new QTreeWidgetItem(QStringList()<<"Global Options");
    Global->setIcon(0,QIcon("://images/options.png"));

    ui.treeWidget->addTopLevelItem(Global);
    Global->setExpanded(true);
    //FixFrame
    QTreeWidgetItem* Fixed_frame=new QTreeWidgetItem(QStringList()<<"Fixed Frame");
    fixed_box = new QComboBox();
    fixed_box->addItem("rslidar");
    fixed_box->addItem("camera_init");
    fixed_box->setMaximumWidth(150);
    fixed_box->setEditable(true);
    connect(fixed_box, SIGNAL(currentTextChanged(QString)), this, SLOT(slot_treewidget_value_change(QString)));
    myrviz->Set_FixedFrame(fixed_box->currentText());
    Global->addChild(Fixed_frame);


    ui.treeWidget->setItemWidget(Fixed_frame,1,fixed_box);

    //Grid
    QTreeWidgetItem* Grid=new QTreeWidgetItem(QStringList()<<"Grid");
    //设置图标
    Grid->setIcon(0,QIcon("://images/classes/Grid.png"));
    //checkbox
    QCheckBox* Grid_Check = new QCheckBox();
    connect(Grid_Check, SIGNAL(stateChanged(int)), this, SLOT(slot_display_grid(int)));
    //添加top节点
    ui.treeWidget->addTopLevelItem(Grid);
    //添加checkbox
    ui.treeWidget->setItemWidget(Grid, 1, Grid_Check);
    //设置grid默认展开状态
    Grid->setExpanded(true);

    //添加Cell Count子节点
    QTreeWidgetItem* Cell_Count=new QTreeWidgetItem(QStringList()<<"Plane Cell Count");
    Grid->addChild(Cell_Count);
    //CellCount添加SpinBox
    Cell_Count_Box = new QSpinBox();
    Cell_Count_Box->setRange(1, 100000);
    Cell_Count_Box->setValue(1000);
    //设置Spinbox的宽度
    Cell_Count_Box->setMaximumWidth(150);
    ui.treeWidget->setItemWidget(Cell_Count,1,Cell_Count_Box);

    //添加color子节点
    QTreeWidgetItem* Grid_Color=new QTreeWidgetItem(QStringList()<<"Color");
    // Grid->addChild(Grid_Color);
    //Color添加ComboBox
    Grid_Color_Box=new QComboBox();
    Grid_Color_Box->addItem("160;160;160");
    //设置Comboox可编辑
    Grid_Color_Box->setEditable(true);
    //设置Combox的宽度
    Grid_Color_Box->setMaximumWidth(150);
    ui.treeWidget->setItemWidget(Grid_Color, 1, Grid_Color_Box);
    Grid_Check->setChecked(true);

    QTreeWidgetItem* Axes = new QTreeWidgetItem(QStringList() << "Axes");
    //设置图标
    Axes->setIcon(0, QIcon("://images/classes/Axes.png"));
    //checkbox
    QCheckBox* Axes_Check = new QCheckBox();
    connect(Axes_Check, SIGNAL(stateChanged(int)), this, SLOT(slot_display_axes(int)));
    //向Treewidget添加TF Top节点
    ui.treeWidget->addTopLevelItem(Axes);
    //向TF添加checkbox
    ui.treeWidget->setItemWidget(Axes, 1, Axes_Check);

    //TF ui
    QTreeWidgetItem* TF=new QTreeWidgetItem(QStringList()<<"TF");
    //设置图标
    TF->setIcon(0,QIcon("://images/classes/TF.png"));
    //checkbox
    QCheckBox* TF_Check=new QCheckBox();
    connect(TF_Check,SIGNAL(stateChanged(int)),this,SLOT(slot_display_tf(int)));
    //向Treewidget添加TF Top节点
    ui.treeWidget->addTopLevelItem(TF);
    //向TF添加checkbox
    ui.treeWidget->setItemWidget(TF,1,TF_Check);

    // //LaserScan
    // QTreeWidgetItem* LaserScan=new QTreeWidgetItem(QStringList()<<"LaserScan");
    // //设置图标
    // LaserScan->setIcon(0,QIcon("://images/classes/LaserScan.png"));
    // //checkbox
    // QCheckBox* Laser_Check=new QCheckBox();
    // connect(Laser_Check,SIGNAL(stateChanged(int)),this,SLOT(slot_display_laser(int)));
    // //向Treewidget添加TF Top节点
    // ui.treeWidget->addTopLevelItem(LaserScan);
    // //向TF添加checkbox
    // ui.treeWidget->setItemWidget(LaserScan,1,Laser_Check);
    // //laser topic
    // QTreeWidgetItem* LaserTopic=new QTreeWidgetItem(QStringList()<<"Topic");
    // Laser_Topic_box=new QComboBox();
    // Laser_Topic_box->addItem("/scan");
    // Laser_Topic_box->setEditable(true);
    // Laser_Topic_box->setMaximumWidth(150);
    // LaserScan->addChild(LaserTopic);
    // ui.treeWidget->setItemWidget(LaserTopic, 1, Laser_Topic_box);

    // package folder 1
    QTreeWidgetItem* DetectPkg1 = new QTreeWidgetItem(QStringList() << "Ruby Detect");
    DetectPkg1->setIcon(0, QIcon("://images/visibility.svg"));
    DetectPkg_Check1 = new QCheckBox();
    connect(DetectPkg_Check1, SIGNAL(stateChanged(int)), this, SLOT(slot_display_detect1(int)));
    ui.treeWidget->addTopLevelItem(DetectPkg1);
    ui.treeWidget->setItemWidget(DetectPkg1, 1, DetectPkg_Check1);

    QTreeWidgetItem* Pointcloud2_1 = new QTreeWidgetItem(QStringList() << "Lidar Pointcloud");
    Pointcloud2_1->setIcon(0, QIcon("://images/PointCloud2.png"));
    DetectPkg1->addChild(Pointcloud2_1);

    //pcd topic
    QTreeWidgetItem* PointCloudTopic_1 = new QTreeWidgetItem(QStringList() << "Topic");
    PointCloud_Topic_box1 = new QComboBox();
    PointCloud_Topic_box1->addItem("/rslidar_P80");
    PointCloud_Topic_box1->setEditable(true);
    PointCloud_Topic_box1->setMaximumWidth(150);
    Pointcloud2_1->addChild(PointCloudTopic_1);
    Pointcloud2_1->setExpanded(true);
    ui.treeWidget->setItemWidget(PointCloudTopic_1, 1, PointCloud_Topic_box1);

    //MarkerArray
    QTreeWidgetItem* MarkerArray_1 = new QTreeWidgetItem(QStringList() << "MarkerArray");
    //设置图标
    MarkerArray_1->setIcon(0, QIcon("://images/classes/MarkerArray.png"));
    //向Treewidget添加TF Top节点
    DetectPkg1->addChild(MarkerArray_1);

    //marker topic
    QTreeWidgetItem* MarkerArrayTopic_1 = new QTreeWidgetItem(QStringList() << "Topic");
    MarkerArray_Topic_box1 = new QComboBox();
    MarkerArray_Topic_box1->addItem("/marker_P80");
    MarkerArray_Topic_box1->setEditable(true);
    MarkerArray_Topic_box1->setMaximumWidth(150);
    MarkerArray_1->addChild(MarkerArrayTopic_1);
    MarkerArray_1->setExpanded(true);
    ui.treeWidget->setItemWidget(MarkerArrayTopic_1, 1, MarkerArray_Topic_box1);

    //************************* */
    // package folder 2
    QTreeWidgetItem* DetectPkg2 = new QTreeWidgetItem(QStringList() << "M1 Detect");
    DetectPkg2->setIcon(0, QIcon("://images/visibility.svg"));
    DetectPkg_Check2 = new QCheckBox();
    connect(DetectPkg_Check2, SIGNAL(stateChanged(int)), this, SLOT(slot_display_detect2(int)));
    ui.treeWidget->addTopLevelItem(DetectPkg2);
    ui.treeWidget->setItemWidget(DetectPkg2, 1, DetectPkg_Check2);

    QTreeWidgetItem* Pointcloud2_2 = new QTreeWidgetItem(QStringList() << "Lidar Pointcloud");
    Pointcloud2_2->setIcon(0, QIcon("://images/PointCloud2.png"));
    DetectPkg2->addChild(Pointcloud2_2);

    //pcd topic
    QTreeWidgetItem* PointCloudTopic_2 = new QTreeWidgetItem(QStringList() << "Topic");
    PointCloud_Topic_box2 = new QComboBox();
    PointCloud_Topic_box2->addItem("/rslidar_M1");
    PointCloud_Topic_box2->setEditable(true);
    PointCloud_Topic_box2->setMaximumWidth(150);
    Pointcloud2_2->addChild(PointCloudTopic_2);
    Pointcloud2_2->setExpanded(true);
    ui.treeWidget->setItemWidget(PointCloudTopic_2, 1, PointCloud_Topic_box2);

    //MarkerArray
    QTreeWidgetItem* MarkerArray_2 = new QTreeWidgetItem(QStringList() << "MarkerArray");
    //设置图标
    MarkerArray_2->setIcon(0, QIcon("://images/classes/MarkerArray.png"));
    //向Treewidget添加TF Top节点
    DetectPkg2->addChild(MarkerArray_2);

    //marker topic
    QTreeWidgetItem* MarkerArrayTopic_2 = new QTreeWidgetItem(QStringList() << "Topic");
    MarkerArray_Topic_box2 = new QComboBox();
    MarkerArray_Topic_box2->addItem("/marker_M1");
    MarkerArray_Topic_box2->setEditable(true);
    MarkerArray_Topic_box2->setMaximumWidth(150);
    MarkerArray_2->addChild(MarkerArrayTopic_2);
    MarkerArray_2->setExpanded(true);
    ui.treeWidget->setItemWidget(MarkerArrayTopic_2, 1, MarkerArray_Topic_box2);

    // package folder 3
    QTreeWidgetItem* DetectPkg3 = new QTreeWidgetItem(QStringList() << "NJU Detect");
    DetectPkg3->setIcon(0, QIcon("://images/visibility.svg"));
    DetectPkg_Check3 = new QCheckBox();
    connect(DetectPkg_Check3, SIGNAL(stateChanged(int)), this, SLOT(slot_display_detect3(int)));
    ui.treeWidget->addTopLevelItem(DetectPkg3);
    ui.treeWidget->setItemWidget(DetectPkg3, 1, DetectPkg_Check3);

    QTreeWidgetItem* Pointcloud2_3 = new QTreeWidgetItem(QStringList() << "Lidar Pointcloud");
    Pointcloud2_3->setIcon(0, QIcon("://images/PointCloud2.png"));
    DetectPkg3->addChild(Pointcloud2_3);

    //pcd topic
    QTreeWidgetItem* PointCloudTopic_3 = new QTreeWidgetItem(QStringList() << "Topic");
    PointCloud_Topic_box3 = new QComboBox();
    PointCloud_Topic_box3->addItem("/njupt/1/pointcloud_compensated");
    PointCloud_Topic_box3->setEditable(true);
    PointCloud_Topic_box3->setMaximumWidth(150);
    Pointcloud2_3->addChild(PointCloudTopic_3);
    Pointcloud2_3->setExpanded(true);
    ui.treeWidget->setItemWidget(PointCloudTopic_3, 1, PointCloud_Topic_box3);

    //MarkerArray
    QTreeWidgetItem* MarkerArray_3 = new QTreeWidgetItem(QStringList() << "MarkerArray");
    //设置图标
    MarkerArray_3->setIcon(0, QIcon("://images/classes/MarkerArray.png"));
    //向Treewidget添加TF Top节点
    DetectPkg3->addChild(MarkerArray_3);

    //marker topic
    QTreeWidgetItem* MarkerArrayTopic_3 = new QTreeWidgetItem(QStringList() << "Topic");
    MarkerArray_Topic_box3 = new QComboBox();
    MarkerArray_Topic_box3->addItem("/marker_new");
    MarkerArray_Topic_box3->setEditable(true);
    MarkerArray_Topic_box3->setMaximumWidth(150);
    MarkerArray_3->addChild(MarkerArrayTopic_3);
    MarkerArray_3->setExpanded(true);
    ui.treeWidget->setItemWidget(MarkerArrayTopic_3, 1, MarkerArray_Topic_box3);


    //SLAM 相关UI********************************
    //slam plus***************************************
    QTreeWidgetItem* SlamPkg1=new QTreeWidgetItem(QStringList()<<"Ruby SLAM");
    SlamPkg1->setIcon(0,QIcon("://images/Slam.png"));
    SlamPkg_Check1 = new QCheckBox();
    connect(SlamPkg_Check1,SIGNAL(stateChanged(int)),this,SLOT(slot_toggle_p80_slam(int)));
    ui.treeWidget->addTopLevelItem(SlamPkg1);
    ui.treeWidget->setItemWidget(SlamPkg1, 1, SlamPkg_Check1);

    QTreeWidgetItem* Slam_Pointcloud2_Registered_1 = new QTreeWidgetItem(QStringList() << "Pointcloud");
    Slam_Pointcloud2_Registered_1->setIcon(0, QIcon("://images/PointCloud2.png"));
    SlamPkg1->addChild(Slam_Pointcloud2_Registered_1);

    //pcd topic
    QTreeWidgetItem* Slam_Pointcloud2_Registered_Topic_1 = new QTreeWidgetItem(QStringList() << "Topic");
    Slam_Pcd_Reg_Topic_box1 = new QComboBox();
    Slam_Pcd_Reg_Topic_box1->addItem("/pointcloud_registered");
    Slam_Pcd_Reg_Topic_box1->setEditable(true);
    Slam_Pcd_Reg_Topic_box1->setMaximumWidth(150);
    Slam_Pointcloud2_Registered_1->addChild(Slam_Pointcloud2_Registered_Topic_1);
    Slam_Pointcloud2_Registered_1->setExpanded(true);
    ui.treeWidget->setItemWidget(Slam_Pointcloud2_Registered_Topic_1, 1, Slam_Pcd_Reg_Topic_box1);

    //Odom
    QTreeWidgetItem* Slam_Odom1 = new QTreeWidgetItem(QStringList() << "Odometry");
    //设置图标
    Slam_Odom1->setIcon(0, QIcon("://images/classes/Odometry.png"));
    //向TGlobal Map添加Path Top节点
    SlamPkg1->addChild(Slam_Odom1);

    //Slam topic
    QTreeWidgetItem* Slam_Odom_Topic1 = new QTreeWidgetItem(QStringList() << "Topic");
    Slam_Odom_Topic_box1 = new QComboBox();
    Slam_Odom_Topic_box1->addItem("/Odometry");
    Slam_Odom_Topic_box1->setEditable(true);
    Slam_Odom_Topic_box1->setMaximumWidth(150);
    Slam_Odom1->addChild(Slam_Odom_Topic1);
    Slam_Odom1->setExpanded(true);
    ui.treeWidget->setItemWidget(Slam_Odom_Topic1, 1, Slam_Odom_Topic_box1);

    //slam2***********************************************
    QTreeWidgetItem* SlamPkg2 = new QTreeWidgetItem(QStringList() << "M1 SLAM");
    SlamPkg2->setIcon(0, QIcon("://images/Slam.png"));
    SlamPkg_Check2 = new QCheckBox();
    connect(SlamPkg_Check2, SIGNAL(stateChanged(int)), this, SLOT(slot_toggle_m1_slam(int)));
    ui.treeWidget->addTopLevelItem(SlamPkg2);
    ui.treeWidget->setItemWidget(SlamPkg2, 1, SlamPkg_Check2);

    QTreeWidgetItem* Slam_Pointcloud2_Registered_2 = new QTreeWidgetItem(QStringList() << "Pointcloud");
    Slam_Pointcloud2_Registered_2->setIcon(0, QIcon("://images/PointCloud2.png"));
    SlamPkg2->addChild(Slam_Pointcloud2_Registered_2);

    //pcd topic
    QTreeWidgetItem* Slam_Pointcloud2_Registered_Topic_2 = new QTreeWidgetItem(QStringList() << "Topic");
    Slam_Pcd_Reg_Topic_box2 = new QComboBox();
    Slam_Pcd_Reg_Topic_box2->addItem("/pointcloud_registered");
    Slam_Pcd_Reg_Topic_box2->setEditable(true);
    Slam_Pcd_Reg_Topic_box2->setMaximumWidth(150);
    Slam_Pointcloud2_Registered_2->addChild(Slam_Pointcloud2_Registered_Topic_2);
    Slam_Pointcloud2_Registered_2->setExpanded(true);
    ui.treeWidget->setItemWidget(Slam_Pointcloud2_Registered_Topic_2, 1, Slam_Pcd_Reg_Topic_box2);

    QTreeWidgetItem* Slam_Odom2 = new QTreeWidgetItem(QStringList() << "Odometry");
    //设置图标
    Slam_Odom2->setIcon(0, QIcon("://images/classes/Odometry.png"));
    //向TLocal Map添加Path Top节点
    SlamPkg2->addChild(Slam_Odom2);

    //Odom topic
    QTreeWidgetItem* Slam_Odom_Topic2 = new QTreeWidgetItem(QStringList() << "Topic");
    Slam_Odom_Topic_box2 = new QComboBox();
    Slam_Odom_Topic_box2->addItem("/Odometry");
    Slam_Odom_Topic_box2->setEditable(true);
    Slam_Odom_Topic_box2->setMaximumWidth(150);
    Slam_Odom2->addChild(Slam_Odom_Topic2);
    Slam_Odom2->setExpanded(true);
    ui.treeWidget->setItemWidget(Slam_Odom_Topic2, 1, Slam_Odom_Topic_box2);


    //slam3=============
    QTreeWidgetItem* SlamPkg3 = new QTreeWidgetItem(QStringList() << "NJU SLAM");
    SlamPkg3->setIcon(0, QIcon("://images/Slam.png"));
    SlamPkg_Check3 = new QCheckBox();
    connect(SlamPkg_Check3, SIGNAL(stateChanged(int)), this, SLOT(slot_toggle_new_slam(int)));
    ui.treeWidget->addTopLevelItem(SlamPkg3);
    ui.treeWidget->setItemWidget(SlamPkg3, 1, SlamPkg_Check3);

    QTreeWidgetItem* Slam_Pointcloud2_Registered_3 = new QTreeWidgetItem(QStringList() << "Pointcloud");
    Slam_Pointcloud2_Registered_3->setIcon(0, QIcon("://images/PointCloud2.png"));
    SlamPkg3->addChild(Slam_Pointcloud2_Registered_3);

    //pcd topic
    QTreeWidgetItem* Slam_Pointcloud2_Registered_Topic_3 = new QTreeWidgetItem(QStringList() << "Topic");
    Slam_Pcd_Reg_Topic_box3 = new QComboBox();
    Slam_Pcd_Reg_Topic_box3->addItem("/pointcloud_registered");
    Slam_Pcd_Reg_Topic_box3->setEditable(true);
    Slam_Pcd_Reg_Topic_box3->setMaximumWidth(150);
    Slam_Pointcloud2_Registered_3->addChild(Slam_Pointcloud2_Registered_Topic_3);
    Slam_Pointcloud2_Registered_3->setExpanded(true);
    ui.treeWidget->setItemWidget(Slam_Pointcloud2_Registered_Topic_3, 1, Slam_Pcd_Reg_Topic_box3);

    QTreeWidgetItem* Slam_Odom3 = new QTreeWidgetItem(QStringList() << "Odometry");
    //设置图标
    Slam_Odom3->setIcon(0, QIcon("://images/classes/Odometry.png"));
    //向TLocal Map添加Path Top节点
    SlamPkg3->addChild(Slam_Odom3);

    //Odom topic
    QTreeWidgetItem* Slam_Odom_Topic3 = new QTreeWidgetItem(QStringList() << "Topic");
    Slam_Odom_Topic_box3 = new QComboBox();
    Slam_Odom_Topic_box3->addItem("/Odometry");
    Slam_Odom_Topic_box3->setEditable(true);
    Slam_Odom_Topic_box3->setMaximumWidth(150);
    Slam_Odom3->addChild(Slam_Odom_Topic3);
    Slam_Odom3->setExpanded(true);
    ui.treeWidget->setItemWidget(Slam_Odom_Topic3, 1, Slam_Odom_Topic_box3);


    //connect
    connect(&qnode,SIGNAL(speed_vel(float,float)),this,SLOT(slot_update_dashboard(float,float)));
    // connect(&qnode,SIGNAL(power_vel(float)),this,SLOT(slot_update_power(float)));
    connect(&qnode,SIGNAL(image_val(QImage)),this,SLOT(slot_update_image(QImage)));
    connect(&qnode,SIGNAL(position(double,double,double)),this,SLOT(slot_update_pos(double,double,double)));
    connect(ui.pushButton_sub_image,SIGNAL(clicked()),this,SLOT(slot_sub_image()));
    // connect(ui.laser_btn,SIGNAL(clicked()),this,SLOT(slot_quick_cmd_clicked()));
    //set start pose
    connect(ui.det_btn,SIGNAL(clicked()),this,SLOT(slot_toggle_det()));
    connect(ui.slam_btn, SIGNAL(clicked()), this, SLOT(slot_toggle_slam()));

}

void MainWindow::slot_toggle_slam() {
    switch (ui.RadarSelectBox->currentIndex()) {
    case 0:
        slot_toggle_m1_slam();
        break;
    case 1:
        slot_toggle_p80_slam();
        break;
    case 2:
        slot_toggle_new_slam();
        break;
    default:
        break;
    }
}
void MainWindow::slot_toggle_m1_slam() {
    DetectPkg_Check1->setChecked(false);
    DetectPkg_Check2->setChecked(false);
    DetectPkg_Check3->setChecked(false);
    SlamPkg_Check1->setChecked(false);
    SlamPkg_Check3->setChecked(false);
    switch (SlamPkg_Check2->checkState()) {
    case CheckState::Checked:
        SlamPkg_Check2->setChecked(false);
        break;
    case CheckState::Unchecked:
        SlamPkg_Check2->setChecked(true);
        break;
    default:
        break;
    }
}
void MainWindow::slot_toggle_p80_slam() {
    DetectPkg_Check1->setChecked(false);
    DetectPkg_Check2->setChecked(false);
    DetectPkg_Check3->setChecked(false);
    SlamPkg_Check3->setChecked(false);
    SlamPkg_Check2->setChecked(false);
    switch (SlamPkg_Check1->checkState()) {
    case CheckState::Checked:
        SlamPkg_Check1->setChecked(false);
        break;
    case CheckState::Unchecked:
        SlamPkg_Check1->setChecked(true);
        break;
    default:
        break;
    }
}
void MainWindow::slot_toggle_new_slam() {
    DetectPkg_Check1->setChecked(false);
    DetectPkg_Check2->setChecked(false);
    DetectPkg_Check3->setChecked(false);
    SlamPkg_Check1->setChecked(false);
    SlamPkg_Check2->setChecked(false);
    switch (SlamPkg_Check3->checkState()) {
    case CheckState::Checked:
        SlamPkg_Check3->setChecked(false);
        break;
    case CheckState::Unchecked:
        SlamPkg_Check3->setChecked(true);
        break;
    default:
        break;
    }
}
void MainWindow::slot_toggle_m1_slam(int state) {
    bool enable = state > 1 ? true : false;
    myrviz->Display_SLAM_PointCloud2_Registered_2(Slam_Pcd_Reg_Topic_box2->currentText(), enable);
    myrviz->Display_SLAM_PointCloud2_Queued_2(Slam_Pcd_Reg_Topic_box2->currentText(), enable);
    myrviz->Display_SLAM_Odometry_2(Slam_Odom_Topic_box2->currentText(), enable);
}
void MainWindow::slot_toggle_p80_slam(int state) {
    bool enable = state > 1 ? true : false;
    myrviz->Display_SLAM_PointCloud2_Registered_1(Slam_Pcd_Reg_Topic_box1->currentText(), enable);
    myrviz->Display_SLAM_PointCloud2_Queued_1(Slam_Pcd_Reg_Topic_box1->currentText(), enable);
    myrviz->Display_SLAM_Odometry_1(Slam_Odom_Topic_box1->currentText(), enable);
}
void MainWindow::slot_toggle_new_slam(int state) {
    bool enable = state > 1 ? true : false;
    myrviz->Display_SLAM_PointCloud2_Registered_3(Slam_Pcd_Reg_Topic_box3->currentText(), enable);
    myrviz->Display_SLAM_PointCloud2_Queued_3(Slam_Pcd_Reg_Topic_box3->currentText(), enable);
    myrviz->Display_SLAM_Odometry_3(Slam_Odom_Topic_box3->currentText(), enable);
}
void MainWindow::slot_update_pos(double x,double y,double z)
{
    ui.pos_x->setText(QString::number(x));
    ui.pos_y->setText(QString::number(y));
    ui.pos_z->setText(QString::number(z));
}
void MainWindow::slot_display_local_map(int state)
{
      bool enable=state>1?true:false;
      QStringList qli=Local_Planner_Color_box->currentText().split(";");
      QColor color=QColor(qli[0].toInt(),qli[1].toInt(),qli[2].toInt());
      myrviz->Display_Local_Map(Local_CostMap_Topic_box->currentText(),LocalMapColorScheme_box->currentText(),Local_Planner_Topic_box->currentText(),color,enable);
}
void MainWindow::slot_display_global_map(int state)
{
      bool enable=state>1?true:false;
      QStringList qli=Global_Planner_Color_box->currentText().split(";");
      QColor color=QColor(qli[0].toInt(),qli[1].toInt(),qli[2].toInt());
      myrviz->Display_Global_Map(Global_CostMap_Topic_box->currentText(),GlobalMapColorScheme_box->currentText(),Global_Planner_Topic_box->currentText(),color,enable);
}
void MainWindow::slot_toggle_det() {
    switch (ui.RadarSelectBox->currentIndex()) {
    case 0:
        slot_toggle_m1_det();
        break;
    case 1:
        slot_toggle_p80_det();
        break;
    case 2:
        slot_toggle_new_det();
        break;
    default:
        break;
    }
}
void MainWindow::slot_toggle_m1_det() {
    SlamPkg_Check1->setChecked(false);
    SlamPkg_Check2->setChecked(false);
    SlamPkg_Check3->setChecked(false);
    DetectPkg_Check1->setChecked(false);
    DetectPkg_Check3->setChecked(false);
    switch (DetectPkg_Check2->checkState())
    {
    case CheckState::Checked:
        DetectPkg_Check2->setChecked(false);
        break;
    case CheckState::Unchecked:
        DetectPkg_Check2->setChecked(true);
        break;
    default:
        break;
    }
}
void MainWindow::slot_toggle_p80_det() {
    SlamPkg_Check1->setChecked(false);
    SlamPkg_Check2->setChecked(false);
    SlamPkg_Check3->setChecked(false);
    DetectPkg_Check2->setChecked(false);
    DetectPkg_Check3->setChecked(false);
    switch (DetectPkg_Check1->checkState()) {
    case CheckState::Checked:
        DetectPkg_Check1->setChecked(false);
        break;
    case CheckState::Unchecked:
        DetectPkg_Check1->setChecked(true);
        break;
    default:
        break;
    }
}
void MainWindow::slot_toggle_new_det() {
    SlamPkg_Check1->setChecked(false);
    SlamPkg_Check2->setChecked(false);
    SlamPkg_Check3->setChecked(false);
    DetectPkg_Check1->setChecked(false);
    DetectPkg_Check2->setChecked(false);
    switch (DetectPkg_Check3->checkState()) {
    case CheckState::Checked:
        DetectPkg_Check3->setChecked(false);
        break;
    case CheckState::Unchecked:
        DetectPkg_Check3->setChecked(true);
        break;
    default:
        break;
    }
}
void MainWindow::slot_display_Path(int state)
{
    bool enable=state>1?true:false;
    QStringList qli=Path_Color_box->currentText().split(";");
    QColor color=QColor(qli[0].toInt(),qli[1].toInt(),qli[2].toInt());
    myrviz->Display_Path(Path_Topic_box->currentText(),color,enable);
}
void MainWindow::slot_display_Map(int state)
{
    bool enable=state>1?true:false;
    myrviz->Display_Map(Map_Topic_box->currentText(),Map_Color_Scheme_box->currentText(),enable);
}
void MainWindow::slot_display_RobotModel(int state)
{
    bool enable=state>1?true:false;
    myrviz->Display_RobotModel(enable);
}
void MainWindow::slot_display_laser(int state)
{
    bool enable=state>1?true:false;
    myrviz->Display_LaserScan(Laser_Topic_box->currentText(),enable);
}
void MainWindow::slot_display_detect1(int state) {
    bool enable = state > 1 ? true : false;
    myrviz->Display_PointCloud2_1(PointCloud_Topic_box1->currentText(), enable);
    myrviz->Display_MarkerArray_1(MarkerArray_Topic_box1->currentText(), enable);
}
void MainWindow::slot_display_detect2(int state) {
    bool enable = state > 1 ? true : false;
    myrviz->Display_PointCloud2_2(PointCloud_Topic_box2->currentText(), enable);
    myrviz->Display_MarkerArray_2(MarkerArray_Topic_box2->currentText(), enable);
}
void MainWindow::slot_display_detect3(int state) {
    bool enable = state > 1 ? true : false;
    myrviz->Display_PointCloud2_3(PointCloud_Topic_box3->currentText(), enable);
    myrviz->Display_MarkerArray_3(MarkerArray_Topic_box3->currentText(), enable);
}
void MainWindow::slot_display_axes(int state) {
    bool enable = state > 1 ? true : false;
    myrviz->Display_Axes(enable);
}
void MainWindow::slot_display_tf(int state)
{
    bool enable=state>1?true:false;
    myrviz->Display_TF(enable);
}
void MainWindow::slot_display_grid(int state)
{
    bool enable=state>1?true:false;
    QStringList qli=Grid_Color_Box->currentText().split(";");
    QColor color=QColor(qli[0].toInt(),qli[1].toInt(),qli[2].toInt());
    myrviz->Display_Grid(Cell_Count_Box->text().toInt(),color,enable);
}
void MainWindow::slot_treewidget_value_change(QString)
{
    myrviz->Set_FixedFrame(fixed_box->currentText());
}
void MainWindow::slot_quick_cmd_clicked()
{
    // laser_cmd=new QProcess;
    // laser_cmd->start("bash");
    // laser_cmd->write(ui.textEdit_laser_cmd->toPlainText().toLocal8Bit()+'\n');
    // connect(laser_cmd,SIGNAL(readyReadStandardError()),this,SLOT(slot_quick_output()));
    // connect(laser_cmd,SIGNAL(readyReadStandardOutput()),this,SLOT(slot_quick_output()));

}
void MainWindow::slot_quick_output()
{
    // ui.textEdit_quick_output->append("<font color=\"#FF0000\">"+laser_cmd->readAllStandardError()+"</font>");
    // ui.textEdit_quick_output->append("<font color=\"#FFFFFF\">"+laser_cmd->readAllStandardOutput()+"</font>");
}
void MainWindow::slot_update_image(QImage im)
{
    ui.label_image->setPixmap(QPixmap::fromImage(im));
}
void MainWindow::slot_sub_image()
{
    qnode.sub_image(ui.lineEdit_image_topic->text());
}
void MainWindow::slot_update_power(float value)
{
    //   ui.label_power_val->setText(QString::number(value).mid(0,5)+"V");
    //   double n=(value-10.5)/(12.5-10.5);
    //   int val=n*100;
    //   ui.progressBar->setValue(val);
}
void MainWindow::slot_update_dashboard(float x,float y)
{
    ui.label_dir_x->setText(x>0?"正向":"反向");
    ui.label_dir_y->setText(y>0?"正向":"反向");
    speed_x_dashBoard->setValue(abs(x)*100);
    speed_y_dashBoard->setValue(abs(y)*100);

}
void MainWindow::slot_pushbtn_click()
{
//   QPushButton* btn=qobject_cast<QPushButton*> (sender());
//   char k=btn->text().toStdString()[0];
//   bool is_all=ui.checkBox_is_all->isChecked();
//   float linear=ui.label_linera->text().toFloat()*0.01;
//   float angular=ui.label_raw->text().toFloat()*0.01;

//   switch (k) {
//     case 'i':
//       qnode.set_cmd_vel(is_all?'I':'i',linear,angular);
//       break;
//   case 'u':
//     qnode.set_cmd_vel(is_all?'U':'u',linear,angular);
//     break;
//   case 'o':
//     qnode.set_cmd_vel(is_all?'O':'o',linear,angular);
//     break;
//   case 'j':
//     qnode.set_cmd_vel(is_all?'J':'j',linear,angular);
//     break;
//   case 'l':
//     qnode.set_cmd_vel(is_all?'L':'l',linear,angular);
//     break;
//   case 'm':
//     qnode.set_cmd_vel(is_all?'M':'m',linear,angular);
//     break;
//   case ',':
//     qnode.set_cmd_vel(is_all?'<':',',linear,angular);
//     break;
//   case '.':
//     qnode.set_cmd_vel(is_all?'>':'.',linear,angular);
//     break;
//   }
}
void MainWindow::slot_linera_value_change(int value)
{
    // ui.label_linera->setText(QString::number(value));
}
void MainWindow::slot_raw_value_change(int value)
{
    // ui.label_raw->setText(QString::number(value));
}
MainWindow::~MainWindow() {}

/*****************************************************************************
** Implementation [Slots]
*****************************************************************************/

void MainWindow::showNoMasterMessage() {
	QMessageBox msgBox;
	msgBox.setText("Couldn't find the ros master.");
	msgBox.exec();
    close();
}

/*
 * These triggers whenever the button is clicked, regardless of whether it
 * is already checked or not.
 */

void MainWindow::on_button_connect_clicked(bool check ) {
	if ( ui.checkbox_use_environment->isChecked() ) {
		if ( !qnode.init() ) {
			showNoMasterMessage();
            ui.treeWidget->setEnabled(false);
		} else {
			ui.button_connect->setEnabled(false);
            ui.treeWidget->setEnabled(true);
            myrviz=new qrviz(ui.layout_rviz);
		}
	} else {
		if ( ! qnode.init(ui.line_edit_master->text().toStdString(),
				   ui.line_edit_host->text().toStdString()) ) {
			showNoMasterMessage();
             ui.treeWidget->setEnabled(false);
		} else {
			ui.button_connect->setEnabled(false);
			ui.line_edit_master->setReadOnly(true);
			ui.line_edit_host->setReadOnly(true);
			ui.line_edit_topic->setReadOnly(true);
             ui.treeWidget->setEnabled(true);
            myrviz=new qrviz(ui.layout_rviz);
		}
	}
}


void MainWindow::on_checkbox_use_environment_stateChanged(int state) {
	bool enabled;
	if ( state == 0 ) {
		enabled = true;
	} else {
		enabled = false;
	}
	ui.line_edit_master->setEnabled(enabled);
	ui.line_edit_host->setEnabled(enabled);
	//ui.line_edit_topic->setEnabled(enabled);
}

/*****************************************************************************
** Implemenation [Slots][manually connected]
*****************************************************************************/

/**
 * This function is signalled by the underlying model. When the model changes,
 * this will drop the cursor down to the last line in the QListview to ensure
 * the user can always see the latest log message.
 */
void MainWindow::updateLoggingView() {
        ui.view_logging->scrollToBottom();
}

/*****************************************************************************
** Implementation [Menu]
*****************************************************************************/

void MainWindow::on_actionAbout_triggered() {
    QMessageBox::about(this, tr("About ..."),tr("<h2>PACKAGE_NAME Test Program 0.10</h2><p>Copyright Yujin Robot</p><p>This package needs an about description.</p>"));
}

/*****************************************************************************
** Implementation [Configuration]
*****************************************************************************/

void MainWindow::ReadSettings() {
    QSettings settings("Qt-Ros Package", "qt_panel");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());
    QString master_url = settings.value("master_url",QString("http://localhost:11311/")).toString();
    QString host_url = settings.value("host_url", QString("localhost")).toString();
    //QString topic_name = settings.value("topic_name", QString("/chatter")).toString();
    ui.line_edit_master->setText(master_url);
    ui.line_edit_host->setText(host_url);
    //ui.line_edit_topic->setText(topic_name);
    bool remember = settings.value("remember_settings", false).toBool();
    ui.checkbox_remember_settings->setChecked(remember);
    bool checked = settings.value("use_environment_variables", false).toBool();
    ui.checkbox_use_environment->setChecked(checked);
    if ( checked ) {
    	ui.line_edit_master->setEnabled(false);
    	ui.line_edit_host->setEnabled(false);
    	//ui.line_edit_topic->setEnabled(false);
    }
}

void MainWindow::WriteSettings() {
    QSettings settings("Qt-Ros Package", "qt_panel");
    settings.setValue("master_url",ui.line_edit_master->text());
    settings.setValue("host_url",ui.line_edit_host->text());
    //settings.setValue("topic_name",ui.line_edit_topic->text());
    settings.setValue("use_environment_variables",QVariant(ui.checkbox_use_environment->isChecked()));
    settings.setValue("geometry", saveGeometry());
    settings.setValue("windowState", saveState());
    settings.setValue("remember_settings",QVariant(ui.checkbox_remember_settings->isChecked()));

}

void MainWindow::closeEvent(QCloseEvent *event)
{
	WriteSettings();
	QMainWindow::closeEvent(event);
}

}  // namespace qt_panel

