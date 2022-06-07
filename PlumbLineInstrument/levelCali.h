#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/features2d/features2d.hpp"
#include <iostream>
#include <fstream>
#include <string>

using namespace std;
using namespace cv;


class LevelCali
{
public:
	LevelCali()
	{
	}
	LevelCali(const char* d, const char* i, Mat& c_m, Mat& d_m) :data_folder(d), image_folder(i), cameraMatrix(c_m), distCoeffs(d_m)
	{
	}


	bool levelPlaneCali();
	Mat getLevelPlaneParams();


public:
	string data_folder;
	string image_folder;
	string image_name = "levelCali.jpg";



	const char* levelPlaneCali_result = "levelPlaneParams.yml";
	Mat cameraMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
	Mat distCoeffs = Mat(5, 1, CV_32FC1, Scalar::all(0));
	Mat rotationMatrix = Mat(3, 3, CV_32FC1, Scalar::all(0));
	Mat translationMatrix = Mat(3, 1, CV_32FC1, Scalar::all(0));
	Mat extrinsicMatrix = Mat(3, 4, CV_32FC1, Scalar::all(0));

	vector<Point2f> points_uv;
	vector<Point3f> points_w;
	vector<Point3f> points_c;

	Mat levelPlaneParams = Mat(3, 1, CV_32FC1);


	Size patternSize = Size(8, 11); // 行列内角点个数
	Size chessboardSize = Size(5, 5); // 棋盘上每个棋盘格的大小10mm
	Size imageSize; // 图像尺寸



};
