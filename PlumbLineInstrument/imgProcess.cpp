#include"imgProcess.h"


//*���ò���ж��Ƿ���ֱ���ϣ��ٵ��õ���ж��Ƿ����߶���*
double ImgPro::dot(double x1, double y1, double x2, double y2) //���
{
	return x1*x2 + y1*y2;
}
int ImgPro::dblcmp(double a, double b)//�Ƚϴ�С
{
	if (fabs(a - b) <= 1E-6) return 0;//����
	if (a>b) return 1;//����
	else return -1;//С��
}
//���ò���ж��Ƿ���ֱ���ϣ��ٵ��õ���ж��Ƿ����߶���
int ImgPro::point_on_line(Point a, Point b, Point c) //��a���ǲ������߶�bc�ϣ�>0���ڣ�=0��˵��غϣ�<0�ڡ�
{
	return dblcmp(dot(b.x - a.x, b.y - a.y, c.x - a.x, c.y - a.y), 0);
}
//**************************************************
double ImgPro::cross(double x1, double y1, double x2, double y2)//�������
{
	return x1*y2 - x2*y1;
}
double ImgPro::ab_cross_ac(Point a, Point b, Point c) //ab��ac�Ĳ��
{
	return cross(b.x - a.x, b.y - a.y, c.x - a.x, c.y - a.y);
}
//��ab�Ƿ���cd�ཻ������Ϊp��1�淶�ཻ��0������һ�߶εĶ˵㣬-1���ཻ��
//p�㼴��������
int ImgPro::ab_cross_cd(Point a, Point b, Point c, Point d, Point &p)
{
	double s1, s2, s3, s4;
	int d1, d2, d3, d4;
	//Point p;
	d1 = dblcmp(s1 = ab_cross_ac(a, b, c), 0);
	d2 = dblcmp(s2 = ab_cross_ac(a, b, d), 0);
	d3 = dblcmp(s3 = ab_cross_ac(c, d, a), 0);
	d4 = dblcmp(s4 = ab_cross_ac(c, d, b), 0);

	//����淶�ཻ���󽻵�
	if ((d1^d2) == -2 && (d3^d4) == -2)
	{
		p.x = (c.x*s2 - d.x*s1) / (s2 - s1);
		p.y = (c.y*s2 - d.y*s1) / (s2 - s1);
		return 1;
	}

	//������淶�ཻ
	if (d1 == 0 && point_on_line(c, a, b) <= 0)
	{
		p = c;
		return 0;
	}
	if (d2 == 0 && point_on_line(d, a, b) <= 0)
	{
		p = d;
		return 0;
	}
	if (d3 == 0 && point_on_line(a, c, d) <= 0)
	{
		p = a;
		return 0;
	}
	if (d4 == 0 && point_on_line(b, c, d) <= 0)
	{
		p = b;
		return 0;
	}
	//������ཻ
	return -1;
}
//�����������
float ImgPro::dis(Point p1, Point p2)
{
	return sqrt((p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y));
}

Mat ImgPro::thinImage(Mat src)
{
	const int maxIterations = -1;
	assert(src.type() == CV_8UC1);
	Mat dst;
	int width = src.cols;
	int height = src.rows;
	src.copyTo(dst);
	int count = 0;  //��¼��������  
	while (true)
	{
		count++;
		if (maxIterations != -1 && count > maxIterations) //���ƴ������ҵ�����������  
			break;
		vector<uchar *> mFlag; //���ڱ����Ҫɾ���ĵ�  
							   //�Ե���  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//��������ĸ����������б��  
				//  p9 p2 p3  
				//  p8 p1 p4  
				//  p7 p6 p5  
				uchar p1 = p[j];
				if (p1 != 1) continue;
				uchar p4 = (j == width - 1) ? 0 : *(p + j + 1);
				uchar p8 = (j == 0) ? 0 : *(p + j - 1);
				uchar p2 = (i == 0) ? 0 : *(p - dst.step + j);
				uchar p3 = (i == 0 || j == width - 1) ? 0 : *(p - dst.step + j + 1);
				uchar p9 = (i == 0 || j == 0) ? 0 : *(p - dst.step + j - 1);
				uchar p6 = (i == height - 1) ? 0 : *(p + dst.step + j);
				uchar p5 = (i == height - 1 || j == width - 1) ? 0 : *(p + dst.step + j + 1);
				uchar p7 = (i == height - 1 || j == 0) ? 0 : *(p + dst.step + j - 1);

				if ((p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) >= 2 && (p2 + p3 + p4 + p5 + p6 + p7 + p8 + p9) <= 6)
				{
					int ap = 0;
					if (p2 == 0 && p3 == 1) ++ap;
					if (p3 == 0 && p4 == 1) ++ap;
					if (p4 == 0 && p5 == 1) ++ap;
					if (p5 == 0 && p6 == 1) ++ap;
					if (p6 == 0 && p7 == 1) ++ap;
					if (p7 == 0 && p8 == 1) ++ap;
					if (p8 == 0 && p9 == 1) ++ap;
					if (p9 == 0 && p2 == 1) ++ap;

					if (ap == 1 && p2 * p4 * p8 == 0 && p2 * p6 * p8 == 0)
					{
						//���  
						mFlag.push_back(p + j);
					}
				}
			}
		}
		//����ǵĵ�ɾ��  
		for (vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}
		//ֱ��û�е����㣬�㷨����  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//��mFlag���  
		}
	}
	return dst * 255;
}

