#pragma once
#include <QThread>
#include <QQueue>
#include "StdAfx.h"

//����������Ǹ���������

class DrawingSheet;
class IfOCREngine;
class GuiDwSheetManager;

class OCREngineThread : public QThread
{
	Q_OBJECT

protected :
	enum OcrStatus
	{
		unSet = -1,      //δָ��
		initial = 0,     // 0 - ��ʼ 

		wait = 1,       // 1-����׼�����˴���״̬
		ready = 2,      // 2-ocr׼����
		going = 3,     //  3-OCR������
		pause = 4,		// 4-��ͣ
		over = 5
	};

	virtual void run( );

	void setEngine( );

	volatile bool stop;

	volatile char m_state;  //��ǰ״̬��

	bool isPublicSet;  //ʹ�ù�������

public:
	OCREngineThread(QObject* parent = nullptr );
	~OCREngineThread( );
	

	DrawingSheet *getActiveDwst();    //��ȡ��ǰ����
	GuiDwSheetManager* Ptr_activeDwManager;  //��ǰ����ҳ��

	IfOCREngine *m_OcrEngine;
	
	QQueue<GuiDwSheetManager*> m_waitList; //�ȴ����� �����ȳ� 

	GuiDwSheetManager * getNext(); //ȡ���е���һ��

	void EmitProcessBar(int value = -1);

	void MsgOut_setOcrStatus(GuiDwSheetManager*,int stats,int num = 0);

public slots:
	void slot_AddDrawingSheet(GuiDwSheetManager* _ds);  //��ӱ�ʶ���ҳ��
	void slot_Clear( );  //����ڲ�һЩ����


signals:
	void signal_TotalPercentagePoint(int);  //һ���ж�����Ҫ����ĵ�
	void signal_ProcessBar(int);  //ÿ����һ������ֱ�����1
	void signal_DrawingSheetStatus(int,int); //-1 :�����⣻0 - ����ʶ�� > 1���ȴ������� ���ֱ�ʾ���

	void signal_OcrComplete();
};




