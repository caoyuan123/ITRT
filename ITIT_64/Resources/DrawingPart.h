#pragma once
#include <cxcore.hpp>
#include <string>
#include "DrawingDef.h"


//����в�������
class DrawingPart  : public DrawingAbstruct
{
protected:
	int m_step ;     //���� - 0�ոճ�ʼ����� -1����ԭͼ��� 2-��ɫ������� 4-������� 8-�˲���� 
public:
	DrawingSheet *Parent_Ds; //�Լ�����һ��������˭
	int PartNum;						 //���ֵ�����0-��ʾ����֣�����1-2-3����ͼ��ʼ���
	std::string StorageName;					 //�洢��ʵ���ļ�����

	std::vector<cv::Point> fltPoints;						 //�˲�������������Mini-Max-Mid��ȡֵ��Χ��Ϊ0-255(�˲�������д��)
	cv::Point effectiveArea[4];			 //�����ĵ����ת�������Σ���Ч����ֻ�



	cv::Mat matOrg;		//ԭʼͼƬ
	cv::Mat matCorrect; //�������ͼƬ
	cv::Mat matFlt;		//�˲����ͼƬ
	cv::Mat matFinal;   //���Ľ��
	
public:
	DrawingPart(DrawingSheet* Parent = nullptr);
	void setOrgMat(const cv::Mat& _inputArray,std::string Name,int _PartNum); //����ԭʼͼ��
	

	cv::Mat getFinalMat(int step = 0);   //�õ����������ͼ��
	int getStep();           //�õ�������

	bool isReadMat();   //��ͼ��ô��
	bool isFilter();    //�˲�����ô��
	bool isCorrect();   //��������ô��
	bool isColored();   //ԭͼ�Ĳ�ɫת������ô��



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
