#pragma once

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMessagebox>
#include <QFileDialog>
#include <QMouseEvent>
#include <QTimer>
#include "ui_PlumbLineInstrument.h"


#include "opencv2/core/core_c.h"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>
#include <numeric>


#include "camera.h"
#include "cameraCali.h"
#include "planeCali.h"
#include "levelCali.h"




#define IMAGE_WIDTH ( 2560 )
#define IMAGE_HEIGHT ( 2048 )
#define LABEL_WIDTH ( 400 )
#define LABEL_HEIGHT ( 500 )
#define LEFT_FOLDER "C:\\Users\\jzpwh\\Desktop\\xy\\calibrateExperiments\\18\\left\\"
#define RIGHT_FOLDER "C:\\Users\\jzpwh\\Desktop\\xy\\calibrateExperiments\\18\\right\\"
#define DATA_FOLDER "C:\\Users\\jzpwh\\Desktop\\xy\\calibrateExperiments\\18\\"


using namespace std;
using namespace cv;



class PlumbLineInstrument : public QMainWindow
{
    Q_OBJECT

public:
    PlumbLineInstrument(QWidget *parent = Q_NULLPTR);

private:
    Ui::PlumbLineInstrumentClass ui;


private slots:
	void on_startCameraBtn_clicked();
	void on_stopCameraBtn_clicked();
	void on_saveFrameBtn_clicked();
	void readFrame(); // 用定时器控制读取相机数据的帧率

	void on_OpenCVCaliBtn_clicked();
	void on_planeCaliBtn_clicked();
	void on_levelCaliBtn_clicked();
	void on_OpenCVParamsBtn_clicked();
	void on_planeParamsBtn_clicked();

	void on_startMeasureBtn_clicked();
	void on_stopMeasureBtn_clicked();

	void on_testBtn_clicked();


public:
	QImage CvMat2QImage(const Mat &mat);
	Mat QImage2CvMat(const QImage &image);


	Point3f uv2xwywzw(Point2f& uvLeft, Point2f& uvRight);
	Point3f uv2xwywzw(Point& uv, Mat& M);
	void uv2xwywzw(Point& uv, Point3f& xwywzw, Mat& M);
	Point3f uv2xcyczc(Point& uv, Mat& M);


	bool findIntersection(Mat& frame, Point& intersection);
	bool findIntersection(Mat& frame, Point& leftPoint, Point& rightPoint);

	bool findBallCenter(Mat& frame, Point& ballCenter);
	bool findTwoBallCenter(Mat& frame, Point& ball_1, Point& ball_2);

	void measureOneBall(Mat& frame, Mat& result, Point3f& coordinate);
	void measureTwoBalls(Mat& frame, Mat& result, Point3f& coord1, Point3f& coord2, float& d);

	void test();

	void calculateBallCoord(Point& Puv_ball, Point3f& Pc_laser, Point3f& Pc_ball);



public:
	QTimer *timer;
	double rate = 30;
	int num = 0;
	bool flagStartCamera = false;
	bool flagStartMeasure = false;
	Camera camera;
	//CameraCali cameraCalibrate = CameraCali(LEFT_FOLDER, RIGHT_FOLDER, DATA_FOLDER, IMAGE_WIDTH, IMAGE_HEIGHT);
	//PlaneCali planeCali = PlaneCali(LEFT_FOLDER, RIGHT_FOLDER, DATA_FOLDER, IMAGE_WIDTH, IMAGE_HEIGHT);
	CameraCali cameraCalibrate = CameraCali(LEFT_FOLDER, RIGHT_FOLDER, DATA_FOLDER, IMAGE_HEIGHT, IMAGE_WIDTH); // 相机旋转90度后，将画面的画幅也旋转
	PlaneCali planeCali = PlaneCali(LEFT_FOLDER, RIGHT_FOLDER, DATA_FOLDER, IMAGE_HEIGHT, IMAGE_WIDTH); // 相机旋转90度后，将画面的画幅也旋转


	Size imageSize = Size(IMAGE_WIDTH, IMAGE_HEIGHT);
	Mat frameLeft = Mat(imageSize, CV_8UC1);
	Mat frameRight = Mat(imageSize, CV_8UC1);
	Size imageSize1 = Size(IMAGE_HEIGHT, IMAGE_WIDTH); // 相机旋转90度放置后
	Mat frameLeft1 = Mat(imageSize1, CV_8UC1);
	Mat frameRight1 = Mat(imageSize1, CV_8UC1);
	float start_time, stop_time; // 处理时间

	double alpha = 0.28975; // 求垂线z方向位移的参数
	double theta = 0.201;
	double L0 = 25.68;
	double h0 = 43.16;
	double y0 = 227.253;

