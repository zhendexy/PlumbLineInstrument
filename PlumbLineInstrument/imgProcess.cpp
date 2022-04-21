#include"imgProcess.h"


//*先用叉积判断是否在直线上，再调用点积判断是否在线段上*
double ImgPro::dot(double x1, double y1, double x2, double y2) //点积
{
	return x1*x2 + y1*y2;
}
int ImgPro::dblcmp(double a, double b)//比较大小
{
	if (fabs(a - b) <= 1E-6) return 0;//等于
	if (a>b) return 1;//大于
	else return -1;//小于
}
//先用叉积判断是否在直线上，再调用点积判断是否在线段上
int ImgPro::point_on_line(Point a, Point b, Point c) //求a点是不是在线段bc上，>0不在，=0与端点重合，<0在。
{
	return dblcmp(dot(b.x - a.x, b.y - a.y, c.x - a.x, c.y - a.y), 0);
}
//**************************************************
double ImgPro::cross(double x1, double y1, double x2, double y2)//叉积运算
{
	return x1*y2 - x2*y1;
}
double ImgPro::ab_cross_ac(Point a, Point b, Point c) //ab与ac的叉积
{
	return cross(b.x - a.x, b.y - a.y, c.x - a.x, c.y - a.y);
}
//求ab是否与cd相交，交点为p。1规范相交，0交点是一线段的端点，-1不相交。
//p点即交点坐标
int ImgPro::ab_cross_cd(Point a, Point b, Point c, Point d, Point &p)
{
	double s1, s2, s3, s4;
	int d1, d2, d3, d4;
	//Point p;
	d1 = dblcmp(s1 = ab_cross_ac(a, b, c), 0);
	d2 = dblcmp(s2 = ab_cross_ac(a, b, d), 0);
	d3 = dblcmp(s3 = ab_cross_ac(c, d, a), 0);
	d4 = dblcmp(s4 = ab_cross_ac(c, d, b), 0);

	//如果规范相交则求交点
	if ((d1^d2) == -2 && (d3^d4) == -2)
	{
		p.x = (c.x*s2 - d.x*s1) / (s2 - s1);
		p.y = (c.y*s2 - d.y*s1) / (s2 - s1);
		return 1;
	}

	//如果不规范相交
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
	//如果不相交
	return -1;
}
//计算两点距离
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
	int count = 0;  //记录迭代次数  
	while (true)
	{
		count++;
		if (maxIterations != -1 && count > maxIterations) //限制次数并且迭代次数到达  
			break;
		vector<uchar *> mFlag; //用于标记需要删除的点  
							   //对点标记  
		for (int i = 0; i < height; ++i)
		{
			uchar * p = dst.ptr<uchar>(i);
			for (int j = 0; j < width; ++j)
			{
				//如果满足四个条件，进行标记  
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
						//标记  
						mFlag.push_back(p + j);
					}
				}
			}
		}
		//将标记的点删除  
		for (vector<uchar *>::iterator i = mFlag.begin(); i != mFlag.end(); ++i)
		{
			**i = 0;
		}
		//直到没有点满足，算法结束  
		if (mFlag.empty())
		{
			break;
		}
		else
		{
			mFlag.clear();//将mFlag清空  
		}
	}
	return dst * 255;
}

//输入三通道图像
vector<Point> ImgPro::Extlaser(Mat mat)
{
	Laser2DPoints.clear();
	//	CStringA fname;
	Mat bin, imageRed, thin;
	vector<Mat> channels;
	split(mat, channels);
	imageRed = channels.at(2);
	//	imshow("red", imageRed);
	blur(imageRed, imageRed, Size(5, 5));//中值滤波
										 //处理 激光 对红色分量 阈值化
	threshold(imageRed, bin, 150, 255, THRESH_BINARY);
	//	imshow("binred", bin);
	//	fname.Format("C:\\Users\\Rachmaninov\\Desktop\\cam1_red.bmp");
	//imwrite(fname.GetBuffer(), imageRed);
	thin = thinImage(bin / 255);
	//	imshow("thinlaser", thin);
	//imshow("bin", bin);

	Mat Result(1024, 1280, CV_8UC1, Scalar(0));
	vector<Point>veclaser;//储存激光点坐标
	Point pt;
	for (int i = 0; i < thin.rows; i++)//行
	{
		for (int j = 0; j < thin.cols; j++)//列
		{
			if (thin.at<uchar>(i, j) == 255)
			{
				pt.x = j;
				pt.y = i;
				veclaser.push_back(pt);
				Laser2DPoints.push_back(pt);
				//cout << pt.x << "   " << pt.y << endl;    //  列  行
				Result.at<uchar>(pt.y, pt.x) = 255;
			}
		}
	}
	return veclaser;
}



