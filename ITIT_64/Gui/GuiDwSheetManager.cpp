#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>

#include "GuiDwSheetManager.h"
#include "GuiMdiCoreShow.h"
#include "DrawingSheet.h"
#include "GuiShowImg.h"
#include "guicvfactory.h"
#include "GuiCorrect.h"
#include "GuiChoiceLine.h"
#include "GuiChoiceZone.h"

#include "DrawingBook.h"
#include "InterfaceTable.h"

#include "ui_GuiDwSt.h"
#include "GlobalSettingQt.h"
#include "itit.h"

#include "GuiFilter.h"

#include "MessyCodeTerminator.h"
#include <highgui.hpp>


Ui_GuiDwSheet::Ui_GuiDwSheet(QWidget *parent)
	: QFrame(parent)
{
	//����100 * 100 ��С
	//setBackgroundPixmap()

	//����ͼƬ�� DrawingSheet������ͼ
	Ptr_DrawingSheet = nullptr;
	ui = new Ui::GuiDwSt();
	ui->setupUi(this);
	ui->label_OcrState->hide();
}

Ui_GuiDwSheet::~Ui_GuiDwSheet()
{
	delete(ui);
}

void Ui_GuiDwSheet::slot_changedBackImg()
{
	if (Ptr_DrawingSheet != nullptr)
	{
		backGroundPage = Ptr_DrawingSheet->getshowPix();
		update();
	}
	
}

void Ui_GuiDwSheet::slot_setDrawingSheet(DrawingSheet* ds)
{
	Ptr_DrawingSheet = ds;
	ui->label->setText(Ptr_DrawingSheet->getName());
	slot_changedBackImg();
}

void Ui_GuiDwSheet::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
	painter.drawPixmap(rect(),backGroundPage);
	QWidget::paintEvent(e);
}

void Ui_GuiDwSheet::slot_setOcrStatus(int stats, int queueNum)
{
	switch(stats)
	{
	case GuiMagOcrStates::hide :   //����
		{
			ui->label_OcrState->setText("");
			ui->label_OcrState->hide();
		}break;
	case GuiMagOcrStates::prepered :   //OCR����׼����
		{
			ui->label_OcrState->show();
			ui->label_OcrState->setText(U8s("ʶ������׼����"));

		}break;
	case GuiMagOcrStates::going :   //OCR��
		{
			ui->label_OcrState->show();
			ui->label_OcrState->setText(U8s("ʶ����..."));

		}break;
	case GuiMagOcrStates::waiting :   //�Ŷ��� �ȴ���� Num
		{
			ui->label_OcrState->show();

			if (queueNum > 0)
			{
				char str[20];
				sprintf(str,"�ȴ�����: %d",queueNum);
				ui->label_OcrState->setText(U8s(str));
			}else
			{
				ui->label_OcrState->setText(U8s("������ʼ"));
			}

		}break;
	case GuiMagOcrStates::pause :  //����ͣ��
		{
			ui->label_OcrState->show();
			ui->label_OcrState->setText(U8s("ʶ����ͣ"));

		}break;

	case GuiMagOcrStates::unOutput :
	{
		ui->label_OcrState->show();
		ui->label_OcrState->setText(U8s("��ɵ�δ���"));

	}break;


	case GuiMagOcrStates::finish :
		{
			ui->label_OcrState->show();
			ui->label_OcrState->setText(U8s("��������"));
		}break;

	default: break;
		 


	}

}

////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
GuiDwSheetManager::GuiDwSheetManager(ITIT *parent)
		: QFrame(nullptr)
{
	m_Parent = parent;
	m_fgSheetStatus = 0;

	vlayout = new QVBoxLayout(this);
	vlayout->setMargin(5);
	GuiThumbNail = new Ui_GuiDwSheet(nullptr);  //���Զ�������
	vlayout->addWidget(GuiThumbNail); 

	ResizeNum = 0;

    this->setFixedSize(200,200);
	
//	this->setMaximumSize(u_setQt.g_szDwStGui);

	//miniSize = QSize(100,100);
	//maxSize = QSize(u_setQt.g_szDwStGui);

	GuiChildWindow = new GuiMdiCoreShow(this);
	GuiChildWindow->setMinimumSize(u_setQt.g_szGuiMdiMini);

	Qt::WindowFlags flags = Qt::Dialog;
	flags |= Qt::WindowMinMaxButtonsHint;
	flags |= Qt::WindowCloseButtonHint;
	GuiChildWindow->setWindowFlags(flags);
	
	m_MdiSubWindow = m_Parent->getMdiArea()->addSubWindow(GuiChildWindow);

	//connect(m_MdiSubWindow,SIGNAL(aboutToActivate()),m_Parent,SLOT(slot_SwitchSubWindow()));

	m_ShowImg = nullptr;
	m_CvFactoryMain = nullptr;
	m_imgProcCorrect = nullptr;
	m_imgProcGuiFilter = nullptr;
	m_imgProcChoiceLine = nullptr;
	m_imgProcChoiceZone = nullptr;

	m_ButtnStatus = new IfGuiDwStManagerCtrl;


//	connect(GuiChildWindow,SIGNAL(hideAccept()),m_MdiSubWindow,SLOT(hide()));
	connect(GuiChildWindow,SIGNAL(hideAccept()),this,SLOT(slot_selected()));  //�ر�ʱ̧����ʾ��ť

	this->setFrameShape(QFrame::Panel);
	this->setFrameShadow(QFrame::Raised);
	this->setLineWidth(5);
}

