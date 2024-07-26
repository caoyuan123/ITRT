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
	//卡死100 * 100 大小
	//setBackgroundPixmap()

	//背景图片是 DrawingSheet的缩略图
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
	case GuiMagOcrStates::hide :   //隐藏
		{
			ui->label_OcrState->setText("");
			ui->label_OcrState->hide();
		}break;
	case GuiMagOcrStates::prepered :   //OCR引擎准备中
		{
			ui->label_OcrState->show();
			ui->label_OcrState->setText(U8s("识别引擎准备中"));

		}break;
	case GuiMagOcrStates::going :   //OCR中
		{
			ui->label_OcrState->show();
			ui->label_OcrState->setText(U8s("识别中..."));

		}break;
	case GuiMagOcrStates::waiting :   //排队中 等待序号 Num
		{
			ui->label_OcrState->show();

			if (queueNum > 0)
			{
				char str[20];
				sprintf(str,"等待队列: %d",queueNum);
				ui->label_OcrState->setText(U8s(str));
			}else
			{
				ui->label_OcrState->setText(U8s("即将开始"));
			}

		}break;
	case GuiMagOcrStates::pause :  //被暂停了
		{
			ui->label_OcrState->show();
			ui->label_OcrState->setText(U8s("识别暂停"));

		}break;

	case GuiMagOcrStates::unOutput :
	{
		ui->label_OcrState->show();
		ui->label_OcrState->setText(U8s("完成但未输出"));

	}break;


	case GuiMagOcrStates::finish :
		{
			ui->label_OcrState->show();
			ui->label_OcrState->setText(U8s("完成已输出"));
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
	GuiThumbNail = new Ui_GuiDwSheet(nullptr);  //会自动析构的
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
	connect(GuiChildWindow,SIGNAL(hideAccept()),this,SLOT(slot_selected()));  //关闭时抬起显示按钮

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
		delete(m_ShowImg);  //m_cvfactory 会自动析构
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
	m_fgSheetStatus = 1; //读完图
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
	
	if (this->getDrawingSheet()->flags_Process.getProcess(DwSt_ProcFlags_OCR计算))
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
		m_Parent->slot_setToolShow(nullptr);  //隐藏小显示窗口
		delete(m_imgProcCorrect);
		m_imgProcCorrect = nullptr;
	}

	if (m_imgProcGuiFilter != nullptr)
	{
		m_Parent->slot_setToolShow(nullptr);  //隐藏小显示窗口

		delete(m_imgProcGuiFilter);
		m_imgProcGuiFilter = nullptr;
	}

	if (m_imgProcChoiceLine != nullptr)
	{
		m_Parent->slot_setToolShow(nullptr);  //隐藏小显示窗口

		delete(m_imgProcChoiceLine);
		m_imgProcChoiceLine = nullptr;
	}

	if (m_imgProcChoiceZone != nullptr)
	{
		m_Parent->slot_setToolShow(nullptr);  //隐藏小显示窗口
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
	
	//1.将原有显示状态拿下
	slot_disposeShowWidget();

	//2.切换显示状态上去
	if (m_imgProcCorrect == nullptr)
	{
		m_imgProcCorrect = new GuiCorrect(nullptr,getDrawingSheet());

		connect(m_imgProcCorrect,SIGNAL(signal_flushButton()),this,SIGNAL(signal_flushButton()));
	}
	
	//3.工具栏的小窗口
	GuiChildWindow->setShowWidget(m_imgProcCorrect);

}

