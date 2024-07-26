#pragma once
#include "StdAfx.h"
#include <core/core.hpp>
#include <string>


class QLine;
class QLineF;



//自定义的线结构
struct CvLine
{
public:
	bool checked;

	CvPoint Point1;
	CvPoint Point2;

public:
	CvLine();
	CvLine(const CvPoint &p1,const CvPoint &p2);
	CvLine(const int p1x,const int p1y,const int p2x,const int p2y);
	CvLine(const QLine& line);
	CvLine(const QLineF& line);
	CvLine(const CvLine& line);
	CvLine(const CvSize &Src,const double &rho,const double &theta);

	QLine getLine(void);
	void getPloar(double &rho,double& theta);  //转极轴式

	bool isHorizontal();  //判断水平线,水平线为True,竖线为False

public:
	static  bool isSimilarLine(CvLine Line1,CvLine Line2,double theta = 0.00865,double rho = 20); //判断是否是相似线
	static  void Descartes2Polar(CvLine src,double &rho,double &theta);  //笛卡尔转极轴
	static  CvLine Polar2Descartes(const CvSize &Src, double rho, double theta);  //极轴转笛卡尔
	static inline void PolarPostiveRho(double &rho,double &theta); //将极轴形式的rho调整为正的
	static inline double diffAngleTheta(const double &Theta1,const double &Theta2);  //给出两个直线角的最小夹角

	//比较的线的两个函数，被比较的线需要 夹角大致相等，竖线不能比较Y，横线不能比较X
	static bool CompareY(const CvLine &L1,const CvLine &L2);
	static bool CompareX(const CvLine &L1,const CvLine &L2);

public:
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;

};



extern void read(const cv::FileNode& node, CvLine& x,const CvLine& default_value = CvLine());
extern void write(cv::FileStorage& fs, const std::string& name, const CvLine& x);





