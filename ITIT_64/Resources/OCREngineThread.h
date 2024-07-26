#pragma once
#include <QThread>
#include <QQueue>
#include "StdAfx.h"

//真正对外的那个启动引擎

class DrawingSheet;
class IfOCREngine;
class GuiDwSheetManager;

class OCREngineThread : public QThread
{
	Q_OBJECT

protected :
	enum OcrStatus
	{
		unSet = -1,      //未指定
		initial = 0,     // 0 - 初始 

		wait = 1,       // 1-引擎准备好了待机状态
		ready = 2,      // 2-ocr准备中
		going = 3,     //  3-OCR进行中
		pause = 4,		// 4-暂停
		over = 5
	};

	virtual void run( );

	void setEngine( );

	volatile bool stop;

	volatile char m_state;  //当前状态机

	bool isPublicSet;  //使用公有设置

public:
	OCREngineThread(QObject* parent = nullptr );
	~OCREngineThread( );
	

	DrawingSheet *getActiveDwst();    //获取当前处理
	GuiDwSheetManager* Ptr_activeDwManager;  //当前处理页面

	IfOCREngine *m_OcrEngine;
	
	QQueue<GuiDwSheetManager*> m_waitList; //等待队列 先入先出 

	GuiDwSheetManager * getNext(); //取队列的下一个

	void EmitProcessBar(int value = -1);

	void MsgOut_setOcrStatus(GuiDwSheetManager*,int stats,int num = 0);

public slots:
	void slot_AddDrawingSheet(GuiDwSheetManager* _ds);  //添加被识别的页面
	void slot_Clear( );  //清除内部一些东西


signals:
	void signal_TotalPercentagePoint(int);  //一共有多少需要计算的点
	void signal_ProcessBar(int);  //每计算一个给半分比条加1
	void signal_DrawingSheetStatus(int,int); //-1 :有问题；0 - 正在识别 > 1；等待队列中 数字表示序号

	void signal_OcrComplete();
};