//������ͨ��ͼ��
vector<Point> ImgPro::Extlaser(Mat mat)
{
	Laser2DPoints.clear();
	//	CStringA fname;
	Mat bin, imageRed, thin;
	vector<Mat> channels;
	split(mat, channels);
	imageRed = channels.at(2);
	//	imshow("red", imageRed);
	blur(imageRed, imageRed, Size(5, 5));//��ֵ�˲�
										 //���� ���� �Ժ�ɫ���� ��ֵ��
	threshold(imageRed, bin, 150, 255, THRESH_BINARY);
	//	imshow("binred", bin);
	//	fname.Format("C:\\Users\\Rachmaninov\\Desktop\\cam1_red.bmp");
	//imwrite(fname.GetBuffer(), imageRed);
	thin = thinImage(bin / 255);
	//	imshow("thinlaser", thin);
	//imshow("bin", bin);

	Mat Result(1024, 1280, CV_8UC1, Scalar(0));
	vector<Point>veclaser;//���漤�������
	Point pt;
	for (int i = 0; i < thin.rows; i++)//��
	{
		for (int j = 0; j < thin.cols; j++)//��
		{
			if (thin.at<uchar>(i, j) == 255)
			{
				pt.x = j;
				pt.y = i;
				veclaser.push_back(pt);
				Laser2DPoints.push_back(pt);
				//cout << pt.x << "   " << pt.y << endl;    //  ��  ��
				Result.at<uchar>(pt.y, pt.x) = 255;
			}
		}
	}
	return veclaser;
}



vector<Point> ImgPro::Extlaser3(Mat& mat, Mat& laser)
{
	//��δ�ﵽ������
	Laser2DPoints.clear();
	vector<Point> vec;
	double threshold = 220;
	int red_tolerant = 3;
	int gray_tolerant = 10;
	int gray_counts = 9;


	Mat temp, temp1;
	/////////////////////////////�ؼ�/////////////////////////////////////////////////
	cvtColor(mat, temp1, COLOR_BGR2GRAY);
	GaussianBlur(temp1, temp1, Size(3, 3), 0, 0);
	imwrite("temp1.jpg", temp1);
	//��ȡ�ֶ���ȡ�����ߵ�ͼ��laser
	cvtColor(laser, temp, COLOR_BGR2GRAY);
	cv::threshold(temp, temp, 50, 255, THRESH_TOZERO);
	GaussianBlur(temp, temp, Size(3, 3), 0, 0);
	imwrite("temp.jpg", temp);
	/////////////////////////////////////////////////////////////////////////////////

	//�����ֱ�ļ���
	//cv::Point pt;//���ڴ�ż��������ĵ�uv����
	//Mat Result(2048, 2560, CV_8UC1, Scalar(0));
	//for (int i = 0; i < temp.rows; i++)
	//{
	//	int sum = 0;
	//	int n = 0;
	//	for (int j = 0; j < temp.cols; j++)
	//	{
	//		int val = (int)temp.at<uchar>(i, j);
	//		if (val > 0)
	//		{
	//			sum += j;
	//			n++;
	//		}
	//	}
	//	if (sum != 0)
	//	{
	//		pt = Point(sum / n, i);
	//		vec.push_back(pt);
	//		Laser2DPoints.push_back(pt);
	//		Result.at<uchar>(pt) = 255;
	//	}
	//}

	//���ˮƽ�ļ���
	cv::Point pt;//���ڴ�ż��������ĵ�uv����
	Mat Result(imageSize, CV_8UC1, Scalar(0));
	for (int i = 0; i < temp.cols; i++)
	{
		int sum = 0;
		int n = 0;
		for (int j = 0; j < temp.rows; j++)
		{
			int val = (int)temp.at<uchar>(j, i);
			if (val > 0)
			{
				sum += j;
				n++;
			}
		}
		if (sum != 0)
		{
			pt = Point(i, sum / n);
			vec.push_back(pt);
			Laser2DPoints.push_back(pt);
			Result.at<uchar>(pt) = 255;
		}
	}



	static int imgnum = 0;
	string imageName;
	imageName = string("./imgpro\\7����") + to_string(imgnum) + string(".jpg");
	imwrite(imageName, Result);
	imgnum++;
	return vec;
}

