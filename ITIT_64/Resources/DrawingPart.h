#pragma once
#include <cxcore.hpp>
#include <string>
#include "DrawingDef.h"


//如果有拆分输入的
class DrawingPart  : public DrawingAbstruct
{
protected:
	int m_step ;     //步骤 - 0刚刚初始化完成 -1读入原图完毕 2-彩色分类完成 4-矫正完成 8-滤波完成 
public:
	DrawingSheet *Parent_Ds; //自己的上一级从属是谁
	int PartNum;						 //部分的名称0-表示不拆分，数字1-2-3从左图开始拆分
	std::string StorageName;					 //存储的实际文件名称

	std::vector<cv::Point> fltPoints;						 //滤波器的三个参数Mini-Max-Mid。取值范围均为0-255(滤波器部分写入)
	cv::Point effectiveArea[4];			 //自由四点的旋转矫正矩形，有效区域分划



	cv::Mat matOrg;		//原始图片
	cv::Mat matCorrect; //矫正后的图片
	cv::Mat matFlt;		//滤波后的图片
	cv::Mat matFinal;   //最后的结果
	
public:
	DrawingPart(DrawingSheet* Parent = nullptr);
	void setOrgMat(const cv::Mat& _inputArray,std::string Name,int _PartNum); //设置原始图像
	

	cv::Mat getFinalMat(int step = 0);   //得到处理到结果的图像
	int getStep();           //得到处理步骤

	bool isReadMat();   //读图了么？
	bool isFilter();    //滤波做了么？
	bool isCorrect();   //矫正做了么？
	bool isColored();   //原图的彩色转换做了么？



public:
	virtual cv::Mat getMat(int step = 0);
	virtual DrawingBook* getDrawingBook();
	virtual DrawingSheet* getDrawingSheet();
	virtual DrawingType* getDrawingType();
	virtual unsigned char getDrawingTableType();



	virtual void setImgProcFlt(cv::InputArray _Flt);
	virtual void setImgProcCorrect(CvPoint Area[4],cv::InputArray _Crt);
	virtual void setImgProcCorrect(cv::Point Area[4],cv::InputArray _Crt);
	virtual void setImgProcFilter(std::vector<cv::Point> _fltPoints ,cv::InputArray _Crt);

public:
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;


};


extern void read(const cv::FileNode& node, DrawingPart& x,const DrawingPart& default_value = DrawingPart(nullptr));

extern void write(cv::FileStorage& fs, const std::string& name, const DrawingPart& x);
