#include "planeCali.h"


/*
激光平面标定
参数：
imageList		存放标定图片名称的txt
rayPlaneCalibrateResult		存放标定结果的txt
rayPlaneParams	激光平面标定结果
projectionMatrix相机的投影矩阵
cameraMatrix	相机的内参数矩阵
distCoeffs		相机的畸变系数
*/
bool PlaneCali::rayPlaneCalibrate(const char* imageFolder, const char* imageList, const char* uvPoints, const char* realPoints, const char* caliPoints,
	const char* caliErr, const char*rayPlaneCali_result, Mat& rayPlaneParams, Mat& projectionMatrix, Mat& cameraMatrix, Mat& distCoeffs)
{
	ifstream imageStore(data_folder + string(imageList)); // 打开存放标定图片名称的txt
	fstream fs_uvPoints(data_folder + string(uvPoints), ios::out);
	fstream fs_realPoints(data_folder + string(realPoints), ios::out);
	fstream fs_caliPoints(data_folder + string(caliPoints), ios::out);
	fstream fs_caliErr(data_folder + string(caliErr), ios::out);
	FileStorage rayLineStore(data_folder + string(rayPlaneCali_result), FileStorage::WRITE);

	vector<Point> rayLinePoints;
	vector<Point> tempPoints;
	vector<int> zws;
	string imageName; // 读取的标定图片的名称
	while (getline(imageStore, imageName)) // 读取txt的每一行（每一行存放了一张标定图片的名称）
	{
		srcImage = imread(imageFolder + imageName);
		Mat srcImage1;
		undistort(srcImage, srcImage1, cameraMatrix, distCoeffs); // 矫正畸变
																  //remap(srcImage, srcImage1, map1, map2, INTER_LINEAR);
		srcImage = srcImage1;
		imwrite(data_folder + string("rectifyImage.jpg"), srcImage);

		string zw_str = "";	// 通过图片文件名，读取zw数据		left01_10.jpg	right01_10.jpg
		char c = imageName[0];
		if (c == 'l')
			for (int i = 7; i < imageName.size(); ++i)
			{
				c = imageName[i];
				if (c == '.')
					break;
				zw_str += c;
			}
		else if (c == 'r')
			for (int i = 8; i < imageName.size(); ++i)
			{
				c = imageName[i];
				if (c == '.')
					break;
				zw_str += c;
			}
		int zw = atoi(zw_str.c_str());

		getRayLinePoints(srcImage, tempPoints); // 提取激光光条中心线uv坐标
		for (int i = 0; i < tempPoints.size(); ++i)
		{
			rayLinePoints.push_back(tempPoints[i]);
			zws.push_back(zw);

			fs_uvPoints << tempPoints[i].x << " " << tempPoints[i].y << " " << zw << endl;
		}
		tempPoints.clear();
	}

	int m = rayLinePoints.size();
	Mat L = Mat::ones(m, 1, CV_32FC1);
	Mat realPointsMat = Mat(m, 3, CV_32FC1);
	for (int i = 0; i < m; ++i)
	{
		Point2f xwyw = uvzw2xwyw(rayLinePoints[i], zws[i], projectionMatrix);
		realPointsMat.at<float>(i, 0) = xwyw.x;
		realPointsMat.at<float>(i, 1) = xwyw.y;
		realPointsMat.at<float>(i, 2) = zws[i];

		fs_realPoints << xwyw.x << " " << xwyw.y << " " << zws[i] << endl;
	}
	solve(realPointsMat, L, rayPlaneParams, DECOMP_SVD);
	rayLineStore << "rayPlaneParams" << rayPlaneParams;

	// 保存每个特征点的空间坐标
	// 计算误差 根据参数反算出的值-实际值
	for (int i = 0; i < m; ++i)
	{
		Point3f caliPoint;
		uv2xwywzw(rayLinePoints[i], caliPoint, projectionMatrix);
		fs_caliPoints << caliPoint.x << " " << caliPoint.y << " " << caliPoint.z << endl;
		fs_caliErr << caliPoint.x - realPointsMat.at<float>(i, 0) << " " << caliPoint.y - realPointsMat.at<float>(i, 1) << " " << caliPoint.z - realPointsMat.at<float>(i, 2) << endl;
	}


	fs_uvPoints.close();
	fs_realPoints.close();
	fs_caliPoints.close();
	fs_caliErr.close();
	rayLineStore.release();
	return true;
}