void ImgPro::deletelaser(Mat &bin, vector<Point>veclaser)
{
	for (int i = 0; i < veclaser.size(); i++)
	{
		int x = veclaser[i].x;//��
		int y = veclaser[i].y;//��
		for (int j = max(x - 10, 0); j < min(x + 10, 1280); j++)//��
		{
			for (int k = max(y - 10, 0); k < min(y + 10, 1024); k++)//��
			{
				bin.at<uchar>(y, j) = 0;
			}
		}
	}
}

void ImgPro::deletelaser2(Mat &bin, vector<Point>veclaser)
{
	for (int i = 0; i < veclaser.size(); i++)
	{
		int x = veclaser[i].x;//��	u
		int y = veclaser[i].y;//��	v
		for (int j = max(y - 80, 0); j < min(y + 80, image_height); j++)//��		v
		{
			bin.at<uchar>(j, x) = 0;
		}
	}
}

//���߶ε��ӳ��� ��� pa  pb  ��ֵ
void ImgPro::OnDrawDotline(Point p1, Point p2, Point & pa, Point & pb, Mat mat, bool Isheng)
{
	double h = mat.rows, w = mat.cols;
	double k = (p1.y - p2.y) / (p1.x - p2.x + 0.000001);//����0.000001 �������ߣ�б�ʿ���Ϊ0����e.x-s.x����Ϊ0  //б��	
	double b = p1.y - k*p1.x;
	if (Isheng)//����
	{
		if (b < 1280)//ԭ��1024
		{
			pa.x = -b / k;//1280
			pa.y = 0;
			pb.x = 0;
			pb.y = b;
		}
		else
		{
			double x = -b / k;
			if (x<w)//1280
			{
				pa.x = -b / k;//1280
				pa.y = 0;
				pb.x = (h - b) / k;
				pb.y = h - 1;;
			}
			else
			{
				pa.x = w - 1;//1280
				pa.y = w*k + b;
				pb.x = (h - b) / k;
				pb.y = h - 1;
			}
		}
	}
	else//����
	{
		if (b >= 0)
		{
			pa.x = 0;
			pa.y = b;
			pb.x = (h - b) / k;
			pb.y = h - 1;
		}
		else
		{
			double y = w * k + b;
			if (y > h)
			{
				pa.x = -b;
				pa.y = 0;
				pb.x = (h - b) / k;
				pb.y = h - 1;//1024
			}
			else
			{
				pa.x = -b;
				pa.y = 0;
				pb.x = w - 1;
				pb.y = y;//1024
			}
		}
	}

}
//ֱ������
int ImgPro::sortline(vector<Vec4i> Lines, vector<Vec4i>&final, bool Isheng)
{
	if (Lines.size() == 0)
	{
		cout << "*************����ֱ��Ϊ�գ��޷�����,��������ǰ֡!**************";
		return -1;
	}
	vector<Vec4i>result_line;
	vector<vector<Vec4i>>linesort;
	//	int dif1,dif2,dif3,dif4;
	int n;
	if (Isheng)//����ֱ������
		n = 50;  //�ؾ���ֵ	
	else
		n = 50;
	linesort.push_back({ Lines[0] });
	//	cout << "****************" << endl;
	for (int i = 1; i < Lines.size(); i++)
	{
		int j;
		Point p1(Lines[i][0], Lines[i][1]);
		Point p2(Lines[i][2], Lines[i][3]);
		double k = (p1.y - p2.y) / (p1.x - p2.x + 0.000001);
		double b = p1.y - k*p1.x;
		//	cout << k << "  " << b << endl;
		for (j = 0; j<linesort.size(); j++)
		{
			Point pt1(linesort[j][0][0], linesort[j][0][1]);
			Point pt2(linesort[j][0][2], linesort[j][0][3]);
			double kt = (pt1.y - pt2.y) / (pt1.x - pt2.x + 0.000001);
			double bt = pt1.y - k*pt1.x;
			if (abs(b - bt)<n)
			{
				linesort[j].push_back(Lines[i]);
				break;
			}
		}
		if (j == linesort.size())
		{
			linesort.push_back({ Lines[i] });
		}
	}
	/////////���˴�/////

	Mat dst3(imageSize, CV_8UC1, Scalar(0));//����ֱ��
	Point p1, p2;
	Point po(0, 0);
	Vec4i lineyj;//�洢�����Զ��
				 //��������
				 /*for (int i = 0; i < linesort.size(); i++)
				 {
				 stringstream ss;
				 ss << i;
				 string s1;
				 ss >> s1;
				 Point pj, py;
				 cout << "�� " << i << "��" << endl;
				 for (int j = 0; j < linesort[i].size(); j++)
				 {
				 Point pjj, pyy;
				 p1 = Point(linesort[i][j][0], linesort[i][j][1]);
				 p2 = Point(linesort[i][j][2], linesort[i][j][3]);
				 double k = (p1.y - p2.y) / (p1.x - p2.x + 0.000001);
				 double b = p1.y - k*p1.x;
				 cout << k << "  " << b << endl;
				 int x = (p1.x + p2.x) / 2;
				 int y = (p1.y + p2.y) / 2;
				 line(dst3, p1, p2, Scalar(255), 1, 8);
				 putText(dst3, s1, Point(x, y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255), 2, 8);
				 }
				 }*/
				 ///////////
	for (int i = 0; i < linesort.size(); i++)
	{
		stringstream ss;
		ss << i;
		string s1;
		ss >> s1;
		Point pj, py;
		p1 = Point(linesort[i][0][0], linesort[i][0][1]);
		p2 = Point(linesort[i][0][2], linesort[i][0][3]);
		if (dis(p1, po) < dis(p2, po))
		{
			pj = p1; py = p2;
		}
		else
		{
			pj = p2; py = p1;
		}
		for (int j = 1; j < linesort[i].size(); j++)
		{
			Point pjj, pyy;
			p1 = Point(linesort[i][j][0], linesort[i][j][1]);
			p2 = Point(linesort[i][j][2], linesort[i][j][3]);
			if (dis(p1, po) < dis(p2, po))
			{
				pjj = p1; pyy = p2;
			}
			else
			{
				pjj = p2; pyy = p1;
			}
			if (dis(pjj, po) < dis(pj, po))
				pj = pjj;
			if (dis(pyy, po) > dis(py, po))
				py = pyy;
		}
		line(dst3, pj, py, Scalar(255), 1, 8);
		int x = (pj.x + py.x) / 2;
		int y = (pj.y + py.y) / 2;
		lineyj[0] = pj.x;
		lineyj[1] = pj.y;
		lineyj[2] = py.x;
		lineyj[3] = py.y;
		//result_line.push_back(lineyj);//���������Զ�� ��û���ӳ�
		final.push_back(lineyj);
		putText(dst3, s1, Point(x, y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255), 2, 8);
	}
	//�ӳ�
	//Vec4i lineby;//�洢��Ե��
	//Point  pt1, pt2, pt3, pt4;
	//for (int i = 0; i < result_line.size(); i++)
	//{
	//	if (Isheng)
	//	{
	//		p1 = Point(result_line[i][0], result_line[i][1]);
	//		p2 = Point(result_line[i][2], result_line[i][3]);
	//		OnDrawDotline(p1, p2, pt1, pt2, dst3, 1);//����
	//		lineby[0] = pt1.x;
	//		lineby[1] = pt1.y;
	//		lineby[2] = pt2.x;
	//		lineby[3] = pt2.y;
	//		final.push_back(lineby);
	//	}
	//	else
	//	{
	//		p1 = Point(result_line[i][0], result_line[i][1]);
	//		p2 = Point(result_line[i][2], result_line[i][3]);
	//		OnDrawDotline(p1, p2, pt3, pt4, dst3, 0);//����
	//		lineby[0] = pt3.x;
	//		lineby[1] = pt3.y;
	//		lineby[2] = pt4.x;
	//		lineby[3] = pt4.y;
	//		final.push_back(lineby);
	//	}
	//}
	if (Isheng)
	{
		cout << "����� ���� ֱ��������" << linesort.size() << endl;
	}
	else
	{
		cout << "����� ���� ֱ��������" << linesort.size() << endl;
	}
	static int imgsortnum = 0;
	string imageName;
	//��������  - end
	if (Isheng)
	{
		////imshow("�������", dst3);
		imageName = string("./imgpro\\5�������") + to_string(imgsortnum) + string(".jpg");
		imwrite(imageName, dst3);
	}

	else
	{
		/////imshow("�������", dst3);
		imageName = string("./imgpro\\5�������") + to_string(imgsortnum) + string(".jpg");
		imwrite(imageName, dst3);
	}
	imgsortnum++;/**/
	if (linesort.size() != 9)
		return 0;//����������9��
	else
		return 1;//��������9��

}

