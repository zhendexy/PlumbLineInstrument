#pragma once


#include <iostream>
#include <fstream>
#include <iomanip>
#include <math.h>
#include <ctime>
#include <cctype>
#include <string>
#include <String>
#include <time.h>
#include <numeric>
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/features2d/features2d.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <windows.h>

#define _SILENCE_STDEXT_HASH_DEPRECATION_WARNINGS

using namespace cv;
using namespace std;
#pragma once//没有这句会报结构体重定义的错误
#define PI 3.1415926
//包含用到的头文件及自定义数据类型
//网格点图像坐标
struct crossdot
{
	Point2f pt;//图像像素坐标
	int indexh;//横向网格线索引
	int indexz;//纵向网格线索引
};
//网格点空间坐标
struct My3DPoint
{
	Point3f pt;//空间坐标，假定位于Z=0平面
	int indexh;//横向网格线索引
	int indexz;//纵向网格线索引
};