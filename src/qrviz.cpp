#include "../include/qt_panel/qrviz.hpp"
#include <QDebug>
#include <QException>
#include <rviz/view_manager.h>
qrviz::qrviz(QVBoxLayout* layout)
{

    //创建rviz panel
    render_panel_=new rviz::RenderPanel();
    //向layout添加
    layout->addWidget(render_panel_);
    //创建rviz控制对象
    manager_=new rviz::VisualizationManager(render_panel_);
    tool_manager_=manager_->getToolManager();
    ROS_ASSERT(manager_!=NULL);
    //初始化render_panel 实现放大缩小等操作
    render_panel_->initialize(manager_->getSceneManager(),manager_);

    manager_->setFixedFrame("map");
    //初始化rviz控制对象
    manager_->initialize();
    manager_->startUpdate();
    manager_->removeAllDisplays();

 }
void qrviz::Display_Local_Map(QString map_topic,QString map_color,QString planner_topic,QColor planner_color,bool enable)
{
    if(Local_Map_!=NULL||Local_Planner_!=NULL)
    {
        delete Local_Map_;
        delete  Local_Planner_;
        Local_Map_=NULL;
        Local_Planner_=NULL;
    }
    Local_Map_=manager_->createDisplay("rviz/Map","LocalMap",enable);
    ROS_ASSERT(Local_Map_!=NULL);
    Local_Planner_=manager_->createDisplay("rviz/Path","LocalPlanner",enable);
    ROS_ASSERT(Local_Planner_!=NULL);
    Local_Map_->subProp("Topic")->setValue(map_topic);
    Local_Map_->subProp("Color Scheme")->setValue(map_color);
//    Local_Planner_->subProp("Topic")->setValue(planner_topic);
//    Local_Planner_->subProp("Color")->setValue(planner_color);


}
void qrviz::Display_Global_Map(QString map_topic,QString map_color,QString planner_topic,QColor planner_color,bool enable)
{
    if(Global_Map_!=NULL||Global_Planner_!=NULL)
    {
        delete Global_Map_;
        delete  Global_Planner_;
        Global_Map_=NULL;
        Global_Planner_=NULL;
    }
    Global_Map_=manager_->createDisplay("rviz/Map","GlobalMap",enable);
    ROS_ASSERT(Global_Map_!=NULL);
    Global_Planner_=manager_->createDisplay("rviz/Path","GlobalPlanner",enable);
    ROS_ASSERT(Global_Planner_!=NULL);
    Global_Map_->subProp("Topic")->setValue(map_topic);
    Global_Map_->subProp("Color Scheme")->setValue(map_color);
//    Global_Planner_->subProp("Topic")->setValue(planner_topic);
//    Global_Planner_->subProp("Color")->setValue(planner_color);


}
void qrviz::Set_Start_Pose()
{
    rviz::Tool* current_tool_=tool_manager_->addTool("rviz/SetInitialPose");
    //设置当前使用的工具
    tool_manager_->setCurrentTool(current_tool_);
}
void qrviz::Set_Goal_Pose()
{
     rviz::Tool* current_tool_=tool_manager_->addTool("rviz/SetGoal");
     //获取属性容器
     rviz::Property* pro=current_tool_->getPropertyContainer();
     //设置发布导航目标点的topic
     pro->subProp("Topic")->setValue("/move_base_simple/goal");
     //设置当前使用的工具
     tool_manager_->setCurrentTool(current_tool_);
}
void qrviz::Set_FixedFrame(QString Frame_name)
{
    manager_->setFixedFrame(Frame_name);
    qDebug()<<"Fixed frame set to: "<<manager_->getFixedFrame();
}
void qrviz::Display_Grid(int Cell_Count,QColor color,bool enable)
{
    if(Grid_!=NULL)
    {
        delete Grid_;
        Grid_=NULL;
    }
    Grid_=manager_->createDisplay("rviz/Grid","myGrid",enable);
    //设置cell Count
    Grid_->subProp("Plane Cell Count")->setValue(Cell_Count);
    //设置颜色
    Grid_->subProp("Color")->setValue(color);
    ROS_ASSERT(Grid_!=NULL);

}
void qrviz::Display_Axes(bool enable) {
    if (Axes_ != NULL) {
        delete Axes_;
        Axes_ = NULL;
    }
    Axes_ = manager_->createDisplay("rviz/Axes", "myAxes", enable);
    Axes_->subProp("Length")->setValue(0.5);
    ROS_ASSERT(Axes_ != NULL);
}
void qrviz::Display_TF(bool enable)
{
    if(TF_!=NULL)
    {
        delete TF_;
        TF_=NULL;
    }
    TF_=manager_->createDisplay("rviz/TF","myTF",enable);
    ROS_ASSERT(TF_!=NULL);
}
void qrviz::Display_LaserScan(QString laser_topic,bool enable)
{
    if(LaserScan_!=NULL)
    {
        delete LaserScan_;
        LaserScan_=NULL;
    }
    LaserScan_=manager_->createDisplay("rviz/LaserScan","myLaser",enable);
    LaserScan_->subProp("Topic")->setValue(laser_topic);
    ROS_ASSERT(LaserScan_!=NULL);
}
void qrviz::Display_PointCloud2_1(QString pointcloud_topic, bool enable) {
    if (PointCloud2_1 != NULL) {
        delete PointCloud2_1;
        PointCloud2_1 = NULL;
    }
    PointCloud2_1 = manager_->createDisplay("rviz/PointCloud2", "myPointCloud2_1", enable);
    PointCloud2_1->subProp("Topic")->setValue(pointcloud_topic);
    PointCloud2_1->subProp("Style")->setValue("Points");
    ROS_ASSERT(PointCloud2_1 != NULL);
}
void qrviz::Display_MarkerArray_1(QString markerarray_topic, bool enable) {
    if (MarkerArray_1 != NULL) {
        delete MarkerArray_1;
        MarkerArray_1 = NULL;
    }
    MarkerArray_1 = manager_->createDisplay("rviz/MarkerArray", "myMarkerArray1", enable);
    MarkerArray_1->subProp("Marker Topic")->setValue(markerarray_topic);
    ROS_ASSERT(MarkerArray_1 != NULL);
}
void qrviz::Display_PointCloud2_2(QString pointcloud_topic, bool enable) {
    if (PointCloud2_2 != NULL) {
        delete PointCloud2_2;
        PointCloud2_2 = NULL;
    }
    PointCloud2_2 = manager_->createDisplay("rviz/PointCloud2", "myPointCloud2_2", enable);
    PointCloud2_2->subProp("Topic")->setValue(pointcloud_topic);
    PointCloud2_2->subProp("Style")->setValue("Points");
    ROS_ASSERT(PointCloud2_2 != NULL);
}
void qrviz::Display_MarkerArray_2(QString markerarray_topic, bool enable) {
    if (MarkerArray_2 != NULL) {
        delete MarkerArray_2;
        MarkerArray_2 = NULL;
    }
    MarkerArray_2 = manager_->createDisplay("rviz/MarkerArray", "myMarkerArray2", enable);
    MarkerArray_2->subProp("Marker Topic")->setValue(markerarray_topic);
    ROS_ASSERT(MarkerArray_2 != NULL);
}
void qrviz::Display_RobotModel(bool enable)
{
    if(RobotModel_!=NULL)
    {
        delete RobotModel_;
        RobotModel_=NULL;
    }
    RobotModel_=manager_->createDisplay("rviz/RobotModel","myRobotModel",enable);
    ROS_ASSERT(RobotModel_!=NULL);
}
void qrviz::Display_Map(QString topic,QString color_scheme,bool enable)
{
    if(Map_!=NULL)
    {
        delete Map_;
        Map_=NULL;
    }
    Map_=manager_->createDisplay("rviz/Map","myMap",enable);
    ROS_ASSERT(Map_!=NULL);
    Map_->subProp("Topic")->setValue(topic);
    Map_->subProp("Color Scheme")->setValue(color_scheme);
}
void qrviz::Display_Path(QString topic,QColor color,bool enable)
{
    if(Path_!=NULL)
    {
        delete Path_;
        Path_=NULL;
    }
    Path_=manager_->createDisplay("rviz/Path","myPath",enable);
    ROS_ASSERT(Path_!=NULL);
    Path_->subProp("Topic")->setValue(topic);
    Path_->subProp("Color")->setValue(color);
}
