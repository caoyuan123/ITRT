#include "itit.h"
#include "GUINewProject.h"
#include "DrawingBook.h"
#include "GuiDwSheetManager.h"
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>

#include "MessyCodeTerminator.h"

#include "ui_itit.h"
#include "Gui_CBITableDictionary.h"
#include "GuiMdiCoreShow.h"
#include "GuiChoiceFont.h"
#include "StdAfx.h"
#include "CBI_TableDictionary.h"
#include "OCREngineThread.h"
#include "QtAxExcelEngine.h"
#include "QxlsxEngine.h"
#include "GlobalSettingQt.h"


//#include "StdAfx.h"

ITIT::ITIT(QWidget *parent, Qt::WindowFlags flags)
	: QMainWindow(parent, flags)
{
	ui = new Ui::ITITClass;
	ui->setupUi(this);
	ui->frame_MiniWindow->hide();

	DrawingBook_Project = nullptr;
	m_NewProjectDialog = nullptr;
	
	m_subwindowActiveLast = nullptr;
	nowActiveSheetManager = nullptr;
	m_miniWinshow = nullptr;

	m_ChoiceFont = new GuiChoiceFont(this);
	g_DrawingTypeList = new DrawingTypeList( );

	m_ChoiceFont->slot_setDrawingTypeList(g_DrawingTypeList);

	Gui_Dictionary = new Gui_CBITableDictionary(this);

	connect(m_ChoiceFont,SIGNAL(signal_calDictionayDialog( )),this,SLOT(slot_DictionaryDialog( )));

	ScrollLayout = new QVBoxLayout(ui->scrollAreaWidgetContents);
	
	CreateStatusBar( );
	CreateAction( );

	connect(this->ui->mdiArea,SIGNAL(subWindowActivated(QMdiSubWindow *)),this,SLOT(slot_SwitchSubWindow(QMdiSubWindow *)));

//	this->setCentralWidget(ui->mdiArea);

	ui->action_Correct->setDisabled(true);
	ui->action_FliterMode->setDisabled(true);
	ui->action_Line->setDisabled(true);
	ui->action_Choice->setDisabled(true);

	ui->action_Begin->setDisabled(true);
	ui->action_Output->setDisabled(true);

	connect(ui->action_Correct,SIGNAL(triggered(bool)),this,SLOT(slot_bnCorrectDown(bool)));
	connect(ui->action_FliterMode,SIGNAL(triggered(bool)),this,SLOT(slot_bnFliterDown(bool)));
	
	connect(ui->action_Line,SIGNAL(triggered(bool)),this,SLOT(slot_bnLineDown(bool)));
	connect(ui->action_Choice,SIGNAL(triggered(bool)),this,SLOT(slot_bnZoneDown(bool)));

	connect(ui->action_Begin,SIGNAL(triggered(bool)),this,SLOT(slot_bnBeginDown(bool)));
	connect(ui->action_Output,SIGNAL(triggered(bool)),this,SLOT(slot_bnOutPutDown(bool)));

	connect(ui->pushButton_Confirm,SIGNAL(clicked( )),this,SLOT(slot_bnNextDown()));
	connect(ui->pushButton_testSave,SIGNAL(clicked( )),this,SLOT(slot_SaveProject()));

	ui->mdiArea->closeAllSubWindows( );

	m_OCREngineThread = new OCREngineThread(this);
	m_ExcelEngine = nullptr;

	connect(m_OCREngineThread,SIGNAL(signal_OcrComplete( )),this,SLOT(slot_flushButton( )));
	connect(ui->action_Abort,SIGNAL(triggered( )),this,SLOT(slot_ShowAbout( )));

#ifdef _DEBUG

#else

	ui->pushButton_testSave->hide();

#endif // _DEBUG
	
	u_setSaveTable.read();

}

void ITIT::CreateStatusBar( )
{
	StatusLabel = new QLabel(U8s("状态栏"));
	StatusLabel->setAlignment(Qt::AlignHCenter);
	StatusLabel->setMinimumSize(StatusLabel->sizeHint( ));
	

	TipsLabel = new QLabel(U8s("提示信息"));
	TipsLabel->setIndent(3);

	statusBar()->addWidget(StatusLabel);
	statusBar()->addWidget(TipsLabel,1);
}