vector<int>ImgPro::sort_indexes(vector<double> &v, bool Isheng)
{

	// ��ʼ����������
	vector<int> idx(v.size());
	//ʹ��iota��������0~��������ֵ
	iota(idx.begin(), idx.end(), 0);

	// ͨ���Ƚ�v��ֵ������idx��������
	if (Isheng)
		sort(idx.begin(), idx.end(), [&v](int i1, int i2) { return v[i1] < v[i2]; });//����
	else
		sort(idx.begin(), idx.end(), [&v](int i1, int i2) { return v[i1] > v[i2]; });//����
	return idx;
}
//�Ժ���ֱ�� ����  ���� ������ֱ��
vector<Vec4i> ImgPro::ranklines(vector<Vec4i>Lines, bool Isheng)
{
	//�� ���� Ϊ9  ��С�� 9
	//int num = 0;
	//vector<int>minnum;
	vector<double> db;
	for (int i = 0; i < Lines.size(); i++)
	{
		Point p1(Lines[i][0], Lines[i][1]);
		Point p2(Lines[i][2], Lines[i][3]);
		double k = (p1.y - p2.y) / (p1.x - p2.x + 0.000001);
		double b = p1.y - k*p1.x;
		db.push_back(b);
	}
	vector<int>index;
	index = sort_indexes(db, Isheng);
	vector<Vec4i>newlines;
	for (int i = 0; i < index.size(); i++)
	{
		newlines.push_back(Lines[index[i]]);
	}
	return newlines;
}

