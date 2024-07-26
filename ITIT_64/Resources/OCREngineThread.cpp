#include "OCREngineThread.h"
#include "OCREngine.h"
#include "GuiDwSheetManager.h"
#include "DrawingSheet.h"
#include "MessyCodeTerminator.h"
#include <QtGui/QtGui>

OCREngineThread::OCREngineThread(QObject* parent )
	: QThread(parent)
{
	Ptr_activeDwManager = nullptr;

	m_OcrEngine = nullptr;

	m_state = OcrStatus::unSet; 

	stop = false;

	isPublicSet = true;
}


OCREngineThread::~OCREngineThread( )
{
	if (m_OcrEngine != nullptr)
	{
		delete(m_OcrEngine);
	}
}


void OCREngineThread::setEngine( )
{
	if (m_OcrEngine == nullptr)
	{
		isPublicSet = getActiveDwst()->isDwTypePublic();
		m_OcrEngine = new OCREngineCv( *(getActiveDwst()->getDrawingType()));
	}else
	{
		//����ʹ�ù����趨�򲻱�� ��
		if (getActiveDwst()->isDwTypePublic() && isPublicSet)
		{

			//do nothing ���ڻ��ǲ��������
			m_OcrEngine->setSetting(*(getActiveDwst()->getDrawingType()));


		}else  //��->˽,˽->˽,˽->�� ���������������Ҫ�����趨
		{
			m_OcrEngine->setSetting(*(getActiveDwst()->getDrawingType()));
		}
	}

	m_state = OcrStatus::ready;
}

void OCREngineThread::MsgOut_setOcrStatus(GuiDwSheetManager* dwstMgr,int stats,int num)
{
	connect(this,SIGNAL(signal_DrawingSheetStatus(int,int)),dwstMgr,SLOT(slot_setOcrStatus(int,int)));

	emit(signal_DrawingSheetStatus(stats,num));

	disconnect(this,SIGNAL(signal_DrawingSheetStatus(int,int)),dwstMgr,SLOT(slot_setOcrStatus(int,int)));
}

void OCREngineThread::run( )
{
	do 
	{
		
		Ptr_activeDwManager = getNext();

		if (Ptr_activeDwManager == nullptr)
		{
			qDebug() << "break OCR";
			break;
		}
		

		MsgOut_setOcrStatus(Ptr_activeDwManager,Ui_GuiDwSheet::GuiMagOcrStates::prepered);

		setEngine( );

		//���������һ���ж�������
		
		int Max = m_OcrEngine->calTotalNums(*getActiveDwst());

		connect(this,SIGNAL(signal_TotalPercentagePoint(int)),Ptr_activeDwManager,SLOT(slot_setProgressBarMax(int)));

		emit(signal_TotalPercentagePoint(Max + 1));

		disconnect(this,SIGNAL(signal_TotalPercentagePoint(int)),Ptr_activeDwManager,SLOT(slot_setProgressBarMax(int)));

		connect(this,SIGNAL(signal_ProcessBar(int)),Ptr_activeDwManager,SLOT(slot_setProgressBarValue(int)));
		
		m_OcrEngine->setOCREnginThread(this); //�󶨷��ͽ���������

		MsgOut_setOcrStatus(Ptr_activeDwManager,Ui_GuiDwSheet::GuiMagOcrStates::going); //��ʽ����
		m_OcrEngine->OCRDrawingSheet(*getActiveDwst( )); 
		
		Ptr_activeDwManager->getDrawingSheet( )->flags_Process.setProcess(DwSt_ProcFlags_OCR����,true);

		//����֮�����ʱ����
		QString current_date = QDateTime::currentDateTime().toString("hh:mm:ss-MM/dd/yyyy");

		Ptr_activeDwManager->getDrawingSheet( )->setOCRTime(current_date.toStdString());

		getActiveDwst( )->drawResultPicture(0);

		EmitProcessBar(Max + 1);

		MsgOut_setOcrStatus(Ptr_activeDwManager,Ui_GuiDwSheet::GuiMagOcrStates::unOutput);  //�����

		emit(signal_OcrComplete( ));

		//����йؽ�����������
		disconnect(this,SIGNAL(signal_ProcessBar(int)),Ptr_activeDwManager,SLOT(slot_setProgressBarValue(int)));
		


		m_OcrEngine->setOCREnginThread(nullptr); //������ͽ���������

	} while (m_waitList.size() > 0);

	m_state = OcrStatus::wait;
}

void OCREngineThread::slot_AddDrawingSheet(GuiDwSheetManager* _ds)
{
	/*
	int sz = m_waitList.size( );

	 //���ظ����, ����Ժ��Ŷ���Ŀ̫���˾�Hash���
	if (sz >= 1)  
	{
		for(int i = sz - 1; i >= 0 ; --i)   //�Ӻ���ǰ��
		{
			if (m_waitList[i] == _ds)
			{
				return;   
			}
		}
	}*/


	//����ֿ��Ƿ����� ��û����������������

	DrawingSheet * ds = _ds->getDrawingSheet();

	MsgOut_setOcrStatus(_ds,Ui_GuiDwSheet::GuiMagOcrStates::prepered);

	if (! ds->isDictionaryReady())
	{//���û��׼�����ֵ�

		if (ds->generateDictionary())
		{
			_ds->slot_setProgressBarMax(0);
			m_waitList.enqueue(_ds);
		}else
		{
			qFatal(U8c("�ֵ����ɴ���"));
			return ;
		}
		
	}else
	{//�Ѿ�׼�������ֵ�

		_ds->slot_setProgressBarMax(0);
		m_waitList.enqueue(_ds);
	}

	//���µȴ�����
	if (m_waitList.size() >= 1)
	{
		MsgOut_setOcrStatus(_ds,Ui_GuiDwSheet::GuiMagOcrStates::waiting,m_waitList.size());
	}
}

GuiDwSheetManager *OCREngineThread::getNext()
{
	if (m_waitList.size() < 1)
	{
		return nullptr;
	}

	GuiDwSheetManager *Rt = m_waitList.dequeue();

	if (m_waitList.size() > 0)
	{
		int sz = m_waitList.size();
		for (int i = 0;i < sz;i++)
		{
			MsgOut_setOcrStatus(m_waitList[i],3,i + 1);
		}
	}
	
	return Rt;
}

DrawingSheet * OCREngineThread::getActiveDwst()
{
	if (Ptr_activeDwManager != nullptr)
	{
		return Ptr_activeDwManager->getDrawingSheet();
	}

	return nullptr;
}

void OCREngineThread::EmitProcessBar(int value)
{
	emit(signal_ProcessBar(value));
}

void OCREngineThread::slot_Clear()
{
	isPublicSet = false;
}