void ITIT::CreateAction( )
{
	ui->menu_Set->setDisabled(true);

	connect(ui->action_NewProject,SIGNAL(triggered()),this,SLOT(slot_newProjectDialog()));
	connect(ui->action_CloseProject,SIGNAL(triggered()),this,SLOT(slot_CloseProject()));
	connect(ui->action_Dictionary,SIGNAL(triggered()),this,SLOT(slot_DictionaryDialog()));
	connect(ui->action_FontSet,SIGNAL(triggered()),this,SLOT(slot_ChoiceFontDialog()));

	ui->action_ZommIn->setVisible(false);
	ui->action_ZoomOut->setVisible(false);
	ui->action_RoiMode->setVisible(false);
}

void ITIT::update()
{
	QMainWindow::update();
}

ITIT::~ITIT()
{
	if (m_ExcelEngine != nullptr)
	{
		delete(m_ExcelEngine);
	}
	
	delete(g_DrawingTypeList);
	delete(ui);
}

void ITIT::slot_newProjectDialog()
{
	if (m_NewProjectDialog == nullptr)
	{
		m_NewProjectDialog = new GUINewProjectDialog(this);
		connect(m_NewProjectDialog,SIGNAL(newProject()),this,SLOT(slot_NewProject()));
		m_NewProjectDialog->setModal(true);
	}
	
	m_NewProjectDialog->show( );
	m_NewProjectDialog->raise( );
	m_NewProjectDialog->activateWindow( );
}

void ITIT::slot_NewProject()
{
	slot_CloseProject();

	DrawingBook_Project = new DrawingBook();

	if (m_NewProjectDialog != nullptr)
	{
		m_NewProjectDialog->setDrawingBook(DrawingBook_Project);
	}
	connect(DrawingBook_Project,SIGNAL(ReadNewSheet(DrawingSheet*)),this,SLOT(slot_addSheetFrame(DrawingSheet*)),Qt::ConnectionType::QueuedConnection);
	m_NewProjectDialog->setDrawingBook(DrawingBook_Project);
}

void ITIT::slot_CloseProject()
{
	//1.退掉 book对象
	if (DrawingBook_Project != nullptr)
	{
		//DrawingBook_Project->deleteLater();
		delete(DrawingBook_Project);
		DrawingBook_Project = nullptr;
	}else
	{
		; // == nullptr
	}
	
	//2.清除显示的对象

	int size = SheetList.size();

	for (int i = 0; i < size;++i)
	{		
		delete(SheetList[i]);
	}

	SheetList.clear();

	nowActiveSheetManager = nullptr;
	m_subwindowActiveLast = nullptr;


	if (m_ExcelEngine != nullptr)
	{
		delete(m_ExcelEngine);
		m_ExcelEngine = nullptr;
	}

	ui->mdiArea->closeAllSubWindows();
	update();
}

void ITIT::slot_addSheetFrame(DrawingSheet* newSheet)
{
	GuiDwSheetManager* _GuiDwSheet = new GuiDwSheetManager(this);
	_GuiDwSheet->slot_setDrawingSheet(newSheet);

	connect(_GuiDwSheet,SIGNAL(signal_flushButton()),this,SLOT(slot_flushButton()));

	ScrollLayout->addWidget(_GuiDwSheet);

	//_GuiDwSheet->show();
	SheetList.push_back(_GuiDwSheet);

}

void ITIT::slot_DictionaryDialog()
{
	if (Gui_Dictionary == nullptr)
	{
		Gui_Dictionary = new Gui_CBITableDictionary(this);
	}

	//设置project的连接
	if (DrawingBook_Project != nullptr)
	{
		Gui_Dictionary->setDictionaryPtr(DrawingBook_Project->m_Dictionary);
	}

	Gui_Dictionary->setModal(true);
	Gui_Dictionary->show();
	Gui_Dictionary->raise();
	Gui_Dictionary->activateWindow();
}

