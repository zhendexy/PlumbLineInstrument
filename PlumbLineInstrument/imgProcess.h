#pragma once

#pragma once
#include"caliHead.h"
class ImgPro
{/*该类用于处理拍摄的图像，主要包含以下功能：
 1、灰度重心法提取光条中心像素坐标
 2、提取网格点像素坐标并排序
 */
public:
	ImgPro(const char* f, int w, int h) :folder(f), image_width(w), image_height(h)
	{
	}


	const char* folder;
	int image_width;
	int image_height;
	Size imageSize = Size(image_width, image_height);

	double dot(double x1, double y1, double x2, double y2); //点积
	int dblcmp(double a, double b);//比较函数
	int point_on_line(Point a, Point b, Point c); //求a点是不是在线段bc上，>0不在，=0与端点重合，<0在。
	double cross(double x1, double y1, double x2, double y2);//叉积
	double ab_cross_ac(Point a, Point b, Point c);//ab与ac的叉积
	int ab_cross_cd(Point a, Point b, Point c, Point d, Point &p);//向量ab与cd的叉积
	float dis(Point p1, Point p2);//计算距离
	Mat thinImage(Mat src);//图像细化
	vector<Point> Extlaser(Mat mat);
	vector<Point> Extlaser3(Mat& mat, Mat& laser);
	void deletelaser(Mat &bin, vector<Point>veclaser);//删除激光线
	void deletelaser2(Mat &bin, vector<Point>veclaser);
	void OnDrawDotline(Point p1, Point p2, Point & pa, Point & pb, Mat mat, bool Isheng);
	int sortline(vector<Vec4i> Lines, vector<Vec4i>&final, bool Isheng);//直线排序
	vector<int> sort_indexes(vector<double> &v, bool Isheng);//索引排序
	vector<Vec4i> ranklines(vector<Vec4i>Lines, bool Isheng);
	int Hough22(Mat contours, vector<crossdot>&veccrossdot);
	void RemoveSmallRegion(Mat &Src, Mat &Dst, int AreaLimit, int CheckMode, int NeihborMode);//去除小区域
	int findcrossdot2(Mat& mat, Mat& laser, vector<crossdot>& cdt);//提取网格点
	void drawText(Mat image, Point origin, string text);//输出文本到图像
	void drawIndex(Mat image, vector<crossdot>cdt);
	int GetABCD(vector<crossdot>cdt, Point2f&PtLU_A, Point2f&PtLD_B, Point2f&PtRD_C, Point2f&PtRU_D);
	vector<Point2d>isInRect(vector<Point2d>Pt_uv, Point2f PtLU_A, Point2f PtLD_B, Point2f PtRD_C, Point2f PtRU_D);
	vector<Point> Laser2DPoints;//激光光条中心点容器
	void drawLaserInRect(Mat img, vector<Point2d>);

};