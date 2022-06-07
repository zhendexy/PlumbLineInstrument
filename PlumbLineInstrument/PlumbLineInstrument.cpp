#include "PlumbLineInstrument.h"

PV_INIT_SIGNAL_HANDLER();//此行有重定义的错误 LNK2005 放在这里不会出错 不能放回PlumbLineInstrument.h中

PlumbLineInstrument::PlumbLineInstrument(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);


	ui.labelLeft->setAlignment(Qt::AlignCenter);
	ui.labelRight->setAlignment(Qt::AlignCenter);

	timer = new QTimer(this);
	timer->setInterval(1000 / rate);//set timer match with FPS
	timer->start();

	//信号与槽函数
	connect(ui.exitBtn, &QPushButton::clicked, this, &PlumbLineInstrument::close);
	connect(timer, SIGNAL(timeout()), this, SLOT(readFrame()));
}



QImage PlumbLineInstrument::CvMat2QImage(const Mat &mat)
{
	// 图像的通道
	int channel = mat.channels();

	// 设立一个表 直接查询 其中 0 2 是无效值 1 3 4 对应的转换值
	const std::map<int, QImage::Format> img_cvt_map{
		{ 1, QImage::Format_Grayscale8 },
		{ 3, QImage::Format_RGB888 },
		{ 4, QImage::Format_ARGB32 }
	};

	QImage image(mat.data, mat.cols, mat.rows,
		static_cast<int>(mat.step),
		img_cvt_map.at(channel));

	// 三通道图像 值做 通道转换
	return channel == 3 ? image.rgbSwapped() : image;
}

Mat PlumbLineInstrument::QImage2CvMat(const QImage &image)
{
	cv::Mat mat;
	const std::map<QImage::Format, int> img_cvt_map{
		{ QImage::Format_Grayscale8, 1 },
		{ QImage::Format_RGB888, 3 },
		{ QImage::Format_ARGB32, 4 }
	};

	auto aa = image.format();
	return cv::Mat(image.height(), image.width(), img_cvt_map.at(image.format()));
}



void PlumbLineInstrument::on_startCameraBtn_clicked()
{
	flagStartCamera = true;
	camera.startCamera();
}

void PlumbLineInstrument::on_stopCameraBtn_clicked()
{
	flagStartCamera = false;
	camera.stopCamera();
}

void PlumbLineInstrument::on_saveFrameBtn_clicked()
{
	PvImage *alInputImage = camera.alBuffer->GetImage();
	alInputImage->Alloc(IMAGE_WIDTH, IMAGE_HEIGHT, PvPixelMono8);
	Mat lMat(imageSize, CV_8UC1, alInputImage->GetDataPointer());
	frameLeft = lMat.clone();
	cv::rotate(frameLeft, frameLeft1, ROTATE_90_CLOCKWISE);

	PvImage *blInputImage = camera.blBuffer->GetImage();
	blInputImage->Alloc(IMAGE_WIDTH, IMAGE_HEIGHT, PvPixelMono8);
	Mat rMat(imageSize, CV_8UC1, blInputImage->GetDataPointer());
	frameRight = rMat.clone();
	cv::rotate(frameRight, frameRight1, ROTATE_90_CLOCKWISE);


	string nameLeft, nameRight;
	if (num < 10)
	{
		nameLeft = "left0" + to_string(num) + ".jpg";
		nameRight = "right0" + to_string(num) + ".jpg";
	}
	else
	{
		nameLeft = "left" + to_string(num) + ".jpg";
		nameRight = "right" + to_string(num) + ".jpg";
	}
	imwrite(nameLeft, frameLeft1);
	imwrite(nameRight, frameRight1);


	++num;

	ui.textBrowser->append(QString::fromLocal8Bit("已保存当前帧画面!"));
	QCoreApplication::processEvents();
}

