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
    connect(ui.horizontalSlider_linera,SIGNAL(valueChanged(int)),this,SLOT(slot_linera_value_change(int)));
    connect(ui.horizontalSlider_raw,SIGNAL(valueChanged(int)),this,SLOT(slot_raw_value_change(int)));
    connect(ui.pushButton_i,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    connect(ui.pushButton_j,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    connect(ui.pushButton_l,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    connect(ui.pushButton_n,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    connect(ui.pushButton_m,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    connect(ui.pushButton_br,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    connect(ui.pushButton_u,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));
    connect(ui.pushButton_o,SIGNAL(clicked()),this,SLOT(slot_pushbtn_click()));


    //init ui
    speed_x_dashBoard=new CCtrlDashBoard(ui.widget_speed_x);
    speed_y_dashBoard=new CCtrlDashBoard(ui.widget_speed_y);
    speed_x_dashBoard->setGeometry(ui.widget_speed_x->rect());
    speed_y_dashBoard->setGeometry(ui.widget_speed_y->rect());
    speed_x_dashBoard->setValue(0);
    speed_y_dashBoard->setValue(0);
    ui.horizontalSlider_linera->setValue(50);
    ui.horizontalSlider_raw->setValue(50);

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
    QTreeWidgetItem* DetectPkg1 = new QTreeWidgetItem(QStringList() << "Lidar P80");
    DetectPkg1->setIcon(0, QIcon("://images/default_package_icon.png"));
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
    QTreeWidgetItem* DetectPkg2 = new QTreeWidgetItem(QStringList() << "Lidar M1");
    DetectPkg2->setIcon(0, QIcon("://images/default_package_icon.png"));
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

    // //RobotModel
    // QTreeWidgetItem* RobotModel=new QTreeWidgetItem(QStringList()<<"RobotModel");
    // //设置图标
    // RobotModel->setIcon(0,QIcon("://images/classes/RobotModel.png"));
    // //checkbox
    // QCheckBox* RobotModel_Check=new QCheckBox();
    // connect(RobotModel_Check,SIGNAL(stateChanged(int)),this,SLOT(slot_display_RobotModel(int)));
    // //向Treewidget添加TF Top节点
    // ui.treeWidget->addTopLevelItem(RobotModel);
    // //向TF添加checkbox
    // ui.treeWidget->setItemWidget(RobotModel,1,RobotModel_Check);


    // //Map
    // QTreeWidgetItem* Map=new QTreeWidgetItem(QStringList()<<"Map");
    // //设置图标
    // Map->setIcon(0,QIcon("://images/classes/Map.png"));
    // //checkbox
    // QCheckBox* Map_Check=new QCheckBox();
    // connect(Map_Check,SIGNAL(stateChanged(int)),this,SLOT(slot_display_Map(int)));
    // //向Treewidget添加Map Top节点
    // ui.treeWidget->addTopLevelItem(Map);
    // //向Map添加checkbox
    // ui.treeWidget->setItemWidget(Map,1,Map_Check);
    // //Map topic
    // QTreeWidgetItem* MapTopic=new QTreeWidgetItem(QStringList()<<"Topic");
    // Map_Topic_box=new QComboBox();
    // Map_Topic_box->addItem("/map");
    // Map_Topic_box->setEditable(true);
    // Map_Topic_box->setMaximumWidth(150);
    // Map->addChild(MapTopic);
    // ui.treeWidget->setItemWidget(MapTopic,1,Map_Topic_box);
    // //Map color scheme
    // QTreeWidgetItem* MapColorScheme=new QTreeWidgetItem(QStringList()<<"Color Scheme");
    // Map_Color_Scheme_box=new QComboBox();
    // Map_Color_Scheme_box->addItem("map");
    // Map_Color_Scheme_box->addItem("costmap");
    // Map_Color_Scheme_box->addItem("raw");
    // Map_Color_Scheme_box->setMaximumWidth(150);
    // Map->addChild(MapColorScheme);
    // ui.treeWidget->setItemWidget(MapColorScheme,1,Map_Color_Scheme_box);

    //Path
    QTreeWidgetItem* Path=new QTreeWidgetItem(QStringList()<<"Path");
    //设置图标
    Path->setIcon(0,QIcon("://images/classes/Path.png"));
    //checkbox
    QCheckBox* Path_Check=new QCheckBox();
    connect(Path_Check,SIGNAL(stateChanged(int)),this,SLOT(slot_display_Path(int)));
    //向Treewidget添加Path Top节点
    ui.treeWidget->addTopLevelItem(Path);
    //向Path添加checkbox
    ui.treeWidget->setItemWidget(Path,1,Path_Check);
    //Path topic
    QTreeWidgetItem* PathTopic=new QTreeWidgetItem(QStringList()<<"Topic");
    Path_Topic_box=new QComboBox();
    Path_Topic_box->addItem("/path");
    Path_Topic_box->setEditable(true);
    Path_Topic_box->setMaximumWidth(150);
    Path->addChild(PathTopic);
    ui.treeWidget->setItemWidget(PathTopic,1,Path_Topic_box);
    //Path color scheme
    QTreeWidgetItem* PathColorScheme=new QTreeWidgetItem(QStringList()<<"Color");
    Path_Color_box=new QComboBox();
    Path_Color_box->addItem("0;12;255");
    Path_Color_box->setEditable(true);
    Path_Color_box->setMaximumWidth(150);
    Path->addChild(PathColorScheme);
    ui.treeWidget->setItemWidget(PathColorScheme,1,Path_Color_box);

    //机器人Navigate 相关UI********************************
    //Golabal Map***************************************
    QTreeWidgetItem* SlamPkg1=new QTreeWidgetItem(QStringList()<<"P80 SLAM");
    SlamPkg1->setIcon(0,QIcon("://images/default_package_icon.png"));
    QCheckBox* SlamPkg_Check1=new QCheckBox();
    connect(SlamPkg_Check1,SIGNAL(stateChanged(int)),this,SLOT(slot_toggle_p80_slam(int)));
    ui.treeWidget->addTopLevelItem(SlamPkg1);
    ui.treeWidget->setItemWidget(SlamPkg1,1,SlamPkg_Check1);

    //Global CostMap
    QTreeWidgetItem* Global_CostMap=new QTreeWidgetItem(QStringList()<<"Costmap");
    //设置图标
    Global_CostMap->setIcon(0,QIcon("://images/classes/Map.png"));
    //Global Map添加子节点
    // SlamPkg1->addChild(Global_CostMap);
    //Map topic
    QTreeWidgetItem* Global_CostMap_Topic=new QTreeWidgetItem(QStringList()<<"Topic");
    Global_CostMap_Topic_box=new QComboBox();
    Global_CostMap_Topic_box->addItem("/move_base/global_costmap/costmap");
    Global_CostMap_Topic_box->setEditable(true);
    Global_CostMap_Topic_box->setMaximumWidth(150);
    Global_CostMap->addChild(Global_CostMap_Topic);
    ui.treeWidget->setItemWidget(Global_CostMap_Topic,1,Global_CostMap_Topic_box);
    //Map color scheme
    QTreeWidgetItem* GlobalMapColorScheme=new QTreeWidgetItem(QStringList()<<"Color Scheme");
    GlobalMapColorScheme_box=new QComboBox();
    GlobalMapColorScheme_box->addItem("costmap");
    GlobalMapColorScheme_box->addItem("map");
    GlobalMapColorScheme_box->addItem("raw");
    GlobalMapColorScheme_box->setMaximumWidth(150);
    Global_CostMap->addChild(GlobalMapColorScheme);
    ui.treeWidget->setItemWidget(GlobalMapColorScheme,1,GlobalMapColorScheme_box);

    //Global Planner
    QTreeWidgetItem* Global_Planner=new QTreeWidgetItem(QStringList()<<"Planner");
    //设置图标
    Global_Planner->setIcon(0,QIcon("://images/classes/Path.png"));
    //向TGlobal Map添加Path Top节点
    // SlamPkg1->addChild(Global_Planner);

    //Path topic
    QTreeWidgetItem* Global_Planner_Topic=new QTreeWidgetItem(QStringList()<<"Topic");
    Global_Planner_Topic_box=new QComboBox();
    Global_Planner_Topic_box->addItem("/move_base/DWAPlannerROS/global_plan");
    Global_Planner_Topic_box->setEditable(true);
    Global_Planner_Topic_box->setMaximumWidth(150);
    Global_Planner->addChild(Global_Planner_Topic);
    ui.treeWidget->setItemWidget(Global_Planner_Topic,1,Global_Planner_Topic_box);
    //Path color scheme
    QTreeWidgetItem* Global_Planner_Color_Scheme=new QTreeWidgetItem(QStringList()<<"Color");
    Global_Planner_Color_box=new QComboBox();
    Global_Planner_Color_box->addItem("255;0;0");
    Global_Planner_Color_box->setEditable(true);
    Global_Planner_Color_box->setMaximumWidth(150);
    Global_Planner->addChild(Global_Planner_Color_Scheme);
    ui.treeWidget->setItemWidget(Global_Planner_Color_Scheme, 1, Global_Planner_Color_box);

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
    ui.treeWidget->setItemWidget(Slam_Odom_Topic1, 1, Slam_Odom_Topic_box1);

    //Local Map***********************************************
    QTreeWidgetItem* SlamPkg2=new QTreeWidgetItem(QStringList()<<"M1 SLAM");
    SlamPkg2->setIcon(0,QIcon("://images/default_package_icon.png"));
    QCheckBox* SlamPkg_Check2=new QCheckBox();
    connect(SlamPkg_Check2,SIGNAL(stateChanged(int)),this,SLOT(slot_toggle_m1_slam(int)));
    ui.treeWidget->addTopLevelItem(SlamPkg2);
    ui.treeWidget->setItemWidget(SlamPkg2,1,SlamPkg_Check2);

    //Local CostMap
    QTreeWidgetItem* Local_CostMap=new QTreeWidgetItem(QStringList()<<"Costmap");
    //设置图标
    Local_CostMap->setIcon(0,QIcon("://images/classes/Map.png"));
    //Local Map添加子节点
    // SlamPkg2->addChild(Local_CostMap);
    //Map topic
    QTreeWidgetItem* Local_CostMap_Topic=new QTreeWidgetItem(QStringList()<<"Topic");
    Local_CostMap_Topic_box=new QComboBox();
    Local_CostMap_Topic_box->addItem("/move_base/local_costmap/costmap");
    Local_CostMap_Topic_box->setEditable(true);
    Local_CostMap_Topic_box->setMaximumWidth(150);
    Local_CostMap->addChild(Local_CostMap_Topic);
    ui.treeWidget->setItemWidget(Local_CostMap_Topic,1,Local_CostMap_Topic_box);
    //Map color scheme
    QTreeWidgetItem* LocalMapColorScheme=new QTreeWidgetItem(QStringList()<<"Color Scheme");
    LocalMapColorScheme_box=new QComboBox();
    LocalMapColorScheme_box->addItem("costmap");
    LocalMapColorScheme_box->addItem("map");
    LocalMapColorScheme_box->addItem("raw");
    LocalMapColorScheme_box->setMaximumWidth(150);
    Local_CostMap->addChild(LocalMapColorScheme);
    ui.treeWidget->setItemWidget(LocalMapColorScheme,1,LocalMapColorScheme_box);

    //Local Planner
    QTreeWidgetItem* Local_Planner=new QTreeWidgetItem(QStringList()<<"Planner");
    //设置图标
    Local_Planner->setIcon(0,QIcon("://images/classes/Path.png"));
    //向TLocal Map添加Path Top节点
    // SlamPkg2->addChild(Local_Planner);

    //Path topic
    QTreeWidgetItem* Local_Planner_Topic=new QTreeWidgetItem(QStringList()<<"Topic");
    Local_Planner_Topic_box=new QComboBox();
    Local_Planner_Topic_box->addItem("/move_base/DWAPlannerROS/local_plan");
    Local_Planner_Topic_box->setEditable(true);
    Local_Planner_Topic_box->setMaximumWidth(150);
    Local_Planner->addChild(Local_Planner_Topic);
    ui.treeWidget->setItemWidget(Local_Planner_Topic,1,Local_Planner_Topic_box);
    //Path color scheme
    QTreeWidgetItem* Local_Planner_Color_Scheme=new QTreeWidgetItem(QStringList()<<"Color");
    Local_Planner_Color_box=new QComboBox();
    Local_Planner_Color_box->addItem("0;12;255");
    Local_Planner_Color_box->setEditable(true);
    Local_Planner_Color_box->setMaximumWidth(150);
    Local_Planner->addChild(Local_Planner_Color_Scheme);
    ui.treeWidget->setItemWidget(Local_Planner_Color_Scheme, 1, Local_Planner_Color_box);

    QTreeWidgetItem* Slam_Odom2 = new QTreeWidgetItem(QStringList() << "Odometry");
    //设置图标
    Slam_Odom2->setIcon(0, QIcon("://images/classes/Odometry.png"));
    //向TLocal Map添加Path Top节点
    SlamPkg2->addChild(Slam_Odom2);

    //Path topic
    QTreeWidgetItem* Slam_Odom_Topic2 = new QTreeWidgetItem(QStringList() << "Topic");
    Slam_Odom_Topic_box2 = new QComboBox();
    Slam_Odom_Topic_box2->addItem("/Odometry");
    Slam_Odom_Topic_box2->setEditable(true);
    Slam_Odom_Topic_box2->setMaximumWidth(150);
    SlamPkg2->addChild(Slam_Odom_Topic2);
    ui.treeWidget->setItemWidget(Slam_Odom_Topic2, 1, Slam_Odom_Topic_box2);


    //connect
    connect(&qnode,SIGNAL(speed_vel(float,float)),this,SLOT(slot_update_dashboard(float,float)));
    // connect(&qnode,SIGNAL(power_vel(float)),this,SLOT(slot_update_power(float)));
    connect(&qnode,SIGNAL(image_val(QImage)),this,SLOT(slot_update_image(QImage)));
    connect(&qnode,SIGNAL(position(double,double,double)),this,SLOT(slot_update_pos(double,double,double)));
    connect(ui.pushButton_sub_image,SIGNAL(clicked()),this,SLOT(slot_sub_image()));
    connect(ui.laser_btn,SIGNAL(clicked()),this,SLOT(slot_quick_cmd_clicked()));
    //set start pose
    connect(ui.m1_det_btn,SIGNAL(clicked()),this,SLOT(slot_toggle_m1_det()));
    connect(ui.p80_det_btn, SIGNAL(clicked()), this, SLOT(slot_toggle_p80_det()));
    connect(ui.m1_slam_btn, SIGNAL(clicked()), this, SLOT(slot_toggle_m1_slam()));
    connect(ui.p80_slam_btn, SIGNAL(clicked()), this, SLOT(slot_toggle_p80_slam()));


}
void MainWindow::slot_toggle_m1_slam()
{
  ui.return_x->setText(ui.pos_x->text());
   ui.return_y->setText(ui.pos_y->text());
    ui.return_z->setText(ui.pos_z->text());
}
void MainWindow::slot_toggle_p80_slam()
{
  qnode.set_goal(ui.return_x->text().toDouble(),ui.return_y->text().toDouble(),ui.return_z->text().toDouble());
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
void MainWindow::slot_toggle_m1_det()
{
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
void MainWindow::slot_toggle_p80_det()
{
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
    laser_cmd=new QProcess;
    laser_cmd->start("bash");
    laser_cmd->write(ui.textEdit_laser_cmd->toPlainText().toLocal8Bit()+'\n');
    connect(laser_cmd,SIGNAL(readyReadStandardError()),this,SLOT(slot_quick_output()));
    connect(laser_cmd,SIGNAL(readyReadStandardOutput()),this,SLOT(slot_quick_output()));

}
void MainWindow::slot_quick_output()
{
    ui.textEdit_quick_output->append("<font color=\"#FF0000\">"+laser_cmd->readAllStandardError()+"</font>");
    ui.textEdit_quick_output->append("<font color=\"#FFFFFF\">"+laser_cmd->readAllStandardOutput()+"</font>");
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
  QPushButton* btn=qobject_cast<QPushButton*> (sender());
  char k=btn->text().toStdString()[0];
  bool is_all=ui.checkBox_is_all->isChecked();
  float linear=ui.label_linera->text().toFloat()*0.01;
  float angular=ui.label_raw->text().toFloat()*0.01;

  switch (k) {
    case 'i':
      qnode.set_cmd_vel(is_all?'I':'i',linear,angular);
      break;
  case 'u':
    qnode.set_cmd_vel(is_all?'U':'u',linear,angular);
    break;
  case 'o':
    qnode.set_cmd_vel(is_all?'O':'o',linear,angular);
    break;
  case 'j':
    qnode.set_cmd_vel(is_all?'J':'j',linear,angular);
    break;
  case 'l':
    qnode.set_cmd_vel(is_all?'L':'l',linear,angular);
    break;
  case 'm':
    qnode.set_cmd_vel(is_all?'M':'m',linear,angular);
    break;
  case ',':
    qnode.set_cmd_vel(is_all?'<':',',linear,angular);
    break;
  case '.':
    qnode.set_cmd_vel(is_all?'>':'.',linear,angular);
    break;
  }
}
void MainWindow::slot_linera_value_change(int value)
{
    ui.label_linera->setText(QString::number(value));
}
void MainWindow::slot_raw_value_change(int value)
{
    ui.label_raw->setText(QString::number(value));
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

