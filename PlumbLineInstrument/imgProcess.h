#pragma once

#pragma once
#include"caliHead.h"
class ImgPro
{/*�������ڴ��������ͼ����Ҫ�������¹��ܣ�
 1���Ҷ����ķ���ȡ����������������
 2����ȡ������������겢����
 */
public:
	ImgPro(const char* f, int w, int h) :folder(f), image_width(w), image_height(h)
	{
	}


	const char* folder;
	int image_width;
	int image_height;
	Size imageSize = Size(image_width, image_height);

	double dot(double x1, double y1, double x2, double y2); //���
	int dblcmp(double a, double b);//�ȽϺ���
	int point_on_line(Point a, Point b, Point c); //��a���ǲ������߶�bc�ϣ�>0���ڣ�=0��˵��غϣ�<0�ڡ�
	double cross(double x1, double y1, double x2, double y2);//���
	double ab_cross_ac(Point a, Point b, Point c);//ab��ac�Ĳ��
	int ab_cross_cd(Point a, Point b, Point c, Point d, Point &p);//����ab��cd�Ĳ��
	float dis(Point p1, Point p2);//�������
	Mat thinImage(Mat src);//ͼ��ϸ��
	vector<Point> Extlaser(Mat mat);
	vector<Point> Extlaser3(Mat& mat, Mat& laser);
	void deletelaser(Mat &bin, vector<Point>veclaser);//ɾ��������
	void deletelaser2(Mat &bin, vector<Point>veclaser);
	void OnDrawDotline(Point p1, Point p2, Point & pa, Point & pb, Mat mat, bool Isheng);
	int sortline(vector<Vec4i> Lines, vector<Vec4i>&final, bool Isheng);//ֱ������
	vector<int> sort_indexes(vector<double> &v, bool Isheng);//��������
	vector<Vec4i> ranklines(vector<Vec4i>Lines, bool Isheng);
	int Hough22(Mat contours, vector<crossdot>&veccrossdot);
	void RemoveSmallRegion(Mat &Src, Mat &Dst, int AreaLimit, int CheckMode, int NeihborMode);//ȥ��С����
	int findcrossdot2(Mat& mat, Mat& laser, vector<crossdot>& cdt);//��ȡ�����
	void drawText(Mat image, Point origin, string text);//����ı���ͼ��
	void drawIndex(Mat image, vector<crossdot>cdt);
	int GetABCD(vector<crossdot>cdt, Point2f&PtLU_A, Point2f&PtLD_B, Point2f&PtRD_C, Point2f&PtRU_D);
	vector<Point2d>isInRect(vector<Point2d>Pt_uv, Point2f PtLU_A, Point2f PtLD_B, Point2f PtRD_C, Point2f PtRU_D);
	vector<Point> Laser2DPoints;//����������ĵ�����
	void drawLaserInRect(Mat img, vector<Point2d>);

};