void PlumbLineInstrument::readFrame()
{
	if (!flagStartCamera)
		return;

	PvResult alOperationResult;
	PvResult blOperationResult;

	// Retrieve next buffer
	PvResult alResult = camera.lPipelineL->RetrieveNextBuffer(&camera.alBuffer, 1000, &alOperationResult);
	PvResult blResult = camera.lPipelineR->RetrieveNextBuffer(&camera.blBuffer, 1000, &blOperationResult);
	if (alResult.IsOK() && blResult.IsOK())
	{
		if (alOperationResult.IsOK() && blOperationResult.IsOK())
		{
			//
			// We now have a valid buffer. This is where you would typically process the buffer.
			// -----------------------------------------------------------------------------------------
			// ...

			if (!flagStartMeasure)
			{
				QImage lQimage(camera.alBuffer->GetDataPointer(), IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_Grayscale8);
				QImage rQimage(camera.blBuffer->GetDataPointer(), IMAGE_WIDTH, IMAGE_HEIGHT, QImage::Format_Grayscale8);

				QMatrix matrix;
				matrix.rotate(90.0);
				lQimage = lQimage.transformed(matrix, Qt::FastTransformation);
				rQimage = rQimage.transformed(matrix, Qt::FastTransformation);

				QPixmap lQPixmap = QPixmap::fromImage(lQimage);
				QPixmap rQPixmap = QPixmap::fromImage(rQimage);
				lQPixmap = lQPixmap.scaled(LABEL_WIDTH, LABEL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				rQPixmap = rQPixmap.scaled(LABEL_WIDTH, LABEL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);

				ui.labelLeft->setPixmap(lQPixmap);
				ui.labelRight->setPixmap(rQPixmap);
			}
			else
			{
				PvImage *alInputImage = camera.alBuffer->GetImage();
				alInputImage->Alloc(IMAGE_WIDTH, IMAGE_HEIGHT, PvPixelMono8);
				Mat lMat(imageSize, CV_8UC1, alInputImage->GetDataPointer());
				cvtColor(lMat, frameLeft, COLOR_RGB2BGR);
				cv::rotate(frameLeft, frameLeft1, ROTATE_90_CLOCKWISE);
				//imwrite("frameLeft1.jpg", frameLeft1);

				PvImage *blInputImage = camera.blBuffer->GetImage();
				blInputImage->Alloc(IMAGE_WIDTH, IMAGE_HEIGHT, PvPixelMono8);
				Mat rMat(imageSize, CV_8UC1, blInputImage->GetDataPointer());
				cvtColor(rMat, frameRight, COLOR_RGB2BGR);
				cv::rotate(frameRight, frameRight1, ROTATE_90_CLOCKWISE);
				//imwrite("frameRight1.jpg", frameRight1);

				//string nameLeft, nameRight;
				//if (num < 10)
				//{
				//	nameLeft = "left0" + to_string(num) + ".jpg";
				//	nameRight = "right0" + to_string(num) + ".jpg";
				//}
				//else
				//{
				//	nameLeft = "left" + to_string(num) + ".jpg";
				//	nameRight = "right" + to_string(num) + ".jpg";
				//}
				//imwrite(nameLeft, lMat);
				//imwrite(nameRight, rMat);

				++num;



				Mat resultLeft = frameLeft1.clone();
				undistort(resultLeft, frameLeft1, cameraMatrix_L, distCoeffs_L); // 单目图像畸变矫正
				resultLeft = frameLeft1.clone();

				//Point3f P;
				//measureOneBall(frameLeft1, resultLeft, P);
				Point3f P1, P2;
				float d;
				measureTwoBalls(frameLeft1, resultLeft, P1, P2, d);




				//cv::Point intersectionLeft, intersectionRight;
				//cv::Point ballCenterLeft, ballCenterRight;
				//Point3f Pc_laser;
				//Point3f Pc_ball;

				////找与垂线的交点
				//Mat templ = frameLeft1.clone();
				//undistort(templ, frameLeft1, cameraMatrix_L, distCoeffs_L); // 单目图像畸变矫正
				//templ = frameLeft1.clone();
				//if (findIntersection(frameLeft1, intersectionLeft))
				//{
				//	circle(templ, intersectionLeft, 3, Scalar(0, 0, 255), 3);
				//	QImage lQimage = CvMat2QImage(templ);
				//	QPixmap lQPixmap = QPixmap::fromImage(lQimage);
				//	lQPixmap = lQPixmap.scaled(LABEL_WIDTH, LABEL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				//	ui.labelLeft->setPixmap(lQPixmap);

				//	Pc_laser = uv2xcyczc(intersectionLeft, cameraMatrix_L);
				//	ui.label_u->setNum(intersectionLeft.x);
				//	ui.label_v->setNum(intersectionLeft.y);
				//	ui.label_xc->setNum(Pc_laser.x);
				//	ui.label_yc->setNum(Pc_laser.y);
				//	ui.label_zc->setNum(Pc_laser.z);

				//	//Point3f Pw_laser = uv2xwywzw(intersectionRight, projectionMatrix_R);
				//	//ui.label_xw->setNum(Pw_laser.x);
				//	//ui.label_yw->setNum(Pw_laser.y);
				//	//ui.label_zw->setNum(Pw_laser.z);

				//	//Mat Pc(3, 1, CV_32FC1);
				//	//Pc.at<float>(0) = Pc_laser.x;
				//	//Pc.at<float>(1) = Pc_laser.y;
				//	//Pc.at<float>(2) = Pc_laser.z;
				//	//Mat R1_(3, 3, CV_32FC1);
				//	//R1_ = rotationMatrix_R.inv();
				//	//Mat Pw = R1_*(Pc - translationVector_R);
				//	//ui.label_xw->setNum(Pw.at<float>(0));
				//	//ui.label_yw->setNum(Pw.at<float>(1));
				//	//ui.label_zw->setNum(Pw.at<float>(2));
				//}

				//if (findBallCenter(frameLeft1, ballCenterLeft))
				//{
				//	circle(templ, ballCenterLeft, 3, Scalar(0, 255, 0), 3);
				//	QImage lQimage = CvMat2QImage(templ);
				//	QPixmap lQPixmap = QPixmap::fromImage(lQimage);
				//	lQPixmap = lQPixmap.scaled(LABEL_WIDTH, LABEL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				//	ui.labelLeft->setPixmap(lQPixmap);

				//	float u = ballCenterLeft.x, v = ballCenterLeft.y;
				//	float a = levelPlaneParams.at<float>(0, 0), b = levelPlaneParams.at<float>(1, 0), c = levelPlaneParams.at<float>(2, 0);
				//	float x1 = Pc_laser.x, y1 = Pc_laser.y, z1 = Pc_laser.z;
				//	float fx = cameraMatrix_L.at<double>(0, 0), fy = cameraMatrix_L.at<double>(1, 1);
				//	float u0 = cameraMatrix_L.at<double>(0, 2), v0 = cameraMatrix_L.at<double>(1, 2);

				//	Pc_ball.z = (c*fx*x1 - a*fx*z1) / (c*(u - u0) - a*fx);
				//	Pc_ball.x = a*(Pc_ball.z - z1) / c + x1;
				//	Pc_ball.y = b*(Pc_ball.z - z1) / c + y1;

				//	ui.label_xw->setNum(Pc_ball.x);
				//	ui.label_yw->setNum(Pc_ball.y);
				//	ui.label_zw->setNum(Pc_ball.z);
				//}

				////找小球球心
				//remap(frameLeft1, frameLeft1, mapl1, mapl2, INTER_LINEAR); // 双目图像立体校正
				//remap(frameRight1, frameRight1, mapr1, mapr2, INTER_LINEAR); // 双目图像立体校正
				//bool flag_ballCenterLeft;
				//if (flag_ballCenterLeft = findBallCenter(frameLeft1, ballCenterLeft))
				//{
				//	circle(frameLeft1, ballCenterLeft, 2, Scalar(0, 255, 0), 2, 8, 0);
				//	QImage lQimage = CvMat2QImage(frameLeft1);
				//	QPixmap lQPixmap = QPixmap::fromImage(lQimage);
				//	lQPixmap = lQPixmap.scaled(LABEL_WIDTH, LABEL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				//	ui.labelLeft->setPixmap(lQPixmap);

				//}
				//bool flag_ballCenterRight;
				//if (flag_ballCenterRight = findBallCenter(frameRight1, ballCenterRight))
				//{
				//	circle(frameRight1, ballCenterRight, 2, Scalar(0, 255, 0), 2, 8, 0);
				//	QImage rQimage = CvMat2QImage(frameRight1);
				//	QPixmap rQPixmap = QPixmap::fromImage(rQimage);
				//	rQPixmap = rQPixmap.scaled(LABEL_WIDTH, LABEL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				//	ui.labelRight->setPixmap(rQPixmap);

				//}

				////双目视觉求小球中心坐标
				//if (flag_ballCenterLeft&&flag_ballCenterRight)
				//{
				//	Point3f Pw_ball = uv2xwywzw(ballCenterLeft, ballCenterRight);
				//	ui.label_ul->setNum(ballCenterLeft.x);
				//	ui.label_vl->setNum(ballCenterLeft.y);
				//	ui.label_ur->setNum(ballCenterRight.x);
				//	ui.label_vr->setNum(ballCenterRight.y);
				//	ui.label_xw1->setNum(Pw_ball.x);
				//	ui.label_yw1->setNum(Pw_ball.y);
				//	ui.label_zw1->setNum(Pw_ball.z);
				//}









				//cv::Point intersectionRight_A, intersectionRight_B;

				//找与圆锥的交线
				//if (findIntersection(frameRight1, intersectionRight_A, intersectionRight_B))
				//{
				//	circle(frameRight1, intersectionRight_A, 3, Scalar(0, 0, 255), 3);
				//	circle(frameRight1, intersectionRight_B, 3, Scalar(0, 0, 255), 3);
				//	QImage rQimage = CvMat2QImage(frameRight1);
				//	QPixmap rQPixmap = QPixmap::fromImage(rQimage);
				//	rQPixmap = rQPixmap.scaled(LABEL_WIDTH, LABEL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
				//	ui.labelRight->setPixmap(rQPixmap);

				//	Point3f Pw_A = uv2xcyczc(intersectionRight_A, cameraMatrix_R);
				//	Point3f Pw_B = uv2xcyczc(intersectionRight_B, cameraMatrix_R);
				//	double L = sqrt((Pw_A.x - Pw_B.x)*(Pw_A.x - Pw_B.x) + (Pw_A.y - Pw_B.y)*(Pw_A.y - Pw_B.y) + (Pw_A.z - Pw_B.z)*(Pw_A.z - Pw_B.z));
				//	double dy = (Pw_A.z + Pw_B.z) / 2 - y0;
				//	L = L - 2 * dy*tan(theta)*tan(alpha);
				//	

				//	double h = 0.5 / tan(alpha)*L + tan(theta) / 2 * L - L0 / 2 * tan(theta);
				//	double z = h - h0;

				//	ui.label_L->setNum(L);
				//	ui.label_h->setNum(h);
				//	ui.label_yyy->setNum((Pw_A.z + Pw_B.z) / 2);
				//	ui.label_zzz->setNum(z);
				//}
			}

		}
		// Release the buffer back to the pipeline
		camera.lPipelineL->ReleaseBuffer(camera.alBuffer);
		camera.lPipelineR->ReleaseBuffer(camera.blBuffer);
	}
}



void PlumbLineInstrument::on_OpenCVCaliBtn_clicked()
{
	cameraCalibrate.leftCali();
	ui.textBrowser->append(QString::fromLocal8Bit("已完成左相机的标定!"));
	QCoreApplication::processEvents();

	cameraCalibrate.rightCali();
	ui.textBrowser->append(QString::fromLocal8Bit("已完成右相机的标定!"));
	QCoreApplication::processEvents();

	cameraCalibrate.stereoCali();
	ui.textBrowser->append(QString::fromLocal8Bit("相机立体标定完成!"));
	QCoreApplication::processEvents();

	cameraCalibrate.stereoRecti();
	ui.textBrowser->append(QString::fromLocal8Bit("已创建图像重投影映射表!"));
	QCoreApplication::processEvents();
}

void PlumbLineInstrument::on_planeCaliBtn_clicked()
{
	//planeCali.leftCali();
	planeCali.leftCali1();
	ui.textBrowser->append(QString::fromLocal8Bit("已完成左相机激光平面的标定!"));
	QCoreApplication::processEvents();

	//planeCali.rightCali();
	//planeCali.rightCali1();
	//ui.textBrowser->append(QString::fromLocal8Bit("已完成右相机激光平面的标定!"));
	//QCoreApplication::processEvents();
}

void PlumbLineInstrument::on_levelCaliBtn_clicked()
{
	string folder = DATA_FOLDER + string("levelCali\\");
	LevelCali levelCali(DATA_FOLDER, folder.c_str(), cameraMatrix_L, distCoeffs_L);

	levelCali.levelPlaneCali();
	levelPlaneParams = levelCali.getLevelPlaneParams();

	ui.textBrowser->append(QString::fromLocal8Bit("已完成左相机水平面的标定!"));
	QCoreApplication::processEvents();
}


void PlumbLineInstrument::on_OpenCVParamsBtn_clicked()
{
	FileStorage singleLStore(DATA_FOLDER + string(singleCalibrate_result_L), FileStorage::READ);
	singleLStore["cameraMatrix"] >> cameraMatrix_L;
	singleLStore["distCoeffs"] >> distCoeffs_L;
	singleLStore["rotationVector"] >> rotationVector_L;
	singleLStore["rotationMatrix"] >> rotationMatrix_L;
	singleLStore["translationVector"] >> translationVector_L;
	singleLStore["extrinsicMatrix"] >> extrinsicMatrix_L;
	singleLStore["projectionMatrix"] >> projectionMatrix_L;
	singleLStore.release();


	FileStorage singleRStore(DATA_FOLDER + string(singleCalibrate_result_R), FileStorage::READ);
	singleRStore["cameraMatrix"] >> cameraMatrix_R;
	singleRStore["distCoeffs"] >> distCoeffs_R;
	singleRStore["rotationVector"] >> rotationVector_R;
	singleRStore["rotationMatrix"] >> rotationMatrix_R;
	singleRStore["translationVector"] >> translationVector_R;
	singleRStore["extrinsicMatrix"] >> extrinsicMatrix_R;
	singleRStore["projectionMatrix"] >> projectionMatrix_R;
	singleRStore.release();

	FileStorage stereoStore(DATA_FOLDER + string(stereoCalibrate_result_L), FileStorage::READ);
	stereoStore["R"] >> R;
	stereoStore["T"] >> T;
	stereoStore["E"] >> E;
	stereoStore["F"] >> F;
	stereoStore.release();

	FileStorage stereoParamsStore(DATA_FOLDER + string(stereoRectifyParams), FileStorage::READ);
	stereoParamsStore["R1"] >> R1;
	stereoParamsStore["R2"] >> R2;
	stereoParamsStore["P1"] >> P1;
	stereoParamsStore["P2"] >> P2;
	stereoParamsStore["Q"] >> Q;
	//stereoParamsStore["mapl1"] >> mapl1;
	//stereoParamsStore["mapl2"] >> mapl2;
	//stereoParamsStore["mapr1"] >> mapr1;
	//stereoParamsStore["mapr2"] >> mapr2;
	stereoParamsStore.release();


	Rect validRoi[2];
	//validRoi[0], validRoi[1] = stereoRectification(stereoRectifyParams, cameraMatrix_L, distCoeffs_L, cameraMatrix_R, distCoeffs_R,
	//	imageSize, R, T, R1, R2, P1, P2, Q, mapl1, mapl2, mapr1, mapr2);
	cv::stereoRectify(cameraMatrix_L, distCoeffs_L, cameraMatrix_R, distCoeffs_R, imageSize1,
		R, T, R1, R2, P1, P2, Q, 0, -1, imageSize1, &validRoi[0], &validRoi[1]);
	initUndistortRectifyMap(cameraMatrix_L, distCoeffs_L, R1, P1, imageSize1, CV_32FC1, mapl1, mapl2);
	initUndistortRectifyMap(cameraMatrix_R, distCoeffs_R, R2, P2, imageSize1, CV_32FC1, mapr1, mapr2);

	initUndistortRectifyMap(cameraMatrix_L, distCoeffs_L, Mat(), Mat(), imageSize1, CV_32FC1, maplx, maply);
	initUndistortRectifyMap(cameraMatrix_R, distCoeffs_R, Mat(), Mat(), imageSize1, CV_32FC1, maprx, mapry);


	ui.textBrowser->append(QString::fromLocal8Bit("已读取OpenCV相机标定数据!"));
	QCoreApplication::processEvents();
}

void PlumbLineInstrument::on_planeParamsBtn_clicked()
{
	FileStorage laserPlaneLStore(DATA_FOLDER + string(laserPlaneCali_result_L), FileStorage::READ);
	laserPlaneLStore["laserPlaneParams"] >> laserPlaneParams_L;
	laserPlaneLStore.release();

	FileStorage laserPlaneRStore(DATA_FOLDER + string(laserPlaneCali_result_R), FileStorage::READ);
	laserPlaneRStore["laserPlaneParams"] >> laserPlaneParams_R;
	laserPlaneRStore.release();

	ui.textBrowser->append(QString::fromLocal8Bit("已读取激光平面标定数据!"));
	QCoreApplication::processEvents();


	FileStorage levelPlaneStore(DATA_FOLDER + string("levelPlaneParams.yml"), FileStorage::READ);
	levelPlaneStore["levelPlaneParams"] >> levelPlaneParams;
	levelPlaneStore.release();
	ui.textBrowser->append(QString::fromLocal8Bit("已读取水平面标定数据!"));
	QCoreApplication::processEvents();
}


void PlumbLineInstrument::on_startMeasureBtn_clicked()
{
	if (flagStartCamera)
	{
		flagStartMeasure = true;
		start_time = clock();
		ui.textBrowser->append(QString::fromLocal8Bit("开始实时测量垂线坐标！"));
		QCoreApplication::processEvents();
	}
}

void PlumbLineInstrument::on_stopMeasureBtn_clicked()
{
	if (flagStartCamera)
	{
		flagStartMeasure = false;
		stop_time = clock();
		ui.textBrowser->append(QString::fromLocal8Bit("停止实时测量垂线坐标！"));
		ui.textBrowser->append(QString::fromStdString("processed photos:" + to_string(num)));
		ui.textBrowser->append(QString::fromStdString("used time:" + to_string(stop_time - start_time) + "ms"));
		ui.textBrowser->append(QString::fromStdString("FPS:" + to_string(num / (stop_time - start_time) * 1000)));
		QCoreApplication::processEvents();
	}
}

void PlumbLineInstrument::on_testBtn_clicked()
{
	test();
}





/*
双目视觉计算世界坐标
参数：
uvLeft	左相机图像坐标
uvRight	右相机图像坐标
P1		左相机立体校正后的投影矩阵
P2		右相机立体校正后的投影矩阵
输出：
Point3f	世界坐标	标定时第一张图片对应的世界坐标系
*/
Point3f PlumbLineInstrument::uv2xwywzw(Point2f& uvLeft, Point2f& uvRight)
{
	//     [u1]      [xw]                      [u2]      [xw]
	//zc1 *|v1| = P1*[yw]                  zc2*|v2| = P2*[yw]
	//     [ 1]      [zw]                      [ 1]      [zw]
	//               [1 ]                                [1 ]

	Mat mLeftP = P1;
	Mat mRightP = P2;

	// 最小二乘法A矩阵
	Mat A = Mat(4, 3, CV_32F);
	A.at<float>(0, 0) = uvLeft.x * mLeftP.at<double>(2, 0) - mLeftP.at<double>(0, 0);
	A.at<float>(0, 1) = uvLeft.x * mLeftP.at<double>(2, 1) - mLeftP.at<double>(0, 1);
	A.at<float>(0, 2) = uvLeft.x * mLeftP.at<double>(2, 2) - mLeftP.at<double>(0, 2);

	A.at<float>(1, 0) = uvLeft.y * mLeftP.at<double>(2, 0) - mLeftP.at<double>(1, 0);
	A.at<float>(1, 1) = uvLeft.y * mLeftP.at<double>(2, 1) - mLeftP.at<double>(1, 1);
	A.at<float>(1, 2) = uvLeft.y * mLeftP.at<double>(2, 2) - mLeftP.at<double>(1, 2);

	A.at<float>(2, 0) = uvRight.x * mRightP.at<double>(2, 0) - mRightP.at<double>(0, 0);
	A.at<float>(2, 1) = uvRight.x * mRightP.at<double>(2, 1) - mRightP.at<double>(0, 1);
	A.at<float>(2, 2) = uvRight.x * mRightP.at<double>(2, 2) - mRightP.at<double>(0, 2);

	A.at<float>(3, 0) = uvRight.y * mRightP.at<double>(2, 0) - mRightP.at<double>(1, 0);
	A.at<float>(3, 1) = uvRight.y * mRightP.at<double>(2, 1) - mRightP.at<double>(1, 1);
	A.at<float>(3, 2) = uvRight.y * mRightP.at<double>(2, 2) - mRightP.at<double>(1, 2);

	// 最小二乘法B矩阵
	Mat B = Mat(4, 1, CV_32F);
	B.at<float>(0, 0) = mLeftP.at<double>(0, 3) - uvLeft.x * mLeftP.at<double>(2, 3);
	B.at<float>(1, 0) = mLeftP.at<double>(1, 3) - uvLeft.y * mLeftP.at<double>(2, 3);
	B.at<float>(2, 0) = mRightP.at<double>(0, 3) - uvRight.x * mRightP.at<double>(2, 3);
	B.at<float>(3, 0) = mRightP.at<double>(1, 3) - uvRight.y * mRightP.at<double>(2, 3);

	Mat XYZ = Mat(3, 1, CV_32F);
	// 采用SVD最小二乘法求解XYZ
	solve(A, B, XYZ, DECOMP_SVD);

	// 世界坐标系中坐标
	Point3f world;
	world.x = XYZ.at<float>(0, 0);
	world.y = XYZ.at<float>(1, 0);
	world.z = XYZ.at<float>(2, 0);

	return world;
}

/*
激光三角法计算世界坐标
世界坐标系由标定的第一张图片确定
参数：
uv		图像坐标
xwywzw	输出世界坐标
M		相机投影矩阵
*/
Point3f PlumbLineInstrument::uv2xwywzw(Point& uv, Mat& M)
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

void PlumbLineInstrument::uv2xwywzw(Point& uv, Point3f& xwywzw, Mat& M)
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

	xwywzw.x = world.at<float>(0, 0);
	xwywzw.y = world.at<float>(1, 0);
	xwywzw.z = world.at<float>(2, 0);
}

