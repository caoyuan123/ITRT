#include "ImgProcFilter.h"
#include <opencv.hpp>
#include <iostream>
#include <algorithm>
#include <qwidget.h>
#include <QDebug>

using namespace std;
using namespace cv;

/*
#if _DEBUG

ImgProcFilter GFilter;
static string window_name = "Photo";
static Mat src;
static string curves_window = "Adjust Curves";
int channel;


static void invalidate()  //更新单元
{
	GFilter.drawMat(GFilter.curves_mat);
	cv::imshow(curves_window,GFilter.curves_mat);

	Mat dst;
	GFilter.adjust(GFilter.gSourceMat, dst);  
	cv::imshow(window_name, dst);

}

static void callbackMouseEvent(int mouseEvent, int x, int y, int flags, void* param)  
{
	switch(mouseEvent) {
	case CV_EVENT_LBUTTONDOWN :
		GFilter.mouseDown(x, y);
		invalidate( );
		break;

	case CV_EVENT_MOUSEMOVE :
		if ( GFilter.mouseMove(x, y) )
		invalidate( );
		break;

	case CV_EVENT_LBUTTONUP :
		GFilter.mouseUp(x, y);
		invalidate( );
		break;
	}
	return;
}


void TestFunctionGraphicFilter(int argc, char *argv[])
{
	//src = imread("E://Cpp//0QtWithOpenCv_INTwoWeek//EX9-底噪//平面图联锁表 (2)-1.bmp");
	src = imread("E://Cpp//IMG_0911.JPG");
	Mat dst;

	namedWindow(window_name);
	imshow(window_name, src);

	namedWindow(curves_window);
	setMouseCallback(curves_window, callbackMouseEvent, nullptr );

	GFilter.filterImgProc(src,dst);
	
	invalidate();


	
	//create window
	//	qWarning("This is a warning message.");
	//	qCritical("This is a critical message.");
	//	qFatal("This is a fatal message.");

	//testFuc();
	
}

#endif

*/


//画一条虚线
void dot_line(Mat &mat, Point &p1, Point &p2, Scalar &color, int step = 8);



void ImgProcFilter::drawHist()
{
	double MaxHinst = *max_element(Hist,Hist + Ndepth);
	int HistNormal[Ndepth] = {0};

	
	backRound  = Mat(256,256,CV_8UC3);
	backRound.setTo( back_color );
	


	for (int i = 0 ;i < Ndepth;i++)
	{
		HistNormal[i] = (int)(Hist[i] * 255 / (int)MaxHinst ) ;

		if (HistNormal[i] > 255)
		{
			HistNormal[i] = 255;
		}

		if (HistNormal[i] >= 1)
		{
			cv::line( backRound, Point(i,255), Point(i,255 - HistNormal[i]), Scalar(255,0,0), 1, 8 );
		}
		
	}

	drawBackRound(false);
}

//算大津OTSU算法的最佳适配
double ImgProcFilter::getThreshVal_Otsu_8u( const Mat& _src )
{
	cv::Size size = _src.size();
	if( _src.isContinuous() )
	{
		size.width *= size.height;
		size.height = 1;
	}

	int i = 0, j = 0;

	memset(Hist,0,sizeof(int)*Ndepth);

	for( i = 0; i < size.height; i++ )
	{
		const uchar* src = _src.data + _src.step * i;
		j = 0;

#if CV_ENABLE_UNROLLED
		for( ; j <= size.width - 4; j += 4 )
		{
			int v0 = src[j], v1 = src[j+1];
			Hist[v0]++; Hist[v1]++;
			v0 = src[j+2]; v1 = src[j+3];
			Hist[v0]++; Hist[v1]++;
		}
#endif
		for( ; j < size.width; j++ )
			Hist[src[j]]++;
	}

	double mu = 0, scale = 1./(size.width*size.height);
	for( i = 0; i < Ndepth; i++ )
		mu += i*(double)Hist[i];

	mu *= scale;  
	double mu1 = 0, q1 = 0;
	double max_sigma = 0;
	max_val = 0;
	max_mu1 = 0;
	max_mu2 = 0;
	for( i = 0; i < Ndepth; i++ )
	{
		double p_i = 0, q2 = 0, mu2 = 0, sigma = 0;

		p_i = Hist[i] * scale; 
		mu1 *= q1;      
		q1 += p_i;       
		q2 = 1. - q1;     

		if( min(q1,q2) < FLT_EPSILON || max(q1,q2) > 1. - FLT_EPSILON )
			continue;

		mu1 = (mu1 + i * p_i)/q1;  
		mu2 = (mu - q1 * mu1)/q2;
		sigma = q1 * q2 * (mu1 - mu2) * (mu1 - mu2);
		if( sigma > max_sigma )
		{
			max_sigma = sigma;
			max_val = i;
			max_mu1 = mu1;
			max_mu2 = mu2;
		}
	}

	return max_val;
}

