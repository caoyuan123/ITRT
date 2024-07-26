#include "ImgProcCorrect.h"
#include <highgui.h>
#include <opencv.hpp>




//////////////////////////////////////////////////////////////////////

//判断是否在区域中
static bool IntheRect(const cv::Point tarPoint,const cv::Point mousePoint,int radius = 3)
{
	if (radius <= cvSqrt((float)(tarPoint.x - mousePoint.x)*(tarPoint.x - mousePoint.x) + (float)(tarPoint.y - mousePoint.y)*(tarPoint.y - mousePoint.y)))
	{
		return false;
	}
	else
	{
		return true;
	}
}

#define PointDrawCricle(Center,radius) cv::circle(temp,Center,radius,cv::Scalar(0,0,255),2)

//给自由四边形 算最大外接矩形
static cv::Rect CircumscribedRectangle(const cv::Point src[],cv::Point dst[])
{
	dst[0].x = src[0].x;
	dst[0].y = src[0].y;

	dst[1].x = src[1].x;
	dst[1].y = src[1].y;

	dst[2].x = src[2].x;
	dst[2].y = src[2].y;

	dst[3].x = src[3].x;
	dst[3].y = src[3].y;

	if (dst[0].y > dst[1].y) //选小的
	{
		dst[0].y = dst[1].y;
	}else
	{
		dst[1].y = dst[0].y;
	}

	if (dst[0].x > dst[3].x) //选小的
	{
		dst[0].x = dst[3].x;
	}else
	{
		dst[3].x = dst[0].x;
	}

	if (dst[1].x > dst[2].x) //选大的
	{
		dst[2].x = dst[1].x;
	}else
	{
		dst[1].x = dst[2].x;
	}

	if (dst[2].y > dst[3].y) //选大的
	{
		dst[3].y = dst[2].y;
	}else
	{
		dst[2].y = dst[3].y;
	}

	return cv::Rect(dst[0].x,dst[0].y,dst[1].x - dst[0].x,dst[3].y - dst[0].y);
}

void ImgProcCorrect::init(const cv::Mat &outside)
{
	tolerance = 10;
	rudis = 7;

	mouse_PushPoint = -1;
	mouse_PressPoint = -1;

	src = outside;

	int width = src.size().width;
	int height = src.size().height;

	temp = src;

	pointList[0].x = width * 0.1;
	pointList[0].y = height * 0.1;

	pointList[1].x = width * 0.9;
	pointList[1].y = height * 0.1;

	pointList[2].x = width * 0.9;
	pointList[2].y = height * 0.9;

	pointList[3].x = width * 0.1;
	pointList[3].y = height * 0.9;

}

int ImgProcCorrect::Point4PressCheck(cv::Point p[],cv::Point mouseP)
{
	if(IntheRect(p[0],mouseP,tolerance))
	{
		return 0;
	}

	if(IntheRect(p[1],mouseP,tolerance))
	{
		return 1;
	}

	if(IntheRect(p[2],mouseP,tolerance))
	{
		return 2;
	}

	if(IntheRect(p[3],mouseP,tolerance))
	{
		return 3;
	}
	return -1;
}

ImgProcCorrect::ImgProcCorrect(const cv::Mat &outside)
{
	init(outside);
}

ImgProcCorrect::~ImgProcCorrect(void)
{

}

bool ImgProcCorrect::mouseDown(int x, int y)
{
	if (x < 0)
	{
		x = 0;
	}

	if (y < 0)
	{
		y = 0;
	}

	if (x > src.size().width)
	{
		x = src.size().width;
	}

	if (y > src.size().height)
	{
		y = src.size().height;
	}

	mouse_PushPoint = Point4PressCheck(pointList, cv::Point(x,y));
	if (mouse_PushPoint<=3 && mouse_PushPoint>= 0)
	{
		mouse_PressPoint = mouse_PushPoint;

	}
	return true;
}

bool ImgProcCorrect::mouseMove(int x, int y)
{
	if (x < 0)
	{
		x = 0;
	}

	if (y < 0)
	{
		y = 0;
	}

	if (x > src.size().width)
	{
		x = src.size().width;
	}

	if (y > src.size().height)
	{
		y = src.size().height;
	}

	mouse_PushPoint = Point4PressCheck(pointList, cv::Point(x,y));

	if (mouse_PressPoint <=3 && mouse_PressPoint >=0)
	{
		pointList[mouse_PressPoint] = cvPoint(x,y);
	}

	return true;
}

bool ImgProcCorrect::mouseUp(int x, int y)
{
	mouse_PressPoint = 255;
	return true;
}

cv::Mat ImgProcCorrect::drawMat(void)
{
	if (src.channels() == 1)
	{
		cv::cvtColor(src,temp,CV_GRAY2BGR);
	}else
	{
		temp = src.clone();
	}

	//外框 - 绿线
	cv::line(temp,newPoint[0],newPoint[1],cvScalar(0,255,0),2);
	cv::line(temp,newPoint[1],newPoint[2],cvScalar(0,255,0),2);
	cv::line(temp,newPoint[2],newPoint[3],cvScalar(0,255,0),2);
	cv::line(temp,newPoint[3],newPoint[0],cvScalar(0,255,0),2);

	//画上圆圈和线 - 红线
	cv::line(temp,pointList[0],pointList[1],cvScalar(128,128,255),4);
	cv::line(temp,pointList[1],pointList[2],cvScalar(128,128,255),4);
	cv::line(temp,pointList[2],pointList[3],cvScalar(128,128,255),4);
	cv::line(temp,pointList[3],pointList[0],cvScalar(128,128,255),4);  

	if (mouse_PushPoint!=255 && mouse_PushPoint<=3 && mouse_PushPoint >= 0)
	{
		PointDrawCricle(pointList[mouse_PushPoint],rudis);
	}


	return temp;


}

cv::Mat ImgProcCorrect::showMat(void)
{
	return temp;
}

void ImgProcCorrect::imgProc(cv::InputArray input,cv::OutputArray dst)
{
	calPoint4CorrectMat(pointList,src,dst);
}

bool ImgProcCorrect::keyUp(int key,int boardKey)
{
	switch (boardKey)
	{
	case 'C':
		{
			CircumscribedRectangle(pointList,newPoint);
			return true;
		}
		break;
	}

	return false;
}

void ImgProcCorrect::setTolerance(int newT)
{
	tolerance = newT;
}

void ImgProcCorrect::calPoint4CorrectMat(cv::Point p[],cv::InputArray src,cv::OutputArray dst)
{
	cv::Point outPoint[4]; 
	cv::Mat afxControl;    //透射控制矩阵

	cv::Rect cr = CircumscribedRectangle(p,outPoint);

	cv::Mat outputTemp;  //中间存储一下,方便切割

	cv::Point2f fPoint[4];
	cv::Point2f fNewPoint[4];

	for (int i =0;i<=3;i++)
	{
		fPoint[i].x = p[i].x;
		fPoint[i].y = p[i].y;
		fNewPoint[i].x = outPoint[i].x;
		fNewPoint[i].y = outPoint[i].y;
	}

	afxControl = cv::getPerspectiveTransform(fPoint,fNewPoint);

	cv::warpPerspective(src,outputTemp,afxControl,src.size());

	outputTemp(cr).copyTo(dst);
}