Point3f PlumbLineInstrument::uv2xcyczc(Point& uv, Mat& M)
{
	//     [u]      [xc]
	// zc *[v] = M *[yc]
	//     [1]      [zc]
	//              [ 1]
	// a*xc + b*yc + c*zc + d = 0
	//
	// [um31-m11 um32-m12 um33-m13] [xc]   [m14-um34]
	// [vm31-m21 vm32-m22 vm33-m23] [yc] = [m24-vm34]
	// [    a        b        c   ] [zc]   [   -d   ]




	float u = uv.x;
	float v = uv.y;
	float a = laserPlaneParams_L.at<float>(0, 0);
	float b = laserPlaneParams_L.at<float>(1, 0);
	float c = laserPlaneParams_L.at<float>(2, 0);
	float d = -1;
	float m11 = M.at<double>(0, 0);
	float m12 = M.at<double>(0, 1);
	float m13 = M.at<double>(0, 2);
	float m14 = 0;
	float m21 = M.at<double>(1, 0);
	float m22 = M.at<double>(1, 1);
	float m23 = M.at<double>(1, 2);
	float m24 = 0;
	float m31 = M.at<double>(2, 0);
	float m32 = M.at<double>(2, 1);
	float m33 = M.at<double>(2, 2);
	float m34 = 0;


	// [um31-m11 um32-m12 um33-m13] [xc]   [m14-um34]
	// [vm31-m21 vm32-m22 vm33-m23] [yc] = [m24-vm34]
	// [    a        b        c   ] [zc]   [   -d   ]
	// xyzc = A_ * B
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

	Mat camera = Mat(3, 1, CV_32FC1);
	camera = A_*B;

	Point3f xcyczc;
	xcyczc.x = camera.at<float>(0, 0);
	xcyczc.y = camera.at<float>(1, 0);
	xcyczc.z = camera.at<float>(2, 0);

	return xcyczc;
}