void ImgProcFilter::Otsu_8u_Filter(InputArray src, OutputArray dst)
 {
	 //检查输入是U8C1的
	 Mat source = src.getMat();
	 CV_Assert(source.channels() != 1 || source.depth() != 8);
	
	 //算otsu参数
	 getThreshVal_Otsu_8u(source);
	 drawHist();  //画背景

	 cv::Point Left(0,0),Right(0,0);  //左右点
	 Left.y = 0;
	 Right.y = 255;

	 Left.x  = cvRound((max_mu1 * 2 + max_val) / 3);
	 Right.x = cvRound((max_mu2 + max_val) / 2 );


	 //给兑换表加控制点

	 addPoint(Right);
	 addPoint(Left);

	 adjust(src,dst);
	 
	 drawMat();

 }

void ImgProcFilter::filterImgProc(InputArray src, OutputArray dst)
 {
	 Mat source = src.getMat( );

	 CV_Assert(source.depth( ) != 8);
	 CV_Assert(source.channels( ) != 1 || source.channels( ) != 3);

	 if (source.channels( ) == 3)
	 {
		 Mat hsv;
		 cvtColor(source, hsv, cv::COLOR_BGR2HSV);

		 Mat PicHsv[3];

		 split(hsv, PicHsv);

		// Gsrc = PicHsv[1];
		// Gsrc = 255 - Gsrc; //反色
		gSourceMat = PicHsv[2];

	 }else
	 {

		 gSourceMat = source;
	 }

	 Otsu_8u_Filter(gSourceMat,dst);
 }

ImgProcFilter::ImgProcFilter(void) : Curve()
{
	//ShowEngine = nullptr;

	FilterType = 0;
	max_mu1 = 0;
	max_mu2 = 0;
	max_val = 0;

	for (int i =0;i < Ndepth;i++)
	{
		Hist[i] = 0;
	}

}


ImgProcFilter::~ImgProcFilter(void) 
{


}



void ImgProcFilter::imgProc(cv::InputArray src,cv::OutputArray dst)
{
	filterImgProc(src,dst);
}

void ImgProcFilter::setSource(cv::InputArray src,cv::OutputArray dst)
{
	Mat source = src.getMat( );

	CV_Assert(source.depth( ) != 8);
	CV_Assert(source.channels() != 1 || source.channels() != 3);

	if (source.channels() == 3)
	{
		Mat hsv;
		cvtColor(source, hsv, cv::COLOR_BGR2HSV);

		Mat PicHsv[3];

		split(hsv, PicHsv);

		// Gsrc = PicHsv[1];
		// Gsrc = 255 - Gsrc; //反色
		gSourceMat = PicHsv[2];

	}else
	{

		gSourceMat = source;
	}
	getThreshVal_Otsu_8u(gSourceMat);
	drawHist();  //画背景
	adjust(src,dst);
	drawMat();
}

std::vector<cv::Point> ImgProcFilter::getCtrlPoint()
{
	return points;
}

cv::Mat ImgProcFilter::showMat(void)
{
//	imshow("debug3",curves_mat);

	return curves_mat;
}











#define SWAP(a, b, t)  do { t = a; a = b; b = t; } while(0)

#define CLIP_RANGE(value, min, max)  ( (value) > (max) ? (max) : (((value) < (min)) ? (min) : (value)) )

#define COLOR_RANGE(value)  CLIP_RANGE((value), 0, 255)

#define DEBUG_PRINT(a)  cout << (a) << endl
#define PRINT_VAR(var)  cout << #var << " = " << (var) <<  endl