void ITIT::slot_ChoiceFontDialog()
{
	if (m_ChoiceFont == nullptr)
	{
		m_ChoiceFont->slot_setDrawingTypeList(g_DrawingTypeList);
	}

	if (DrawingBook_Project != nullptr)
	{
		m_ChoiceFont->slot_setDrawingBook(DrawingBook_Project);
	}else
	{
		m_ChoiceFont->slot_setDrawingBook(nullptr);
	}

	m_ChoiceFont->setModal(true);
	m_ChoiceFont->show();
	m_ChoiceFont->raise();
	m_ChoiceFont->activateWindow();
}

QMdiArea* ITIT::getMdiArea()
{
	return ui->mdiArea;
}

void ITIT::setButtonStatus(IfGuiDwStManagerCtrl *buttonCtrl)
{
	buttonCtrl->setQAction(1,ui->action_Correct);
	buttonCtrl->setQAction(2,ui->action_FliterMode);
	buttonCtrl->setQAction(4,ui->action_Line);
	buttonCtrl->setQAction(5,ui->action_Choice);
	buttonCtrl->setQAction(7,ui->action_Begin);
	buttonCtrl->setQAction(8,ui->action_Output);

	switch (buttonCtrl->Btn_Wizard())
	{
	case 0:
		{
			ui->pushButton_Confirm->setText(U8s("开始"));

		}break;
	case 1:
		{
			ui->pushButton_Confirm->setText(U8s("下一步"));
		}break;
	case 2:
		{
			ui->pushButton_Confirm->setText(U8s("确认准备"));
		}break;
	case 3:
		{
			ui->pushButton_Confirm->setText(U8s("更新参数"));
		}break;
	case 4:
		{
			//ui->pushButton_Confirm->setText(U8s("输出结果"));
		}break;
	default:
		break;
	}



	if (ui->action_Correct->isChecked() || ui->action_FliterMode->isChecked()||ui->action_Line->isChecked())
	{
		ui->pushButton_Confirm->setText(U8s("下一步"));
	}

	if (ui->action_Correct->isChecked() || ui->action_FliterMode->isChecked()||ui->action_Line->isChecked() ||ui->action_Choice->isChecked())
	{
		
	}else
	{
		ui->pushButton_Confirm->setText(U8s("开始"));
	}




}

void ITIT::slot_flushButton()
{
	if (nowActiveSheetManager != nullptr)
	{
		setButtonStatus(nowActiveSheetManager->getbuttonSate());
	}
}

void ITIT::slot_SwitchSubWindow(QMdiSubWindow* subWindow)
{
	QMdiSubWindow* temp = subWindow;

	if (m_subwindowActiveLast != temp)  //切换了激活的窗口的话
	{
		//1.给出当前活动控件
		if (temp != nullptr)
		{
			nowActiveSheetManager = ((GuiMdiCoreShow*) temp->widget())->getParent();

			if (nowActiveSheetManager->getMiniWindowShow())
			{
				ui->frame_MiniWindow->show();
			}else
			{
				ui->frame_MiniWindow->hide();
			}
			slot_flushButton();
			
			//小窗口的切换没有写

		}else  //失去焦点
		{
			ui->action_Correct->setDisabled(true);
			ui->action_FliterMode->setDisabled(true);
			ui->action_Line->setDisabled(true);
			ui->action_Choice->setDisabled(true);

			ui->action_Begin->setDisabled(true);
			ui->action_Output->setDisabled(true);
			//ui->pushButton_Confirm->hide();
		}
		m_subwindowActiveLast = temp;
	}
	else ; //没切换无所谓
}

void ITIT::slot_bnCorrectDown(bool pushDown)
{
	if (pushDown)
	{
		if (nowActiveSheetManager != nullptr)
		{
			nowActiveSheetManager->bnCorrectDown();
		}

	}else
	{
		if (nowActiveSheetManager != nullptr)
		{

			//主动弹起按钮的话
			nowActiveSheetManager->slot_showImg();
		}
	}
	
}

void ITIT::slot_bnFliterDown(bool pushDown)
{
	if (pushDown)
	{
		if (nowActiveSheetManager != nullptr)
		{
			nowActiveSheetManager->bnFliterDown();
		}

	}else
	{
		if (nowActiveSheetManager != nullptr)
		{
			//主动弹起按钮的话
			nowActiveSheetManager->slot_showImg();
		}
	}
}