void PlumbLineInstrument::calculateBallCoord(Point& Puv_ball,Point3f& Pc_laser, Point3f& Pc_ball)
{
	//联立垂线方程和成像方程
	//得到四个方程
	//组合成矩阵，求最小二乘解

	float m11 = cameraMatrix_L.at<double>(0, 0);
	float m12 = cameraMatrix_L.at<double>(0, 1);
	float m13 = cameraMatrix_L.at<double>(0, 2);
	float m21 = cameraMatrix_L.at<double>(1, 0);
	float m22 = cameraMatrix_L.at<double>(1, 1);
	float m23 = cameraMatrix_L.at<double>(1, 2);
	float m31 = cameraMatrix_L.at<double>(2, 0);
	float m32 = cameraMatrix_L.at<double>(2, 1);
	float m33 = cameraMatrix_L.at<double>(2, 2);
	float a = levelPlaneParams.at<float>(0, 0);
	float b = levelPlaneParams.at<float>(0, 1);
	float c = levelPlaneParams.at<float>(0, 2);
	float u = Puv_ball.x;
	float v = Puv_ball.y;
	float x1 = Pc_laser.x;
	float y1 = Pc_laser.y;
	float z1 = Pc_laser.z;


	Mat A = Mat(4, 3, CV_32FC1);
	A.at<float>(0, 0) = u*m31 - m11;
	A.at<float>(0, 1) = u*m32 - m12;
	A.at<float>(0, 2) = u*m33 - m13;
	A.at<float>(1, 0) = v*m31 - m21;
	A.at<float>(1, 1) = v*m32 - m22;
	A.at<float>(1, 2) = v*m33 - m23;
	A.at<float>(2, 0) = c;
	A.at<float>(2, 1) = 0;
	A.at<float>(2, 2) = -a;
	A.at<float>(3, 0) = 0;
	A.at<float>(3, 1) = c;
	A.at<float>(3, 2) = -b;

	Mat B = Mat(4, 1, CV_32FC1);
	B.at<float>(0, 0) = 0;
	B.at<float>(0, 1) = 0;
	B.at<float>(0, 2) = c*x1 - a*z1;
	B.at<float>(0, 3) = c*y1 - b*z1;

	Mat XYZ = Mat(3, 1, CV_32FC1);
	solve(A, B, XYZ, DECOMP_SVD);

	Pc_ball.x = XYZ.at<float>(0, 0);
	Pc_ball.y = XYZ.at<float>(0, 1);
	Pc_ball.z = XYZ.at<float>(0, 2);
}






