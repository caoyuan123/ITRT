#pragma once
#include <cxcore.hpp>
//
#include <vector>
#include "StdAfx.h"



//OCR引擎
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
	int s_adjWordX; //字间距的X容忍
	int s_adjLineY; //行间距的Y容忍,判定同行使用

	int s_adjPointX; // 点的容忍度
	int s_adjPointY; // 点的容忍度

	float minThreshold;  //最低阈值

	int stdWidth;   //标准字宽 （{的字宽）
	int stdHight;   //标准字高 （{的字高）

	float stdThreshold;  //标准拟合的阈值 需要输入获取
	/*
	bool slide_bl_Threshold;  //使用滑动阈值
	float slide_Step; //滑动步长
	int slide_Target;  //滑动目标 一般在100以下
	*/


public:
	void setStdWordSize(int _width,int _hight);  //设置标准字大小
	void setStdThreshold(float _float32);  //标准阈值是被拟合对象的阈值

    void setSetting(const DrawingType&);

	float getStdThreshold( ); 
	
	int getAdjPointX( ); //同点的X容忍  
	int getAdjPointY( ); //同点的Y容忍  ，同点检索使用

	int getAdjWordX( );  //字间距的X容忍 , 同Rect字使用
	int getAdjLineY( );  //行间距的Y容忍  , 同行检索使用

	float getMinThreshold(int num = 0 );  //最低阈值
	
	OCREngineBase( );
	OCREngineBase(int W, int H, float _Threshold);  //基础的自高宽必须给指定
};

class OCREngineThread;



class IfOCREngine
{
public:
	virtual void OCRDrawingSheet(DrawingSheet &Sheet) = 0;

	virtual void setSetting(const OCREngineBase& _set) = 0;
	virtual void setSetting(const DrawingType&) = 0;

	virtual void setStdWordSize(int _width,int _hight) = 0;  //设置标准字大小
	virtual void setStdThreshold(float _float32) = 0;  //标准阈值是被拟合对象的阈值 
	
	virtual void setOCREnginThread(OCREngineThread * _ptr = nullptr) = 0;

	virtual int calTotalNums(DrawingSheet &Sheet) = 0;   //计算大约有多少计算任务
};



class OCREngineCv : public IfOCREngine , OCREngineBase
{
	OCREngineThread * ptr_thread;

//	CBI_TableDictionary* ptr_Dictionary;

protected:
	void OCRMainEngine(std::vector<FontPicture*> FontPictures,const cv::Mat &_mat,std::vector<OCR_ResultOrlWord*> &result,DrawingZone* _Dz);
	void DrawShowPicture(std::vector<OCR_ResultOrlWord*> &Orl_result,const cv::Mat &src,cv::Mat &_Mat);  //调试用的绘画示意图
	void DrawShowPicture(std::vector<OCRAlternativePoint*> &Orl_result,cv::Mat &src,cv::Mat &_Mat);
	
	static void DrawShowPicture(std::vector<OCR_ResultOrlWord*> &Orl_result,cv::Mat &_Mat);


	//将原始点转换成待选点。又称翻表
	void MergeSameRect(const std::vector<OCR_ResultOrlWord*> &Orl_result,std::vector<OCRAlternativePoint*> &AlternativeWords); 
	 //合并到一行当中
	void MergeIntoLine(const std::vector<OCRAlternativePoint*> &AlternativeWords,std::vector<OCRLine*> &_OCRLine); 
	//删除和线相交的点
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

public:  //对操作流程的接口
	 virtual void OCRDrawingSheet(DrawingSheet &Sheet);
	 virtual int OCRDrawingZone(DrawingZone &Zone,bool calNums = false);  //静态的主引擎,后面是计数模式
	 virtual int calTotalNums(DrawingSheet &Sheet);   //计算大约有多少计算任务


	//图像处理的主引擎,1.词组列表,被处理的对象图,结果-未测试
	
	  int isRectInclude(cv::Rect &_Rect1,cv::Rect &_Rect2,int adjust = 2,bool jdL = false);

};