void GuiDwSheetManager::bnFliterDown()
{
	//设置按钮状态
	getbuttonSate( )->saveQActionSate(2);
	emit(signal_flushButton( ));

	//1.将原有显示状态拿下
	slot_disposeShowWidget( );


	//2.新建一个显示Factory 和 处理单元 还有将主界面的准备给调整单元以及确认按钮准备好

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
	//设置按钮状态
	getbuttonSate( )->saveQActionSate(4);
	emit(signal_flushButton());

	//1.将原有显示状态拿下
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
	//设置按钮状态
	getbuttonSate( )->saveQActionSate(5);

	emit(signal_flushButton());

	//1.将原有显示状态拿下
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

	//计算输出结果图片 到目录下
	//使用名称为 savePath + 图片名称 + JG 二字

	

	if (u_setSaveTable.ResultPictureOutput)
	{
		cv::Mat R = _DrawingSheet->drawResultPicture( );

		std::string SaveRPic = _DrawingSheet->getDrawingBook()->savePath.toLocal8Bit() + "\\结果图\\" + _DrawingSheet->getName().toLocal8Bit() + "JG.jpg";

		cv::imwrite(SaveRPic,R);
	}


	//计算输出路径 
	QString ExcelBookPath = _DrawingSheet->getDrawingBook()->savePath  + "\\" + u_setSaveTable.ExcelName;

	IfExcel *_Excel = m_Parent->getExcelEngine( );
	_Excel->open(ExcelBookPath);

	QString _sheetName = _DrawingSheet->getName( );
	_DrawingSheet->toiLockTable92();
	_Excel->addSheet(_sheetName);
	
	//添加表头
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

	QMessageBox::information(nullptr,U8s("输出完毕"),U8s("同目录下检查结果"));

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
	//检查自身状态返回给下一步按钮用的指示
	//0-代表未开始
	//1-代表下一步
	//2-代表完成




	return 0;
}






///////////////////////////////////////////////////////////////////////////////////


IfGuiDwStManagerCtrl::IfGuiDwStManagerCtrl()
{
	memset(BtnStatus,0,sizeof(char) * BtnMax);  //所有按钮设置为0
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

	if (! Fp.getProcess(DwSt_ProcFlags_读取))
	{
		memset(BtnStatus,0,sizeof(char) * 8); //没有加载图按钮均为0

		BtnStatus[6] = 0;  //只有向导按钮是开始状态
	}else
	{
		BtnStatus[1] |= 0x01;  //读完图就可以矫正
	    BtnStatus[6] = 0;  //只有向导按钮是开始状态
		if (! Fp.getProcess(DwSt_ProcFlags_矫正))  
		{
			//没做矫正不让往下做
			BtnStatus[2] = 0;
			BtnStatus[3] = 0;
			BtnStatus[4] = 0;
			BtnStatus[5] = 0;
			BtnStatus[6] = 0;

		}else
		{
			BtnStatus[2] |= 0x01;   //矫正允许

			if (! Fp.getProcess(DwSt_ProcFlags_滤波))
			{
				//做了矫正没做滤波,不让合并
				BtnStatus[3] = 0;
				BtnStatus[4] = 0;
				BtnStatus[5] = 0;
				BtnStatus[6] = 1;

			}else
			{
				//不强制要求 合并图像,但是滤波后可以选择合并图像
				BtnStatus[3] |= 0x01;
				BtnStatus[4] |= 0x01;
				BtnStatus[6] = 1;
				if (!Fp.getProcess(DwSt_ProcFlags_线处理))
				{
					//还没做线处理来这里
					BtnStatus[5] = 0;  //没做线处理不让合并Zone
					BtnStatus[6] = 1;
				}else
				{
					BtnStatus[5] |= 0x01;
					BtnStatus[6] = 2;  //最后的准备完成
				}
			}
		}
	}

	//如果流程已经可以开始计算OCR了
	if ((Fp & DwSt_ProcFlags_ReadyOCR) == DwSt_ProcFlags_ReadyOCR)
	{
		BtnStatus[7] |= 0x01;
		BtnStatus[6] = 3;
	}else
	{
		BtnStatus[7] = 0x00;
	}

	//如果OCR计算完毕就可以输出了
	if (Fp.getProcess(DwSt_ProcFlags_OCR计算))  
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

	if ((BtnStatus[num] & 0x01) == 0x01)  //第一位
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
		throw new std::exception("输入有误");
	}

	if (num == -1)
	{
		BtnStatus[1] &= 0x01;
		BtnStatus[2] &= 0x01;
		BtnStatus[3] &= 0x01;
		BtnStatus[4] &= 0x01;
		BtnStatus[5] &= 0x01;
	}

	else if (num < 6 && num > 0)  //1-5 这几个按钮状态互斥
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








