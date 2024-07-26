#include "GuiShowImg.h"
#include <QtGui/QtGui>
#include <opencv.hpp>

GuiShowImg::GuiShowImg()
{
	Ptr_source = nullptr;
	source = cv::Mat::eye(cv::Size(500,500),1);
}


cv::Mat GuiShowImg::drawMat(void)
{
	if (Ptr_source != nullptr)
	{
		return *Ptr_source;
	}else
	{
		return source;
	}
	
}

cv::Mat GuiShowImg::showMat(void)
{
	if (Ptr_source != nullptr)
	{
		return *Ptr_source;
	}else
	{
		return source;
	}
}

void GuiShowImg::setMat(cv::Mat &src)
{
	source = src;
	Ptr_source = nullptr;
}

void GuiShowImg::setIpl(IplImage* src)
{

	/*
	source = cv::Mat(src);

	//source = cv::Mat(src->width,src->height,CV_MAKETYPE(src->depth,src->nChannels),src->imageData);

	Ptr_source = nullptr;*/

	qFatal("x64位不准使用这个接口");

}

GuiShowImg::~GuiShowImg()
{
	//qDebug() << "GUIshowImgR";


}

bool GuiShowImg::mouseDown(int x, int y)
{
	return true;
}

void GuiShowImg::setMatPtr(cv::Mat *src)
{
	Ptr_source = src;
	source = cv::noArray().getMat();
}

