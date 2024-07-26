#include "IfGUI.h"
#include <qwidget.h>



bool IfGUI::keyDown(int key,int boardKey)
{
	return false;
}

bool IfGUI::keyUp(int key,int boardKey)
{
	return false;
}



bool IfGUI::mouseDown(int x, int y)
{
	return false;
}

bool IfGUI::mouseMove(int x, int y)
{
	return false;
}

bool IfGUI::mouseUp(int x, int y)
{
	return false;
}

cv::Mat IfGUI::drawMat()
{
	return cv::noArray().getMat();
}

void IfGUI::drawMat(cv::Mat& src)
{
	src = cv::noArray().getMat();
}

void IfGUI::imgProc(cv::InputArray src,cv::OutputArray dst)
{

}


IfGUI::~IfGUI()
{

}


