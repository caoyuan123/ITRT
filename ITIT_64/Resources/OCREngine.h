#pragma once
#include <cxcore.hpp>
//
#include <vector>
#include "StdAfx.h"



//OCR����
class CBI_TableDictionary;
class DrawingZone;
class DrawingSheet;
class DrawingType;
class OCR_ResultOrlWord;
class OCRAlternativePoint;
class OCRLine;
class FontPicture;

struct OCREngineBase
{
private:
	int s_adjWordX; //�ּ���X����
	int s_adjLineY; //�м���Y����,�ж�ͬ��ʹ��

	int s_adjPointX; // ������̶�
	int s_adjPointY; // ������̶�

	float minThreshold;  //�����ֵ

	int stdWidth;   //��׼�ֿ� ��{���ֿ�
	int stdHight;   //��׼�ָ� ��{���ָߣ�

	float stdThreshold;  //��׼��ϵ���ֵ ��Ҫ�����ȡ
	/*
	bool slide_bl_Threshold;  //ʹ�û�����ֵ
	float slide_Step; //��������
	int slide_Target;  //����Ŀ�� һ����100����
	*/


public:
	void setStdWordSize(int _width,int _hight);  //���ñ�׼�ִ�С
	void setStdThreshold(float _float32);  //��׼��ֵ�Ǳ���϶������ֵ

    void setSetting(const DrawingType&);

	float getStdThreshold( ); 
	
	int getAdjPointX( ); //ͬ���X����  
	int getAdjPointY( ); //ͬ���Y����  ��ͬ�����ʹ��

	int getAdjWordX( );  //�ּ���X���� , ͬRect��ʹ��
	int getAdjLineY( );  //�м���Y����  , ͬ�м���ʹ��

	float getMinThreshold(int num = 0 );  //�����ֵ
	
	OCREngineBase( );
	OCREngineBase(int W, int H, float _Threshold);  //�������Ը߿�����ָ��
};

class OCREngineThread;



class IfOCREngine
{
public:
	virtual void OCRDrawingSheet(DrawingSheet &Sheet) = 0;

	virtual void setSetting(const OCREngineBase& _set) = 0;
	virtual void setSetting(const DrawingType&) = 0;

	virtual void setStdWordSize(int _width,int _hight) = 0;  //���ñ�׼�ִ�С
	virtual void setStdThreshold(float _float32) = 0;  //��׼��ֵ�Ǳ���϶������ֵ 
	
	virtual void setOCREnginThread(OCREngineThread * _ptr = nullptr) = 0;

	virtual int calTotalNums(DrawingSheet &Sheet) = 0;   //�����Լ�ж��ټ�������
};



class OCREngineCv : public IfOCREngine , OCREngineBase
{
	OCREngineThread * ptr_thread;

//	CBI_TableDictionary* ptr_Dictionary;

protected:
	void OCRMainEngine(std::vector<FontPicture*> FontPictures,const cv::Mat &_mat,std::vector<OCR_ResultOrlWord*> &result,DrawingZone* _Dz);
	void DrawShowPicture(std::vector<OCR_ResultOrlWord*> &Orl_result,const cv::Mat &src,cv::Mat &_Mat);  //�����õĻ滭ʾ��ͼ
	void DrawShowPicture(std::vector<OCRAlternativePoint*> &Orl_result,cv::Mat &src,cv::Mat &_Mat);
	
	static void DrawShowPicture(std::vector<OCR_ResultOrlWord*> &Orl_result,cv::Mat &_Mat);


	//��ԭʼ��ת���ɴ�ѡ�㡣�ֳƷ���
	void MergeSameRect(const std::vector<OCR_ResultOrlWord*> &Orl_result,std::vector<OCRAlternativePoint*> &AlternativeWords); 
	 //�ϲ���һ�е���
	void MergeIntoLine(const std::vector<OCRAlternativePoint*> &AlternativeWords,std::vector<OCRLine*> &_OCRLine); 
	//ɾ�������ཻ�ĵ�
	void DeleteCrossLineRect(std::vector<OCR_ResultOrlWord*> &Orl_result);


public:
	OCREngineCv(const OCREngineBase& _set);
	OCREngineCv(const DrawingType&);
	~OCREngineCv(void);

	 virtual void setSetting(const OCREngineBase& _set);

	 virtual void setSetting(const DrawingType&);

	 virtual void setStdWordSize(int _width,int _hight);
	 virtual void setStdThreshold(float _float32);

	 virtual void setOCREnginThread(OCREngineThread * _ptr = nullptr);

public:  //�Բ������̵Ľӿ�
	 virtual void OCRDrawingSheet(DrawingSheet &Sheet);
	 virtual int OCRDrawingZone(DrawingZone &Zone,bool calNums = false);  //��̬��������,�����Ǽ���ģʽ
	 virtual int calTotalNums(DrawingSheet &Sheet);   //�����Լ�ж��ټ�������


	//ͼ�����������,1.�����б�,������Ķ���ͼ,���-δ����
	
	  int isRectInclude(cv::Rect &_Rect1,cv::Rect &_Rect2,int adjust = 2,bool jdL = false);

};