bool PlaneCali::laserPlaneCali(const char* imageFolder, const char* imageList, const char* laserList, const char* uvPoints, const char* realPoints, const char* caliPoints,
	const char* caliErr, const char* laserPlaneCali_result, Mat& laserPlaneParams, Mat& projectionMatrix, Mat& cameraMatrix, Mat& distCoeffs)
{
	AllocConsole();
	freopen("CONOUT$", "w+t", stdout);

	double start_time = clock();
	CaliCalculate cal(cameraMatrix, distCoeffs, projectionMatrix);//计算类对象
	Mat map1, map2;
	initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), Mat(), imageSize, CV_32FC1, map1, map2);//计算无畸变和修正转换映射

	ifstream imageStore(imageFolder + string(imageList)); // 打开存放标定图片名称的txt
	ifstream laserStore(imageFolder + string(laserList));
	string imageName, laserName;
	vector<string> imageNames, laserNames;
	vector<Mat> images, lasers;
	while (getline(imageStore, imageName))
	{
		getline(laserStore, laserName);
		laserNames.push_back(laserName);
		imageNames.push_back(imageName);

		Mat dst;
		Mat image = imread(imageFolder + imageName);
		undistort(image, dst, cameraMatrix, distCoeffs);
		//remap(image, dst, map1, map2, INTER_LINEAR);
		images.push_back(dst.clone());
		imwrite(imageFolder + string("rect_") + imageName, dst);

		Mat laser = imread(imageFolder + laserName);
		undistort(laser, dst, cameraMatrix, distCoeffs);
		//remap(laser, dst, map1, map2, INTER_LINEAR);
		lasers.push_back(dst.clone());
		imwrite(imageFolder + string("rect_") + laserName, dst);

		ImgNum++;
	}



	//求网格世界坐标系到摄像机坐标系的RT矩阵
	ImgPro imgpro(imageFolder, image_width, image_height);//图像处理类对象

	vector<Point3f> laserPt_C;
	string PCfile = "./PtCloud\\Pt_XcYcZc.txt";
	string Ptuvfile = "./PtCloud\\Pt_uv.txt";//激光光条中心
	ofstream Ptuvout(Ptuvfile);//打开文件
	ofstream PCout(PCfile);//打开文件
	Mat ExtriM = Mat::zeros(3, 4, CV_64FC1);//合并RT得到的外参
	for (int i = 0; i <ImgNum; i = i + 1)//计算
	{
		Mat img = images[i].clone();
		Mat laser = lasers[i].clone();
		vector<crossdot> cdt;
		int Getcdtok = imgpro.findcrossdot2(img, laser, cdt);//提取网格线交点
		if (Getcdtok == 0)
		{
			cout << "******第" << i << "帧网格线交点提取失败！******" << endl;
			continue;
		}//如果t=0则提取网格线交点失败，继续下一次循环
		else
			cout << "******第" << i << "帧网格线交点提取成功！******" << endl;
		imgpro.drawIndex(img, cdt);//画出网格线交点及索引
								   //imshow("show", img);

		bool GetRTok = cal.getR_T(cdt);//得到网格坐标系到摄像机坐标系的RT矩阵
		if (!GetRTok)continue;
		cout << "旋转矩阵: " << cal.RotationMat << endl;
		cout << "平移向量：" << cal.TransVec << endl;
		/*
		Mat RTMatrix=Mat::zeros(3, 4, CV_64FC1);
		cal.mergeRT(RTMatrix, cal.RotationMat, cal.TransVec);
		RTname.Format("./RT\\RT%d",i);
		ofstream outRT(RTname.GetBuffer());
		outRT << RTMatrix.at<double>(0, 0)<<","<< RTMatrix.at<double>(0, 1)
		<< "," << RTMatrix.at<double>(0, 2) << "," << RTMatrix.at<double>(1, 0)
		<< "," << RTMatrix.at<double>(1, 1) << "," << RTMatrix.at<double>(1, 2)
		<< "," << RTMatrix.at<double>(2, 0) << "," << RTMatrix.at<double>(2, 1)
		<< "," << RTMatrix.at<double>(2, 2) << "," << RTMatrix.at<double>(0, 3)
		<< "," << RTMatrix.at<double>(1, 3) << "," << RTMatrix.at<double>(2, 3); // cal.RotationMat << endl << cal.TransVec;
		outRT.close();
		*/
		vector<Point2d> pt_uv;
		vector<Point2d> pt_XwYw;//Zw=0,棋盘格坐标系
		vector<Point3d> Pt_XcYcZc;
		for (int i = 0; i < imgpro.Laser2DPoints.size(); i++)
			pt_uv.push_back(imgpro.Laser2DPoints[i]);//point to point2d
													 //Point2f A, B, C, D;
													 //int  getdotok = imgpro.GetABCD(cdt, A, B, C, D);
													 //if (getdotok == -1)continue;
													 //Vector<Point2d>pt_uvInRect;
													 //pt_uvInRect = imgpro.isInRect(pt_uv,A, B, C, D);
		if (Ptuvout.is_open())
		{
			//cout << "lasercentsize:" << pt_uvInRect.size()<<endl;
			for (unsigned int k = 0; k < pt_uv.size(); k++)
				//Ptuvout << pt_uvInRect[k].x << " " << pt_uvInRect[k].y << endl;
				Ptuvout << pt_uv[k].x << " " << pt_uv[k].y << endl;
		}
		imgpro.drawLaserInRect(img, pt_uv);//激光提取效果
		cal.mergeRT(ExtriM, cal.RotationMat, cal.TransVec);
		cal.CalWorldPoint(pt_uv, cal.cameraMatrix, ExtriM, pt_XwYw);
		cal.test_accur(cdt, ExtriM);//网格点坐标重建检验网格点提取精度
		cal.WorldToCam(cal.RotationMat, cal.TransVec, pt_XwYw, Pt_XcYcZc);
		if (PCout.is_open())
		{
			for (unsigned int k = 0; k < Pt_XcYcZc.size(); k++)
			{
				PCout << Pt_XcYcZc[k].x << " " << Pt_XcYcZc[k].y << " " << Pt_XcYcZc[k].z << "\n";
				laserPt_C.push_back(Pt_XcYcZc[k]);
			}
		}
	}
	Ptuvout.close();
	PCout.close();
	cout << "处理已完成!";
	int waitein;
	cin >> waitein;


	
	FileStorage laserParamsStore(data_folder + string(laserPlaneCali_result), FileStorage::WRITE);
	int m = laserPt_C.size();
	Mat L = Mat::ones(m, 1, CV_32FC1);
	Mat realPointsMat = Mat(m, 3, CV_32FC1);
	for (int i = 0; i < m; ++i)
	{
		realPointsMat.at<float>(i, 0) = laserPt_C[i].x;
		realPointsMat.at<float>(i, 1) = laserPt_C[i].y;
		realPointsMat.at<float>(i, 2) = laserPt_C[i].z;
	}
	solve(realPointsMat, L, laserPlaneParams, DECOMP_SVD);
	laserParamsStore << "laserPlaneParams" << laserPlaneParams;
	laserParamsStore.release();

	return true;
}