GuiDwSheetManager::~GuiDwSheetManager()
{
	GuiChildWindow->m_blClose = true;

	slot_disposeShowWidget();
	
	delete(m_ButtnStatus);

	/*
	if (m_ShowImg != nullptr)
	{
		delete(m_ShowImg);  //m_cvfactory ���Զ�����
	}*/
}

void GuiDwSheetManager::slot_changedBackImg()
{
	GuiThumbNail->slot_changedBackImg();
}

void GuiDwSheetManager::slot_setDrawingSheet(DrawingSheet* ds)
{
	GuiThumbNail->slot_setDrawingSheet(ds);
	GuiChildWindow->setWindowTitle(getDrawingSheet()->getName());
	m_fgSheetStatus = 1; //����ͼ
}

void GuiDwSheetManager::slot_selected(bool isSelect)
{
	if (isSelect)
	{
		this->setFrameShadow(QFrame::Sunken);
		m_MdiSubWindow->show( );
		
		switch (u_setQt.g_nGuiMdiNormalMode)
		{
		case 0:
			m_MdiSubWindow->showMinimized();
			break;
		case 1:
			break;
		case 2:
			m_MdiSubWindow->showMaximized();
			break;
		}

		slot_showImg();
	}else
	{
		this->setFrameShadow(QFrame::Raised);
		/*
		if (m_Parent->getMdiArea()->activeSubWindow()->widget() == GuiChildWindow)
		{
			GuiChildWindow->showMinimized();
		}*/

		slot_disposeShowWidget( );

		m_MdiSubWindow->hide( );
	}
}

void GuiDwSheetManager::mousePressEvent(QMouseEvent *e)
{

	if (e->buttons( ) == Qt::LeftButton)
	{
		if (this->frameShadow( ) == QFrame::Raised)
		{
			slot_selected(true);
		}else
		{
			slot_selected(false);
		}
		emit(signal_clicked( ));
	} 
}

void GuiDwSheetManager::focusInEvent(QFocusEvent *e)
{
	//throw std::logic_error("The method or operation is not implemented.");
}

void GuiDwSheetManager::focusOutEvent(QFocusEvent *e)
{
	//throw std::logic_error("The method or operation is not implemented.");
}

DrawingSheet* GuiDwSheetManager::getDrawingSheet()
{
	return GuiThumbNail->getDrawingSheet();
}

void GuiDwSheetManager::slot_showImg()
{
	getbuttonSate( )->saveQActionSate(-1);
	emit(signal_flushButton());

	slot_disposeShowWidget();
	
	if (m_ShowImg == nullptr)
	{
		m_ShowImg = new GuiShowImg();
	}
	
	if (this->getDrawingSheet()->flags_Process.getProcess(DwSt_ProcFlags_OCR����))
	{
		m_ShowImg->setMat(this->getDrawingSheet()->getMat(4));
	}else
	{
		m_ShowImg->setMat(this->getDrawingSheet()->getMat());
	}
	

	if (m_CvFactoryMain == nullptr)
	{
		m_CvFactoryMain = new GuiCvFactory(nullptr,m_ShowImg);
		m_CvFactoryMain->setZoomMode(2);

		GuiChildWindow->setShowWidget(m_CvFactoryMain);
	}

	m_CvFactoryMain->show( );
}