bool PlumbLineInstrument::findIntersection(Mat& frame, Point& intersection)
{
	//*******删除小球区域*******

	Mat src;
	cvtColor(frame, src, COLOR_BGR2GRAY);
	Mat image;
	cvtColor(frame, image, COLOR_BGR2GRAY);

	//二值化
	//过滤掉太暗太亮的区域
	Mat pre = src.clone();
	for (int v = 0; v < pre.rows; v++)
		for (int u = 0; u < pre.cols; u++)
		{
			int val = pre.at<uchar>(v, u);
			if (val < 50)
				pre.at<uchar>(v, u) = 0;
			else
				pre.at<uchar>(v, u) = 255;
		}
	//imwrite("pre.jpg", pre);

	//连续的腐蚀和膨胀
	//使小球与背景和垂线分离开
	Mat mid;
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(pre, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	//imwrite("mid.jpg", mid);

	vector<vector<Point>> contours;
	vector<Vec4i> hireachy;
	findContours(mid, contours, hireachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

	for (size_t t = 0; t < contours.size(); t++)
	{
		if (contourArea(contours[t]) < 3000)
			continue;

		Rect rect = boundingRect(contours[t]);
		float ratio = float(rect.width) / float(rect.height);
		if (ratio < 1.1 && ratio > 0.9)
		{
			vector<Point> ppt = { Point(rect.x,rect.y),Point(rect.x + rect.width,rect.y),Point(rect.x + rect.width,rect.y + rect.height),Point(rect.x ,rect.y + rect.height) };
			fillPoly(image, ppt, Scalar(0, 0, 0));

			imwrite("image.jpg", image);
		}
	}


	//*******找光斑点*******

	//在直线下方一定距离设方框 求和
	//包含光斑的框 求和值会更大 完全包含的最大
	//框要确保能包含光斑
	//找到之后求灰度质心
	threshold(image, image, 200, 255, THRESH_TOZERO); // 图像二值化

	int box_u0 = 0;				// 经验值，要自己设
	int box_v0 = 0;				// 经验值，要自己设
	int w = 100, h = 1280, step = 20; // 经验值，要自己设
	Mat select;
	int maxIndex = -1;
	int sum = 0, temp = 0;
	for (int u = box_u0; u < image.cols - w; u += step)
	{
		//对模板框中的部分求和
		select = image(Rect(u, box_v0, w, h));
		temp = cv::sum(select)[0];
		maxIndex++;
		if (temp >= sum)
			sum = temp;
		if (temp < sum)
		{
			maxIndex--;
			select = image(Rect(u - step, box_v0, w, h));
			break;
		}
	}

	//灰度质心法确定交点中心坐标
	int u, v;
	int fenzi1 = 0, fenzi2 = 0, fenmu = 0;
	for (int i = 0; i < select.cols; i++)
		for (int j = 0; j < select.rows; j++)
		{
			fenzi1 += i*select.at<uchar>(Point(i, j));
			fenzi2 += j*select.at<uchar>(Point(i, j));
			fenmu += select.at<uchar>(Point(i, j));
		}
	if (fenmu != 0)
	{
		u = fenzi1 / fenmu;
		v = fenzi2 / fenmu;

		intersection.x = u + box_u0 + maxIndex*step;
		intersection.y = v + box_v0;
		return true;
	}

	return false;
}

bool PlumbLineInstrument::findIntersection(Mat& frame, Point& leftPoint, Point& rightPoint)
{
	Mat image;
	cvtColor(frame, image, COLOR_BGR2GRAY);
	threshold(image, image, 100, 255, THRESH_BINARY); // 图像二值化

	Mat select;
	select = image(Rect(0, 0, 2048, 1280));

	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	Mat out;
	morphologyEx(select, out, MORPH_CLOSE, element);
	morphologyEx(out, out, MORPH_DILATE, element);
	morphologyEx(out, out, MORPH_DILATE, element);
	morphologyEx(out, out, MORPH_ERODE, element);
	morphologyEx(out, out, MORPH_ERODE, element);
	//morphologyEx(out, out, MORPH_ERODE, element);
	//morphologyEx(out, out, MORPH_ERODE, element);

	vector< vector< Point> > contours;
	findContours(out, contours, RETR_EXTERNAL, CHAIN_APPROX_NONE);
	vector<vector<Point>>::iterator itc;
	int maxSize = 0;
	for (itc = contours.begin(); itc != contours.end(); itc++)
	{
		maxSize = itc->size() > maxSize ? itc->size() : maxSize;
	}
	for (itc = contours.begin(); itc != contours.end(); itc++)
		if (itc->size() == maxSize)
			break;

	Mat out1 = Mat(out.size(), CV_8UC1, Scalar(0));
	drawContours(out1, contours, itc - contours.begin(), Scalar(255));

	out = out1.clone();
	leftPoint.x = out.cols;
	leftPoint.y = 0;
	rightPoint.x = 0;
	rightPoint.y = 0;
	for (int i = 0; i < out.rows; i++)
		for (int j = 0; j < out.cols; j++)
		{
			int val = out.at<uchar>(i, j);
			if (val != 0)
			{
				if (j < leftPoint.x)
				{
					leftPoint.x = j;
					leftPoint.y = i;
				}
				if (j > rightPoint.x)
				{
					rightPoint.x = j;
					rightPoint.y = i;
				}
			}
		}



	if (leftPoint.x == out.cols&&rightPoint.x == 0)
		return false;

	return true;
}

bool PlumbLineInstrument::findBallCenter(Mat& frame, Point& ballCenter)
{
	Mat src;
	cvtColor(frame, src, COLOR_BGR2GRAY);

	//二值化
	//过滤掉太暗太亮的区域
	Mat pre = src.clone();
	for (int v = 0; v < pre.rows; v++)
		for (int u = 0; u < pre.cols; u++)
		{
			int val = pre.at<uchar>(v, u);
			if (val < 30 || val>200)
				pre.at<uchar>(v, u) = 0;
			else
				pre.at<uchar>(v, u) = 255;
		}
	//imwrite("pre.jpg", pre);

	//连续的腐蚀和膨胀
	//使小球与背景和垂线分离开
	Mat mid;
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(pre, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	//imwrite("mid.jpg", mid);

	vector<vector<Point>> contours;
	vector<Vec4i> hireachy;
	findContours(mid, contours, hireachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

	Mat result = Mat::zeros(mid.size(), CV_8UC3);
	for (size_t t = 0; t < contours.size(); t++)
	{
		if (contourArea(contours[t]) < 3000)
			continue;

		Rect rect = boundingRect(contours[t]);
		float ratio = float(rect.width) / float(rect.height);
		if (ratio < 1.2 && ratio > 0.8)
		{
			drawContours(result, contours, t, Scalar(0, 0, 255), -1, 8, Mat(), 0, Point());

			Moments moment;//矩
			Mat temp(contours[t]); // 当前轮廓
			moment = moments(temp, false);
			if (moment.m00 != 0) // 除数不能为0
			{
				ballCenter.x = cvRound(moment.m10 / moment.m00); // 计算重心横坐标
				ballCenter.y = cvRound(moment.m01 / moment.m00); // 计算重心纵坐标
			}
			circle(result, ballCenter, 2, Scalar(0, 255, 0), 2, 8, 0);
			//imwrite("result.jpg", result);

			circle(src, ballCenter, 2, Scalar(0, 255, 0), 2, 8, 0);
			//imwrite("result1.jpg", src);
			break;
		}
	}


	return true;
}

bool PlumbLineInstrument::findTwoBallCenter(Mat& frame, Point& ball_1, Point& ball_2)
{
	Mat src;
	cvtColor(frame, src, COLOR_BGR2GRAY);

	//二值化
	//过滤掉太暗太亮的区域
	Mat pre = src.clone();
	for (int v = 0; v < pre.rows; v++)
		for (int u = 0; u < pre.cols; u++)
		{
			int val = pre.at<uchar>(v, u);
			if (val < 50 )
				pre.at<uchar>(v, u) = 0;
			else
				pre.at<uchar>(v, u) = 255;
		}
	imwrite("pre.jpg", pre);

	//连续的腐蚀和膨胀
	//使小球与背景和垂线分离开
	Mat mid;
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5));
	morphologyEx(pre, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_ERODE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	morphologyEx(mid, mid, MORPH_DILATE, element);
	imwrite("mid.jpg", mid);

	vector<vector<Point>> contours;
	vector<Vec4i> hireachy;
	findContours(mid, contours, hireachy, RETR_TREE, CHAIN_APPROX_SIMPLE, Point());

	Mat result = Mat::zeros(mid.size(), CV_8UC3);
	int ball_number = 0;
	for (size_t t = 0; t < contours.size(); t++)
	{
		if (contourArea(contours[t]) < 3000)
			continue;

		Rect rect = boundingRect(contours[t]);
		float ratio = float(rect.width) / float(rect.height);
		if (ratio < 1.1 && ratio > 0.9)
		{
			ball_number++;
			drawContours(result, contours, t, Scalar(0, 0, 255), -1, 8, Mat(), 0, Point());

			Point pt;
			Moments moment;//矩
			Mat temp(contours[t]); // 当前轮廓
			moment = moments(temp, false);
			if (moment.m00 != 0) // 除数不能为0
			{
				pt.x = cvRound(moment.m10 / moment.m00); // 计算重心横坐标
				pt.y = cvRound(moment.m01 / moment.m00); // 计算重心纵坐标
			}
			circle(result, pt, 2, Scalar(0, 255, 0), 2, 8, 0);
			imwrite("result.jpg", result);

			circle(src, pt, 2, Scalar(0, 255, 0), 2, 8, 0);
			imwrite("result1.jpg", src);

			if (ball_number == 1)
				ball_1 = pt;
			if (ball_number == 2)
			{
				ball_2 = pt;
				break;
			}
		}
	}

	//交换ball_1和ball_2，保证ball_1是上面的那个球
	if (ball_1.y > ball_2.y)
	{
		Point temp = ball_1;
		ball_1 = ball_2;
		ball_2 = temp;
	}


	return true;
}



void PlumbLineInstrument::measureOneBall(Mat& frame, Mat& result, Point3f& coordinate)
{
	Point intersectionLeft;
	Point ballCenterLeft;
	Point3f Pc_laser;
	Point3f Pc_ball;

	if (findIntersection(frame, intersectionLeft))
	{
		circle(result, intersectionLeft, 3, Scalar(0, 0, 255), 3);
		QImage lQimage = CvMat2QImage(result);
		QPixmap lQPixmap = QPixmap::fromImage(lQimage);
		lQPixmap = lQPixmap.scaled(LABEL_WIDTH, LABEL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui.labelLeft->setPixmap(lQPixmap);

		Pc_laser = uv2xcyczc(intersectionLeft, cameraMatrix_L);
		ui.label_u->setNum(intersectionLeft.x);
		ui.label_v->setNum(intersectionLeft.y);
		ui.label_xc->setNum(Pc_laser.x);
		ui.label_yc->setNum(Pc_laser.y);
		ui.label_zc->setNum(Pc_laser.z);
	}

	if (findBallCenter(frame, ballCenterLeft))
	{
		circle(result, ballCenterLeft, 3, Scalar(0, 255, 0), 3);
		QImage lQimage = CvMat2QImage(result);
		QPixmap lQPixmap = QPixmap::fromImage(lQimage);
		lQPixmap = lQPixmap.scaled(LABEL_WIDTH, LABEL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui.labelLeft->setPixmap(lQPixmap);

		//float u = ballCenterLeft.x, v = ballCenterLeft.y;
		//float a = levelPlaneParams.at<float>(0, 0), b = levelPlaneParams.at<float>(1, 0), c = levelPlaneParams.at<float>(2, 0);
		//float x1 = Pc_laser.x, y1 = Pc_laser.y, z1 = Pc_laser.z;
		//float fx = cameraMatrix_L.at<double>(0, 0), fy = cameraMatrix_L.at<double>(1, 1);
		//float u0 = cameraMatrix_L.at<double>(0, 2), v0 = cameraMatrix_L.at<double>(1, 2);

		//Pc_ball.z = (c*fx*x1 - a*fx*z1) / (c*(u - u0) - a*fx);
		//Pc_ball.x = a*(Pc_ball.z - z1) / c + x1;
		//Pc_ball.y = b*(Pc_ball.z - z1) / c + y1;

		calculateBallCoord(ballCenterLeft, Pc_laser, Pc_ball);

		ui.label_xc1->setNum(Pc_ball.x);
		ui.label_yc1->setNum(Pc_ball.y);
		ui.label_zc1->setNum(Pc_ball.z);
	}

	coordinate = Pc_ball;
}

void PlumbLineInstrument::measureTwoBalls(Mat& frame, Mat& result, Point3f& coord1, Point3f& coord2, float& d)
{
	Point intersectionLeft;
	Point ball_1, ball_2;
	Point3f Pc_laser;
	Point3f Pc_ball1, Pc_ball2;

	if (findIntersection(frame, intersectionLeft))
	{
		circle(result, intersectionLeft, 3, Scalar(0, 255, 0), 3);
		QImage lQimage = CvMat2QImage(result);
		QPixmap lQPixmap = QPixmap::fromImage(lQimage);
		lQPixmap = lQPixmap.scaled(LABEL_WIDTH, LABEL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui.labelLeft->setPixmap(lQPixmap);

		Pc_laser = uv2xcyczc(intersectionLeft, cameraMatrix_L);
		ui.label_u->setNum(intersectionLeft.x);
		ui.label_v->setNum(intersectionLeft.y);
		ui.label_xc->setNum(Pc_laser.x);
		ui.label_yc->setNum(Pc_laser.y);
		ui.label_zc->setNum(Pc_laser.z);
	}

	if (findTwoBallCenter(frame, ball_1, ball_2))
	{
		circle(result, ball_1, 3, Scalar(0, 0, 255), 3);
		circle(result, ball_2, 3, Scalar(0, 0, 255), 3);
		QImage lQimage = CvMat2QImage(result);
		QPixmap lQPixmap = QPixmap::fromImage(lQimage);
		lQPixmap = lQPixmap.scaled(LABEL_WIDTH, LABEL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
		ui.labelLeft->setPixmap(lQPixmap);
		ui.label_u1->setNum(ball_1.x);
		ui.label_v1->setNum(ball_1.y);
		ui.label_u2->setNum(ball_2.x);
		ui.label_v2->setNum(ball_2.y);

		//float a = levelPlaneParams.at<float>(0, 0), b = levelPlaneParams.at<float>(1, 0), c = levelPlaneParams.at<float>(2, 0);
		//float x1 = Pc_laser.x, y1 = Pc_laser.y, z1 = Pc_laser.z;
		//float fx = cameraMatrix_L.at<double>(0, 0), fy = cameraMatrix_L.at<double>(1, 1);
		//float u0 = cameraMatrix_L.at<double>(0, 2), v0 = cameraMatrix_L.at<double>(1, 2);

		//float u = ball_1.x, v = ball_1.y;
		//Pc_ball1.z = (c*fx*x1 - a*fx*z1) / (c*(u - u0) - a*fx);
		//Pc_ball1.x = a*(Pc_ball1.z - z1) / c + x1;
		//Pc_ball1.y = b*(Pc_ball1.z - z1) / c + y1;

		//u = ball_2.x;
		//v = ball_2.y;
		//Pc_ball2.z = (c*fx*x1 - a*fx*z1) / (c*(u - u0) - a*fx);
		//Pc_ball2.x = a*(Pc_ball2.z - z1) / c + x1;
		//Pc_ball2.y = b*(Pc_ball2.z - z1) / c + y1;

		calculateBallCoord(ball_1, Pc_laser, Pc_ball1);
		calculateBallCoord(ball_2, Pc_laser, Pc_ball2);


		ui.label_xc1->setNum(Pc_ball1.x);
		ui.label_yc1->setNum(Pc_ball1.y);
		ui.label_zc1->setNum(Pc_ball1.z);
		ui.label_xc2->setNum(Pc_ball2.x);
		ui.label_yc2->setNum(Pc_ball2.y);
		ui.label_zc2->setNum(Pc_ball2.z);
	}

	coord1 = Pc_ball1;
	coord2 = Pc_ball2;
	d = sqrt((coord1.x - coord2.x)*(coord1.x - coord2.x) + (coord1.y - coord2.y)*(coord1.y - coord2.y) + (coord1.z - coord2.z)*(coord1.z - coord2.z));

	ui.label_d->setNum(d);
}


void PlumbLineInstrument::test()
{
	Mat frame = imread("test.jpg");
	Mat result;
	undistort(frame, result, cameraMatrix_L, distCoeffs_L); // 单目图像畸变矫正
	frame = result.clone();

	Point3f P1, P2;
	float d;
	measureTwoBalls(frame, result, P1, P2, d);

}