/*
提取激光光条中心点坐标
参数：
srcImage	输入图像，提前矫正畸变
T_rayLine	二值化阈值
redtolerant
graytolerant
linePoints	输出的中心点坐标
*/
void PlaneCali::getRayLinePoints(Mat& srcImage, vector<Point>& linePoints)
{
	cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);

	//midImage = grayImage.clone();
	//threshold(midImage, midImage, T_rayLine, 255, THRESH_BINARY);
	//maskImage = Mat::ones(midImage.size(), CV_8UC1); // 去除小连通区域的图像mask
	//findSmallRegions(midImage, maskImage);
	//imwrite(DATA_FOLDER + string("maskImage_gray.jpg"), maskImage);
	//bitwise_not(maskImage, maskImage);
	//bitwise_and(grayImage, maskImage, grayImage);
	imwrite(data_folder + string("grayImage.jpg"), grayImage);


	vector<Mat> BGR;
	split(srcImage, BGR); // opencv读取图像是BGR的
	redImage = BGR.at(2).clone(); // 取出BGR中的R分量

	midImage = redImage.clone();
	threshold(midImage, midImage, T, 255, THRESH_BINARY);
	maskImage = Mat::zeros(midImage.size(), CV_8UC1); // 去除小连通区域的图像mask
	deleteSmallRegions(midImage, maskImage);
	imwrite(data_folder + string("maskImage_red.jpg"), maskImage);
	for (int i = 0; i < redImage.rows; ++i)
		for (int j = 0; j < redImage.cols; ++j)
		{
			int v = redImage.at<uchar>(i, j);
			redImage.at<uchar>(i, j) = v > T ? v : 0; // redImage二值化
		}
	bitwise_and(redImage, maskImage, redImage);
	imwrite(data_folder + string("redImage.jpg"), redImage);

	//激光线是横着的，所以扫描列
	int num = 9;
	int max_red, max_gray;
	cv::Point ij_red, ij_gray;
	vector<int> maxs_red, maxs_gray;
	vector<cv::Point> ijs_red, ijs_gray;
	for (int j = 0; j < redImage.cols; ++j)
	{
		//找到redImage这一列中灰度最大的点
		max_red = redImage.at<uchar>(0, j);
		ij_red.x = 0;
		ij_red.y = j;
		for (int i = 0; i < redImage.rows; ++i)
		{
			if (redImage.at<uchar>(i, j) > max_red)
			{
				max_red = redImage.at<uchar>(i, j);
				ij_red.x = i;
				ij_red.y = j;
			}
		}
		if (max_red == 0)
			continue;
		//找到redImage这一列的候选点
		for (int i = 0; i < redImage.rows; ++i)
		{
			if (redImage.at<uchar>(i, j) > max_red - redtolerant)
			{
				maxs_red.push_back(redImage.at<uchar>(i, j));
				ijs_red.push_back(cv::Point(i, j));
			}
		}


		//将redImage中候选点对应到grayImage，找到候选点中灰度值最大的点
		max_gray = grayImage.at<uchar>(ij_red.x, ij_red.y);
		ij_gray = ij_red;
		for (int i = 0; i < ijs_red.size(); ++i)
		{
			if (grayImage.at<uchar>(ij_red.x, ij_red.y) > max_gray)
			{
				max_gray = grayImage.at<uchar>(ij_red.x, ij_red.y);
				ij_gray.x = i;
				ij_gray.y = j;
			}
		}

		//找到grayImage这一列的候选点
		//不控制候选点不超过9个貌似效果更好
		for (int i = 0; i < grayImage.rows; ++i)
		{
			if (grayImage.at<uchar>(i, j) > max_gray - graytolerant)
			{
				maxs_gray.push_back(grayImage.at<uchar>(i, j));
				ijs_gray.push_back(cv::Point(i, j));
			}
		}

		////找到grayImage这一列的候选点
		////从max_gray的位置开始找候选点
		//int i = ij_gray.x;
		//maxs_gray.push_back(max_gray);
		//ijs_gray.push_back(ij_gray);
		//for (int n = 1; n < min(i + 1, grayImage.rows - i); ++n)
		//{
		//	if (grayImage.at<uchar>(i - n, j) > max_gray - graytolerant)
		//	{
		//		maxs_gray.push_back(grayImage.at<uchar>(i - n, j));
		//		ijs_gray.push_back(cv::Point(i - n, j));
		//	}
		//	if (grayImage.at<uchar>(i + n, j) > max_gray - graytolerant)
		//	{
		//		maxs_gray.push_back(grayImage.at<uchar>(i + n, j));
		//		ijs_gray.push_back(cv::Point(i + n, j));
		//	}

		//	//控制候选点个数不超过9个
		//	if (maxs_gray.size() > num)
		//		break;
		//}



		//求候选点的灰度重心，作为激光中心点的坐标
		int u, v;
		int fenzi1 = 0, fenzi2 = 0, fenmu = 0;
		for (int i = 0; i < ijs_gray.size(); ++i)
		{
			fenzi1 += ijs_gray[i].x*grayImage.at<uchar>(ij_red.x, ij_red.y);
			//fenzi2 += ijs_gray[i].y*grayImage.at<uchar>(ij_red.x, ij_red.y);
			fenmu += grayImage.at<uchar>(ij_red.x, ij_red.y);
		}
		u = fenzi1 / fenmu;
		//v = fenzi2 / fenmu;
		v = j;
		//现在uv是指行和列，将其转化为uv坐标
		double temp = v;
		v = u;
		u = temp;
		linePoints.push_back(cv::Point(u, v));

		maxs_red.clear();
		maxs_gray.clear();
		ijs_red.clear();
		ijs_gray.clear();
	}

	//直线方程
	// pt2((rho - image.rows * sin(theta))/cos(theta), image.rows)
	// pt2(image.cols, (rho - image.cols * cos(theta))/sin(theta))
	Vec4f line;
	fitLine(linePoints, line, DIST_HUBER, 0, 0.01, 0.01);
	double cos_theta = line[0];
	double sin_theta = line[1];
	double u0 = line[2], v0 = line[3];
	double phi = atan2(sin_theta, cos_theta) + CV_PI / 2.0;
	double rho = v0 * cos_theta - u0 * sin_theta;

	//(rho - image.rows * sin(theta)) / cos(theta)
	for (int i = 0; i < linePoints.size(); ++i)
	{
		linePoints[i].y = (rho - linePoints[i].x * cos(phi)) / sin(phi);
	}



	rayLineImage = srcImage.clone();
	for (int i = 0; i < linePoints.size(); ++i)
	{
		rayLineImage.at<Vec3b>(linePoints[i])[0] = 0;
		rayLineImage.at<Vec3b>(linePoints[i])[1] = 0;
		rayLineImage.at<Vec3b>(linePoints[i])[2] = 255;
	}
	imwrite(data_folder + string("rayLineImage.jpg"), rayLineImage);
}

