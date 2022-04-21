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

using namespace std;
using namespace cv;


class CameraCali
{
public:
	CameraCali(const char* l_f, const char* r_f, const char* d_f, const int w, const int h)
		:left_folder(l_f), right_folder(r_f), data_folder(d_f), image_width(w), image_height(h)
	{
	}


private:
	const char* left_folder;
	const char* right_folder;
	const char* data_folder;
	const int image_width;
	const int image_height;


	const char* imageList_L = "caliberationpics_L.txt"; // 左相机的标定图片名称列表
	const char* imageList_R = "caliberationpics_R.txt"; // 右相机的标定图片名称列表
	const char* singleCalibrate_result_L = "calibrationresults_L.yml"; // 存放左相机的标定结果
	const char* singleCalibrate_result_R = "calibrationresults_R.yml"; // 存放右相机的标定结果
	const char* stereoCalibrate_result_L = "stereocalibrateresult_L.yml"; // 存放立体标定结果
	const char* stereoCalibrate_result_R = "stereocalibrateresult_R.yml";
	const char* stereoRectifyParams = "stereoRectifyParams.yml"; // 存放立体校正参数
	vector<vector<Point2f>> corners_seq_L; // 所有角点坐标
	vector<vector<Point2f>> corners_seq_R;
	vector<vector<Point3f>> objectPoints_L; // 三维坐标
	vector<vector<Point3f>> objectPoints_R;
	Mat cameraMatrix_L = Mat(3, 3, CV_32FC1, Scalar::all(0)); // 相机的内参数
	Mat cameraMatrix_R = Mat(3, 3, CV_32FC1, Scalar::all(0)); // 初始化相机的内参数
	Mat distCoeffs_L = Mat(1, 5, CV_32FC1, Scalar::all(0)); // 相机的畸变系数
	Mat distCoeffs_R = Mat(1, 5, CV_32FC1, Scalar::all(0)); // 初始化相机的畸变系数
	Mat R, T, E, F; // 立体标定参数
	Mat R1, R2, P1, P2, Q; // 立体校正参数
	Mat mapl1, mapl2, mapr1, mapr2; // 图像重投影映射表	立体矫正校正
	Size patternSize = Size(8, 6); // 行列内角点个数
	Size chessboardSize = Size(7, 7); // 棋盘上每个棋盘格的大小10mm
	Size imageSize = Size(image_width, image_height); // 图像尺寸
	Rect validRoi[2];



private:
	//相机标定用到的函数
	bool singleCameraCalibrate(const char* imageFolder, const char* imageList, const char* singleCalibrateResult, vector<vector<Point3f>>& objectPoints,
		vector<vector<Point2f>>& corners_seq, Mat& cameraMatrix, Mat& distCoeffs, Size& imageSize, Size patternSize, Size chessboardSize);
	bool stereoCalibrate(const char* stereoCalibrateResult, vector<vector<Point3f>> objectPoints, vector<vector<Point2f>> imagePoints1, vector<vector<Point2f>> imagePoints2,
		Mat& cameraMatrix1, Mat& distCoeffs1, Mat& cameraMatrix2, Mat& distCoeffs2, Size& imageSize, Mat& R, Mat& T, Mat& E, Mat& F);
	Rect stereoRectification(const char* stereoRectifyParams, Mat& cameraMatrix1, Mat& distCoeffs1, Mat& cameraMatrix2, Mat& distCoeffs2,
		Size& imageSize, Mat& R, Mat& T, Mat& R1, Mat& R2, Mat& P1, Mat& P2, Mat& Q, Mat& mapl1, Mat& mapl2, Mat& mapr1, Mat& mapr2);
	bool computeDisparityImage(const char* imageName1, const char* imageName2, Mat& img1_rectified,
		Mat& img2_rectified, Mat& mapl1, Mat& mapl2, Mat& mapr1, Mat& mapr2, Rect validRoi[2], Mat& disparity);
	bool computeDisparityImage(Mat& img1, Mat& img2, Mat& img1_rectified,
		Mat& img2_rectified, Mat& mapl1, Mat& mapl2, Mat& mapr1, Mat& mapr2, Rect validRoi[2], Mat& disparity);


public:
	//接口
	void leftCali();
	void rightCali();
	void stereoCali();
	void stereoRecti();
	void computeDisparity(const char* imgName1, const char* imgName2, Mat& img1_rectified, Mat& img2_rectified);
	void computeDisparity(Mat& img1, Mat& img2, Mat& img1_rectified, Mat& img2_rectified);


};