#pragma once
#include<cxcore.hpp>
#include <vector>
#include "IfGUI.h"




//����ͨ�������ߵ���
class Curve :  virtual public IfGUI
{

protected:
	cv::Scalar color;
	cv::Scalar back_color;
	int tolerance;    //��갴�»��ƶ�ʱ���������ߵ����Χ
	bool is_mouse_down;
	std::vector<cv::Point> points;  //control points ���ߵ����п��Ƶ�
	std::vector<cv::Point>::iterator current;  //pointer to current point ��ǰ���Ƶ��ָ��

	std::vector<cv::Point>::iterator  find(int x);
	std::vector<cv::Point>::iterator  find(int x, int y);
	std::vector<cv::Point>::iterator  add(int x, int y);

	void createColorTables(uchar colorTables[256]);  //����ͼ��Ի�����
	cv::Mat backRound;                                   //��������ͼ

protected:
	void drawBackRound(bool drawBackColor = true);  //������ߵı���ͼ,�Ƿ��ػ���������ɫ

public:
	cv::Mat curves_mat;            //����ͼ,�н������Ǹ�

public:
	Curve();
	virtual ~Curve();

	int calcCurve(double *z); //���ڲ����õķ�������������
	int calcCurve(uchar *z);  //���ڲ����õķ�������������

	
	virtual void drawMat(cv::Mat &mat);    //����������

	virtual cv::Mat drawMat(void);

	virtual bool mouseDown(int x, int y);  //����갴�£������mouseDown()����
	virtual bool mouseMove(int x, int y);  //������ƶ��������mouseMove()����
	virtual bool mouseUp(int x, int y);    //�����̧�������mouseUp()����


	//���·������ڣ��ñ�̷�ʽ��������
	void clearPoints(); //������������еĵ�
	int	 addPoint(const cv::Point &p); //����һ����
	int  deletePoint(const cv::Point &p); //ɾ��һ����
	int  movePoint(const cv::Point &p, int x, int y); //�ƶ�һ����

	//ʵʩ���ߵ���
	int adjust(cv::InputArray src,cv:: OutputArray dst,cv:: InputArray mask = cv::noArray());

	virtual cv::Mat showMat(void);


};

//��ͨ����������
class Curves {
protected:
	void createColorTables(uchar colorTables[][256]);
public:
	Curves( );
	virtual ~Curves();

	Curve RGBChannel;   //RGB��ͨ��
	Curve RedChannel;   //Redͨ��
	Curve GreenChannel; //Greenͨ��
	Curve BlueChannel;  //Blueͨ��

	Curve * CurrentChannel; //��ǰͨ����ָ��

	void draw(cv::Mat &mat);  //�����߻���mat��
	void mouseDown(int x, int y); //����갴�£������mouseDown()����
	bool mouseMove(int x, int y); //������ƶ��������mouseMove()����
	void mouseUp(int x, int y); //�����̧�������mouseUp()����

	//ʵʩ���ߵ���
	int adjust(cv::InputArray src, cv::OutputArray dst, cv::InputArray mask = cv::noArray());

};


//������������Զ�������˲���
class ImgProcFilter :public Curve , virtual public IfGUI
{


protected: 
	static const int Ndepth = 256;  //ͼ��λ��
	int Hist[Ndepth];               //Ƶ��ֱ��ͼ

	//QWidget* ShowEngine;     //��ʾ����

protected: 
	//��������ֱ��ͼ
	void drawHist();  

public:
	cv::Mat gSourceMat;      //��ɫԭͼ
	
	int FilterType;          //��ɫ�˲���ʽ
	double max_mu1,max_mu2;   //��������ǰƽ���ͺ�ƽ��
	double max_val;           //�����������ֵ

public:
	ImgProcFilter(void);
	~ImgProcFilter(void);

	//������ϲ���
	double getThreshVal_Otsu_8u(const cv::Mat& _src);  

	//�������˲���ʵ��
	void Otsu_8u_Filter(cv::InputArray src,cv:: OutputArray dst); 
	

	//�������˲����������ж�,�������8u �����ǲ�ɫ,�������һ��
	void filterImgProc(cv::InputArray src, cv::OutputArray dst); 

	//����Ĳ����ӿ�
	virtual void imgProc(cv::InputArray src,cv::OutputArray dst);

	virtual cv::Mat showMat(void);

	//��������ӿ�
	void setSource(cv::InputArray src,cv::OutputArray dst);

	std::vector<cv::Point> getCtrlPoint();
};

//����������һ������Ĳ��Ժ���
extern void TestFunctionGraphicFilter(int argc, char *argv[]);




