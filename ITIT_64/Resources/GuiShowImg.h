#pragma once
#include <cxcore.h>
#include "IfGUI.h"

//��ʾͼƬ,source����ǳ�����洢
class GuiShowImg :  public IfGUI
{
	cv::Mat *Ptr_source;
	cv::Mat source;
	
public:
	GuiShowImg();
	~GuiShowImg();

	void setMat(cv::Mat &src);
	void setMatPtr(cv::Mat *src);
	void setIpl(IplImage* src);
	virtual cv::Mat drawMat(void);
	virtual cv::Mat showMat(void);

	virtual bool mouseDown(int x, int y);




};