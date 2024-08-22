#include "widget.h"

myWidget::myWidget(QWidget* parent) : QWidget(parent) {
    initUI();
    setupVideoStreams();
}

myWidget::~myWidget() { }

void myWidget::initUI() {
    // 创建网格布局
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setMargin(10);
    mainLayout->setSpacing(5);

    QHBoxLayout* buttonLayout = new QHBoxLayout(this);
    QHBoxLayout* videoLayout = new QHBoxLayout(this);

    // 创建三个模式切换按钮
    QPushButton* button1 = new QPushButton("Mode 1");
    QPushButton* button2 = new QPushButton("Mode 2");
    QPushButton* button3 = new QPushButton("Mode 3");

    // 将按钮添加到布局的第一行
    buttonLayout->addWidget(button1);
    buttonLayout->addWidget(button2);
    buttonLayout->addWidget(button3);
    mainLayout->addLayout(buttonLayout);

    // 连接按钮点击信号到相应的槽函数
    connect(button1, &QPushButton::clicked, this, &myWidget::onMode1Clicked);
    connect(button2, &QPushButton::clicked, this, &myWidget::onMode2Clicked);
    connect(button3, &QPushButton::clicked, this, &myWidget::onMode3Clicked);

    // 创建两个图像显示区域
    videoLabel1 = new QLabel(this);
    videoLabel2 = new QLabel(this);

    // 设置标签大小和边框
    videoLabel1->setFixedSize(320, 240);
    videoLabel1->setStyleSheet("border: 1px solid black;");
    videoLabel2->setFixedSize(320, 240);
    videoLabel2->setStyleSheet("border: 1px solid black;");

    // 将图像显示区域添加到布局的第二行
    videoLayout->addWidget(videoLabel1);
    videoLayout->addWidget(videoLabel2);
    mainLayout->addLayout(videoLayout);

    this->setLayout(mainLayout);
}

void myWidget::setupVideoStreams() {

    // 设置定时器以定期更新视频帧
    QTimer* timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &myWidget::updateFrames);
    timer->start(33); // 大约每33ms更新一次，约30fps
}

void myWidget::updateFrames() {
    cv::Mat frame1, frame2;

    // 从视频流中读取帧

    if (!frame1.empty() and !frame2.empty()) {
        // 将OpenCV的Mat格式转为QImage
        QImage qimg1(frame1.data, frame1.cols, frame1.rows, frame1.step, QImage::Format_RGB888);
        QImage qimg2(frame2.data, frame2.cols, frame2.rows, frame2.step, QImage::Format_RGB888);

        // 显示图像
        videoLabel1->setPixmap(QPixmap::fromImage(qimg1).scaled(videoLabel1->size(), Qt::KeepAspectRatio));
        videoLabel2->setPixmap(QPixmap::fromImage(qimg2).scaled(videoLabel2->size(), Qt::KeepAspectRatio));
    }
}

void myWidget::onMode1Clicked() {
    // 处理Mode 1的功能
}

void myWidget::onMode2Clicked() {
    // 处理Mode 2的功能
}

void myWidget::onMode3Clicked() {
    // 处理Mode 3的功能
}