void GuiDwSheetManager::slot_disposeShowWidget( )
{

	GuiChildWindow->removeShowWidget( );

	if (m_ShowImg != nullptr)
	{
		delete(m_ShowImg);
		m_ShowImg = nullptr;
	}

	if (m_CvFactoryMain != nullptr)
	{
		m_CvFactoryMain->setParent(nullptr);
		delete(m_CvFactoryMain);
		m_CvFactoryMain = nullptr;
	}

	if (m_imgProcCorrect != nullptr)
	{
		m_Parent->slot_setToolShow(nullptr);  //����С��ʾ����
		delete(m_imgProcCorrect);
		m_imgProcCorrect = nullptr;
	}

	if (m_imgProcGuiFilter != nullptr)
	{
		m_Parent->slot_setToolShow(nullptr);  //����С��ʾ����

		delete(m_imgProcGuiFilter);
		m_imgProcGuiFilter = nullptr;
	}

	if (m_imgProcChoiceLine != nullptr)
	{
		m_Parent->slot_setToolShow(nullptr);  //����С��ʾ����

		delete(m_imgProcChoiceLine);
		m_imgProcChoiceLine = nullptr;
	}

	if (m_imgProcChoiceZone != nullptr)
	{
		m_Parent->slot_setToolShow(nullptr);  //����С��ʾ����
		delete(m_imgProcChoiceZone);
		m_imgProcChoiceZone = nullptr;
	}
}

IfGuiDwStManagerCtrl* GuiDwSheetManager::getbuttonSate()
{
	m_ButtnStatus->setBasicStatus(getDrawingSheet());

	return m_ButtnStatus;
}

void GuiDwSheetManager::bnCorrectDown()
{
	getbuttonSate( )->saveQActionSate(1);
	emit(signal_flushButton());
	
	//1.��ԭ����ʾ״̬����
	slot_disposeShowWidget();

	//2.�л���ʾ״̬��ȥ
	if (m_imgProcCorrect == nullptr)
	{
		m_imgProcCorrect = new GuiCorrect(nullptr,getDrawingSheet());

		connect(m_imgProcCorrect,SIGNAL(signal_flushButton()),this,SIGNAL(signal_flushButton()));
	}
	
	//3.��������С����
	GuiChildWindow->setShowWidget(m_imgProcCorrect);

}

void GuiDwSheetManager::bnFliterDown()
{
	//���ð�ť״̬
	getbuttonSate( )->saveQActionSate(2);
	emit(signal_flushButton( ));

	//1.��ԭ����ʾ״̬����
	slot_disposeShowWidget( );


	//2.�½�һ����ʾFactory �� ����Ԫ ���н��������׼����������Ԫ�Լ�ȷ�ϰ�ť׼����

	if (m_imgProcGuiFilter == nullptr)
	{
		m_imgProcGuiFilter = new GuiFilter();
		m_imgProcGuiFilter->setDrawing(getDrawingSheet());

		connect(m_imgProcGuiFilter,SIGNAL(signal_flushButton()),this,SIGNAL(signal_flushButton()));
	}

	GuiChildWindow->setShowWidget(m_imgProcGuiFilter->m_ShowMainWindow);
	
	m_Parent->slot_setToolShow(m_imgProcGuiFilter->m_AdjustShow);


	//
}

void GuiDwSheetManager::bnLineDown()
{
	//���ð�ť״̬
	getbuttonSate( )->saveQActionSate(4);
	emit(signal_flushButton());

	//1.��ԭ����ʾ״̬����
	slot_disposeShowWidget();

	if (m_imgProcChoiceLine == nullptr)
	{
		m_imgProcChoiceLine = new GuiChoiceLine();
		m_imgProcChoiceLine->setDrawing(getDrawingSheet());

		connect(m_imgProcChoiceLine,SIGNAL(signal_flushButton()),this,SIGNAL(signal_flushButton()));
	}

	GuiChildWindow->setShowWidget(m_imgProcChoiceLine);
	m_Parent->slot_setToolShow(m_imgProcChoiceLine->m_AdjustShow);
}

void GuiDwSheetManager::bnZoneDown()
{
	//���ð�ť״̬
	getbuttonSate( )->saveQActionSate(5);

	emit(signal_flushButton());

	//1.��ԭ����ʾ״̬����
	slot_disposeShowWidget();

	if (m_imgProcChoiceZone == nullptr)
	{
		m_imgProcChoiceZone = new GuiChoiceZone();
		m_imgProcChoiceZone->setDrawing(getDrawingSheet());

		connect(m_imgProcChoiceZone,SIGNAL(signal_flushButton()),this,SIGNAL(signal_flushButton()));
	}

	GuiChildWindow->setShowWidget(m_imgProcChoiceZone);
	
}