//�����߶�,�����߶ε��ӳ���
//vector<crossdot> Hough22(Mat contours)
int ImgPro::Hough22(Mat contours, vector<crossdot>&veccrossdot)
{
	Mat dst1(imageSize, CV_8UC1, Scalar(0));//����ֱ��
											 //Mat dst2(1024, 1280, CV_8UC1, Scalar(0));//����ֱ��
	vector<Vec4i> Lines, Lines_heng, Lines_zong;
	HoughLinesP(contours, Lines, 1, CV_PI / 180, 50, 20, 10);
	Point p1, p2;
	//for (int i = 0; i < Lines.size(); i++)
	//{
	//	p1 = Point(Lines[i][0], Lines[i][1]);
	//	p2 = Point(Lines[i][2], Lines[i][3]);
	//	line(dst1, p1, p2, Scalar(255), 1, 8);
	//}
	//	Point p1, p2;
	for (int i = 0; i < Lines.size(); i++)
	{
		p1 = Point(Lines[i][0], Lines[i][1]);
		p2 = Point(Lines[i][2], Lines[i][3]);
		double k = (p1.y - p2.y) / (p1.x - p2.x + 0.000001);
		int t1 = abs(k - 1);
		int t2 = abs(k - (-1));
		//	cout << angle << endl;
		if (t1<t2)//����
		{
			Lines_zong.push_back(Lines[i]);
		}
		if (t2<t1)
		{
			Lines_heng.push_back(Lines[i]);
		}
	}
	//cout << "*************��" << endl;
	//for (int i = 0; i < Lines_heng.size(); i++)
	//{
	//	p1 = Point(Lines_heng[i][0], Lines_heng[i][1]);
	//	p2 = Point(Lines_heng[i][2], Lines_heng[i][3]);
	//	double k = (p1.y - p2.y) / (p1.x - p2.x + 0.000001);
	//	double b = p1.y - k*p1.x;
	//	cout << k << "  " << b << endl;
	//	line(dst1, p1, p2, Scalar(255), 1, 8);
	//}
	//cout << "*************��" << endl;
	//for (int i = 0; i < Lines_zong.size(); i++)
	//{
	//	p1 = Point(Lines_zong[i][0], Lines_zong[i][1]);
	//	p2 = Point(Lines_zong[i][2], Lines_zong[i][3]);
	//	double k = (p1.y - p2.y) / (p1.x - p2.x + 0.000001);
	//	double b = p1.y - k*p1.x;
	//	cout << k << "  " << b << endl;
	//	//	line(dst1, p1, p2, Scalar(255), 1, 8);
	//}
	vector<Vec4i>linesort_heng, linesort_zong;//���������ֱ��
	int t1, t2;
	t1 = sortline(Lines_heng, linesort_heng, 1);
	t2 = sortline(Lines_zong, linesort_zong, 0);
	if (t1 != 1 || t2 != 1)//��һ�������ھ���
		return 0;//ֱ�߼�����ھ��࣬����ȡ�����߽��㣬��֡ͼ����Ч	
				 //cout << "at hough22";//tiaoshitianjia �ڴ˷��أ�������
	vector<Vec4i>newlineh, newlinez;//������ֱ��
	newlineh = ranklines(linesort_heng, 1);
	newlinez = ranklines(linesort_zong, 0);

	cout << "*************��" << endl;
	for (int i = 0; i < linesort_heng.size(); i++)
	{
		p1 = Point(newlineh[i][0], newlineh[i][1]);
		p2 = Point(newlineh[i][2], newlineh[i][3]);
		double k = (p1.y - p2.y) / (p1.x - p2.x + 0.000001);
		double b = p1.y - k*p1.x;
		//		cout << k << "  " << b << endl;
		//	line(dst1, p1, p2, Scalar(255), 1, 8);
	}
	cout << "*************��" << endl;
	for (int i = 0; i < linesort_zong.size(); i++)
	{
		p1 = Point(newlinez[i][0], newlinez[i][1]);
		p2 = Point(newlinez[i][2], newlinez[i][3]);
		double k = (p1.y - p2.y) / (p1.x - p2.x + 0.000001);
		double b = p1.y - k*p1.x;
		//		cout << k << "  " << b << endl;
		//	line(dst1, p1, p2, Scalar(255), 1, 8);
	}

	//	vector<crossdot>veccrossdot;
	crossdot cdt;
	Point2f  pp;
	for (int i = 0; i < newlineh.size(); i++)
	{
		Point pt1(newlineh[i][0], newlineh[i][1]);//��a
		Point pt2(newlineh[i][2], newlineh[i][3]);//��b  �߶�ab
		double k1 = (pt1.y - pt2.y) / (pt1.x - pt2.x + 0.000001);
		double b1 = pt1.y - k1*pt1.x;
		for (int j = 0; j < linesort_zong.size(); j++)
		{
			Point pt3(newlinez[j][0], newlinez[j][1]); //��c
			Point pt4(newlinez[j][2], newlinez[j][3]); //��d �߶�cd
			double k2 = (pt3.y - pt4.y) / (pt3.x - pt4.x + 0.000001);
			double b2 = pt3.y - k2*pt3.x;
			pp.x = (b2 - b1) / (k1 - k2);
			pp.y = k1*pp.x + b1;
			if (pp.x < 2560 && pp.x>0 && pp.y < 2048 && pp.y>0)
			{
				cdt.pt = pp;
				cdt.indexh = i;
				cdt.indexz = j;
				veccrossdot.push_back(cdt);
				cout << "" << i << " , " << j << "  " << pp.x << "   " << pp.y << endl;
				circle(dst1, cdt.pt, 10, Scalar(255, 255, 255), 1);
			}
			//Point p(0, 0);
			//if (ab_cross_cd(pt1, pt2, pt3, pt4, p))
			//{
			//	if (p.x != 0 && p.y != 0)//����н���
			//	{
			//		cdt.pt = p;
			//		cdt.indexh = i;
			//		cdt.indexz = j;
			//		veccrossdot.push_back(cdt);
			//		circle(dst1, p, 10, Scalar(255, 255, 255), 1);
			//	}
			//}
		}
	}
	cout << "��������" << veccrossdot.size() << endl;

	//for (int i = 0; i < newlineh.size(); i++)
	//{
	//	stringstream ss;
	//	ss << i;
	//	string s1;
	//	ss >> s1;
	//	p1 = Point(newlineh[i][0], newlineh[i][1]);
	//	p2 = Point(newlineh[i][2], newlineh[i][3]);
	//	int x = (p1.x + p2.x) / 2;
	//	int y = (p1.y + p2.y) / 2;
	//	line(dst1, p1, p2, Scalar(255), 1, 8);
	////	putText(dst1, s1, Point(x, y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255), 1, 8);
	//}
	//for (int i = 0; i < newlinez.size(); i++)
	//{
	//	stringstream ss;
	//	ss << i;
	//	string s1;
	//	ss >> s1;
	//	p1 = Point(newlinez[i][0], newlinez[i][1]);
	//	p2 = Point(newlinez[i][2], newlinez[i][3]);

	//	int x = (p1.x + p2.x) / 2;
	//	int y = (p1.y + p2.y) / 2;
	//	line(dst1, p1, p2, Scalar(255), 1, 8);
	//	//putText(dst1, s1, Point(x, y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255), 1, 8);
	//}
	//imshow("�ӳ�", dst1);
	static int imgCount = 0;
	string imageName;
	imageName = string("./imgpro\\6�����") + to_string(imgCount) + string(".jpg");
	imwrite(imageName, dst1);
	imgCount++;/**/
			   //	return  dst1;
	return 1;//�����߽�����ȡ����������1
			 //return veccrossdot;

}

