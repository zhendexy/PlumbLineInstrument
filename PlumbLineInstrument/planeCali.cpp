#include "planeCali.h"


/*
����ƽ��궨
������
imageList		��ű궨ͼƬ���Ƶ�txt
rayPlaneCalibrateResult		��ű궨�����txt
rayPlaneParams	����ƽ��궨���
projectionMatrix�����ͶӰ����
cameraMatrix	������ڲ�������
distCoeffs		����Ļ���ϵ��
*/
bool PlaneCali::rayPlaneCalibrate(const char* imageFolder, const char* imageList, const char* uvPoints, const char* realPoints, const char* caliPoints,
	const char* caliErr, const char*rayPlaneCali_result, Mat& rayPlaneParams, Mat& projectionMatrix, Mat& cameraMatrix, Mat& distCoeffs)
{
	ifstream imageStore(data_folder + string(imageList)); // �򿪴�ű궨ͼƬ���Ƶ�txt
	fstream fs_uvPoints(data_folder + string(uvPoints), ios::out);
	fstream fs_realPoints(data_folder + string(realPoints), ios::out);
	fstream fs_caliPoints(data_folder + string(caliPoints), ios::out);
	fstream fs_caliErr(data_folder + string(caliErr), ios::out);
	FileStorage rayLineStore(data_folder + string(rayPlaneCali_result), FileStorage::WRITE);

	vector<Point> rayLinePoints;
	vector<Point> tempPoints;
	vector<int> zws;
	string imageName; // ��ȡ�ı궨ͼƬ������
	while (getline(imageStore, imageName)) // ��ȡtxt��ÿһ�У�ÿһ�д����һ�ű궨ͼƬ�����ƣ�
	{
		srcImage = imread(imageFolder + imageName);
		Mat srcImage1;
		undistort(srcImage, srcImage1, cameraMatrix, distCoeffs); // ��������
																  //remap(srcImage, srcImage1, map1, map2, INTER_LINEAR);
		srcImage = srcImage1;
		imwrite(data_folder + string("rectifyImage.jpg"), srcImage);

		string zw_str = "";	// ͨ��ͼƬ�ļ�������ȡzw����		left01_10.jpg	right01_10.jpg
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

		getRayLinePoints(srcImage, tempPoints); // ��ȡ�������������uv����
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

	// ����ÿ��������Ŀռ�����
	// ������� ���ݲ����������ֵ-ʵ��ֵ
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
	CaliCalculate cal(cameraMatrix, distCoeffs, projectionMatrix);//���������
	Mat map1, map2;
	initUndistortRectifyMap(cameraMatrix, distCoeffs, Mat(), Mat(), imageSize, CV_32FC1, map1, map2);//�����޻��������ת��ӳ��

	ifstream imageStore(imageFolder + string(imageList)); // �򿪴�ű궨ͼƬ���Ƶ�txt
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



	//��������������ϵ�����������ϵ��RT����
	ImgPro imgpro(imageFolder, image_width, image_height);//ͼ���������

	vector<Point3f> laserPt_C;
	string PCfile = "./PtCloud\\Pt_XcYcZc.txt";
	string Ptuvfile = "./PtCloud\\Pt_uv.txt";//�����������
	ofstream Ptuvout(Ptuvfile);//���ļ�
	ofstream PCout(PCfile);//���ļ�
	Mat ExtriM = Mat::zeros(3, 4, CV_64FC1);//�ϲ�RT�õ������
	for (int i = 0; i <ImgNum; i = i + 1)//����
	{
		Mat img = images[i].clone();
		Mat laser = lasers[i].clone();
		vector<crossdot> cdt;
		int Getcdtok = imgpro.findcrossdot2(img, laser, cdt);//��ȡ�����߽���
		if (Getcdtok == 0)
		{
			cout << "******��" << i << "֡�����߽�����ȡʧ�ܣ�******" << endl;
			continue;
		}//���t=0����ȡ�����߽���ʧ�ܣ�������һ��ѭ��
		else
			cout << "******��" << i << "֡�����߽�����ȡ�ɹ���******" << endl;
		imgpro.drawIndex(img, cdt);//���������߽��㼰����
								   //imshow("show", img);

		bool GetRTok = cal.getR_T(cdt);//�õ���������ϵ�����������ϵ��RT����
		if (!GetRTok)continue;
		cout << "��ת����: " << cal.RotationMat << endl;
		cout << "ƽ��������" << cal.TransVec << endl;
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
		vector<Point2d> pt_XwYw;//Zw=0,���̸�����ϵ
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
		imgpro.drawLaserInRect(img, pt_uv);//������ȡЧ��
		cal.mergeRT(ExtriM, cal.RotationMat, cal.TransVec);
		cal.CalWorldPoint(pt_uv, cal.cameraMatrix, ExtriM, pt_XwYw);
		cal.test_accur(cdt, ExtriM);//����������ؽ������������ȡ����
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
	cout << "���������!";
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
��ȡ����������ĵ�����
������
srcImage	����ͼ����ǰ��������
T_rayLine	��ֵ����ֵ
redtolerant
graytolerant
linePoints	��������ĵ�����
*/
void PlaneCali::getRayLinePoints(Mat& srcImage, vector<Point>& linePoints)
{
	cvtColor(srcImage, grayImage, COLOR_BGR2GRAY);

	//midImage = grayImage.clone();
	//threshold(midImage, midImage, T_rayLine, 255, THRESH_BINARY);
	//maskImage = Mat::ones(midImage.size(), CV_8UC1); // ȥ��С��ͨ�����ͼ��mask
	//findSmallRegions(midImage, maskImage);
	//imwrite(DATA_FOLDER + string("maskImage_gray.jpg"), maskImage);
	//bitwise_not(maskImage, maskImage);
	//bitwise_and(grayImage, maskImage, grayImage);
	imwrite(data_folder + string("grayImage.jpg"), grayImage);


	vector<Mat> BGR;
	split(srcImage, BGR); // opencv��ȡͼ����BGR��
	redImage = BGR.at(2).clone(); // ȡ��BGR�е�R����

	midImage = redImage.clone();
	threshold(midImage, midImage, T, 255, THRESH_BINARY);
	maskImage = Mat::zeros(midImage.size(), CV_8UC1); // ȥ��С��ͨ�����ͼ��mask
	deleteSmallRegions(midImage, maskImage);
	imwrite(data_folder + string("maskImage_red.jpg"), maskImage);
	for (int i = 0; i < redImage.rows; ++i)
		for (int j = 0; j < redImage.cols; ++j)
		{
			int v = redImage.at<uchar>(i, j);
			redImage.at<uchar>(i, j) = v > T ? v : 0; // redImage��ֵ��
		}
	bitwise_and(redImage, maskImage, redImage);
	imwrite(data_folder + string("redImage.jpg"), redImage);

	//�������Ǻ��ŵģ�����ɨ����
	int num = 9;
	int max_red, max_gray;
	cv::Point ij_red, ij_gray;
	vector<int> maxs_red, maxs_gray;
	vector<cv::Point> ijs_red, ijs_gray;
	for (int j = 0; j < redImage.cols; ++j)
	{
		//�ҵ�redImage��һ���лҶ����ĵ�
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
		//�ҵ�redImage��һ�еĺ�ѡ��
		for (int i = 0; i < redImage.rows; ++i)
		{
			if (redImage.at<uchar>(i, j) > max_red - redtolerant)
			{
				maxs_red.push_back(redImage.at<uchar>(i, j));
				ijs_red.push_back(cv::Point(i, j));
			}
		}


		//��redImage�к�ѡ���Ӧ��grayImage���ҵ���ѡ���лҶ�ֵ���ĵ�
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

		//�ҵ�grayImage��һ�еĺ�ѡ��
		//�����ƺ�ѡ�㲻����9��ò��Ч������
		for (int i = 0; i < grayImage.rows; ++i)
		{
			if (grayImage.at<uchar>(i, j) > max_gray - graytolerant)
			{
				maxs_gray.push_back(grayImage.at<uchar>(i, j));
				ijs_gray.push_back(cv::Point(i, j));
			}
		}

		////�ҵ�grayImage��һ�еĺ�ѡ��
		////��max_gray��λ�ÿ�ʼ�Һ�ѡ��
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

		//	//���ƺ�ѡ�����������9��
		//	if (maxs_gray.size() > num)
		//		break;
		//}



		//���ѡ��ĻҶ����ģ���Ϊ�������ĵ������
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
		//����uv��ָ�к��У�����ת��Ϊuv����
		double temp = v;
		v = u;
		u = temp;
		linePoints.push_back(cv::Point(u, v));

		maxs_red.clear();
		maxs_gray.clear();
		ijs_red.clear();
		ijs_gray.clear();
	}

	//ֱ�߷���
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
	// ��ȡ��ͨ���򣬲��޳�С�����ͨ����
	vector<vector<Point>> contours;           //��ֵͼ������������
	vector<Vec4i> hierarchy;                  //4��int�������ֱ��ʾ��ǰ�������ӵ��������
	findContours(srcImage, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE);             //�����������

	vector<vector<Point>>::iterator k;                    //��������������������

	for (k = contours.begin(); k != contours.end();)      //��������,�����������
	{
		if (contourArea(*k, false) < msize)
		{//ɾ��ָ��Ԫ�أ�����ָ��ɾ��Ԫ����һ��Ԫ��λ�õĵ�����
			k = contours.erase(k);
		}
		else
			++k;
	}

	//contours[i]�����i��������contours[i].size()�����i�����������е����ص�
	for (int i = 0; i < contours.size(); i++)
	{
		for (int j = 0; j < contours[i].size(); j++)
		{
			//��ȡ�����ϵ������
			Point P = Point(contours[i][j].x, contours[i][j].y);
		}
		drawContours(dstImage, contours, i, Scalar(255), -1, 8);
	}
}

void PlaneCali::findSmallRegions(Mat &srcImage, Mat &dstImage)
{
	// ��ȡ��ͨ���򣬲��޳�С�����ͨ����
	vector<vector<Point>> contours;           //��ֵͼ������������
	vector<Vec4i> hierarchy;                  //4��int�������ֱ��ʾ��ǰ�������ӵ��������
	findContours(srcImage, contours, hierarchy, RETR_LIST, CHAIN_APPROX_NONE);             //�����������

	vector<vector<Point>>::iterator k;                    //��������������������

	for (k = contours.begin(); k != contours.end();)      //��������,�����������
	{
		if (contourArea(*k, false) > msize)
		{//ɾ��ָ��Ԫ�أ�����ָ��ɾ��Ԫ����һ��Ԫ��λ�õĵ�����
			k = contours.erase(k);
		}
		else
			++k;
	}

	//contours[i]�����i��������contours[i].size()�����i�����������е����ص�
	for (int i = 0; i < contours.size(); i++)
	{
		for (int j = 0; j < contours[i].size(); j++)
		{
			//��ȡ�����ϵ������
			Point P = Point(contours[i][j].x, contours[i][j].y);
		}
		drawContours(dstImage, contours, i, Scalar(255), -1, 8);
	}
}

/*
���ù�դ�߱궨����ƽ��
�󼤹�ƽ�����������������
������
uv		ͼ������
zw		��դ�߶���
xwyw	�����������
M		���ͶӰ����
*/
void PlaneCali::uvzw2xwyw(Point &uv, float zw, Point2f &xwyw, Mat &M)
{
	//    [u]      [xw]
	// s *[v] = M *[yw]
	//    [1]      [zw]
	//             [ 1]
	//
	// ��ȥs�õ�������
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
	// ��ȥs�õ�������
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
�������Ƿ�������������
��������ϵ�ɱ궨�ĵ�һ��ͼƬȷ��
������
uv		ͼ������
xwywzw	�����������
M		���ͶӰ����
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





//��ͳ��դ�߷����궨
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


//����PnP������̬�µı궨
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