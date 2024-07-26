#include "GuiCorrect.h"
#include "GuiShowImg.h"
#include "guicvfactory.h"
#include "ImgProcCorrect.h"
#include "ui_guiCorrect.h"
#include "DrawingDef.h"
#include <QtGui/QtGui>


GuiCorrect::GuiCorrect(QWidget *parent,DrawingAbstruct* Drawing)
	: QWidget(parent)
{
	ui = new Ui::Correct;
	ui->setupUi(this);

	if (Drawing != nullptr)
	{
		m_ImgProcCorrect = new ImgProcCorrect(Drawing->getMat(1));
	}else
	{
		m_ImgProcCorrect = new ImgProcCorrect(cv::noArray().getMat());
	}

#ifdef _DEBUG	
	setAttribute(Qt::WA_DeleteOnClose);
#endif

	m_showImg = new GuiShowImg();

	PageSrc = new GuiCvFactory(nullptr,m_ImgProcCorrect);
	PageDst = new GuiCvFactory(nullptr);

	Ptr_DrawingTarget = Drawing;

	PageSrc->setZoomMode(2);
	PageDst->setZoomMode(1);
	PageSrc->setMouseTracking(true);

	ui->stackedWidget->addWidget(PageSrc);
	ui->stackedWidget->addWidget(PageDst);

	connect(ui->pB_Page1,SIGNAL(clicked()),this,SLOT(bB_Page1_Clicked()));
	connect(ui->pB_Page2,SIGNAL(clicked()),this,SLOT(bB_Page2_Clicked()));
	connect(ui->pushButton_accept,SIGNAL(clicked()),this,SLOT(AcceptWriteIn()));
}



GuiCorrect::~GuiCorrect()
{
	delete(m_ImgProcCorrect);

}

void GuiCorrect::bB_Page1_Clicked()
{
	ui->stackedWidget->setCurrentIndex(0);
}

void GuiCorrect::bB_Page2_Clicked()
{
	cv::Mat t; 
	PageSrc->imgProc(NULL,t);

	m_showImg->setMat(t);
	PageDst->setIfGui(m_showImg);

	ui->stackedWidget->setCurrentIndex(1);
}

void GuiCorrect::setDrawing(DrawingAbstruct* Drawing)
{
	Ptr_DrawingTarget = Drawing;
}

void GuiCorrect::AcceptWriteIn()
{
	cv::Mat t; 
	PageSrc->imgProc(NULL,t);

	Ptr_DrawingTarget->setImgProcCorrect(m_ImgProcCorrect->pointList,t);

	m_showImg->setMat(t);
	PageDst->setIfGui(m_showImg);

	ui->stackedWidget->setCurrentIndex(1);

	emit(signal_flushButton());
}



