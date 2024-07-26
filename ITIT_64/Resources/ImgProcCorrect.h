#pragma once
#include "IfGUI.h"
#include <cv.h>

class DrawingAbstruct;



//图像选择矫正 cv::Mat 版本
class ImgProcCorrect : virtual public IfGUI
{
	void init(const cv::Mat &outside);

protected:
	int tolerance;    //鼠标按下或移动时，捕获曲线点的误差范围
	int rudis;


	cv::Point newPoint[4];    //外接矩形 - 绿线

	

	cv::Mat src;     //输入源图
	cv::Mat dst;     //处理后的结果图
	cv::Mat temp;    //中间过程保存图-最后只会释放这个东西

	

	int Point4PressCheck(cv::Point p[],cv::Point mouseP);  //四个点鼠标是否经过检查
	 
	static void calPoint4CorrectMat(cv::Point p[],cv::InputArray input,cv::OutputArray output); //计算的主算法

	int mouse_PushPoint;            //鼠标按住的点
	int mouse_PressPoint;           //鼠标经过的点

	            


public:
	ImgProcCorrect(const cv::Mat &outside);
	~ImgProcCorrect(void);

	cv::Point pointList[4]; //0-左上,1-右上,2-右下,3-左下  红线

	virtual bool mouseDown(int x, int y);
	virtual bool mouseMove(int x, int y);
	virtual bool mouseUp(int x, int y);
	virtual cv::Mat drawMat(void);
	virtual cv::Mat showMat(void);
	virtual void imgProc(cv::InputArray src,cv::OutputArray dst);
	virtual bool keyUp(int key,int boardKey);

	void setTolerance(int newT);
};

