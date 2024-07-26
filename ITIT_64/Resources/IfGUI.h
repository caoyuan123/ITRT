#pragma once

#include <core/core.hpp>

class QWidget;

//GUI与CV的接口大虚类
class IfGUI
{
public:
	virtual bool keyDown(int key,int boardKey) ; // 键盘按下,组合按键+普通按键
	virtual bool keyUp(int key,int boardKey) ;   // 键盘抬起,

	virtual bool mouseDown(int x, int y); //左键按下
	virtual bool mouseMove(int x, int y); //左键移动
	virtual bool mouseUp(int x, int y);   //左键抬起

	virtual void    drawMat(cv::Mat& src);  //画在提供的图片上

	//************************************
	// Method:    drawMat - 画在自己准备的画布上,然后返回图片。触发重绘-然后返回画图
	// FullName:  IfGUI::drawMat
	// Access:    virtual public 
	// Returns:   cv::Mat
	// Qualifier: xdy
	// Parameter: 
	//************************************
	virtual cv::Mat drawMat(void) = 0;			//自己画然后返回一个纯新的,必须有这个方法

	//************************************
	// Method:    ShowMat - 返回自己的画的图片，只返回图片不触发重绘
	// FullName:  IfGUI::getShowMat
	// Access:    virtual public 
	// Returns:   cv::Mat
	// Qualifier:
	// Parameter: void
	//************************************
	virtual cv::Mat showMat(void) =0;        //返回显示图像,必须有这个方法

	virtual void imgProc(cv::InputArray src,cv::OutputArray dst);  //图像处理接口



    virtual ~IfGUI();
};