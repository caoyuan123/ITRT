#pragma once

#include <core/core.hpp>

class QWidget;

//GUI��CV�Ľӿڴ�����
class IfGUI
{
public:
	virtual bool keyDown(int key,int boardKey) ; // ���̰���,��ϰ���+��ͨ����
	virtual bool keyUp(int key,int boardKey) ;   // ����̧��,

	virtual bool mouseDown(int x, int y); //�������
	virtual bool mouseMove(int x, int y); //����ƶ�
	virtual bool mouseUp(int x, int y);   //���̧��

	virtual void    drawMat(cv::Mat& src);  //�����ṩ��ͼƬ��

	//************************************
	// Method:    drawMat - �����Լ�׼���Ļ�����,Ȼ�󷵻�ͼƬ�������ػ�-Ȼ�󷵻ػ�ͼ
	// FullName:  IfGUI::drawMat
	// Access:    virtual public 
	// Returns:   cv::Mat
	// Qualifier: xdy
	// Parameter: 
	//************************************
	virtual cv::Mat drawMat(void) = 0;			//�Լ���Ȼ�󷵻�һ�����µ�,�������������

	//************************************
	// Method:    ShowMat - �����Լ��Ļ���ͼƬ��ֻ����ͼƬ�������ػ�
	// FullName:  IfGUI::getShowMat
	// Access:    virtual public 
	// Returns:   cv::Mat
	// Qualifier:
	// Parameter: void
	//************************************
	virtual cv::Mat showMat(void) =0;        //������ʾͼ��,�������������

	virtual void imgProc(cv::InputArray src,cv::OutputArray dst);  //ͼ����ӿ�



    virtual ~IfGUI();
};