//CheckMode: 0����ȥ��������1����ȥ��������; NeihborMode��0����4����1����8����;  
//���ȥ��С��ͨ����CheckMode = 1, NeihborMode = 1 
//ȥ���׶�CheckMode = 0��NeihborMode = 0
void ImgPro::RemoveSmallRegion(Mat &Src, Mat &Dst, int AreaLimit, int CheckMode, int NeihborMode)
{
	int RemoveCount = 0;
	//�½�һ����ǩͼ���ʼ��Ϊ0���ص㣬Ϊ�˼�¼ÿ�����ص����״̬�ı�ǩ��0����δ��飬1�������ڼ��,2�����鲻�ϸ���Ҫ��ת��ɫ����3������ϸ������   
	//��ʼ����ͼ��ȫ��Ϊ0��δ���  
	Mat PointLabel = Mat::zeros(Src.size(), CV_8UC1);
	if (CheckMode == 1)//ȥ��С��ͨ����İ�ɫ��  
	{
		//		cout << "ȥ��С��ͨ��." << endl;
		for (int i = 0; i < Src.rows; i++)
		{
			for (int j = 0; j < Src.cols; j++)
			{
				if (Src.at<uchar>(i, j) < 10)
				{
					PointLabel.at<uchar>(i, j) = 3;//��������ɫ����Ϊ�ϸ�����Ϊ3  
				}
			}
		}
	}
	else//ȥ���׶�����ɫ������  
	{
		//		cout << "ȥ���׶�" << endl;
		for (int i = 0; i < Src.rows; i++)
		{
			for (int j = 0; j < Src.cols; j++)
			{
				if (Src.at<uchar>(i, j) > 10)
				{
					PointLabel.at<uchar>(i, j) = 3;//���ԭͼ�ǰ�ɫ���򣬱��Ϊ�ϸ�����Ϊ3  
				}
			}
		}
	}


	vector<Point2i>NeihborPos;//������ѹ������  
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(0, 1));
	if (NeihborMode == 1)
	{
		//		cout << "Neighbor mode: 8����." << endl;
		NeihborPos.push_back(Point2i(-1, -1));
		NeihborPos.push_back(Point2i(-1, 1));
		NeihborPos.push_back(Point2i(1, -1));
		NeihborPos.push_back(Point2i(1, 1));
	}
	else cout << "Neighbor mode: 4����." << endl;
	int NeihborCount = 4 + 4 * NeihborMode;
	int CurrX = 0, CurrY = 0;
	//��ʼ���  
	for (int i = 0; i < Src.rows; i++)
	{
		for (int j = 0; j < Src.cols; j++)
		{
			if (PointLabel.at<uchar>(i, j) == 0)//��ǩͼ�����ص�Ϊ0����ʾ��δ���Ĳ��ϸ��  
			{   //��ʼ���  
				vector<Point2i>GrowBuffer;//��¼������ص�ĸ���  
				GrowBuffer.push_back(Point2i(j, i));
				PointLabel.at<uchar>(i, j) = 1;//���Ϊ���ڼ��  
				int CheckResult = 0;


				for (int z = 0; z < GrowBuffer.size(); z++)
				{
					for (int q = 0; q < NeihborCount; q++)
					{
						CurrX = GrowBuffer.at(z).x + NeihborPos.at(q).x;
						CurrY = GrowBuffer.at(z).y + NeihborPos.at(q).y;
						if (CurrX >= 0 && CurrX<Src.cols&&CurrY >= 0 && CurrY<Src.rows)  //��ֹԽ��    
						{
							if (PointLabel.at<uchar>(CurrY, CurrX) == 0)
							{
								GrowBuffer.push_back(Point2i(CurrX, CurrY));  //��������buffer    
								PointLabel.at<uchar>(CurrY, CurrX) = 1;           //���������ļ���ǩ�������ظ����    
							}
						}
					}
				}
				if (GrowBuffer.size()>AreaLimit) //�жϽ�����Ƿ񳬳��޶��Ĵ�С����1Ϊδ������2Ϊ����    
					CheckResult = 2;
				else
				{
					CheckResult = 1;
					RemoveCount++;//��¼�ж�������ȥ��  
				}


				for (int z = 0; z < GrowBuffer.size(); z++)
				{
					CurrX = GrowBuffer.at(z).x;
					CurrY = GrowBuffer.at(z).y;
					PointLabel.at<uchar>(CurrY, CurrX) += CheckResult;//��ǲ��ϸ�����ص㣬����ֵΪ2  
				}
				//********�����õ㴦�ļ��**********    


			}
		}


	}


	CheckMode = 255 * (1 - CheckMode);
	//��ʼ��ת�����С������    
	for (int i = 0; i < Src.rows; ++i)
	{
		for (int j = 0; j < Src.cols; ++j)
		{
			if (PointLabel.at<uchar>(i, j) == 2)
			{
				Dst.at<uchar>(i, j) = CheckMode;
			}
			else if (PointLabel.at<uchar>(i, j) == 3)
			{
				Dst.at<uchar>(i, j) = Src.at<uchar>(i, j);

			}
		}
	}
	//	cout << RemoveCount << " objects removed." << endl;
}

