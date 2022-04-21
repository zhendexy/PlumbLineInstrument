#include "CameraCali.h"


/*
��Ŀ�궨
������
imageList		��ű궨ͼƬ���Ƶ�txt
singleCalibrateResult	��ű궨�����txt
objectPoints	��������ϵ�е������
corners_seq		���ͼ���еĽǵ�,��������궨
cameraMatrix	������ڲ�������
distCoeffs		����Ļ���ϵ��
imageSize		����ͼ��ĳߴ磨���أ�
patternSize		�궨��ÿ�еĽǵ����, �궨��ÿ�еĽǵ���� (15, 9)
chessboardSize	������ÿ������ı߳���mm��
ע�⣺�����ؾ�ȷ��ʱ���������뵥ͨ����8λ���߸�����ͼ����������ͼ������Ͳ�ͬ�����������궨�����������ڲ�������ͻ���ϵ�������ڳ�ʼ��ʱҲҪ����ע�����͡�
*/
bool CameraCali::singleCameraCalibrate(const char* imageFolder, const char* imageList, const char* singleCalibrateResult, vector<vector<Point3f>>& objectPoints,
	vector<vector<Point2f>>& corners_seq, Mat& cameraMatrix, Mat& distCoeffs, Size& imageSize, Size patternSize, Size chessboardSize)