void PlaneCali::deleteSmallRegions(Mat & srcImage, Mat & dstImage)
{
	// 提取连通区域，并剔除小面积联通区域
	vector<vector<Point>> contours;           //二值图像轮廓的容器
	vector<Vec4i> hierarchy;                  //4个int向量，分别表示后、前、父、子的索引编号
	findContours(srcImage, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE);             //检测所有轮廓

	vector<vector<Point>>::iterator k;                    //迭代器，访问容器数据

	for (k = contours.begin(); k != contours.end();)      //遍历容器,设置面积因子
	{
		if (contourArea(*k, false) < msize)
		{//删除指定元素，返回指向删除元素下一个元素位置的迭代器
			k = contours.erase(k);
		}
		else
			++k;
	}

	//contours[i]代表第i个轮廓，contours[i].size()代表第i个轮廓上所有的像素点
	for (int i = 0; i < contours.size(); i++)
	{
		for (int j = 0; j < contours[i].size(); j++)
		{
			//获取轮廓上点的坐标
			Point P = Point(contours[i][j].x, contours[i][j].y);
		}
		drawContours(dstImage, contours, i, Scalar(255), -1, 8);
	}
}

void PlaneCali::findSmallRegions(Mat &srcImage, Mat &dstImage)
{
	// 提取连通区域，并剔除小面积联通区域
	vector<vector<Point>> contours;           //二值图像轮廓的容器
	vector<Vec4i> hierarchy;                  //4个int向量，分别表示后、前、父、子的索引编号
	findContours(srcImage, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE);             //检测所有轮廓

	vector<vector<Point>>::iterator k;                    //迭代器，访问容器数据

	for (k = contours.begin(); k != contours.end();)      //遍历容器,设置面积因子
	{
		if (contourArea(*k, false) > msize)
		{//删除指定元素，返回指向删除元素下一个元素位置的迭代器
			k = contours.erase(k);
		}
		else
			++k;
	}

	//contours[i]代表第i个轮廓，contours[i].size()代表第i个轮廓上所有的像素点
	for (int i = 0; i < contours.size(); i++)
	{
		for (int j = 0; j < contours[i].size(); j++)
		{
			//获取轮廓上点的坐标
			Point P = Point(contours[i][j].x, contours[i][j].y);
		}
		drawContours(dstImage, contours, i, Scalar(255), -1, 8);
	}
}