int ImgPro::findcrossdot2(Mat& mat, Mat& laser, vector<crossdot>& cdt)
{
	static int imgProcCount = 0;
	string imageName;
	vector<Point>veclaser;
	Mat gray, edge, bin;
	cvtColor(mat, gray, COLOR_BGR2GRAY);
	blur(gray, gray, Size(5, 5));//��ֵ�˲�
	threshold(gray, bin, 70, 255, THRESH_BINARY);
	imageName = string("./imgpro\\1��ֵ��") + to_string(imgProcCount) + string(".jpg");
	imwrite(imageName, bin);
	veclaser = Extlaser3(mat, laser);
	deletelaser2(bin, veclaser);
	imageName = string("./imgpro\\2ɾ������") + to_string(imgProcCount) + string(".jpg");
	imwrite(imageName, bin);
	bin = bin / 255;
	Mat thin = thinImage(bin);
	imageName = string("./imgpro\\3ϸ��") + to_string(imgProcCount) + string(".jpg");
	imwrite(imageName, thin * 255);
	Mat removesmallregion(imageSize, CV_8UC1, Scalar(0));
	RemoveSmallRegion(thin, removesmallregion, 40, 1, 1);
	imageName = string("./imgpro\\4ɾ��С����") + to_string(imgProcCount) + string(".jpg");
	imwrite(imageName, removesmallregion);
	imgProcCount++;
	//vector<crossdot> cdt;
	Mat dst2(imageSize, CV_8UC1, Scalar(0));//����ֱ��

											 //	dst2 = Hough22(removesmallregion, cdt);
											 //	return dst2;
	int t;
	t = Hough22(removesmallregion, cdt); cout << t;
	return t;
}