//样条1,1,1,1
//所有点的x坐标,y坐标,曲线点x数量,t_x曲线点集,z_y曲线点集,曲线点Y数量,输出Y值点集
static double spline(double *x, double *y, int n, double *t, int m, double *z)
{
	double* dy = new double[n];
	memset(dy, 0, sizeof(double) * n);
	dy[0] = -0.5;

	double* ddy = new double[n];
	memset(ddy, 0, sizeof(double) * n);

	double h1;
	double* s = new double[n];
	double h0 = x[1] - x[0];

	s[0] = 3.0 * (y[1] - y[0]) / (2.0 * h0) - ddy[0] * h0 / 4.0;
	for( int j = 1; j <= n - 2; ++j )
	{
		h1 = x[j + 1] - x[j];
		double alpha = h0 / (h0 + h1);
		double beta = (1.0 - alpha) * (y[j] - y[j - 1]) / h0;
		beta = 3.0 * (beta + alpha * ( y[j + 1] - y[j] ) / h1);
		dy[j] = -alpha / (2.0 + (1.0 - alpha) * dy[j - 1]);
		s[j] = (beta - (1.0 - alpha) * s[j - 1]);
		s[j] = s[j] / (2.0 + (1.0 - alpha) * dy[j - 1]);
		h0 = h1;
	}
	dy[n-1] = (3.0*(y[n-1] - y[n-2]) / h1 + ddy[n-1] * h1/2.0 - s[n-2]) / (2.0 + dy[n-2]);

	for( int j = n - 2; j >= 0; --j )
	{
		dy[j] = dy[j] * dy[j + 1] + s[j];
	}

	for( int j = 0; j <= n - 2; ++j )
	{
		s[j] = x[j + 1] - x[j];
	}

	for( int j = 0; j <= n - 2; ++j )
	{
		h1 = s[j] * s[j];
		ddy[j] = 6.0 * (y[j+1] - y[j]) / h1 - 2.0 * (2.0 * dy[j] + dy[j+1]) / s[j];
	}

	h1 = s[n-2] * s[n-2];
	ddy[n-1] = 6.0 * (y[n-2] - y[n-1]) / h1 + 2.0 * (2.0 * dy[n-1] + dy[n-2]) / s[n-2];
	double g = 0.0;
	for(int i=0; i<=n-2; i++)
	{
		h1 = 0.5 * s[i] * (y[i] + y[i+1]);
		h1 = h1 - s[i] * s[i] * s[i] * (ddy[i] + ddy[i+1]) / 24.0;
		g = g + h1;
	}

	for(int j=0; j<=m-1; j++)
	{
		int i;
		if( t[j] >= x[n-1] ) {
			i = n - 2;
		} else {
			i = 0;
			while(t[j] > x[i+1]) {
				i = i + 1;
			}
		}
		h1 = (x[i+1] - t[j]) / s[i];
		h0 = h1 * h1;
		z[j] = (3.0 * h0 - 2.0 * h0 * h1) * y[i];
		z[j] = z[j] + s[i] * (h0 - h0 * h1) * dy[i];
		h1 = (t[j] - x[i]) / s[i];
		h0 = h1 * h1;
		z[j] = z[j] + (3.0 * h0 - 2.0 * h0 * h1) * y[i+1];
		z[j] = z[j] - s[i] * (h0 - h0 * h1) * dy[i+1];
	}

	delete [] s;
	delete [] dy;
	delete [] ddy;

	return(g);
}

#define WITHIN(x1, delta, x2) ( (delta) > 0 ) ? ( (x1) <= (x2) ) : ( (x1) >= (x2) ) //区域内
#define EXCEED(x1, delta, x2) ( (delta) > 0 ) ? ( (x1) >= (x2) ) : ( (x1) <= (x2) ) //外

//画虚线
void dot_line(cv::Mat &mat, const cv::Point &p1, const cv::Point &p2,  const cv::Scalar &color,
	int thickness = 1, int lineType = 8, int line_step = 6, int blank_step = 6 );

