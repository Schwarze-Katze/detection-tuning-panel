#pragma once
#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QLayout>
#include <QPushButton>
#include <QLabel>
#include <QTimer>
#include <opencv2/opencv.hpp>

class myWidget : public QWidget {
    Q_OBJECT;
public:
    myWidget(QWidget* parent = 0);
    ~myWidget();

private:
    QLabel* videoLabel1, * videoLabel2;

private:
    void initUI();
    void setupVideoStreams();
    void updateFrames();

private slots:
    void onMode1Clicked();
    void onMode2Clicked();
    void onMode3Clicked();
};

#endif //WIDGET_H