#include "caliCalculate.h"
//构造函数，内参矩阵及畸变参数初始化
/*Calculate::Calculate()//左相机
{
cameraMatrix.at<double>(0, 0) = 1187.80888200587;//fx
cameraMatrix.at<double>(0, 1) = -2.27161305982071;//s
cameraMatrix.at<double>(0, 2) = 639.032182822068;//u0
cameraMatrix.at<double>(1, 1) = 1188.05360580893;//fy
cameraMatrix.at<double>(1, 2) = 514.142317480921;//v0
cameraMatrix.at<double>(2, 2) = 1.0;

distCoeffs.at<double>(0, 0) = -0.426271449143598;///k1
distCoeffs.at<double>(0, 1) = 0.157178694181141;//k2
distCoeffs.at<double>(0, 2) = 0.000285610416225541;//p1
distCoeffs.at<double>(0, 3) = 0.00112003229100272;//p2
distCoeffs.at<double>(0, 4) = 0.0723672233045222;//k3
}*/

CaliCalculate::CaliCalculate()//右相机
{
	cameraMatrix.at<double>(0, 0) = 1176.39523939026;//1219.37503093218; //fx
	cameraMatrix.at<double>(0, 1) = -3.08522380508897;//0; //s
	cameraMatrix.at<double>(0, 2) = 654.905644043961;//644.423115166987; //u0
	cameraMatrix.at<double>(1, 1) = 1175.31005072637;//1218.35533262916; //fy
	cameraMatrix.at<double>(1, 2) = 507.462556712558;//514.443952018582; //v0
	cameraMatrix.at<double>(2, 2) = 1.0;

	distCoeffs.at<double>(0, 0) = -0.427244251241396;///-0.457137125002836;
	distCoeffs.at<double>(0, 1) = 0.209713780553138;//0.234545113147944;
	distCoeffs.at<double>(0, 2) = 0.000213689515065958;//-0.00264504726455653;
	distCoeffs.at<double>(0, 3) = 0.000464517455678508;//0.00380991759213225;
	distCoeffs.at<double>(0, 4) = -0.0175058345960471;//-0.0382479356523482;
}

CaliCalculate::~CaliCalculate()
{
}

void CaliCalculate::mergeRT(Mat&M, Mat R, Mat T)
{
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
		{
			M.at<double>(i, j) = R.at<double>(i, j);
		}
	for (int i = 0; i < 3; i++)
		M.at<double>(i, 3) = T.at<double>(i, 0);
}
//由网格点计算网格坐标系到摄像机坐标系的RT矩阵
bool CaliCalculate::getR_T(vector<crossdot>cdt)
{
	vector<Point2f>Point_uv;
	vector<Point3f>Point_W;

	Point3d tmp3dpt;
	for (int i = 0; i < cdt.size(); i++)
	{
		Point_uv.push_back(cdt[i].pt);
		tmp3dpt.x = 5.5 * cdt[i].indexh;
		tmp3dpt.y = 5.5 * cdt[i].indexz;
		tmp3dpt.z = 0;
		Point_W.push_back(tmp3dpt);
	}
	bool IsGetRT = solvePnP(Point_W, Point_uv, cameraMatrix, NewdistCoeffs, RotationMat, TransVec, false);//,CV_EPNP, CV_EPNP

	Rodrigues(RotationMat, RotationMat);
	//cout << RotationMat<<endl;
	//cout << TransVec;
	return IsGetRT;
}
//由光条像素坐标计算世界坐标系下的坐标（Xw,Yw,Zw=0
//输入参数为光条中心像素坐标，内参外参矩阵
//第四个参数为输出的世界坐标（Xw,Yw）注意Zw=0
void CaliCalculate::CalWorldPoint(vector<Point2d>point_uv, Mat IntrinsicM, Mat ExtriM3x4, vector<Point2d>&Point_XwYw)
{
	Mat proj;
	proj = IntrinsicM*ExtriM3x4;
	Mat temp = Mat(2, 2, CV_64FC1);
	double um, vm;
	double Xw, Yw;//世界坐标
	Point2d tmp_point;
	for (unsigned int k = 0; k < point_uv.size(); k++)
	{
		temp.at<double>(0, 0) = proj.at<double>(2, 0)*point_uv[k].x - proj.at<double>(0, 0);
		temp.at<double>(0, 1) = proj.at<double>(2, 1)*point_uv[k].x - proj.at<double>(0, 1);
		temp.at<double>(1, 0) = proj.at<double>(2, 0)*point_uv[k].y - proj.at<double>(1, 0);
		temp.at<double>(1, 1) = proj.at<double>(2, 1)*point_uv[k].y - proj.at<double>(1, 1);
		temp = temp.inv();//求逆矩阵
		um = proj.at<double>(0, 3) - proj.at<double>(2, 3)*point_uv[k].x;
		vm = proj.at<double>(1, 3) - proj.at<double>(2, 3)*point_uv[k].y;
		Xw = um*temp.at<double>(0, 0) + vm*temp.at<double>(0, 1);
		Yw = um*temp.at<double>(1, 0) + vm*temp.at<double>(1, 1);
		tmp_point.x = Xw;
		tmp_point.y = Yw;
		Point_XwYw.push_back(tmp_point);

	}
	//Mat mz = Mat::zeros(Size(5, 5), CV_8UC1); // 全零矩阵 
}
//将光条的世界坐标转换到摄像机坐标(Xw,Yw,Zw=0->XcYcZc)
void CaliCalculate::WorldToCam(Mat R, Mat T, vector<Point2d>Point_XwYw, vector<Point3d>&Point_XcYcZc)
{
	Point3d tmp_point;
	Mat WorldPoint = Mat(3, 1, CV_64FC1);
	WorldPoint.at<double>(2, 0) = 0;
	Mat CamPoint(3, 1, CV_64FC1);
	for (unsigned int k = 0; k < Point_XwYw.size(); k++)
	{
		WorldPoint.at<double>(0, 0) = Point_XwYw[k].x;
		WorldPoint.at<double>(1, 0) = Point_XwYw[k].y;
		CamPoint = R*WorldPoint + T;
		tmp_point.x = CamPoint.at<double>(0, 0);
		tmp_point.y = CamPoint.at<double>(1, 0);
		tmp_point.z = CamPoint.at<double>(2, 0);
		Point_XcYcZc.push_back(tmp_point);
	}
}

void CaliCalculate::test_accur(vector<crossdot> cdt, Mat ExtriM)
{
	vector<Point2f>Point_W;
	Point2d tmppt;//Xw,Yw,Zw=0
	vector<Point2d>pt_grid;
	for (int i = 0; i < cdt.size(); i++)
	{
		pt_grid.push_back(cdt[i].pt);
		tmppt.x = 5.5 * cdt[i].indexh;
		tmppt.y = 5.5 * cdt[i].indexz;
		Point_W.push_back(tmppt);
	}
	vector<Point2d>pt_XwYw;
	CalWorldPoint(pt_grid, cameraMatrix, ExtriM, pt_XwYw);
	cout << "网格点坐标重建:" << endl;
	for (int i = 0; i < pt_XwYw.size(); i++)
	{
		double xer = pt_XwYw[i].x - Point_W[i].x;
		double yer = pt_XwYw[i].y - Point_W[i].y;
		double er = sqrt(xer*xer + yer*yer);
		cout << "(" << pt_XwYw[i].x << "," << pt_XwYw[i].y << ")" << "  error:" << er << endl;
	}
}