void dot_line(cv::Mat &mat, const cv::Point &p1, const cv::Point &p2,  const cv::Scalar &color,
	int thickness, int lineType, int line_step, int blank_step )
{
	if ( p1 == p2 ) return;

	//validate line_step
	line_step = ::abs(line_step);
	if ( line_step == 0 ) line_step = 1;

	//validate blank_step
	blank_step = ::abs(blank_step);
	if ( blank_step == 0 ) blank_step = 1;

	//dot_ratio = blank_step / line_step;
	double dot_ratio = blank_step * 1.0f / line_step;

	//calculat step_x, step_y
	double len = 0, step_x = 0, step_y = 0;
	len = (double)sqrt( (double)(p1.x - p2.x) * (p1.x - p2.x) + (double)(p1.y - p2.y) * (p1.y - p2.y) );
	step_x = (p2.x - p1.x) / len  * line_step;
	step_y = (p2.y - p1.y) / len  * line_step;

	double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	x1 = p1.x;  y1 = p1.y;  //start from Point p1

	//draw line step by step, until meet Point p2
	if ( ::abs(p1.x - p2.x) > ::abs(p1.y - p2.y) ) {
		//step in direction of x-coordination
		while ( WITHIN(x1, step_x, p2.x) ) {
			if ( EXCEED(x1 + step_x * (1 + dot_ratio), step_x, p2.x )) {
				x2 = p2.x;
				y2 = p2.y;
			} else if ( EXCEED(x1 + step_x, step_x, p2.x )) {
				x2 = p2.x;
				y2 = p2.y;
			} else {
				x2 = x1 + step_x;
				y2 = y1 + step_y;
			}
			line(mat, Point(x1, y1), Point(x2, y2), color, thickness, lineType);
			//step
			x1 = x2 + step_x * dot_ratio;
			y1 = y2 + step_y * dot_ratio;
		}

	} else {
		//step in direction of y-coordination
		while ( WITHIN(y1, step_y, p2.y) ) {
			if ( EXCEED(y1 + step_y * (1 + dot_ratio), step_y, p2.y )) {
				x2 = p2.x;
				y2 = p2.y;
			} else if ( EXCEED(y1 + step_y, step_y, p2.y )) {
				x2 = p2.x;
				y2 = p2.y;
			} else {
				x2 = x1 + step_x;
				y2 = y1 + step_y;
			}
			line(mat, Point(x1, y1), Point(x2, y2), color, thickness, lineType);
			//step
			x1 = x2 + step_x * dot_ratio;
			y1 = y2 + step_y * dot_ratio;
		}
	}
}

Curve::Curve()
{
	color = Scalar(0,0,0);
	back_color = Scalar(255,255,255);  //白
	tolerance = 3;
	is_mouse_down = false;
	points.push_back( Point(0, 0) );
	points.push_back( Point(255, 255) );

	current = points.begin();

	curves_mat = Mat::ones(256, 256, CV_8UC3);
	backRound  = Mat(256,256,CV_8UC3);

	drawBackRound();
}

Curve::~Curve()
{
}

std::vector<cv::Point>::iterator  Curve::find(int x)
{
	std::vector<cv::Point>::iterator iter = points.begin();
	for (iter = points.begin(); iter != points.end(); ++iter ) {
		if ( ::abs(iter->x - x ) <= tolerance )
			return iter;
	}
	return points.end();
}

std::vector<cv::Point>::iterator  Curve::find(int x, int y)
{
	std::vector<cv::Point>::iterator iter = points.begin();
	for (iter = points.begin(); iter != points.end(); ++iter ) {
		if ( ::abs(iter->x - x ) <= tolerance && ::abs(iter->y - y ) <= tolerance )
			return iter;
	}
	return points.end();
}

std::vector<cv::Point>::iterator  Curve::add(int x, int y)
{
	std::vector<cv::Point>::iterator it = find(x);
	if ( it == points.end() ) {
		cv::Point p(x, y);
		std::vector<cv::Point>::iterator iter = points.begin();
		for (iter = points.begin(); iter != points.end(); ++iter ) {

			if ( iter == points.begin() && iter->x > p.x) {
				DEBUG_PRINT("points insert at beginning");
				return points.insert( iter, p );
			}

			if ( iter->x < x &&  (iter + 1) != points.end() &&  (iter + 1)->x > p.x) {
				DEBUG_PRINT("points insert");
				return points.insert( iter + 1, p );
			}
		}
		DEBUG_PRINT("points append");
		return points.insert( points.end(), p );
	} else {
		return it;
	}
}

void Curve::createColorTables(uchar colorTables[256])
{
	double z[256];

	calcCurve(z);


	for (int i = 0; i < 256; ++i ) {
		colorTables[i] = z[i];
	}
}

