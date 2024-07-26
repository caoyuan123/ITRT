#pragma once
#include<cxcore.hpp>
#include <vector>
#include "IfGUI.h"




//单个通道的曲线调整
class Curve :  virtual public IfGUI
{

protected:
	cv::Scalar color;
	cv::Scalar back_color;
	int tolerance;    //鼠标按下或移动时，捕获曲线点的误差范围
	bool is_mouse_down;
	std::vector<cv::Point> points;  //control points 曲线的所有控制点
	std::vector<cv::Point>::iterator current;  //pointer to current point 当前控制点的指针

	std::vector<cv::Point>::iterator  find(int x);
	std::vector<cv::Point>::iterator  find(int x, int y);
	std::vector<cv::Point>::iterator  add(int x, int y);

	void createColorTables(uchar colorTables[256]);  //生成图像对换矩阵
	cv::Mat backRound;                                   //背景格子图

protected:
	void drawBackRound(bool drawBackColor = true);  //画点格线的背景图,是否重画背景白颜色

public:
	cv::Mat curves_mat;            //曲线图,有交互的那个

public:
	Curve();
	virtual ~Curve();

	int calcCurve(double *z); //供内部调用的方法：计算曲线
	int calcCurve(uchar *z);  //供内部调用的方法：计算曲线

	
	virtual void drawMat(cv::Mat &mat);    //画调整曲线

	virtual cv::Mat drawMat(void);

	virtual bool mouseDown(int x, int y);  //当鼠标按下，请调用mouseDown()方法
	virtual bool mouseMove(int x, int y);  //当鼠标移动，请调用mouseMove()方法
	virtual bool mouseUp(int x, int y);    //当鼠标抬起，请调用mouseUp()方法


	//以下方法用于：用编程方式生成曲线
	void clearPoints(); //清除曲线上所有的点
	int	 addPoint(const cv::Point &p); //增加一个点
	int  deletePoint(const cv::Point &p); //删除一个点
	int  movePoint(const cv::Point &p, int x, int y); //移动一个点

	//实施曲线调整
	int adjust(cv::InputArray src,cv:: OutputArray dst,cv:: InputArray mask = cv::noArray());

	virtual cv::Mat showMat(void);


};

//三通道调整函数
class Curves {
protected:
	void createColorTables(uchar colorTables[][256]);
public:
	Curves( );
	virtual ~Curves();

	Curve RGBChannel;   //RGB总通道
	Curve RedChannel;   //Red通道
	Curve GreenChannel; //Green通道
	Curve BlueChannel;  //Blue通道

	Curve * CurrentChannel; //当前通道的指针

	void draw(cv::Mat &mat);  //将曲线画在mat上
	void mouseDown(int x, int y); //当鼠标按下，请调用mouseDown()方法
	bool mouseMove(int x, int y); //当鼠标移动，请调用mouseMove()方法
	void mouseUp(int x, int y); //当鼠标抬起，请调用mouseUp()方法

	//实施曲线调整
	int adjust(cv::InputArray src, cv::OutputArray dst, cv::InputArray mask = cv::noArray());

};


//这货是完整的自动处理的滤波器
class ImgProcFilter :public Curve , virtual public IfGUI
{


protected: 
	static const int Ndepth = 256;  //图像位深
	int Hist[Ndepth];               //频度直方图

	//QWidget* ShowEngine;     //显示引擎

protected: 
	//画背景的直方图
	void drawHist();  

public:
	cv::Mat gSourceMat;      //灰色原图
	
	int FilterType;          //彩色滤波方式
	double max_mu1,max_mu2;   //大津参数的前平均和后平均
	double max_val;           //大津参数的最佳值

public:
	ImgProcFilter(void);
	~ImgProcFilter(void);

	//算大津最合参数
	double getThreshVal_Otsu_8u(const cv::Mat& _src);  

	//完整的滤波器实现
	void Otsu_8u_Filter(cv::InputArray src,cv:: OutputArray dst); 
	

	//完整的滤波器带输入判断,输入必须8u 可以是彩色,和下面的一样
	void filterImgProc(cv::InputArray src, cv::OutputArray dst); 

	//对外的操作接口
	virtual void imgProc(cv::InputArray src,cv::OutputArray dst);

	virtual cv::Mat showMat(void);

	//持续处理接口
	void setSource(cv::InputArray src,cv::OutputArray dst);

	std::vector<cv::Point> getCtrlPoint();
};

//单独测试这一个玩意的测试函数
extern void TestFunctionGraphicFilter(int argc, char *argv[]);