{
	int n_boards = 0;
	ifstream imageStore(data_folder + string(imageList)); // �򿪴�ű궨ͼƬ���Ƶ�txt
	FileStorage resultStore(data_folder + string(singleCalibrateResult), FileStorage::WRITE); // ����궨�����txt
																							  // ��ʼ��ȡ�ǵ�����
	vector<Point2f> corners; // ���һ��ͼƬ�Ľǵ����� 
	string imageName; // ��ȡ�ı궨ͼƬ������
	while (getline(imageStore, imageName)) // ��ȡtxt��ÿһ�У�ÿһ�д����һ�ű궨ͼƬ�����ƣ�
	{
		n_boards++;
		Mat imageInput = imread(imageFolder + imageName);
		cvtColor(imageInput, imageInput, COLOR_RGB2GRAY);
		imageSize.width = imageInput.cols; // ��ȡͼƬ�Ŀ��
		imageSize.height = imageInput.rows; // ��ȡͼƬ�ĸ߶�
											// ���ұ궨��Ľǵ�
		bool found = findChessboardCorners(imageInput, patternSize, corners); // ���һ������int flags��ȱʡֵΪ��CALIB_CB_ADAPTIVE_THRESH | CALIB_CB_NORMALIZE_IMAGE
																			  // �����ؾ�ȷ������findChessboardCorners���Զ�������cornerSubPix��Ϊ�˸��Ӿ�ϸ���������Լ��ٵ���һ�Ρ�
		if (found) // �����еĽǵ㶼���ҵ�
		{
			TermCriteria criteria = TermCriteria(TermCriteria::Type::EPS + TermCriteria::Type::MAX_ITER, 40, 0.001); // ��ֹ��׼������40�λ��ߴﵽ0.001�����ؾ���
			cornerSubPix(imageInput, corners, Size(11, 11), Size(-1, -1), criteria);// �������ǵ�ͼ��ֻ��ϴ󣬽��������ڵ���һЩ����11�� 11��Ϊ��ʵ���ڵ�һ�룬��ʵ��СΪ��11*2+1�� 11*2+1��--��23�� 23��
			corners_seq.push_back(corners); // ����ǵ�����
											// ���ƽǵ�
											//drawChessboardCorners(imageInput, patternSize, corners, true);
											//imshow("cornersframe", imageInput);
											//waitKey(500); // ��ͣ0.5s

			Mat imageCorners = imageInput.clone();
			cvtColor(imageCorners, imageCorners, COLOR_GRAY2RGB);
			drawChessboardCorners(imageCorners, patternSize, corners, true);
			string cornersName = "corners_" + imageName;
			imwrite(data_folder + string("corners\\") + cornersName, imageCorners);
		}
	}

	// ��������궨
	int pic, i, j;
	for (pic = 0; pic < n_boards; pic++)
	{
		vector<Point3f> realPointSet;
		for (i = 0; i < patternSize.height; i++)
		{
			for (j = 0; j < patternSize.width; j++)
			{
				Point3f realPoint;
				// ����궨��λ����������ϵZ=0��ƽ��
				realPoint.x = j * chessboardSize.width;
				realPoint.y = i * chessboardSize.height;
				realPoint.z = 0;
				realPointSet.push_back(realPoint);
			}
		}
		objectPoints.push_back(realPointSet);
	}
	// ִ�б궨����
	vector<Mat> rvec; // ��ת����
	vector<Mat> tvec; // ƽ������
	cv::calibrateCamera(objectPoints, corners_seq, imageSize, cameraMatrix, distCoeffs, rvec, tvec, 0);
	// ����궨���
	resultStore << "cameraMatrix" << cameraMatrix; // ����ڲ�������
	resultStore << "distCoeffs" << distCoeffs; // �������ϵ��
											   // �����һ��ͼƬ��������
	Mat rotationVector, rotationMatrix, translationVector, extrinsicMatrix, projectionMatrix;
	rotationVector = rvec[0];
	resultStore << "rotationVector" << rotationVector; // �����ת����
	Rodrigues(rotationVector, rotationMatrix);
	resultStore << "rotationMatrix" << rotationMatrix; // �����ת����
	translationVector = tvec[0];
	resultStore << "translationVector" << translationVector; // ���ƽ������
	hconcat(rotationMatrix, translationVector, extrinsicMatrix);
	resultStore << "extrinsicMatrix" << extrinsicMatrix; // ������������
	projectionMatrix = cameraMatrix*extrinsicMatrix;
	resultStore << "projectionMatrix" << projectionMatrix; // ���ͶӰ����
														   // ������ͶӰ�㣬��ԭͼ�ǵ�Ƚϣ��õ����
	double errPerImage = 0.; // ÿ��ͼ������
	double errAverage = 0.; // ����ͼ���ƽ�����
	double totalErr = 0.; // ����ܺ�
	vector<Point2f> projectImagePoints; // ��ͶӰ��
	fstream fs_imageErr(data_folder + string("errPerImage.txt"), ios::out);
	for (i = 0; i < n_boards; i++)
	{
		vector<Point3f> tempObjectPoints = objectPoints[i]; // ��ʱ��ά��
															// ������ͶӰ��
		projectPoints(tempObjectPoints, rvec[i], tvec[i], cameraMatrix, distCoeffs, projectImagePoints);
		// �����µ�ͶӰ����ɵ�ͶӰ��֮������
		vector<Point2f> tempCornersPoints = corners_seq[i];// ��ʱ��ž�ͶӰ��
		Mat tempCornersPointsMat = Mat(1, tempCornersPoints.size(), CV_32FC2); // ���������ͨ����Mat��Ϊ�˼������
		Mat projectImagePointsMat = Mat(1, projectImagePoints.size(), CV_32FC2);
		// ��ֵ
		for (int j = 0; j < tempCornersPoints.size(); j++)
		{
			projectImagePointsMat.at<Vec2f>(0, j) = Vec2f(projectImagePoints[j].x, projectImagePoints[j].y);
			tempCornersPointsMat.at<Vec2f>(0, j) = Vec2f(tempCornersPoints[j].x, tempCornersPoints[j].y);
		}
		// opencv���norm������ʵ�����������ͨ���ֱ�ֿ��������(X1-X2)^2��ֵ��Ȼ��ͳһ��ͣ������и���
		errPerImage = norm(tempCornersPointsMat, projectImagePointsMat, NORM_L2) / (patternSize.width * patternSize.height);
		totalErr += errPerImage;
		fs_imageErr << "No." + to_string(i + 1) + "errPerImage" << errPerImage << endl;
	}
	fs_imageErr << "totalErr/n_boards" << totalErr / n_boards;
	fs_imageErr.close();
	imageStore.close();
	resultStore.release();
	return true;
}