void Curve::drawBackRound(bool drawBackColor)
{
	if (drawBackColor)
	{
		backRound  = Mat(256,256,CV_8UC3);
		backRound.setTo( back_color );
	}
	
	
	//draw lines
	dot_line(backRound, cv::Point( 0, 0), cv::Point( 255, 0), Scalar(0,0,255), 1, 8, 4, 4);
	dot_line(backRound, cv::Point( 0, 255), cv::Point( 255, 255), Scalar(0,0,255), 1, 8, 4, 4);

	//左右边线会挡直方图
	dot_line(backRound, cv::Point( 0, 0), cv::Point( 0, 255), Scalar(0,0,255), 1, 8, 4, 4);    
	dot_line(backRound, cv::Point( 255, 0), cv::Point( 255, 255), Scalar(0,0,255), 1, 8, 4, 4);

	dot_line(backRound, cv::Point(63, 0), cv::Point(63, 255), Scalar(0,0,255), 1, 8, 4, 4);
	dot_line(backRound, cv::Point(127, 0), cv::Point(127, 255), Scalar(0,0,255), 1, 8, 4, 4);
	dot_line(backRound, cv::Point(191, 0), cv::Point(191, 255), Scalar(0,0,255), 1, 8, 4, 4);
	dot_line(backRound, cv::Point(0,  255 - 63), cv::Point(255,  255 - 63), Scalar(0,0,255), 1, 8, 4, 4);
	dot_line(backRound, cv::Point(0, 255 - 127), cv::Point(255, 255 - 127), Scalar(0,0,255), 1, 8, 4, 4);
	dot_line(backRound, cv::Point(0, 255 - 191), cv::Point(255, 255 - 191), Scalar(0,0,255), 1, 8, 4, 4);

}

int Curve::calcCurve(double *output_y)
{
	//控制点小于2，返回直接输出
	if ( points.size() < 2) {
		for (int i = 0; i < 256; ++i )
			output_y[i] = 255 - i;
		return 0;
	}

	//控制点两个,需要补一下非初始起始然后中间线性
	if ( points.size() == 2 ) {
		std::vector<cv::Point>::iterator point1 = points.begin();
		std::vector<cv::Point>::iterator point2 = point1 + 1;

		double delta_y = 0;
		if ( point2->x != point1->x )
			delta_y  = (point2->y - point1->y) * 1.0 / (point2->x - point1->x);

		//左点非线性,线性部分,右点非线性
		for ( int i = 0; i < 256; ++i ) {
			if ( i < point1->x ) { 
				output_y[i] = point1->y;
			} else if ( i >= point1->x && i < point2->x ) {
				output_y[i] = COLOR_RANGE( point1->y + delta_y * (i - point1->x) );
			} else {
				output_y[i] = point2->y;
			}
		}
		return 0;
	}


	//大于2时计算曲线点

	int n = points.size();  //点数

	//算控制点的X坐标和Y坐标
	double *x = new double[ n ];
	double *y = new double[ n ];
	std::vector<cv::Point>::iterator start_point = points.end();
	std::vector<cv::Point>::iterator end_point = points.end();
	std::vector<cv::Point>::iterator iter = points.begin();
	int k = 0;
	for (iter = points.begin(); iter != points.end(); ++iter, ++k ) {
		if ( k == 0 ) start_point = iter;
		x[k] = iter->x - start_point->x;
		y[k] = iter->y;
		end_point = iter;
	}

	//如果起始和终止点混乱了,则不做变化
	if (start_point == points.end() || end_point == points.end() || start_point == end_point) {
		for (int i = 0; i < 256; ++i )
			output_y[i] = 255 - i;
		return 1;
	}

	//计算曲线区间范围
	int m = end_point->x - start_point->x;
	double *t = new double[m];  //曲线范围输出点集X
	double *z = new double[m];  //曲线范围输出点集Y

	//初始化X点集
	for ( int i = 0; i< m; ++i ) {
		t[i] = i;
	}

	//计算各段曲线,Y值结果保存在Z组中
	spline(x, y, n, t, m, z);

	//create output
	for ( int i = 0; i < 256; ++i ) {
		if ( i < start_point->x ) {
			output_y[i] = start_point->y;
		} else if ( i >= start_point->x && i < end_point->x ) {
			output_y[i] = CLIP_RANGE(z[i - start_point->x], 0, 255);
		} else {
			output_y[i] = end_point->y;
		}
	}

	delete [] x;
	delete [] y;
	delete [] t;
	delete [] z;
	return 0;
}