void GuiDwSheetManager::bnOutPutDown()
{

	DrawingSheet *_DrawingSheet = getDrawingSheet();

	//����������ͼƬ ��Ŀ¼��
	//ʹ������Ϊ savePath + ͼƬ���� + JG ����

	

	if (u_setSaveTable.ResultPictureOutput)
	{
		cv::Mat R = _DrawingSheet->drawResultPicture( );

		std::string SaveRPic = _DrawingSheet->getDrawingBook()->savePath.toLocal8Bit() + "\\���ͼ\\" + _DrawingSheet->getName().toLocal8Bit() + "JG.jpg";

		cv::imwrite(SaveRPic,R);
	}


	//�������·�� 
	QString ExcelBookPath = _DrawingSheet->getDrawingBook()->savePath  + "\\" + u_setSaveTable.ExcelName;

	IfExcel *_Excel = m_Parent->getExcelEngine( );
	_Excel->open(ExcelBookPath);

	QString _sheetName = _DrawingSheet->getName( );
	_DrawingSheet->toiLockTable92();
	_Excel->addSheet(_sheetName);
	
	//��ӱ�ͷ
	QVariantList Hear92Ch;

	for (int i =0;i < Table92Max;i++)
	{
		Hear92Ch.push_back(U8s(CBITable::HeadersChinese[CBITable::Header2Excel1992[i + 1] ].data())) ;
	}

	QVariantList Header;

	Header.push_back(Hear92Ch);

	//_Excel->setRangeCell(1,1,1,15,Header);

	_Excel->outPutStdTableHead92(_sheetName);


	int rows = _DrawingSheet->out_CbiTableResult.getExcelValue().size();

	_Excel->setRangeCell(3,1, rows + 2 , 15 ,_DrawingSheet->out_CbiTableResult.getExcelValue());

	_Excel->setCell(rows + 3,1,QVariant(_DrawingSheet->getOCRTime().data()));

	_Excel->save();

	_Excel->close();

	QMessageBox::information(nullptr,U8s("������"),U8s("ͬĿ¼�¼����"));

	GuiThumbNail->slot_setOcrStatus(Ui_GuiDwSheet::GuiMagOcrStates::finish);

}


bool GuiDwSheetManager::getMiniWindowShow()
{
	if (m_imgProcGuiFilter != nullptr)
	{
		return true;
	}else if (m_imgProcChoiceLine != nullptr)
	{
		return true;
	}else
	{
		return false;
	}
}


void GuiDwSheetManager::bnAcceptDown()
{
	if (m_imgProcGuiFilter != nullptr)
	{
		m_imgProcGuiFilter->AcceptWriteIn();
	}

	if (m_imgProcCorrect != nullptr)
	{
		m_imgProcCorrect->AcceptWriteIn();
	}

	if (m_imgProcChoiceLine != nullptr)
	{
		m_imgProcChoiceLine->AcceptWriteIn();
	}

	if (m_imgProcChoiceZone != nullptr)
	{
		m_imgProcChoiceZone->AcceptWriteIn();
	}
}


void GuiDwSheetManager::slot_setOcrStatus(int stats, int queueNum /*= 0*/)
{
	GuiThumbNail->slot_setOcrStatus(stats,queueNum);
	if (stats == 5)
	{
		slot_showImg();
	}
}


void GuiDwSheetManager::slot_setProgressBarMax(int Max /*= 100*/)
{
	GuiThumbNail->ui->progressBar->setMaximum(Max);
}


void GuiDwSheetManager::slot_setProgressBarValue(int value /*= -1*/)
{
	if (value < 0)
	{
		int v = GuiThumbNail->ui->progressBar->value( );
		v += ::abs(value);
		GuiThumbNail->ui->progressBar->setValue(v);

	}else
	{
		GuiThumbNail->ui->progressBar->setValue(value);
	}
}


void GuiDwSheetManager::slot_saveTemp()
{
	getDrawingSheet( )->TempSave( );
}


int GuiDwSheetManager::checkOcrReady()
{
	return 0;
}

int GuiDwSheetManager::checkNextButton()
{
	//�������״̬���ظ���һ����ť�õ�ָʾ
	//0-����δ��ʼ
	//1-������һ��
	//2-�������




	return 0;
}






///////////////////////////////////////////////////////////////////////////////////


IfGuiDwStManagerCtrl::IfGuiDwStManagerCtrl()
{
	memset(BtnStatus,0,sizeof(char) * BtnMax);  //���а�ť����Ϊ0
}

char IfGuiDwStManagerCtrl::ShowStuts()
{
	return BtnStatus[0];
}

char IfGuiDwStManagerCtrl::Btn_Correct()
{
	return BtnStatus[1];
}

char IfGuiDwStManagerCtrl::Btn_Fliter()
{
	return BtnStatus[2];
}

char IfGuiDwStManagerCtrl::Btn_Merge()
{
	return BtnStatus[3];
}

