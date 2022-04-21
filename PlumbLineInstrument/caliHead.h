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
#pragma once//û�����ᱨ�ṹ���ض���Ĵ���
#define PI 3.1415926
//�����õ���ͷ�ļ����Զ�����������
//�����ͼ������
struct crossdot
{
	Point2f pt;//ͼ����������
	int indexh;//��������������
	int indexz;//��������������
};
//�����ռ�����
struct My3DPoint
{
	Point3f pt;//�ռ����꣬�ٶ�λ��Z=0ƽ��
	int indexh;//��������������
	int indexz;//��������������
};