//����ַ�����ͼ����drawIndex()�е���
void ImgPro::drawText(Mat image, Point origin, string text)
{
	//���û����ı�����ز��� 
	int font_face = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;//DUPLEX;//SIMPLEX;//COMPLEX;
	double font_scale = 1.5;
	int thickness = 3;
	int baseline;
	//��ȡ�ı���ĳ���
	cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);
	cv::putText(image, text, origin, font_face, font_scale, cv::Scalar(255, 255, 255), thickness, 8, 0);
}
//��������㼰����
void ImgPro::drawIndex(Mat image, vector<crossdot>cdt)
{
	for (int i = 0; i < cdt.size(); i++)
	{
		circle(image, cdt[i].pt, 10, CV_RGB(0, 255, 255), 2, 8, 0);//��������߽���
		ostringstream os;
		//os << "(" << cdt[i].indexh << "," << cdt[i].indexz << ")"; //������ַ����������int����i������ 
		os << 9 * cdt[i].indexh + cdt[i].indexz; //������ַ����������int����i������ 
		string text = os.str(); //�����ַ�������str������ȡ���е�����
		drawText(image, cdt[i].pt, text);//������
	}
	static int imgCount = 0;
	string imageName;
	imageName = string("./imgpro\\���������") + to_string(imgCount) + string(".jpg");
	imwrite(imageName, image);
	imgCount++;/**/
}
//��ñ�Ե���ĸ���
int ImgPro::GetABCD(vector<crossdot>cdt, Point2f&PtLU_A, Point2f&PtLD_B, Point2f&PtRD_C, Point2f&PtRU_D)
{
	PtLU_A = Point(-1, -1);
	PtLD_B = Point(-1, -1);
	PtRD_C = Point(-1, -1);
	PtRU_D = Point(-1, -1);
	for (int i = 0; i < cdt.size(); i++)
	{
		if ((cdt[i].indexh == 0) && (cdt[i].indexz == 0))
			PtLU_A = cdt[i].pt;
		if ((cdt[i].indexh == 8) && (cdt[i].indexz == 0))
			PtLD_B = cdt[i].pt;
		if ((cdt[i].indexh == 8) && (cdt[i].indexz == 8))
			PtRD_C = cdt[i].pt;
		if ((cdt[i].indexh == 0) && (cdt[i].indexz == 8))
			PtRU_D = cdt[i].pt;
	}
	//cout << endl <<"�ĵ����꣺ "<< PtLU_A << PtLD_B << PtRD_C << PtRU_D<<endl;
	Point2f pttmp = Point(-1, -1);
	if ((PtLU_A == pttmp) || (PtLD_B == pttmp) || (PtRD_C == pttmp) || (PtRU_D == pttmp))
		return -1;//������ABCD�е�ĳ��
	else
		return 0;
}
//�жϵ��Ƿ���������������(�����⣬�ú��������ã�����)
vector<Point2d>ImgPro::isInRect(vector<Point2d>Pt_uv, Point2f PtLU_A, Point2f PtLD_B, Point2f PtRD_C, Point2f PtRU_D)
{
	Point Pt_E;
	double ret1, ret2, ret3, ret4;
	vector<Point2d>LaserPtInRect;
	Point A = PtLU_A;
	Point B = PtLD_B;
	Point C = PtRD_C;
	Point D = PtRU_D; cout << "ABCD" << endl << A << B << C << D << endl;
	for (int i = 0; i < Pt_uv.size(); i++)
	{
		Pt_E = Pt_uv[i]; //cout << Pt_E;
		ret1 = ab_cross_ac(A, B, Pt_E);
		ret2 = ab_cross_ac(B, C, Pt_E);//
		ret3 = ab_cross_ac(C, D, Pt_E);//
		ret4 = ab_cross_ac(D, A, Pt_E);//
		if ((ret1<0) && (ret2<0) && (ret3<0) && (ret4<0))
		{
			LaserPtInRect.push_back(Pt_uv[i]);
		}
		else
			continue;
	}
	return LaserPtInRect;
}
void ImgPro::drawLaserInRect(Mat img, vector<Point2d>LaserPtInRect)
{
	for (int i = 0; i < LaserPtInRect.size(); i++)
	{
		circle(img, LaserPtInRect[i], 1, CV_RGB(0, 0, 0), 1, 8, 0);
	}
	static int imgn = 0;
	string imageName;
	imageName = string("./imgpro\\������ȡ���") + to_string(imgn) + string(".jpg");
	imwrite(imageName, img);
	imgn++;/**/
}