/*
利用光栅尺标定激光平面
求激光平面特征点的世界坐标
参数：
uv		图像坐标
zw		光栅尺读数
xwyw	输出世界坐标
M		相机投影矩阵
*/
void PlaneCali::uvzw2xwyw(Point &uv, float zw, Point2f &xwyw, Mat &M)
{
	//    [u]      [xw]
	// s *[v] = M *[yw]
	//    [1]      [zw]
	//             [ 1]
	//
	// 消去s得到方程组
	// [um31-m11 um32-m22] [xw] = [m14-um34-(um33-m13)zw]
	// [vm31-m11 vm32-m22] [yw]   [m24-vm34-(vm33-m23)zw]
	//
	// A * xwyw = B
	// xwyw = A_ * B

	float u = uv.x;
	float v = uv.y;
	float m11 = M.at<double>(0, 0);
	float m12 = M.at<double>(0, 1);
	float m13 = M.at<double>(0, 2);
	float m14 = M.at<double>(0, 3);
	float m21 = M.at<double>(1, 0);
	float m22 = M.at<double>(1, 1);
	float m23 = M.at<double>(1, 2);
	float m24 = M.at<double>(1, 3);
	float m31 = M.at<double>(2, 0);
	float m32 = M.at<double>(2, 1);
	float m33 = M.at<double>(2, 2);
	float m34 = M.at<double>(2, 3);

	Mat A = Mat(2, 2, CV_32FC1);
	A.at<float>(0, 0) = u*m31 - m11;
	A.at<float>(0, 1) = u*m32 - m12;
	A.at<float>(1, 0) = v*m31 - m21;
	A.at<float>(1, 1) = v*m32 - m22;
	Mat A_ = Mat(2, 2, CV_32FC1);
	A_ = A.inv();
	Mat B = Mat(2, 1, CV_32FC1);
	B.at<float>(0, 0) = m14 - u*m34 - (u*m33 - m13)*zw;
	B.at<float>(1, 0) = m24 - v*m34 - (v*m33 - m23)*zw;

	Mat world = Mat(2, 1, CV_32FC1);
	world = A_*B;

	xwyw.x = world.at<float>(0, 0);
	xwyw.y = world.at<float>(1, 0);
}

