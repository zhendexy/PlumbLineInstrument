#pragma once

#include"caliHead.h"
class CaliCalculate
{/*��������������ص�һЩ������
 1������������������ϵ�����������ϵ��RT����
 2���ɹ������������������������õ�����
 3������ƴ�ӵ���ؼ���*/
public:
	CaliCalculate();
	~CaliCalculate();
	CaliCalculate(Mat& c, Mat& d, Mat& p)
	{
		cameraMatrix = c.clone();
		distCoeffs = d.clone();
	}

public:
	Mat RotationMat, TransVec;//��������ϵ�����������ϵ��RT����
	Mat cameraMatrix = Mat::zeros(3, 3, CV_64FC1);//�ڲξ���
	Mat distCoeffs = Mat::zeros(1, 5, CV_64FC1);//�������
												//Mat NewCameraMatrix;
	Mat NewdistCoeffs = Mat::zeros(1, 5, CV_64FC1);//�������
	void mergeRT(Mat&M, Mat R, Mat T);
	bool getR_T(vector<crossdot>cdt); //������������������ϵ�����������ϵ��RT����
	void CalWorldPoint(vector<Point2d>point_uv, Mat IntrinsicM, Mat ExtriM3x4, vector<Point2d>&Point_XwYw);
	void WorldToCam(Mat R, Mat T, vector<Point2d>Point_XwYw, vector<Point3d>&Point_XcYcZc);
	void test_accur(vector<crossdot> cdt, Mat ExtriM);
};
