#pragma once
#include "IfGUI.h"
#include <cv.h>

class DrawingAbstruct;



//ͼ��ѡ����� cv::Mat �汾
class ImgProcCorrect : virtual public IfGUI
{
	void init(const cv::Mat &outside);

protected:
	int tolerance;    //��갴�»��ƶ�ʱ���������ߵ����Χ
	int rudis;


	cv::Point newPoint[4];    //��Ӿ��� - ����

	

	cv::Mat src;     //����Դͼ
	cv::Mat dst;     //�����Ľ��ͼ
	cv::Mat temp;    //�м���̱���ͼ-���ֻ���ͷ��������

	

	int Point4PressCheck(cv::Point p[],cv::Point mouseP);  //�ĸ�������Ƿ񾭹����
	 
	static void calPoint4CorrectMat(cv::Point p[],cv::InputArray input,cv::OutputArray output); //��������㷨

	int mouse_PushPoint;            //��갴ס�ĵ�
	int mouse_PressPoint;           //��꾭���ĵ�

	            


public:
	ImgProcCorrect(const cv::Mat &outside);
	~ImgProcCorrect(void);

	cv::Point pointList[4]; //0-����,1-����,2-����,3-����  ����

	virtual bool mouseDown(int x, int y);
	virtual bool mouseMove(int x, int y);
	virtual bool mouseUp(int x, int y);
	virtual cv::Mat drawMat(void);
	virtual cv::Mat showMat(void);
	virtual void imgProc(cv::InputArray src,cv::OutputArray dst);
	virtual bool keyUp(int key,int boardKey);

	void setTolerance(int newT);
};

