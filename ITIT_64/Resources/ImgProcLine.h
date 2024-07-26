#pragma once
#include <cxcore.h>
#include <vector>

#include "CvLine.h"

class DrawingSheet;

//��ʶ��

class ImgProcLine
{
private:
	std::vector<CvLine> NearLine(std::vector<CvLine> &Org);  //����ߵĺ���
	double NearLine_theta,NearLine_rho;   //����Ĳ���
	//DrawingSheet *Ptr_DrawingSheet;  //ͼֽҳ��ָ�� ���Ĳ�������

public:  //���¶������ò���
	int ThresholdType; //0-canny 1-BININV 
	bool broadPlus; //�Ƿ�Ҫ�ӱ�

public:
	int VTar;  //��������ж��ٸ�����
	int HTar;

	double HRoit;     //��ռ����
	double VRoit;

	double VAngle1,VAngle2;    //������ֱ��theta�Ƕ�ȡֵ��Χ
	double HAngle1,HAngle2;

public:
	std::vector<CvLine> VLine; // ʶ�����Ĵ�ֱ��
	std::vector<CvLine> HLine; // ʶ������ˮƽ��

	std::vector<CvLine> *Ptr_VLine; // ʶ�����Ĵ�ֱ��

public:
	void ImgProc(cv::InputArray _inputArray);  //ֱ��ʹ��ָ�뷽ʽ����,����ⲿ����
	void ImgProc(DrawingSheet* Ptr_DwSt);  

	int method;				//���� //Ĭ�� = CV_HOUGH_STANDARD
//	int nearDistence;		//���Ƶľ������ ����߿���

	ImgProcLine(void);
	~ImgProcLine(void);
};






extern int testZonemian(int argc, char *argv[]);