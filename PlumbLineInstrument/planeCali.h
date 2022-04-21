#pragma once

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


#include "imgProcess.h"
#include "caliCalculate.h"


/*
ʵ���˴�ͳ���ù�դ�ߵı궨
ʵ������PnP������������̬�µı궨
*/




using namespace std;
using namespace cv;

class PlaneCali
{
public:
	PlaneCali(const char* l_f, const char* r_f, const char* d_f, const int w, const int h, Mat& c_l, Mat& c_r, Mat& d_l, Mat& d_r, Mat& p_l, Mat& p_r)
		:left_folder(l_f), right_folder(r_f), data_folder(d_f), image_width(w), image_height(h),
		cameraMatrix_L(c_l), cameraMatrix_R(c_r), distCoeffs_L(d_l), distCoeffs_R(d_r), projectionMatrix_L(p_l), projectionMatrix_R(p_r)
	{
	}

	PlaneCali(const char* l_f, const char* r_f, const char* d_f, const int w, const int h)
		:left_folder(l_f), right_folder(r_f), data_folder(d_f), image_width(w), image_height(h)
	{
		FileStorage singleLStore(data_folder + string(singleCalibrate_result_L), FileStorage::READ);
		singleLStore["cameraMatrix"] >> cameraMatrix_L;
		singleLStore["distCoeffs"] >> distCoeffs_L;
		singleLStore["projectionMatrix"] >> projectionMatrix_L;
		singleLStore.release();


		FileStorage singleRStore(data_folder + string(singleCalibrate_result_R), FileStorage::READ);
		singleRStore["cameraMatrix"] >> cameraMatrix_R;
		singleRStore["distCoeffs"] >> distCoeffs_R;
		singleRStore["projectionMatrix"] >> projectionMatrix_R;
		singleRStore.release();
	}


private:
	const char* left_folder;
	const char* right_folder;
	const char* data_folder;
	const int image_width;
	const int image_height;
	const char* singleCalibrate_result_L = "calibrationresults_L.yml"; // ���������ı궨���
	const char* singleCalibrate_result_R = "calibrationresults_R.yml"; // ���������ı궨���
	Mat cameraMatrix_L = Mat(3, 3, CV_32FC1, Scalar::all(0)); // ������ڲ���
	Mat cameraMatrix_R = Mat(3, 3, CV_32FC1, Scalar::all(0)); // ��ʼ��������ڲ���
	Mat distCoeffs_L = Mat(1, 5, CV_32FC1, Scalar::all(0)); // ����Ļ���ϵ��
	Mat distCoeffs_R = Mat(1, 5, CV_32FC1, Scalar::all(0)); // ��ʼ������Ļ���ϵ��
	Mat projectionMatrix_L = Mat(3, 4, CV_32FC1, Scalar::all(0)); // ͶӰ����
	Mat projectionMatrix_R = Mat(3, 4, CV_32FC1, Scalar::all(0));


	const char* laserCaliimageList_L = "laserCaliberationPics_L.txt"; // ���������ƽ��궨��ͼƬ�����б�
	const char* laserCaliimageList_R = "laserCaliberationPics_R.txt"; // ���������ƽ��궨��ͼƬ�����б�
	const char* laserImageLists_R = "laserImageLists_R.txt";
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
	Size imageSize = Size(image_width, image_height); // ͼ��ߴ�
	
	Mat srcImage, grayImage, redImage, rayLineImage, midImage, maskImage;
	int T = 80; // ͼ���ֵ������ֵ
	int redtolerant = 3, graytolerant = 10;
	int msize = 400; // ȥ��С��ͨ�����������

	int ImgNum = 0;







	bool rayPlaneCalibrate(const char* imageFolder, const char* imageList, const char* uvPoints, const char* realPoints, const char* caliPoints,
		const char* caliErr, const char*rayPlaneCali_result, Mat& rayPlaneParams, Mat& projectionMatrix, Mat& cameraMatrix, Mat& distCoeffs);

	bool laserPlaneCali(const char* imageFolder, const char* imageList, const char* laserList, const char* uvPoints, const char* realPoints, const char* caliPoints,
		const char* caliErr, const char*rayPlaneCali_result, Mat& rayPlaneParams, Mat& projectionMatrix, Mat& cameraMatrix, Mat& distCoeffs);


	void getRayLinePoints(Mat& srcImage, vector<Point>& linePoints);
	void deleteSmallRegions(Mat &pSrc, Mat &pDst);
	void findSmallRegions(Mat &pSrc, Mat &pDst);

	Point2f uvzw2xwyw(Point uv, float zw, Mat &P);
	void uvzw2xwyw(Point &uv, float zw, Point2f &xwyw, Mat &M);
	Point3f uv2xwywzw(Point uv, Mat& M);
	void uv2xwywzw(Point &uv, Point3f &xwywzw, Mat& M);


public:
	//�ӿ�
	void leftCali();
	void rightCali();

	void leftCali1();
	void rightCali1();
};