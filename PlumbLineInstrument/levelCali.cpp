#include "levelCali.h"



bool LevelCali::levelPlaneCali()
{
	//��points_uv
	vector<Point2f> corners;
	Mat imageInput = imread(image_folder + image_name);
	cvtColor(imageInput, imageInput, COLOR_BGR2GRAY);
	imageSize.width = imageInput.cols;
	imageSize.height = imageInput.rows;

	bool found = findChessboardCorners(imageInput, patternSize, corners);

	if (found)
	{
		TermCriteria criteria = TermCriteria(TermCriteria::Type::EPS + TermCriteria::Type::MAX_ITER, 40, 0.001);
		cornerSubPix(imageInput, corners, Size(11, 11), Size(-1, -1), criteria);

		Mat imageCorners= imageInput.clone();
		cvtColor(imageCorners, imageCorners, COLOR_GRAY2RGB);
		drawChessboardCorners(imageCorners, patternSize, corners, true);
		imwrite(image_folder + string("corners.jpg"), imageCorners);

		points_uv = corners;
	}


	//��points_w		�谴�սǵ��˳��ֵ
	int ii = patternSize.height;
	for (int i = 0; i < patternSize.height; i++)
	{
		for (int j = 0; j < patternSize.width; j++)
		{
			Point3f realPoint;
			// ����궨��λ����������ϵZ=0��ƽ��
			realPoint.x = j * chessboardSize.width;
			realPoint.y = i * chessboardSize.height;
			realPoint.z = 0;
			points_w.push_back(realPoint);
		}
	}


	//PnP������̸�����ϵ���������ϵ��R��t����
	solvePnP(points_w, points_uv, cameraMatrix, distCoeffs, rotationMatrix, translationMatrix, false);
	Rodrigues(rotationMatrix, rotationMatrix); // ����ת����ת��Ϊ��ת����


	//Xw->Xc
	Point3f tmp_point;
	Mat WorldPoint = Mat(3, 1, CV_32FC1);
	WorldPoint.at<float>(2, 0) = 0;
	Mat CamPoint(3, 1, CV_32FC1);
	for (int k = 0; k < points_w.size(); k++)
	{
		WorldPoint.at<float>(0, 0) = points_w[k].x;
		WorldPoint.at<float>(1, 0) = points_w[k].y;
		CamPoint = rotationMatrix*WorldPoint + translationMatrix;
		tmp_point.x = CamPoint.at<float>(0, 0);
		tmp_point.y = CamPoint.at<float>(1, 0);
		tmp_point.z = CamPoint.at<float>(2, 0);
		points_c.push_back(tmp_point);
	}


	//���ˮƽ��
	if (points_c.size() > 0)
	{
		FileStorage levelParamsStore(data_folder + string(levelPlaneCali_result), FileStorage::WRITE);
		int m = points_c.size();
		Mat L = Mat::ones(m, 1, CV_32FC1);
		Mat realPointsMat = Mat(m, 3, CV_32FC1);
		for (int i = 0; i < m; ++i)
		{
			realPointsMat.at<float>(i, 0) = points_c[i].x;
			realPointsMat.at<float>(i, 1) = points_c[i].y;
			realPointsMat.at<float>(i, 2) = points_c[i].z;
		}
		solve(realPointsMat, L, levelPlaneParams, DECOMP_SVD);
		levelParamsStore << "levelPlaneParams" << levelPlaneParams;
		levelParamsStore.release();

		return true;
	}
	

	return false;
}


Mat LevelCali::getLevelPlaneParams()
{
	return levelPlaneParams.clone();
}