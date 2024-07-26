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
		//继续使用公有设定则不变更 。
		if (getActiveDwst()->isDwTypePublic() && isPublicSet)
		{

			//do nothing 现在还是不搞这个了
			m_OcrEngine->setSetting(*(getActiveDwst()->getDrawingType()));


		}else  //公->私,私->私,私->公 以上三种情况均需要重新设定
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

		//让引擎计算一共有多少任务
		
		int Max = m_OcrEngine->calTotalNums(*getActiveDwst());

		connect(this,SIGNAL(signal_TotalPercentagePoint(int)),Ptr_activeDwManager,SLOT(slot_setProgressBarMax(int)));

		emit(signal_TotalPercentagePoint(Max + 1));

		disconnect(this,SIGNAL(signal_TotalPercentagePoint(int)),Ptr_activeDwManager,SLOT(slot_setProgressBarMax(int)));

		connect(this,SIGNAL(signal_ProcessBar(int)),Ptr_activeDwManager,SLOT(slot_setProgressBarValue(int)));
		
		m_OcrEngine->setOCREnginThread(this); //绑定发送进度条对象

		MsgOut_setOcrStatus(Ptr_activeDwManager,Ui_GuiDwSheet::GuiMagOcrStates::going); //正式计算
		m_OcrEngine->OCRDrawingSheet(*getActiveDwst( )); 
		
		Ptr_activeDwManager->getDrawingSheet( )->flags_Process.setProcess(DwSt_ProcFlags_OCR计算,true);

		//完事之后打上时间标记
		QString current_date = QDateTime::currentDateTime().toString("hh:mm:ss-MM/dd/yyyy");

		Ptr_activeDwManager->getDrawingSheet( )->setOCRTime(current_date.toStdString());

		getActiveDwst( )->drawResultPicture(0);

		EmitProcessBar(Max + 1);

		MsgOut_setOcrStatus(Ptr_activeDwManager,Ui_GuiDwSheet::GuiMagOcrStates::unOutput);  //待输出

		emit(signal_OcrComplete( ));

		//清空有关进度条的内容
		disconnect(this,SIGNAL(signal_ProcessBar(int)),Ptr_activeDwManager,SLOT(slot_setProgressBarValue(int)));
		


		m_OcrEngine->setOCREnginThread(nullptr); //解除发送进度条对象

	} while (m_waitList.size() > 0);

	m_state = OcrStatus::wait;
}

void OCREngineThread::slot_AddDrawingSheet(GuiDwSheetManager* _ds)
{
	/*
	int sz = m_waitList.size( );

	 //防重复添加, 如果以后排队项目太多了就Hash表吧
	if (sz >= 1)  
	{
		for(int i = sz - 1; i >= 0 ; --i)   //从后向前查
		{
			if (m_waitList[i] == _ds)
			{
				return;   
			}
		}
	}*/


	//检查字库是否生成 若没有生成则现在生成

	DrawingSheet * ds = _ds->getDrawingSheet();

	MsgOut_setOcrStatus(_ds,Ui_GuiDwSheet::GuiMagOcrStates::prepered);

	if (! ds->isDictionaryReady())
	{//如果没有准备好字典

		if (ds->generateDictionary())
		{
			_ds->slot_setProgressBarMax(0);
			m_waitList.enqueue(_ds);
		}else
		{
			qFatal(U8c("字典生成错误"));
			return ;
		}
		
	}else
	{//已经准备好了字典

		_ds->slot_setProgressBarMax(0);
		m_waitList.enqueue(_ds);
	}

	//更新等待队列
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








