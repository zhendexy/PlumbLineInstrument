#pragma once

#include"caliHead.h"
class CaliCalculate
{/*该类包含与计算相关的一些函数：
 1、计算网格世界坐标系到摄像机坐标系的RT矩阵
 2、由光条的像素坐标计算世界坐标得到点云
 3、点云拼接的相关计算*/
public:
	CaliCalculate();
	~CaliCalculate();
	CaliCalculate(Mat& c, Mat& d, Mat& p)
	{
		cameraMatrix = c.clone();
		distCoeffs = d.clone();
	}

public:
	Mat RotationMat, TransVec;//网格坐标系到摄像机坐标系的RT矩阵
	Mat cameraMatrix = Mat::zeros(3, 3, CV_64FC1);//内参矩阵
	Mat distCoeffs = Mat::zeros(1, 5, CV_64FC1);//畸变参数
												//Mat NewCameraMatrix;
	Mat NewdistCoeffs = Mat::zeros(1, 5, CV_64FC1);//畸变参数
	void mergeRT(Mat&M, Mat R, Mat T);
	bool getR_T(vector<crossdot>cdt); //由网格点计算网格坐标系到摄像机坐标系的RT矩阵
	void CalWorldPoint(vector<Point2d>point_uv, Mat IntrinsicM, Mat ExtriM3x4, vector<Point2d>&Point_XwYw);
	void WorldToCam(Mat R, Mat T, vector<Point2d>Point_XwYw, vector<Point3d>&Point_XcYcZc);
	void test_accur(vector<crossdot> cdt, Mat ExtriM);
};
