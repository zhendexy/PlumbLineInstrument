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
	void readFrame(); // �ö�ʱ�����ƶ�ȡ������ݵ�֡��

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
	CameraCali cameraCalibrate = CameraCali(LEFT_FOLDER, RIGHT_FOLDER, DATA_FOLDER, IMAGE_HEIGHT, IMAGE_WIDTH); // �����ת90�Ⱥ󣬽�����Ļ���Ҳ��ת
	PlaneCali planeCali = PlaneCali(LEFT_FOLDER, RIGHT_FOLDER, DATA_FOLDER, IMAGE_HEIGHT, IMAGE_WIDTH); // �����ת90�Ⱥ󣬽�����Ļ���Ҳ��ת


	Size imageSize = Size(IMAGE_WIDTH, IMAGE_HEIGHT);
	Mat frameLeft = Mat(imageSize, CV_8UC1);
	Mat frameRight = Mat(imageSize, CV_8UC1);
	Size imageSize1 = Size(IMAGE_HEIGHT, IMAGE_WIDTH); // �����ת90�ȷ��ú�
	Mat frameLeft1 = Mat(imageSize1, CV_8UC1);
	Mat frameRight1 = Mat(imageSize1, CV_8UC1);
	float start_time, stop_time; // ����ʱ��

	double alpha = 0.28975; // ����z����λ�ƵĲ���
	double theta = 0.201;
	double L0 = 25.68;
	double h0 = 43.16;
	double y0 = 227.253;

	const char* imageName_L = "C:\\Users\\jzpwh\\Desktop\\xy\\calibrateExperiments\\09\\right06.jpg"; // ���ڼ����ȵ�ͼ��
	const char* imageName_R = "C:\\Users\\jzpwh\\Desktop\\xy\\calibrateExperiments\\09\\����ͼ��04_����ƽ���ĸ�.jpg";
	const char* imageList_L = "caliberationpics_L.txt"; // ������ı궨ͼƬ�����б�
	const char* imageList_R = "caliberationpics_R.txt"; // ������ı궨ͼƬ�����б�
	const char* singleCalibrate_result_L = "calibrationresults_L.yml"; // ���������ı궨���
	const char* singleCalibrate_result_R = "calibrationresults_R.yml"; // ���������ı궨���
	const char* stereoCalibrate_result_L = "stereocalibrateresult_L.yml"; // �������궨���
	const char* stereoCalibrate_result_R = "stereocalibrateresult_R.yml";
	const char* stereoRectifyParams = "stereoRectifyParams.yml"; // �������У������
	Mat cameraMatrix_L = Mat(3, 3, CV_32FC1, Scalar::all(0)); // ������ڲ���
	Mat cameraMatrix_R = Mat(3, 3, CV_32FC1, Scalar::all(0)); // ��ʼ��������ڲ���
	Mat distCoeffs_L = Mat(1, 5, CV_32FC1, Scalar::all(0)); // ����Ļ���ϵ��
	Mat distCoeffs_R = Mat(1, 5, CV_32FC1, Scalar::all(0)); // ��ʼ������Ļ���ϵ��
	Mat rotationVector_L = Mat(3, 1, CV_32FC1, Scalar::all(0)); // ��һ��ͼƬ�궨���Ӧ����������ϵ����ת����
	Mat rotationVector_R = Mat(3, 1, CV_32FC1, Scalar::all(0));
	Mat rotationMatrix_L = Mat(3, 3, CV_32FC1, Scalar::all(0)); // ��һ��ͼƬ�궨���Ӧ����������ϵ����ת����
	Mat rotationMatrix_R = Mat(3, 3, CV_32FC1, Scalar::all(0));
	Mat translationVector_L = Mat(3, 1, CV_32FC1, Scalar::all(0)); // ��һ��ͼƬ�궨���Ӧ����������ϵ��ƽ������
	Mat translationVector_R = Mat(3, 1, CV_32FC1, Scalar::all(0));
	Mat extrinsicMatrix_L = Mat(3, 4, CV_32FC1, Scalar::all(0)); // ��һ��ͼƬ�궨���Ӧ����������ϵ�������
	Mat extrinsicMatrix_R = Mat(3, 4, CV_32FC1, Scalar::all(0));
	Mat projectionMatrix_L = Mat(3, 4, CV_32FC1, Scalar::all(0)); // ��һ��ͼƬ�궨���Ӧ����������ϵ��ͶӰ����
	Mat projectionMatrix_R = Mat(3, 4, CV_32FC1, Scalar::all(0));
	Mat R, T, E, F; // ����궨����
	Mat R1, R2, P1, P2, Q; // ����У������
	Mat mapl1, mapl2, mapr1, mapr2; // ͼ����ͶӰӳ���	�������У��
	Mat maplx, maply, maprx, mapry; // ͼ����ͶӰӳ���	���������Ŀ����


	const char* rayLineimageList_L = "rayLinecaliberationpics_L.txt"; // ���������ƽ��궨��ͼƬ�����б�
	const char* rayLineimageList_R = "rayLinecaliberationpics_R.txt"; // ���������ƽ��궨��ͼƬ�����б�
	const char* uvPoints_L = "uvPoints_L.txt"; // ���ͼ���ϼ���������ĵ�����
	const char* uvPoints_R = "uvPoints_R.txt";
	const char* realPoints_L = "realPoints_L.txt"; // ��ż���ƽ������������
	const char* realPoints_R = "realPoints_R.txt";
	const char* caliPoints_L = "caliPoints_L.txt"; // ��Ÿ��ݱ궨��������ļ���ƽ������������
	const char* caliPoints_R = "caliPoints_R.txt";
	const char* caliErr_L = "caliErr_L.txt"; // ��ű궨���
	const char* caliErr_R = "caliErr_R.txt";
	const char* laserPlaneCali_result_L = "laserPlaneParams_L.yml"; // ������������ƽ��ı궨���
	const char* laserPlaneCali_result_R = "laserPlaneParams_R.yml"; // ������������ƽ��ı궨���
	Mat laserPlaneParams_L = Mat(3, 1, CV_32FC1); // ���������ƽ��궨���� A B C
	Mat laserPlaneParams_R = Mat(3, 1, CV_32FC1);


	Mat levelPlaneParams = Mat(3, 1, CV_32FC1); // �����ˮƽ��궨���� A B C


};