Point2f PlaneCali::uvzw2xwyw(Point uv, float zw, Mat &M)
{
	//    [u]      [xw]
	// s *[v] = M *[yw]
	//    [1]      [zw]
	//             [ 1]
	//
	// 消去s得到方程组
	// [um31-m11 um32-m22] [xw] = [m14-um34-(um33-m13)zw]
	// [vm31-m11 vm32-m22] [yw]   [m24-vm34-(vm33-m23)zw]
	//
	// A * xwyw = B
	// xwyw = A_ * B


	float u = uv.x;
	float v = uv.y;
	float m11 = M.at<double>(0, 0);
	float m12 = M.at<double>(0, 1);
	float m13 = M.at<double>(0, 2);
	float m14 = M.at<double>(0, 3);
	float m21 = M.at<double>(1, 0);
	float m22 = M.at<double>(1, 1);
	float m23 = M.at<double>(1, 2);
	float m24 = M.at<double>(1, 3);
	float m31 = M.at<double>(2, 0);
	float m32 = M.at<double>(2, 1);
	float m33 = M.at<double>(2, 2);
	float m34 = M.at<double>(2, 3);
	//float u = 562;
	//float v = 192;
	//zw = 0;
	//float m11 = -2066.92975377865;
	//float m12 = -7.05114282200192;
	//float m13 = 1223.09869029927;
	//float m14 = 505787.625081854;
	//float m21 = 125.089188682421;
	//float m22 = -2324.59339213365;
	//float m23 = 455.412657983516;
	//float m24 = 486641.305942175;
	//float m31 = 0.266947611747578;
	//float m32 = -0.0128936038434843;
	//float m33 = 0.963624785672406;
	//float m34 = 560.583950766622;



	Mat A = Mat(2, 2, CV_32FC1);
	A.at<float>(0, 0) = u*m31 - m11;
	A.at<float>(0, 1) = u*m32 - m12;
	A.at<float>(1, 0) = v*m31 - m21;
	A.at<float>(1, 1) = v*m32 - m22;
	Mat A_ = Mat(2, 2, CV_32FC1);
	A_ = A.inv();
	Mat B = Mat(2, 1, CV_32FC1);
	B.at<float>(0, 0) = m14 - u*m34 - (u*m33 - m13)*zw;
	B.at<float>(1, 0) = m24 - v*m34 - (v*m33 - m23)*zw;

	Mat world = Mat(2, 1, CV_32FC1);
	world = A_*B;

	Point2f xwyw;
	xwyw.x = world.at<float>(0, 0);
	xwyw.y = world.at<float>(1, 0);
	return xwyw;
}