void ITIT::slot_setToolShow(QWidget* qw)
{
	if (qw == nullptr)
	{
		if (m_miniWinshow != nullptr)
		{
			ui->verticalLayout_miniWindow->removeWidget(m_miniWinshow);
			m_miniWinshow = nullptr;
		}
		
		ui->frame_MiniWindow->hide( );

	}else
	{
		if (m_miniWinshow != nullptr)
		{
			ui->verticalLayout_miniWindow->removeWidget(m_miniWinshow);
			m_miniWinshow = nullptr;
		}

		ui->verticalLayout_miniWindow->addWidget(qw);
		m_miniWinshow = qw;

		ui->frame_MiniWindow->show( );

	}
}

void ITIT::slot_bnNextDown( )
{
	if (nowActiveSheetManager != nullptr)
	{
		nowActiveSheetManager->bnAcceptDown( );

		if(ui->action_Correct->isChecked( ))
		{
			slot_bnFliterDown(true);
		}

		else if(ui->action_FliterMode->isChecked( ))
		{
			slot_bnLineDown(true);
		}

		else if(ui->action_Line->isChecked( ))
		{
			slot_bnZoneDown(true);


		}
		else if (ui->action_Choice->isChecked( ))
		{



		}else
		{
			slot_bnCorrectDown(true);
		}

		// 去按下开始识别按钮





	}
}

void ITIT::slot_bnLineDown(bool pushDown)
{
	if (pushDown)
	{
		if (nowActiveSheetManager != nullptr)
		{
			nowActiveSheetManager->bnLineDown();
		}
	}
	else
	{
		if (nowActiveSheetManager != nullptr)
		{
			//主动弹起按钮的话
			nowActiveSheetManager->slot_showImg();
		}
	}
}

void ITIT::slot_bnZoneDown(bool pushDown)
{
	if (pushDown)
	{
		if (nowActiveSheetManager != nullptr)
		{
			nowActiveSheetManager->bnZoneDown( );
		}

	}else
	{
		if (nowActiveSheetManager != nullptr)
		{
			//主动弹起按钮的话
			nowActiveSheetManager->slot_showImg( );
		}
	}
}

void ITIT::slot_bnBeginDown(bool pushDown)
{
	//增加检查 字典文件加载步骤

	m_OCREngineThread->slot_AddDrawingSheet(nowActiveSheetManager);

	if (!m_OCREngineThread->isRunning( ))
	{
		m_OCREngineThread->start( );
	}
	else
	{
		QMessageBox::information(nullptr,U8s("提示"),U8s("当前存在正在识别的项目\r\n本页已进入等待队列"));
		//qWarning() << U8s("同时只能运行一个识别");
	}

	nowActiveSheetManager->slot_showImg( );
}

void ITIT::slot_bnOutPutDown(bool pushDown)
{
	if (nowActiveSheetManager != nullptr)
	{
		nowActiveSheetManager->bnOutPutDown( );
	}
}

void ITIT::slot_SaveProject( )
{
	if (DrawingBook_Project != nullptr)
	{
		//qDebug() << "保存SheetTest!!";

		//cv::FileStorage fs("testSheet.yaml",cv::FileStorage::WRITE);
		//fs << "Project" << *(DrawingBook_Project);
		//fs.release();

		if (nowActiveSheetManager != nullptr)
		{
			nowActiveSheetManager->slot_saveTemp();
		}

	}
}

void ITIT::closeEvent(QCloseEvent *e)
{
	delete(g_DrawingTypeList);

	if (m_ExcelEngine != nullptr)
	{
		delete(m_ExcelEngine);
	}

	u_setSaveTable.write();

	QMainWindow::closeEvent(e);
}

IfExcel* ITIT::getExcelEngine( )
{
	if (m_ExcelEngine == nullptr)
	{

		//m_ExcelEngine = new QtAxExcelEngine( );
		m_ExcelEngine = new QxlsxEngine( );
		m_ExcelEngine->initialize(true);
	}

	return m_ExcelEngine;
}

void ITIT::slot_ShowAbout()
{
	QString str;

	str += U8s("内部版本:");

	str += selfVer;

	QMessageBox::information(nullptr,U8s("关于"),str);
}

void ITIT::slot_bnZoomOutDown()
{

}

void ITIT::slot_bnZoomInDown()
{

}

void ITIT::slot_bnHandMoveDown()
{

}