int Curve::calcCurve(uchar *output_y)
{
	//控制点小于2，返回直接输出
	if ( points.size() < 2) {
		for (int i = 0; i < 256; ++i )
			output_y[i] = 255 - i;
		return 0;
	}

	//控制点两个,需要补一下非初始起始然后中间线性
	if ( points.size() == 2 ) {
		std::vector<cv::Point>::iterator point1 = points.begin();
		std::vector<cv::Point>::iterator point2 = point1 + 1;

		double delta_y = 0;
		if ( point2->x != point1->x )
			delta_y  = (point2->y - point1->y) * 1.0 / (point2->x - point1->x);

		//左点非线性,线性部分,右点非线性
		for ( int i = 0; i < 256; ++i ) {
			if ( i < point1->x ) { 
				output_y[i] = point1->y;
			} else if ( i >= point1->x && i < point2->x ) {
				output_y[i] = COLOR_RANGE( point1->y + delta_y * (i - point1->x) );
			} else {
				output_y[i] = point2->y;
			}
		}
		return 0;
	}


	//大于2时计算曲线点

	int n = points.size();  //点数

	//算控制点的X坐标和Y坐标
	double *x = new double[ n ];
	double *y = new double[ n ];
	std::vector<cv::Point>::iterator start_point = points.end();
	std::vector<cv::Point>::iterator end_point = points.end();
	std::vector<cv::Point>::iterator iter = points.begin();
	int k = 0;
	for (iter = points.begin(); iter != points.end(); ++iter, ++k ) {
		if ( k == 0 ) start_point = iter;
		x[k] = iter->x - start_point->x;
		y[k] = iter->y;
		end_point = iter;
	}

	//如果起始和终止点混乱了,则不做变化
	if (start_point == points.end() || end_point == points.end() || start_point == end_point) {
		for (int i = 0; i < 256; ++i )
			output_y[i] = 255 - i;
		return 1;
	}

	//计算曲线区间范围
	int m = end_point->x - start_point->x;
	double *t = new double[m];  //曲线范围输出点集X
	double *z = new double[m];  //曲线范围输出点集Y

	//初始化X点集
	for ( int i = 0; i< m; ++i ) {
		t[i] = i;
	}

	//计算各段曲线,Y值结果保存在Z组中
	spline(x, y, n, t, m, z);

	//create output
	for ( int i = 0; i < 256; ++i ) {
		if ( i < start_point->x ) {
			output_y[i] = start_point->y;
		} else if ( i >= start_point->x && i < end_point->x ) {
			output_y[i] = CLIP_RANGE(z[i - start_point->x], 0, 255);
		} else {
			output_y[i] = end_point->y;
		}
	}

	delete [] x;
	delete [] y;
	delete [] t;
	delete [] z;
	return 0;
}

void Curve::drawMat(Mat &mat)
{
	int thinkness = 1;
	int n = 0;
	cv::Point lastPoint(0,0);

	//clear background

//	

#ifdef _DEBUG
	std::vector<cv::Point>::iterator it = points.begin();
	for (it = points.begin(); it != points.end(); ++it)
	{
		qDebug() << "point:  "<< it->x << ", " << it->y ;
	}
#endif


	//draw lines
	mat = backRound.clone();

	//create curve
	double z[256];
	calcCurve(z);
	for (int i = 1; i < 256; ++i ) {
		cv::line( mat, cv::Point(i-1, 255 - z[i-1]), cv::Point(i, 255 - z[i]), color, 1, 8 );
	}

	//draw control points
	std::vector<cv::Point>::iterator iter = points.begin();
	//std::vector<Point>::iterator iter_next;
	for (iter = points.begin(); iter != points.end(); ++iter, ++n )
	{
		thinkness = (iter == current) ? -1 : 1;   
		cv::rectangle(mat, cv::Point(iter->x - 2, 255 - iter->y + 2),
		cv::Point(iter->x + 2, 255 - iter->y - 2), color, thinkness, 8);
	}

	curves_mat = mat.clone();

//	imshow("debug",mat);
//	imshow("debug2",curves_mat);

}