	const char* imageName_L = "C:\\Users\\jzpwh\\Desktop\\xy\\calibrateExperiments\\09\\right06.jpg"; // 用于检测深度的图像
	const char* imageName_R = "C:\\Users\\jzpwh\\Desktop\\xy\\calibrateExperiments\\09\\测试图像04_向右平移四格.jpg";
	const char* imageList_L = "caliberationpics_L.txt"; // 左相机的标定图片名称列表
	const char* imageList_R = "caliberationpics_R.txt"; // 右相机的标定图片名称列表
	const char* singleCalibrate_result_L = "calibrationresults_L.yml"; // 存放左相机的标定结果
	const char* singleCalibrate_result_R = "calibrationresults_R.yml"; // 存放右相机的标定结果
	const char* stereoCalibrate_result_L = "stereocalibrateresult_L.yml"; // 存放立体标定结果
	const char* stereoCalibrate_result_R = "stereocalibrateresult_R.yml";
	const char* stereoRectifyParams = "stereoRectifyParams.yml"; // 存放立体校正参数
	Mat cameraMatrix_L = Mat(3, 3, CV_32FC1, Scalar::all(0)); // 相机的内参数
	Mat cameraMatrix_R = Mat(3, 3, CV_32FC1, Scalar::all(0)); // 初始化相机的内参数
	Mat distCoeffs_L = Mat(1, 5, CV_32FC1, Scalar::all(0)); // 相机的畸变系数
	Mat distCoeffs_R = Mat(1, 5, CV_32FC1, Scalar::all(0)); // 初始化相机的畸变系数
	Mat rotationVector_L = Mat(3, 1, CV_32FC1, Scalar::all(0)); // 第一张图片标定板对应的世界坐标系的旋转向量
	Mat rotationVector_R = Mat(3, 1, CV_32FC1, Scalar::all(0));
	Mat rotationMatrix_L = Mat(3, 3, CV_32FC1, Scalar::all(0)); // 第一张图片标定板对应的世界坐标系的旋转矩阵
	Mat rotationMatrix_R = Mat(3, 3, CV_32FC1, Scalar::all(0));
	Mat translationVector_L = Mat(3, 1, CV_32FC1, Scalar::all(0)); // 第一张图片标定板对应的世界坐标系的平移向量
	Mat translationVector_R = Mat(3, 1, CV_32FC1, Scalar::all(0));
	Mat extrinsicMatrix_L = Mat(3, 4, CV_32FC1, Scalar::all(0)); // 第一张图片标定板对应的世界坐标系的外参数
	Mat extrinsicMatrix_R = Mat(3, 4, CV_32FC1, Scalar::all(0));
	Mat projectionMatrix_L = Mat(3, 4, CV_32FC1, Scalar::all(0)); // 第一张图片标定板对应的世界坐标系的投影矩阵
	Mat projectionMatrix_R = Mat(3, 4, CV_32FC1, Scalar::all(0));
	Mat R, T, E, F; // 立体标定参数
	Mat R1, R2, P1, P2, Q; // 立体校正参数
	Mat mapl1, mapl2, mapr1, mapr2; // 图像重投影映射表	立体矫正校正
	Mat maplx, maply, maprx, mapry; // 图像重投影映射表	左右相机单目矫正


	const char* rayLineimageList_L = "rayLinecaliberationpics_L.txt"; // 左相机激光平面标定的图片名称列表
	const char* rayLineimageList_R = "rayLinecaliberationpics_R.txt"; // 右相机激光平面标定的图片名称列表
	const char* uvPoints_L = "uvPoints_L.txt"; // 存放图像上激光光条中心点数据
	const char* uvPoints_R = "uvPoints_R.txt";
	const char* realPoints_L = "realPoints_L.txt"; // 存放激光平面特征点数据
	const char* realPoints_R = "realPoints_R.txt";
	const char* caliPoints_L = "caliPoints_L.txt"; // 存放根据标定参数计算的激光平面特征点数据
	const char* caliPoints_R = "caliPoints_R.txt";
	const char* caliErr_L = "caliErr_L.txt"; // 存放标定误差
	const char* caliErr_R = "caliErr_R.txt";
	const char* laserPlaneCali_result_L = "laserPlaneParams_L.yml"; // 存放左相机激光平面的标定结果
	const char* laserPlaneCali_result_R = "laserPlaneParams_R.yml"; // 存放右相机激光平面的标定结果
	Mat laserPlaneParams_L = Mat(3, 1, CV_32FC1); // 左右相机的平面标定参数 A B C
	Mat laserPlaneParams_R = Mat(3, 1, CV_32FC1);


	Mat levelPlaneParams = Mat(3, 1, CV_32FC1); // 左相机水平面标定参数 A B C


};
