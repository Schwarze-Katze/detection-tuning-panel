import sys
from PyQt5.QtCore import QTimer, QSize, QSharedMemory, QBuffer, QDataStream
from PyQt5.QtWidgets import QApplication, QWidget, QVBoxLayout, QHBoxLayout, QPushButton, QLabel
from PyQt5.QtGui import QImage, QPixmap
import cv2


class MyWidget(QWidget):
    def __init__(self, parent=None):
        super(MyWidget, self).__init__(parent)
        self.initUI()
        self.capture1 = cv2.VideoCapture('scripts/video1.mp4')
        self.capture2 = cv2.VideoCapture('scripts/video2.mp4')
        self.setupVideoStreams()

    def initUI(self):
        # 创建垂直布局
        mainLayout = QVBoxLayout()
        mainLayout.setContentsMargins(10, 10, 10, 10)
        mainLayout.setSpacing(5)

        buttonLayout = QHBoxLayout()
        videoLayout = QHBoxLayout()

        # 创建三个模式切换按钮
        button1 = QPushButton("Mode 1")
        button2 = QPushButton("Mode 2")
        button3 = QPushButton("Mode 3")

        # 将按钮添加到布局的第一行
        buttonLayout.addWidget(button1)
        buttonLayout.addWidget(button2)
        buttonLayout.addWidget(button3)
        mainLayout.addLayout(buttonLayout)

        # 连接按钮点击信号到相应的槽函数
        button1.clicked.connect(self.onMode1Clicked)
        button2.clicked.connect(self.onMode2Clicked)
        button3.clicked.connect(self.onMode3Clicked)

        # 创建两个图像显示区域
        self.videoLabel1 = QLabel(self)
        self.videoLabel2 = QLabel(self)

        # 设置标签大小和边框
        # self.videoLabel1.setFixedSize(320, 240)
        self.videoLabel1.setStyleSheet("border: 1px solid black;")
        # self.videoLabel2.setFixedSize(320, 240)
        self.videoLabel2.setStyleSheet("border: 1px solid black;")

        # 将图像显示区域添加到布局的第二行
        videoLayout.addWidget(self.videoLabel1, stretch=1)
        videoLayout.addWidget(self.videoLabel2, stretch=1)
        mainLayout.addLayout(videoLayout)

        mainLayout.sizeHint = lambda: QSize(1024, 768)
        self.setLayout(mainLayout)

    def setupVideoStreams(self):
        self.shm = QSharedMemory()
        self.shm.setKey("detect1")
        # 设置定时器以定期更新视频帧
        self.timer = QTimer(self)
        self.timer.timeout.connect(self.updateFrames)
        fps = self.capture1.get(cv2.CAP_PROP_FPS)
        self.timer.start(100)  # 1000/100 = 10fps

    def updateFrames(self):
        ret1, frame1 = self.capture1.read()
        ret2, frame2 = self.capture2.read()

        buffer1=QBuffer()
        buffer1.open(QBuffer.ReadWrite)
        out1=QDataStream(buffer1)
        out1.writeRawData(frame1.data)
        
        if (not self.shm.create(buffer1.size())):
            print("无法创建共享内存段！")
            return
        self.shm.lock()
        
        # 从视频流中读取帧

        if ret1 and ret2:
            # 将OpenCV的Mat格式转为QImage
            qimg1 = QImage(
                frame1.data, frame1.shape[1], frame1.shape[0], frame1.strides[0], QImage.Format_RGB888)
            qimg2 = QImage(
                frame2.data, frame2.shape[1], frame2.shape[0], frame2.strides[0], QImage.Format_RGB888)

            def cropSize(s: QSize):
                s.setWidth(s.width()-2)
                s.setHeight(s.height()-2)
                return s
            
            # 显示图像
            self.videoLabel1.setPixmap(QPixmap.fromImage(qimg1).scaled(
                cropSize(self.videoLabel1.size()), aspectRatioMode=1))  # border: 1px
            self.videoLabel2.setPixmap(QPixmap.fromImage(qimg2).scaled(
                cropSize(self.videoLabel2.size()), aspectRatioMode=1))

    def onMode1Clicked(self):
        print("Mode 1 clicked")

    def onMode2Clicked(self):
        print("Mode 2 clicked")

    def onMode3Clicked(self):
        print("Mode 3 clicked")


if __name__ == "__main__":
    app = QApplication(sys.argv)
    widget = MyWidget()
    widget.show()
    app.exec_()