cv::Mat Curve::drawMat(void)
{
	drawMat(curves_mat);
	return curves_mat;
}

bool Curve::mouseDown(int x, int y)
{
	if (x < 256 && y < 256)
	{
		y = 255 - y;
		current = add( x , y );
		is_mouse_down = true;

		return true;
	}else
	{
		return false;
	}
	
}

bool Curve::mouseMove(int x, int y)
{
	y = 255 - y;
	if ( is_mouse_down ) {
		if (current != points.end()) {
			int prev_x = 0;
			int next_x = 255;

			if (current != points.begin()) {
				int prev_y = (current - 1)->y;
				prev_x = (current - 1)->x;

				//match the previous point
				if ( points.size() > 2 && ::abs(x - prev_x) <= tolerance && ::abs(y - prev_y) <= tolerance ) {
					current--;
					current = points.erase(current);
					DEBUG_PRINT("erase previous");
					return true;
				}

				//if x less than x of previou point
				if ( x <= prev_x) {
					//DEBUG_PRINT("less than prev_x");
					return true;
				}
			}

			if ( ( current + 1) != points.end()) {
				int next_y = (current + 1)->y;
				next_x = (current + 1)->x;

				//match the next point
				if ( points.size() > 2 && ::abs(x - next_x) <= tolerance && ::abs(y - next_y) <= tolerance ) {
					current = points.erase(current);
					DEBUG_PRINT("erase next");
					return true;
				}

				//if x great than x of next point
				if ( x >= next_x) {
					//DEBUG_PRINT("large than next_x");
					return true;
				}
			}

			current->x = CLIP_RANGE(x, 0, 255);
			current->y = CLIP_RANGE(y, 0, 255);

			return true;
		}
	}
	return false;
}

bool Curve::mouseUp(int x, int y)
{

	y = 255 - y;
	is_mouse_down = false;

	return true;
}

void Curve::clearPoints()
{
	points.clear();

	points.push_back( cv::Point(0, 0) );
	points.push_back( cv::Point(255, 255) );

	current = points.begin();

	drawMat();
}

int  Curve::addPoint(const Point &p)
{
	std::vector<cv::Point>::iterator iter = add(p.x, p.y);
	current = iter;
	if ( iter != points.end() )
		return 1;
	else
		return 0;
}

int  Curve::deletePoint(const Point &p)
{
	std::vector<Point>::iterator iter;
	iter = find( p.x, p.y );
	if ( iter != points.end() ) {
		if ( current == iter )
			current = points.end();
		points.erase(iter);
		return 1;
	}
	return 0;
}

int  Curve::movePoint(const Point &p, int x, int y)
{
	std::vector<Point>::iterator iter;
	iter = find( p.x, p.y );
	if ( iter != points.end() ) {
		iter->x = x;
		iter->y = y;
		return 1;
	}
	return 0;
}

int  Curve::adjust(InputArray src, OutputArray dst, InputArray mask /*= noArray()*/)
{
	Mat input = src.getMat();
	if( input.empty( ) ) {
		return -1;
	}

	dst.create(src.size(), src.type());
	Mat output = dst.getMat();

	bool hasMask = true;
	Mat msk = mask.getMat();
	if (msk.empty())
		hasMask = false;

	const uchar *in = nullptr;
	const uchar *pmask = nullptr;
	uchar *out = nullptr;
	int width = input.cols;
	int height = input.rows;

	uchar colorTables[256];

	//createColorTables( colorTables ); //被上面那个一步到位的取代了

	calcCurve(colorTables);
	Mat lut(1,256,CV_8UC1,colorTables);
	//adjust each pixel

	if ( hasMask ) {


		/*
#ifdef _OPENMP
#pragma omp parallel for
#endif*/
		for (int y = 0; y < height; y ++) 
		{

			/*
#ifdef _OPENMP
		uchar *in;
		uchar *pmask;
		uchar *out;
#endif*/
			in = input.ptr<uchar>(y);
			out = output.ptr<uchar>(y);
			pmask = msk.ptr<uchar>(y);
			for (int x = 0; x < width; x ++) 
			{
				*out = (colorTables[*in] * pmask[x] / 255.0) + (*in) * (255 - pmask[x]) / 255.0;
				out++; in++;
			}
		}
	} 
	
	else {

		/*
#ifdef _OPENMP
#pragma omp parallel for
#endif
		// 方法优化 和下面那个玩意计算一致
		for (int y = 0; y < height; y ++) 
		{
#ifdef _OPENMP
	uchar *in;
	uchar *out;
#endif
			in = input.ptr<uchar>(y);
			out = output.ptr<uchar>(y);
			for (int x = 0; x < width; x ++) 
			{
				*out = colorTables[*in];
				out++;in++;
			}
		}*/

		cv::LUT(input,lut,output);  //和上面那个是一样的东西,cv库的优化比Openmp要好 
	}



	return 0;
}