vector<Point> ImgPro::Extlaser3(Mat& mat, Mat& laser)
{
	//并未达到亚像素
	Laser2DPoints.clear();
	vector<Point> vec;
	double threshold = 220;
	int red_tolerant = 3;
	int gray_tolerant = 10;
	int gray_counts = 9;


	Mat temp, temp1;
	/////////////////////////////关键/////////////////////////////////////////////////
	cvtColor(mat, temp1, COLOR_BGR2GRAY);
	GaussianBlur(temp1, temp1, Size(3, 3), 0, 0);
	imwrite("temp1.jpg", temp1);
	//读取手动提取激光线的图像laser
	cvtColor(laser, temp, COLOR_BGR2GRAY);
	cv::threshold(temp, temp, 50, 255, THRESH_TOZERO);
	GaussianBlur(temp, temp, Size(3, 3), 0, 0);
	imwrite("temp.jpg", temp);
	/////////////////////////////////////////////////////////////////////////////////

	//检测竖直的激光
	//cv::Point pt;//用于存放激光线中心点uv坐标
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

	//检测水平的激光
	cv::Point pt;//用于存放激光线中心点uv坐标
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
	imageName = string("./imgpro\\7激光") + to_string(imgnum) + string(".jpg");
	imwrite(imageName, Result);
	imgnum++;
	return vec;
}

