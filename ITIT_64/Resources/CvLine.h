#pragma once
#include "StdAfx.h"
#include <core/core.hpp>
#include <string>


class QLine;
class QLineF;



//�Զ�����߽ṹ
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
	void getPloar(double &rho,double& theta);  //ת����ʽ

	bool isHorizontal();  //�ж�ˮƽ��,ˮƽ��ΪTrue,����ΪFalse

public:
	static  bool isSimilarLine(CvLine Line1,CvLine Line2,double theta = 0.00865,double rho = 20); //�ж��Ƿ���������
	static  void Descartes2Polar(CvLine src,double &rho,double &theta);  //�ѿ���ת����
	static  CvLine Polar2Descartes(const CvSize &Src, double rho, double theta);  //����ת�ѿ���
	static inline void PolarPostiveRho(double &rho,double &theta); //��������ʽ��rho����Ϊ����
	static inline double diffAngleTheta(const double &Theta1,const double &Theta2);  //��������ֱ�߽ǵ���С�н�

	//�Ƚϵ��ߵ��������������Ƚϵ�����Ҫ �нǴ�����ȣ����߲��ܱȽ�Y�����߲��ܱȽ�X
	static bool CompareY(const CvLine &L1,const CvLine &L2);
	static bool CompareX(const CvLine &L1,const CvLine &L2);

public:
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;

};



extern void read(const cv::FileNode& node, CvLine& x,const CvLine& default_value = CvLine());
extern void write(cv::FileStorage& fs, const std::string& name, const CvLine& x);