cv::Mat Curve::showMat(void)
{
	return curves_mat;
}



//==========================================================
// Curves
//==========================================================
Curves::Curves()
{
	CurrentChannel = &RGBChannel;
}

Curves::~Curves()
{
}

void Curves::draw(Mat &mat)
{
	if (CurrentChannel)  CurrentChannel->drawMat(mat);
}

void Curves::mouseDown(int x, int y)
{
	if (CurrentChannel)  CurrentChannel->mouseDown(x, y);
}

bool Curves::mouseMove(int x, int y)
{
	if (CurrentChannel)
		return CurrentChannel->mouseMove(x, y);
	return false;
}

void Curves::mouseUp(int x, int y)
{
	if (CurrentChannel)  CurrentChannel->mouseUp(x, y);
}

void Curves::createColorTables(uchar colorTables[][256])
{
	double z[256];

	BlueChannel.calcCurve(z);
	for (int i = 0; i < 256; ++i ) {
		colorTables[0][i] = z[i];
	}

	GreenChannel.calcCurve(z);
	for (int i = 0; i < 256; ++i )
		colorTables[1][i] = z[i];

	RedChannel.calcCurve(z);
	for (int i = 0; i < 256; ++i ) {
		colorTables[2][i] = z[i];
	}

	uchar value = 0;
	RGBChannel.calcCurve(z);
	for (int i = 0; i < 256; ++i ) {
		for (int c = 0; c < 3; c++ ) {
			value = colorTables[c][i];
			colorTables[c][i] = z[value];
		}
	}
}

int Curves::adjust(InputArray src, OutputArray dst, InputArray mask)
{
	Mat input = src.getMat();
	if( input.empty() ) {
		return -1;
	}

	dst.create(src.size(), src.type());
	Mat output = dst.getMat();

	bool hasMask = true;
	Mat msk = mask.getMat();
	if (msk.empty())
		hasMask = false;

	const uchar *in = 0;
	const uchar *pmask = 0;
	uchar *out = 0;
	int width = input.cols;
	int height = input.rows;
	int channels = input.channels();

	uchar colorTables[3][256];

	//create color tables
	createColorTables( colorTables );

	//adjust each pixel

	if ( hasMask ) {
		/*
#ifdef HAVE_OPENMP
#pragma omp parallel for
#endif*/
		for (int y = 0; y < height; y ++) {
/*
#ifdef HAVE_OPENMP
			const uchar *in;
			const uchar *pmask;
			uchar *out;
#endif*/

			in = input.ptr<uchar>(y);
			out = output.ptr<uchar>(y);
			pmask = msk.ptr<uchar>(y);
			for (int x = 0; x < width; x ++) {
				for (int c = 0; c < 3; c++) {
					*out = (colorTables[c][*in] * pmask[x] / 255.0)
						+ (*in) * (255 - pmask[x]) / 255.0;
					out++; in++;
				}
				for (int c = 0; c < channels - 3; c++) {
					*out++ = *in++;
				}
			}
		}
	} else {

		/*
#ifdef HAVE_OPENMP
#pragma omp parallel for
#endif*/
		for (int y = 0; y < height; y ++) {

			/*
#ifdef HAVE_OPENMP //线程变量独立
			const uchar *in;
			uchar *out;
#endif*/
			in = input.ptr<uchar>(y);
			out = output.ptr<uchar>(y);
			for (int x = 0; x < width; x ++) {
				for (int c = 0; c < 3; c++) {
					*out++ = colorTables[c][*in++];
				}
				for (int c = 0; c < channels - 3; c++) {
					*out++ = *in++;
				}
			}
		}
	}

	return 0;
}




