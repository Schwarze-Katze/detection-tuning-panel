/**
 * @file /include/qt_panel/main_window.hpp
 *
 * @brief Qt based gui for qt_panel.
 *
 * @date November 2010
 **/
#ifndef qt_panel_MAIN_WINDOW_H
#define qt_panel_MAIN_WINDOW_H

/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtWidgets/QMainWindow>
#include "ui_main_window.h"
#include "qnode.hpp"
#include "CCtrlDashBoard.h"
#include <QImage>
#include <QProcess>
#include <QComboBox>
#include <QSpinBox>
#include "qrviz.hpp"
/*****************************************************************************
** Namespace
*****************************************************************************/

namespace qt_panel {

/*****************************************************************************
** Interface [MainWindow]
*****************************************************************************/
/**
 * @brief Qt central, all operations relating to the view part here.
 */
class MainWindow : public QMainWindow {
Q_OBJECT

public:
	MainWindow(int argc, char** argv, QWidget *parent = 0);
	~MainWindow();

	void ReadSettings(); // Load up qt program settings at startup
	void WriteSettings(); // Save qt program settings when closing

	void closeEvent(QCloseEvent *event); // Overloaded function
	void showNoMasterMessage();

public Q_SLOTS:
	/******************************************
	** Auto-connections (connectSlotsByName())
	*******************************************/
	void on_actionAbout_triggered();
	void on_button_connect_clicked(bool check );
	void on_checkbox_use_environment_stateChanged(int state);

    /******************************************
    ** Manual connections
    *******************************************/
    void updateLoggingView(); // no idea why this can't connect automatically
    void slot_linera_value_change(int);
    void slot_raw_value_change(int);
    void slot_pushbtn_click();
    void slot_update_dashboard(float,float);
    void slot_update_power(float);
    void slot_update_image(QImage);
    void slot_sub_image();
    void slot_quick_cmd_clicked();
    void slot_quick_output();
    void slot_treewidget_value_change(QString);
    void slot_display_grid(int);
    void slot_display_axes(int);
    void slot_display_tf(int);
    void slot_display_laser(int);
    void slot_display_detect1(int);
    void slot_display_detect2(int);
    void slot_display_RobotModel(int);
    void slot_display_Map(int);
    void slot_display_Path(int);
    void slot_toggle_m1_det();
    void slot_toggle_p80_det();
    void slot_display_local_map(int state);
    void slot_display_global_map(int state);
    void slot_update_pos(double,double,double);
    void slot_toggle_m1_slam();
    void slot_toggle_p80_slam();
private:
	Ui::MainWindowDesign ui;
	QNode qnode;
    CCtrlDashBoard* speed_x_dashBoard;
    CCtrlDashBoard* speed_y_dashBoard;
    QProcess *laser_cmd;
    qrviz* myrviz;
    QComboBox* fixed_box;
    QSpinBox* Cell_Count_Box;
    QComboBox* Grid_Color_Box;
    QComboBox* Laser_Topic_box;
    QCheckBox* DetectPkg_Check1;
    QComboBox* PointCloud_Topic_box1;
    QComboBox* MarkerArray_Topic_box1;
    QCheckBox* DetectPkg_Check2;
    QComboBox* PointCloud_Topic_box2;
    QComboBox* MarkerArray_Topic_box2;
    QComboBox* Map_Topic_box;
    QComboBox* Map_Color_Scheme_box;
    QComboBox* Path_Topic_box;
    QComboBox* Path_Color_box;
    QComboBox* Slam_Odom_Topic_box1;
    QComboBox* Slam_Odom_Topic_box2;
    //Navigate
    QComboBox* Global_CostMap_Topic_box;
    QComboBox* GlobalMapColorScheme_box;
    QComboBox* Local_CostMap_Topic_box;
    QComboBox* LocalMapColorScheme_box;
    QComboBox* Global_Planner_Topic_box;
    QComboBox* Global_Planner_Color_box;
    QComboBox* Local_Planner_Topic_box;
    QComboBox* Local_Planner_Color_box;

    double return_x;
    double return_y;
    double return_z;

// signals:
};

}  // namespace qt_panel

#endif // qt_panel_MAIN_WINDOW_H