void ImgPro::deletelaser(Mat &bin, vector<Point>veclaser)
{
	for (int i = 0; i < veclaser.size(); i++)
	{
		int x = veclaser[i].x;//列
		int y = veclaser[i].y;//行
		for (int j = max(x - 10, 0); j < min(x + 10, 1280); j++)//列
		{
			for (int k = max(y - 10, 0); k < min(y + 10, 1024); k++)//行
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
		int x = veclaser[i].x;//列	u
		int y = veclaser[i].y;//行	v
		for (int j = max(y - 80, 0); j < min(y + 80, image_height); j++)//行		v
		{
			bin.at<uchar>(j, x) = 0;
		}
	}
}

//画线段的延长线 输出 pa  pb  的值
void ImgPro::OnDrawDotline(Point p1, Point p2, Point & pa, Point & pb, Mat mat, bool Isheng)
{
	double h = mat.rows, w = mat.cols;
	double k = (p1.y - p2.y) / (p1.x - p2.x + 0.000001);//不加0.000001 会变成曲线，斜率可能为0，即e.x-s.x可能为0  //斜率	
	double b = p1.y - k*p1.x;
	if (Isheng)//横向
	{
		if (b < 1280)//原：1024
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
	else//纵向
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
//直线排序
int ImgPro::sortline(vector<Vec4i> Lines, vector<Vec4i>&final, bool Isheng)
{
	if (Lines.size() == 0)
	{
		cout << "*************输入直线为空，无法排序,已跳过当前帧!**************";
		return -1;
	}
	vector<Vec4i>result_line;
	vector<vector<Vec4i>>linesort;
	//	int dif1,dif2,dif3,dif4;
	int n;
	if (Isheng)//横向直线排序
		n = 50;  //截距阈值	
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
	/////////出了错/////

	Mat dst3(imageSize, CV_8UC1, Scalar(0));//横向直线
	Point p1, p2;
	Point po(0, 0);
	Vec4i lineyj;//存储最近最远点
				 //画出分类
				 /*for (int i = 0; i < linesort.size(); i++)
				 {
				 stringstream ss;
				 ss << i;
				 string s1;
				 ss >> s1;
				 Point pj, py;
				 cout << "第 " << i << "类" << endl;
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
		//result_line.push_back(lineyj);//连接最近最远点 ，没有延长
		final.push_back(lineyj);
		putText(dst3, s1, Point(x, y), FONT_HERSHEY_SIMPLEX, 1, Scalar(255), 2, 8);
	}
	//延长
	//Vec4i lineby;//存储边缘点
	//Point  pt1, pt2, pt3, pt4;
	//for (int i = 0; i < result_line.size(); i++)
	//{
	//	if (Isheng)
	//	{
	//		p1 = Point(result_line[i][0], result_line[i][1]);
	//		p2 = Point(result_line[i][2], result_line[i][3]);
	//		OnDrawDotline(p1, p2, pt1, pt2, dst3, 1);//横向
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
	//		OnDrawDotline(p1, p2, pt3, pt4, dst3, 0);//纵向
	//		lineby[0] = pt3.x;
	//		lineby[1] = pt3.y;
	//		lineby[2] = pt4.x;
	//		lineby[3] = pt4.y;
	//		final.push_back(lineby);
	//	}
	//}
	if (Isheng)
	{
		cout << "分类后 横向 直线类数：" << linesort.size() << endl;
	}
	else
	{
		cout << "分类后 纵向 直线类数：" << linesort.size() << endl;
	}
	static int imgsortnum = 0;
	string imageName;
	//画出分类  - end
	if (Isheng)
	{
		////imshow("横向分类", dst3);
		imageName = string("./imgpro\\5横向分类") + to_string(imgsortnum) + string(".jpg");
		imwrite(imageName, dst3);
	}

	else
	{
		/////imshow("纵向分类", dst3);
		imageName = string("./imgpro\\5纵向分类") + to_string(imgsortnum) + string(".jpg");
		imwrite(imageName, dst3);
	}
	imgsortnum++;/**/
	if (linesort.size() != 9)
		return 0;//分类结果不是9类
	else
		return 1;//分类结果是9类

}

vector<int>ImgPro::sort_indexes(vector<double> &v, bool Isheng)
{

	// 初始化索引向量
	vector<int> idx(v.size());
	//使用iota对向量赋0~？的连续值
	iota(idx.begin(), idx.end(), 0);

	// 通过比较v的值对索引idx进行排序
	if (Isheng)
		sort(idx.begin(), idx.end(), [&v](int i1, int i2) { return v[i1] < v[i2]; });//升序
	else
		sort(idx.begin(), idx.end(), [&v](int i1, int i2) { return v[i1] > v[i2]; });//降序
	return idx;
}
//对横纵直线 排序  返回 排序后的直线
vector<Vec4i> ImgPro::ranklines(vector<Vec4i>Lines, bool Isheng)
{
	//分 数量 为9  和小于 9
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

//检测出线段,并画线段的延长线
//vector<crossdot> Hough22(Mat contours)
int ImgPro::Hough22(Mat contours, vector<crossdot>&veccrossdot)
{
	Mat dst1(imageSize, CV_8UC1, Scalar(0));//纵向直线
											 //Mat dst2(1024, 1280, CV_8UC1, Scalar(0));//横向直线
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
		if (t1<t2)//纵向
		{
			Lines_zong.push_back(Lines[i]);
		}
		if (t2<t1)
		{
			Lines_heng.push_back(Lines[i]);
		}
	}
	//cout << "*************横" << endl;
	//for (int i = 0; i < Lines_heng.size(); i++)
	//{
	//	p1 = Point(Lines_heng[i][0], Lines_heng[i][1]);
	//	p2 = Point(Lines_heng[i][2], Lines_heng[i][3]);
	//	double k = (p1.y - p2.y) / (p1.x - p2.x + 0.000001);
	//	double b = p1.y - k*p1.x;
	//	cout << k << "  " << b << endl;
	//	line(dst1, p1, p2, Scalar(255), 1, 8);
	//}
	//cout << "*************纵" << endl;
	//for (int i = 0; i < Lines_zong.size(); i++)
	//{
	//	p1 = Point(Lines_zong[i][0], Lines_zong[i][1]);
	//	p2 = Point(Lines_zong[i][2], Lines_zong[i][3]);
	//	double k = (p1.y - p2.y) / (p1.x - p2.x + 0.000001);
	//	double b = p1.y - k*p1.x;
	//	cout << k << "  " << b << endl;
	//	//	line(dst1, p1, p2, Scalar(255), 1, 8);
	//}
	vector<Vec4i>linesort_heng, linesort_zong;//储存分类后的直线
	int t1, t2;
	t1 = sortline(Lines_heng, linesort_heng, 1);
	t2 = sortline(Lines_zong, linesort_zong, 0);
	if (t1 != 1 || t2 != 1)//哪一类检测少于九类
		return 0;//直线检测少于九类，则不提取网格线交点，该帧图像无效	
				 //cout << "at hough22";//tiaoshitianjia 在此返回！！！！
	vector<Vec4i>newlineh, newlinez;//排序后的直线
	newlineh = ranklines(linesort_heng, 1);
	newlinez = ranklines(linesort_zong, 0);

	cout << "*************横" << endl;
	for (int i = 0; i < linesort_heng.size(); i++)
	{
		p1 = Point(newlineh[i][0], newlineh[i][1]);
		p2 = Point(newlineh[i][2], newlineh[i][3]);
		double k = (p1.y - p2.y) / (p1.x - p2.x + 0.000001);
		double b = p1.y - k*p1.x;
		//		cout << k << "  " << b << endl;
		//	line(dst1, p1, p2, Scalar(255), 1, 8);
	}
	cout << "*************纵" << endl;
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
		Point pt1(newlineh[i][0], newlineh[i][1]);//点a
		Point pt2(newlineh[i][2], newlineh[i][3]);//点b  线段ab
		double k1 = (pt1.y - pt2.y) / (pt1.x - pt2.x + 0.000001);
		double b1 = pt1.y - k1*pt1.x;
		for (int j = 0; j < linesort_zong.size(); j++)
		{
			Point pt3(newlinez[j][0], newlinez[j][1]); //点c
			Point pt4(newlinez[j][2], newlinez[j][3]); //点d 线段cd
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
			//	if (p.x != 0 && p.y != 0)//如果有交点
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
	cout << "交点数：" << veccrossdot.size() << endl;

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
	//imshow("延长", dst1);
	static int imgCount = 0;
	string imageName;
	imageName = string("./imgpro\\6网格点") + to_string(imgCount) + string(".jpg");
	imwrite(imageName, dst1);
	imgCount++;/**/
			   //	return  dst1;
	return 1;//网格线交点提取正常，返回1
			 //return veccrossdot;

}

//CheckMode: 0代表去除黑区域，1代表去除白区域; NeihborMode：0代表4邻域，1代表8邻域;  
//如果去除小连通区域CheckMode = 1, NeihborMode = 1 
//去除孔洞CheckMode = 0，NeihborMode = 0
void ImgPro::RemoveSmallRegion(Mat &Src, Mat &Dst, int AreaLimit, int CheckMode, int NeihborMode)
{
	int RemoveCount = 0;
	//新建一幅标签图像初始化为0像素点，为了记录每个像素点检验状态的标签，0代表未检查，1代表正在检查,2代表检查不合格（需要反转颜色），3代表检查合格或不需检查   
	//初始化的图像全部为0，未检查  
	Mat PointLabel = Mat::zeros(Src.size(), CV_8UC1);
	if (CheckMode == 1)//去除小连通区域的白色点  
	{
		//		cout << "去除小连通域." << endl;
		for (int i = 0; i < Src.rows; i++)
		{
			for (int j = 0; j < Src.cols; j++)
			{
				if (Src.at<uchar>(i, j) < 10)
				{
					PointLabel.at<uchar>(i, j) = 3;//将背景黑色点标记为合格，像素为3  
				}
			}
		}
	}
	else//去除孔洞，黑色点像素  
	{
		//		cout << "去除孔洞" << endl;
		for (int i = 0; i < Src.rows; i++)
		{
			for (int j = 0; j < Src.cols; j++)
			{
				if (Src.at<uchar>(i, j) > 10)
				{
					PointLabel.at<uchar>(i, j) = 3;//如果原图是白色区域，标记为合格，像素为3  
				}
			}
		}
	}


	vector<Point2i>NeihborPos;//将邻域压进容器  
	NeihborPos.push_back(Point2i(-1, 0));
	NeihborPos.push_back(Point2i(1, 0));
	NeihborPos.push_back(Point2i(0, -1));
	NeihborPos.push_back(Point2i(0, 1));
	if (NeihborMode == 1)
	{
		//		cout << "Neighbor mode: 8邻域." << endl;
		NeihborPos.push_back(Point2i(-1, -1));
		NeihborPos.push_back(Point2i(-1, 1));
		NeihborPos.push_back(Point2i(1, -1));
		NeihborPos.push_back(Point2i(1, 1));
	}
	else cout << "Neighbor mode: 4邻域." << endl;
	int NeihborCount = 4 + 4 * NeihborMode;
	int CurrX = 0, CurrY = 0;
	//开始检测  
	for (int i = 0; i < Src.rows; i++)
	{
		for (int j = 0; j < Src.cols; j++)
		{
			if (PointLabel.at<uchar>(i, j) == 0)//标签图像像素点为0，表示还未检查的不合格点  
			{   //开始检查  
				vector<Point2i>GrowBuffer;//记录检查像素点的个数  
				GrowBuffer.push_back(Point2i(j, i));
				PointLabel.at<uchar>(i, j) = 1;//标记为正在检查  
				int CheckResult = 0;


				for (int z = 0; z < GrowBuffer.size(); z++)
				{
					for (int q = 0; q < NeihborCount; q++)
					{
						CurrX = GrowBuffer.at(z).x + NeihborPos.at(q).x;
						CurrY = GrowBuffer.at(z).y + NeihborPos.at(q).y;
						if (CurrX >= 0 && CurrX<Src.cols&&CurrY >= 0 && CurrY<Src.rows)  //防止越界    
						{
							if (PointLabel.at<uchar>(CurrY, CurrX) == 0)
							{
								GrowBuffer.push_back(Point2i(CurrX, CurrY));  //邻域点加入buffer    
								PointLabel.at<uchar>(CurrY, CurrX) = 1;           //更新邻域点的检查标签，避免重复检查    
							}
						}
					}
				}
				if (GrowBuffer.size()>AreaLimit) //判断结果（是否超出限定的大小），1为未超出，2为超出    
					CheckResult = 2;
				else
				{
					CheckResult = 1;
					RemoveCount++;//记录有多少区域被去除  
				}


				for (int z = 0; z < GrowBuffer.size(); z++)
				{
					CurrX = GrowBuffer.at(z).x;
					CurrY = GrowBuffer.at(z).y;
					PointLabel.at<uchar>(CurrY, CurrX) += CheckResult;//标记不合格的像素点，像素值为2  
				}
				//********结束该点处的检查**********    


			}
		}


	}


	CheckMode = 255 * (1 - CheckMode);
	//开始反转面积过小的区域    
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
	blur(gray, gray, Size(5, 5));//中值滤波
	threshold(gray, bin, 70, 255, THRESH_BINARY);
	imageName = string("./imgpro\\1二值化") + to_string(imgProcCount) + string(".jpg");
	imwrite(imageName, bin);
	veclaser = Extlaser3(mat, laser);
	deletelaser2(bin, veclaser);
	imageName = string("./imgpro\\2删除激光") + to_string(imgProcCount) + string(".jpg");
	imwrite(imageName, bin);
	bin = bin / 255;
	Mat thin = thinImage(bin);
	imageName = string("./imgpro\\3细化") + to_string(imgProcCount) + string(".jpg");
	imwrite(imageName, thin * 255);
	Mat removesmallregion(imageSize, CV_8UC1, Scalar(0));
	RemoveSmallRegion(thin, removesmallregion, 40, 1, 1);
	imageName = string("./imgpro\\4删除小区域") + to_string(imgProcCount) + string(".jpg");
	imwrite(imageName, removesmallregion);
	imgProcCount++;
	//vector<crossdot> cdt;
	Mat dst2(imageSize, CV_8UC1, Scalar(0));//横向直线

											 //	dst2 = Hough22(removesmallregion, cdt);
											 //	return dst2;
	int t;
	t = Hough22(removesmallregion, cdt); cout << t;
	return t;
}


//输出字符串到图像，在drawIndex()中调用
void ImgPro::drawText(Mat image, Point origin, string text)
{
	//设置绘制文本的相关参数 
	int font_face = cv::FONT_HERSHEY_SCRIPT_SIMPLEX;//DUPLEX;//SIMPLEX;//COMPLEX;
	double font_scale = 1.5;
	int thickness = 3;
	int baseline;
	//获取文本框的长宽
	cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);
	cv::putText(image, text, origin, font_face, font_scale, cv::Scalar(255, 255, 255), thickness, 8, 0);
}
//画出网格点及索引
void ImgPro::drawIndex(Mat image, vector<crossdot>cdt)
{
	for (int i = 0; i < cdt.size(); i++)
	{
		circle(image, cdt[i].pt, 10, CV_RGB(0, 255, 255), 2, 8, 0);//标出网格线交点
		ostringstream os;
		//os << "(" << cdt[i].indexh << "," << cdt[i].indexz << ")"; //向输出字符串流中输出int整数i的内容 
		os << 9 * cdt[i].indexh + cdt[i].indexz; //向输出字符串流中输出int整数i的内容 
		string text = os.str(); //利用字符串流的str函数获取流中的内容
		drawText(image, cdt[i].pt, text);//标出序号
	}
	static int imgCount = 0;
	string imageName;
	imageName = string("./imgpro\\网格点排序") + to_string(imgCount) + string(".jpg");
	imwrite(imageName, image);
	imgCount++;/**/
}
//获得边缘的四个点
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
	//cout << endl <<"四点坐标： "<< PtLU_A << PtLD_B << PtRD_C << PtRU_D<<endl;
	Point2f pttmp = Point(-1, -1);
	if ((PtLU_A == pttmp) || (PtLD_B == pttmp) || (PtRD_C == pttmp) || (PtRU_D == pttmp))
		return -1;//不存在ABCD中的某点
	else
		return 0;
}
//判断点是否在网格线区域内(有问题，该函数不可用！！！)
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
	imageName = string("./imgpro\\激光提取结果") + to_string(imgn) + string(".jpg");
	imwrite(imageName, img);
	imgn++;/**/
}