/*
˫Ŀ�궨:����������������ת���� R,ƽ������ T, ��������E, ��������F
������
stereoCalibrateResult	�������궨�����txt
objectPoints			��ά��
imagePoints				��άͼ���ϵĵ�
cameraMatrix			����ڲ���
distCoeffs				�������ϵ��
imageSize				ͼ��ߴ�
R		���������Ե���ת����
T		���������Ե�ƽ������
E		��������
F		��������
*/
bool CameraCali::stereoCalibrate(const char* stereoCalibrateResult, vector<vector<Point3f>> objectPoints, vector<vector<Point2f>> imagePoints1, vector<vector<Point2f>> imagePoints2,
	Mat& cameraMatrix1, Mat& distCoeffs1, Mat& cameraMatrix2, Mat& distCoeffs2, Size& imageSize, Mat& R, Mat& T, Mat& E, Mat& F)

{
	FileStorage stereoStore(data_folder + string(stereoCalibrateResult), FileStorage::WRITE);
	TermCriteria criteria = TermCriteria(TermCriteria::COUNT | TermCriteria::EPS, 30, 1e-6); // ��ֹ����
	cv::stereoCalibrate(objectPoints, imagePoints1, imagePoints2, cameraMatrix1, distCoeffs1,
		cameraMatrix2, distCoeffs2, imageSize, R, T, E, F, CALIB_FIX_INTRINSIC, criteria); // ע�����˳�򣬿��Ե�������ļ��в鿴�����ⷵ��ʱ����
	stereoStore << "cameraMatrix1" << cameraMatrix1;
	stereoStore << "cameraMatrix2" << cameraMatrix2;
	stereoStore << "distCoeffs1" << distCoeffs1;
	stereoStore << "distCoeffs2" << distCoeffs2;
	stereoStore << "R" << R;
	stereoStore << "T" << T;
	stereoStore << "E" << E;
	stereoStore << "F" << F;
	stereoStore.release();
	return true;
}

/*
����У��
������
stereoRectifyParams		�������У�������txt
cameraMatrix			����ڲ���
distCoeffs				�������ϵ��
imageSize				ͼ��ߴ�
R						���������Ե���ת����
T						���������Ե�ƽ������
R1, R2					�ж�����תУ��
P1, P2					����ͶӰ����
Q						��ͶӰ����
map1, map2				��ͶӰӳ���
*/
Rect CameraCali::stereoRectification(const char* stereoRectifyParams, Mat& cameraMatrix1, Mat& distCoeffs1, Mat& cameraMatrix2, Mat& distCoeffs2,
	Size& imageSize, Mat& R, Mat& T, Mat& R1, Mat& R2, Mat& P1, Mat& P2, Mat& Q, Mat& mapl1, Mat& mapl2, Mat& mapr1, Mat& mapr2)

{
	Rect validRoi[2];
	FileStorage stereoStore(data_folder + string(stereoRectifyParams), FileStorage::WRITE);
	cv::stereoRectify(cameraMatrix1, distCoeffs1, cameraMatrix2, distCoeffs2, imageSize,
		R, T, R1, R2, P1, P2, Q, 0, -1, imageSize, &validRoi[0], &validRoi[1]);
	// ��������ͼ�����ͶӰӳ���
	stereoStore << "R1" << R1;
	stereoStore << "R2" << R2;
	stereoStore << "P1" << P1;
	stereoStore << "P2" << P2;
	stereoStore << "Q" << Q;
	cout << "R1:" << endl;
	cout << R1 << endl;
	cout << "R2:" << endl;
	cout << R2 << endl;
	cout << "P1:" << endl;
	cout << P1 << endl;
	cout << "P2:" << endl;
	cout << P2 << endl;
	cout << "Q:" << endl;
	cout << Q << endl;
	initUndistortRectifyMap(cameraMatrix1, distCoeffs1, R1, P1, imageSize, CV_32FC1, mapl1, mapl2);
	initUndistortRectifyMap(cameraMatrix2, distCoeffs2, R2, P2, imageSize, CV_32FC1, mapr1, mapr2);
	//stereoStore << "mapl1" << mapl1;
	//stereoStore << "mapl2" << mapl2;
	//stereoStore << "mapr1" << mapr1;
	//stereoStore << "mapr2" << mapr2;
	stereoStore.release();
	return validRoi[0], validRoi[1];
}