/*
激光三角法计算世界坐标
世界坐标系由标定的第一张图片确定
参数：
uv		图像坐标
xwywzw	输出世界坐标
M		相机投影矩阵
*/
Point3f PlaneCali::uv2xwywzw(Point uv, Mat& M)
{
	//     [u]      [xw]
	// zw *[v] = M *[yw]
	//     [1]      [zw]
	//              [ 1]
	// a*xw + b*yw + c*zw*d = 0
	//
	// [um31-m11 um32-m12 um33-m13] [xw]   [m14-um34]
	// [vm31-m21 vm32-m22 vm33-m23] [yw] = [m24-vm34]
	// [    a        b        c   ] [zw]   [   -d   ]




	float u = uv.x;
	float v = uv.y;
	float a = laserPlaneParams_R.at<float>(0, 0);
	float b = laserPlaneParams_R.at<float>(1, 0);
	float c = laserPlaneParams_R.at<float>(2, 0);
	float d = -1;
	float m11 = M.at<double>(0, 0);
	float m12 = M.at<double>(0, 1);
	float m13 = M.at<double>(0, 2);
	float m14 = M.at<double>(0, 3);
	float m21 = M.at<double>(1, 0);
	float m22 = M.at<double>(1, 1);
	float m23 = M.at<double>(1, 2);
	float m24 = M.at<double>(1, 3);
	float m31 = M.at<double>(2, 0);
	float m32 = M.at<double>(2, 1);
	float m33 = M.at<double>(2, 2);
	float m34 = M.at<double>(2, 3);


	// [um31-m11 um32-m12 um33-m13] [xw]   [m14-um34]
	// [vm31-m21 vm32-m22 vm33-m23] [yw] = [m24-vm34]
	// [    a        b        c   ] [zw]   [   -d   ]
	// xyzw = A_ * B
	Mat A = Mat_<float>(3, 3);
	A.at<float>(0, 0) = u*m31 - m11;
	A.at<float>(0, 1) = u*m32 - m12;
	A.at<float>(0, 2) = u*m33 - m13;
	A.at<float>(1, 0) = v*m31 - m21;
	A.at<float>(1, 1) = v*m32 - m22;
	A.at<float>(1, 2) = v*m33 - m23;
	A.at<float>(2, 0) = a;
	A.at<float>(2, 1) = b;
	A.at<float>(2, 2) = c;
	Mat A_ = Mat_<float>(3, 3);
	A_ = A.inv();

	Mat B = Mat(3, 1, CV_32FC1);
	B.at<float>(0, 0) = m14 - u*m34;
	B.at<float>(1, 0) = m24 - v*m34;
	B.at<float>(2, 0) = -d;

	Mat world = Mat(3, 1, CV_32FC1);
	world = A_*B;

	Point3f xwywzw;
	xwywzw.x = world.at<float>(0, 0);
	xwywzw.y = world.at<float>(1, 0);
	xwywzw.z = world.at<float>(2, 0);

	return xwywzw;
}