char IfGuiDwStManagerCtrl::Btn_Line()
{
	return BtnStatus[4];
}

char IfGuiDwStManagerCtrl::Btn_Zone()
{
	return BtnStatus[5];
}

char IfGuiDwStManagerCtrl::Btn_Wizard()
{
	return BtnStatus[6];
}

char IfGuiDwStManagerCtrl::Btn_Begin()
{
	return BtnStatus[7];
}

char IfGuiDwStManagerCtrl::Btn_Output()
{
	return BtnStatus[8];
}


void IfGuiDwStManagerCtrl::setBasicStatus(DrawingSheet *dwSt)
{
#define Fp dwSt->flags_Process

	if (! Fp.getProcess(DwSt_ProcFlags_��ȡ))
	{
		memset(BtnStatus,0,sizeof(char) * 8); //û�м���ͼ��ť��Ϊ0

		BtnStatus[6] = 0;  //ֻ���򵼰�ť�ǿ�ʼ״̬
	}else
	{
		BtnStatus[1] |= 0x01;  //����ͼ�Ϳ��Խ���
	    BtnStatus[6] = 0;  //ֻ���򵼰�ť�ǿ�ʼ״̬
		if (! Fp.getProcess(DwSt_ProcFlags_����))  
		{
			//û����������������
			BtnStatus[2] = 0;
			BtnStatus[3] = 0;
			BtnStatus[4] = 0;
			BtnStatus[5] = 0;
			BtnStatus[6] = 0;

		}else
		{
			BtnStatus[2] |= 0x01;   //��������

			if (! Fp.getProcess(DwSt_ProcFlags_�˲�))
			{
				//���˽���û���˲�,���úϲ�
				BtnStatus[3] = 0;
				BtnStatus[4] = 0;
				BtnStatus[5] = 0;
				BtnStatus[6] = 1;

			}else
			{
				//��ǿ��Ҫ�� �ϲ�ͼ��,�����˲������ѡ��ϲ�ͼ��
				BtnStatus[3] |= 0x01;
				BtnStatus[4] |= 0x01;
				BtnStatus[6] = 1;
				if (!Fp.getProcess(DwSt_ProcFlags_�ߴ���))
				{
					//��û���ߴ���������
					BtnStatus[5] = 0;  //û���ߴ����úϲ�Zone
					BtnStatus[6] = 1;
				}else
				{
					BtnStatus[5] |= 0x01;
					BtnStatus[6] = 2;  //����׼�����
				}
			}
		}
	}

	//��������Ѿ����Կ�ʼ����OCR��
	if ((Fp & DwSt_ProcFlags_ReadyOCR) == DwSt_ProcFlags_ReadyOCR)
	{
		BtnStatus[7] |= 0x01;
		BtnStatus[6] = 3;
	}else
	{
		BtnStatus[7] = 0x00;
	}

	//���OCR������ϾͿ��������
	if (Fp.getProcess(DwSt_ProcFlags_OCR����))  
	{
		BtnStatus[8] |= 0x01;
		BtnStatus[6] = 3;
	}else
	{
		BtnStatus[8] = 0x00;
	}

}

void IfGuiDwStManagerCtrl::setQAction(int num,QAction* pButton) const
{
	if (num < 0 || num >= BtnMax)
	{
		qFatal("setQAction Error!");
	}

//	qDebug() << num << ":-"<< int(BtnStatus[num]) ;

	if ((BtnStatus[num] & 0x01) == 0x01)  //��һλ
	{
		pButton->setEnabled(true);
	}else
	{
		pButton->setDisabled(true);
	}

	if ((BtnStatus[num] & 0x02) == 0x02)
	{
		pButton->setChecked(true);
	}else
	{
		pButton->setChecked(false);
	}
}

void IfGuiDwStManagerCtrl::saveQActionSate(int num)
{
	if (num < -1 || num >= BtnMax)
	{
		throw new std::exception("��������");
	}

	if (num == -1)
	{
		BtnStatus[1] &= 0x01;
		BtnStatus[2] &= 0x01;
		BtnStatus[3] &= 0x01;
		BtnStatus[4] &= 0x01;
		BtnStatus[5] &= 0x01;
	}

	else if (num < 6 && num > 0)  //1-5 �⼸����ť״̬����
	{
		BtnStatus[1] &= 0x01;
		BtnStatus[2] &= 0x01;
		BtnStatus[3] &= 0x01;
		BtnStatus[4] &= 0x01;
		BtnStatus[5] &= 0x01;

		BtnStatus[num] |= 0x02;
	}else
	{
		BtnStatus[num] |= 0x02;
	}
		
}