/*
�����Ӳ�ͼ
������
imageName1	����������ͼ��
imageName2	����������ͼ��
img1_rectified	��ӳ����������ͼ��
img2_rectified	��ӳ�����Ҳ����ͼ��
map	��ͶӰӳ���
*/
bool CameraCali::computeDisparityImage(const char* imageName1, const char* imageName2, Mat& img1_rectified,
	Mat& img2_rectified, Mat& mapl1, Mat& mapl2, Mat& mapr1, Mat& mapr2, Rect validRoi[2], Mat& disparity)

{
	// ���ȣ����������������ͼƬ�����ع�
	Mat img1 = imread(string(imageName1));
	Mat img2 = imread(string(imageName2));
	//frameLeft = img1.clone();
	//frameRight = img2.clone();
	if (img1.empty() | img2.empty())
	{
		cout << "ͼ��Ϊ��" << endl;
	}
	Mat gray_img1, gray_img2;
	cvtColor(img1, gray_img1, COLOR_BGR2GRAY);
	cvtColor(img2, gray_img2, COLOR_BGR2GRAY);
	Mat canvas(imageSize.height, imageSize.width * 2, CV_8UC1); // ע����������
	Mat canLeft = canvas(Rect(0, 0, imageSize.width, imageSize.height));
	Mat canRight = canvas(Rect(imageSize.width, 0, imageSize.width, imageSize.height));
	gray_img1.copyTo(canLeft);
	gray_img2.copyTo(canRight);
	imwrite(data_folder + string("У��ǰ�������ͼ��.jpg"), canvas);
	remap(gray_img1, img1_rectified, mapl1, mapl2, INTER_LINEAR);
	remap(gray_img2, img2_rectified, mapr1, mapr2, INTER_LINEAR);
	imwrite(data_folder + string("�����У��ͼ��.jpg"), img1_rectified);
	imwrite(data_folder + string("�����У��ͼ��.jpg"), img2_rectified);

	//QImage QImage1 = CvMat2QImage(img1_rectified);
	//QImage QImage2 = CvMat2QImage(img2_rectified);
	//QPixmap QPixmap1 = QPixmap::fromImage(QImage1);
	//QPixmap QPixmap2 = QPixmap::fromImage(QImage2);
	//QPixmap1 = QPixmap1.scaled(500, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	//QPixmap2 = QPixmap2.scaled(500, 400, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	//ui.labelLeft->setPixmap(QPixmap1);
	//ui.labelRight->setPixmap(QPixmap2);

	img1_rectified.copyTo(canLeft);
	img2_rectified.copyTo(canRight);
	rectangle(canLeft, validRoi[0], Scalar(255, 255, 255), 5, 8);
	rectangle(canRight, validRoi[1], Scalar(255, 255, 255), 5, 8);
	for (int j = 0; j <= canvas.rows; j += 16)
		line(canvas, Point(0, j), Point(canvas.cols, j), Scalar(0, 255, 0), 1, 8);
	imwrite(data_folder + string("У�����������ͼ��.jpg"), canvas);
	// ��������ƥ��
	Ptr<StereoBM> bm = StereoBM::create(16, 9); // Ptr<>��һ������ָ��
	bm->compute(img1_rectified, img2_rectified, disparity); // �����Ӳ�ͼ
	disparity.convertTo(disparity, CV_32F, 1.0 / 16);
	// ��һ���Ӳ�ӳ��
	normalize(disparity, disparity, 0, 256, NORM_MINMAX, -1);
	return true;
}
bool CameraCali::computeDisparityImage(Mat& img1, Mat& img2, Mat& img1_rectified,
	Mat& img2_rectified, Mat& mapl1, Mat& mapl2, Mat& mapr1, Mat& mapr2, Rect validRoi[2], Mat& disparity)
{
	// ���ȣ����������������ͼƬ�����ع�
	if (img1.empty() | img2.empty())
	{
		cout << "ͼ��Ϊ��" << endl;
	}
	Mat gray_img1, gray_img2;
	cvtColor(img1, gray_img1, COLOR_BGR2GRAY);
	cvtColor(img2, gray_img2, COLOR_BGR2GRAY);
	Mat canvas(imageSize.height, imageSize.width * 2, CV_8UC1); // ע����������
	Mat canLeft = canvas(Rect(0, 0, imageSize.width, imageSize.height));
	Mat canRight = canvas(Rect(imageSize.width, 0, imageSize.width, imageSize.height));
	gray_img1.copyTo(canLeft);
	gray_img2.copyTo(canRight);
	imwrite(data_folder + string("У��ǰ�������ͼ��.jpg"), canvas);
	remap(gray_img1, img1_rectified, mapl1, mapl2, INTER_LINEAR);
	remap(gray_img2, img2_rectified, mapr1, mapr2, INTER_LINEAR);
	imwrite(data_folder + string("�����У��ͼ��.jpg"), img1_rectified);
	imwrite(data_folder + string("�����У��ͼ��.jpg"), img2_rectified);

	//QImage QImage1 = CvMat2QImage(img1_rectified);
	//QImage QImage2 = CvMat2QImage(img2_rectified);
	//QPixmap QPixmap1 = QPixmap::fromImage(QImage1);
	//QPixmap QPixmap2 = QPixmap::fromImage(QImage2);
	//QPixmap1 = QPixmap1.scaled(LABEL_WIDTH, LABEL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	//QPixmap2 = QPixmap2.scaled(LABEL_WIDTH, LABEL_HEIGHT, Qt::KeepAspectRatio, Qt::SmoothTransformation);
	//ui.labelLeft->setPixmap(QPixmap1);
	//ui.labelRight->setPixmap(QPixmap2);

	img1_rectified.copyTo(canLeft);
	img2_rectified.copyTo(canRight);
	rectangle(canLeft, validRoi[0], Scalar(255, 255, 255), 5, 8);
	rectangle(canRight, validRoi[1], Scalar(255, 255, 255), 5, 8);
	for (int j = 0; j <= canvas.rows; j += 16)
		line(canvas, Point(0, j), Point(canvas.cols, j), Scalar(0, 255, 0), 1, 8);
	imwrite(data_folder + string("У�����������ͼ��.jpg"), canvas);
	// ��������ƥ��
	Ptr<StereoBM> bm = StereoBM::create(16, 9); // Ptr<>��һ������ָ��
	bm->compute(img1_rectified, img2_rectified, disparity); // �����Ӳ�ͼ
	disparity.convertTo(disparity, CV_32F, 1.0 / 16);
	// ��һ���Ӳ�ӳ��
	normalize(disparity, disparity, 0, 256, NORM_MINMAX, -1);
	return true;
}




void CameraCali::leftCali()
{
	singleCameraCalibrate(left_folder, imageList_L, singleCalibrate_result_L, objectPoints_L, corners_seq_L, cameraMatrix_L,
		distCoeffs_L, imageSize, patternSize, chessboardSize);
}

void CameraCali::rightCali()
{
	singleCameraCalibrate(right_folder, imageList_R, singleCalibrate_result_R, objectPoints_R, corners_seq_R, cameraMatrix_R,
		distCoeffs_R, imageSize, patternSize, chessboardSize);
}

void CameraCali::stereoCali()
{
	stereoCalibrate(stereoCalibrate_result_L, objectPoints_L, corners_seq_L, corners_seq_R, cameraMatrix_L, distCoeffs_L,
		cameraMatrix_R, distCoeffs_R, imageSize, R, T, E, F);
}

void CameraCali::stereoRecti()
{
	validRoi[0], validRoi[1] = stereoRectification(stereoRectifyParams, cameraMatrix_L, distCoeffs_L, cameraMatrix_R, distCoeffs_R,
		imageSize, R, T, R1, R2, P1, P2, Q, mapl1, mapl2, mapr1, mapr2);
}
