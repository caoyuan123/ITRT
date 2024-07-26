#include "GuiFilter.h"
#include "DrawingDef.h"
#include "ImgProcFilter.h"

#include <QtGui/QtGui>
#include "GuiShowImg.h"
#include "ui_GuiFilterAdjust.h"
#include <opencv.hpp>

#include "GlobalSettingQt.h"

GuiFilter::GuiFilter(QWidget* adjustWidget,QWidget* showMainWindow ) 
	: GuiCvFactory(nullptr,nullptr)
{
	ui = new Ui::GuiFilterAdjust;

	m_AdjustShow = new QWidget(adjustWidget);
	ui->setupUi(m_AdjustShow);
	this->setParent(ui->widget);
	m_ImgProcFilter = new ImgProcFilter();
	this->setIfGui(m_ImgProcFilter);

	src = cv::noArray().getMat();
	dst = cv::noArray().getMat();

	m_ShowImg = new GuiShowImg();
	
	Ptr_DrawingTarget = nullptr;

	m_ShowMainWindow = new GuiCvFactory(showMainWindow,m_ShowImg);
	m_ShowMainWindow->setZoomMode(2);

	connect(ui->pushButton_skip,SIGNAL(clicked()),this,SLOT(slot_skip()));
	connect(ui->pushButton_Auto,SIGNAL(clicked()),this,SLOT(slot_Auto()));
	connect(ui->pushButton_accept,SIGNAL(clicked()),this,SLOT(slot_accept()));

	setZoomMode(1);

	debugShow = true;
}



GuiFilter::~GuiFilter()
{
	
	if (m_ShowMainWindow != nullptr)
	{
		delete(m_ShowMainWindow);
	}

	if (m_ShowImg != nullptr)
	{
		delete(m_ShowImg);
	}

	if (m_AdjustShow != nullptr)
	{
		setParent(nullptr);
		delete(m_AdjustShow);
	}

	if (m_ImgProcFilter != nullptr)
	{
		delete(m_ImgProcFilter);
	}

}

void GuiFilter::show()
{
	m_AdjustShow->show();
	m_ShowMainWindow->show();
}

void GuiFilter::hide()
{
	m_AdjustShow->hide();
	m_ShowMainWindow->hide();
}

void GuiFilter::paintEvent(QPaintEvent * q)
{
	GuiCvFactory::paintEvent(q);
	m_ImgProcFilter->adjust(m_ImgProcFilter->gSourceMat,dst);
	m_ShowMainWindow->update();

}

void GuiFilter::slot_accept()
{
	emit(accept());

	AcceptWriteIn();
}

void GuiFilter::slot_Auto()
{
	m_ImgProcFilter->clearPoints();
	m_ImgProcFilter->filterImgProc(m_ImgProcFilter->gSourceMat,dst);
	update();
}

void GuiFilter::slot_skip()
{
	m_ImgProcFilter->clearPoints();
	update();
}

void GuiFilter::setDrawing(DrawingAbstruct* Drawing)
{
	Ptr_DrawingTarget = Drawing;

	setMat(Drawing->getMat(2));
}

void GuiFilter::AcceptWriteIn()
{
	if (Ptr_DrawingTarget != nullptr)
	{
		Ptr_DrawingTarget->setImgProcFilter(m_ImgProcFilter->getCtrlPoint(),dst);
	}


	emit(signal_flushButton());
}

void GuiFilter::setMat(cv::Mat &source)
{
	src = source;
	
	if (u_setQt.g_blFltAuto)
	{
		m_ImgProcFilter->filterImgProc(src,dst);
	}else
	{
		m_ImgProcFilter->setSource(src,dst);
	}

	m_ShowImg->setMatPtr(&dst);

//	m_ShowMainWindow->setIfGui(m_ShowImg);
}