void PlaneCali::uv2xwywzw(Point &uv, Point3f &xwywzw, Mat& M)
{
	//     [u]      [xw]
	// zw *[v] = M *[yw]
	//     [1]      [zw]
	//              [ 1]
	// a*xw + b*yw + c*zw*d = 0
	//
	// [um31-m11 um32-m12 um33-m13] [xw]   [m14-um34]
	// [vm31-m21 vm32-m22 vm33-m23] [yw] = [m24-vm34]
	// [    a        b        c   ] [zw]   [   -d   ]




	float u = uv.x;
	float v = uv.y;
	float a = laserPlaneParams_L.at<float>(0, 0);
	float b = laserPlaneParams_L.at<float>(1, 0);
	float c = laserPlaneParams_L.at<float>(2, 0);
	float d = -1;
	float m11 = M.at<double>(0, 0);
	float m12 = M.at<double>(0, 1);
	float m13 = M.at<double>(0, 2);
	float m14 = M.at<double>(0, 3);
	float m21 = M.at<double>(1, 0);
	float m22 = M.at<double>(1, 1);
	float m23 = M.at<double>(1, 2);
	float m24 = M.at<double>(1, 3);
	float m31 = M.at<double>(2, 0);
	float m32 = M.at<double>(2, 1);
	float m33 = M.at<double>(2, 2);
	float m34 = M.at<double>(2, 3);


	// [um31-m11 um32-m12 um33-m13] [xw]   [m14-um34]
	// [vm31-m21 vm32-m22 vm33-m23] [yw] = [m24-vm34]
	// [    a        b        c   ] [zw]   [   -d   ]
	// xyzw = A_ * B
	Mat A = Mat_<float>(3, 3);
	A.at<float>(0, 0) = u*m31 - m11;
	A.at<float>(0, 1) = u*m32 - m12;
	A.at<float>(0, 2) = u*m33 - m13;
	A.at<float>(1, 0) = v*m31 - m21;
	A.at<float>(1, 1) = v*m32 - m22;
	A.at<float>(1, 2) = v*m33 - m23;
	A.at<float>(2, 0) = a;
	A.at<float>(2, 1) = b;
	A.at<float>(2, 2) = c;
	Mat A_ = Mat_<float>(3, 3);
	A_ = A.inv();

	Mat B = Mat(3, 1, CV_32FC1);
	B.at<float>(0, 0) = m14 - u*m34;
	B.at<float>(1, 0) = m24 - v*m34;
	B.at<float>(2, 0) = -d;

	Mat world = Mat(3, 1, CV_32FC1);
	world = A_*B;

	xwywzw.x = world.at<float>(0, 0);
	xwywzw.y = world.at<float>(1, 0);
	xwywzw.z = world.at<float>(2, 0);
}





//传统光栅尺方法标定
void PlaneCali::leftCali()
{
	rayPlaneCalibrate(left_folder, laserCaliimageList_L, uvPoints_L, realPoints_L, caliPoints_L, caliErr_L, laserPlaneCali_result_L,
		laserPlaneParams_L, projectionMatrix_L, cameraMatrix_L, distCoeffs_L);
}

void PlaneCali::rightCali()
{
	rayPlaneCalibrate(right_folder, laserCaliimageList_R, uvPoints_R, realPoints_R, caliPoints_R, caliErr_R, laserPlaneCali_result_R,
		laserPlaneParams_R, projectionMatrix_R, cameraMatrix_R, distCoeffs_R);
}


//利用PnP任意姿态下的标定
void PlaneCali::leftCali1()
{

}

void PlaneCali::rightCali1()
{
	//const char* folder = (string(data_folder) + "laserCali\\").c_str();
	//const char* folder = "C:\\Users\\jzpwh\\Documents\\Visual Studio 2015\\Projects\\PlumbLineInstrument\\PlumbLineInstrument\\";
	string folder = data_folder + string("laserCali\\right\\");
	laserPlaneCali(folder.c_str(), laserCaliimageList_R, laserImageLists_R, uvPoints_R, realPoints_R, caliPoints_R, caliErr_R, laserPlaneCali_result_R,
		laserPlaneParams_R, projectionMatrix_R, cameraMatrix_R, distCoeffs_R);
}