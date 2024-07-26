#pragma once
#include <cxcore.h>
#include <vector>

#include "CvLine.h"

class DrawingSheet;

//线识别

class ImgProcLine
{
private:
	std::vector<CvLine> NearLine(std::vector<CvLine> &Org);  //拟合线的函数
	double NearLine_theta,NearLine_rho;   //上面的参数
	//DrawingSheet *Ptr_DrawingSheet;  //图纸页的指针 最后的操作对象

public:  //以下都是配置参数
	int ThresholdType; //0-canny 1-BININV 
	bool broadPlus; //是否要加边

public:
	int VTar;  //大概竖线有多少个左右
	int HTar;

	double HRoit;     //行占比例
	double VRoit;

	double VAngle1,VAngle2;    //极坐标直线theta角度取值范围
	double HAngle1,HAngle2;

public:
	std::vector<CvLine> VLine; // 识别结果的垂直线
	std::vector<CvLine> HLine; // 识别结果的水平线

	std::vector<CvLine> *Ptr_VLine; // 识别结果的垂直线

public:
	void ImgProc(cv::InputArray _inputArray);  //直接使用指针方式操作,结果外部保存
	void ImgProc(DrawingSheet* Ptr_DwSt);  

	int method;				//方法 //默认 = CV_HOUGH_STANDARD
//	int nearDistence;		//相似的距离就是 最大线宽了

	ImgProcLine(void);
	~ImgProcLine(void);
};






extern int testZonemian(int argc, char *argv[]);