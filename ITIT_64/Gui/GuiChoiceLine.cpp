#include "GuiChoiceLine.h"
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>

#include "ImgProc.h"
#include "CvLine.h"
#include <opencv.hpp>
#include "QtCv.h"
#include "DrawingDef.h"
#include "ImgProcLine.h"
#include "ui_GuiChoiceLineAdjust.h"
#include <vector>
#include "DrawingSheet.h"
#include <QMenu>
#include "MessyCodeTerminator.h"




//////////////////////////////////////////////////////////////////////
GuiChoiceLine::GuiChoiceLine(QWidget *parent)
	: QGraphicsView(parent)
{
	this->setStyleSheet("padding: 0px; border: 0px;");
	startPoint = QPoint(0,0);

	Ptr_DrawingTarget = nullptr;

	backGround = nullptr;
	_ImgProcLine = new ImgProcLine();

	modeStye = 0;
	selectGuiLine = nullptr;
	scaleRoit = 1;

	m_AdjustShow = new QWidget();
	ui_Adjust = new Ui::GuiChoiceLineAdjust;
	ui_Adjust->setupUi(m_AdjustShow);

	connect(ui_Adjust->pushButton_drawingLine,SIGNAL(clicked(bool)),this,SLOT(Slot_BnDrawLine(bool)));

	HColor = QColor(255,0,192);
	VColor = QColor(255,192,0);

	connect(ui_Adjust->spinBox_H,SIGNAL(valueChanged(int)),this,SLOT(Slot_ChangeHLineWidth(int)));
	connect(ui_Adjust->spinBox_V,SIGNAL(valueChanged(int)),this,SLOT(Slot_ChangeVLineWidth(int)));

	scence = new QGraphicsScene;

	this->setScene(scence);

	this->setForegroundBrush(QColor(255,255,255,50));

	this->setBackgroundBrush(QPixmap(":/ITIT/bj.png"));

	connect(ui_Adjust->pushButton_colorH,SIGNAL(clicked()),this,SLOT(SLot_ChangeHLineColor()));
	connect(ui_Adjust->pushButton_colorV,SIGNAL(clicked()),this,SLOT(SLot_ChangeVLineColor()));

}



GuiChoiceLine::~GuiChoiceLine()
{
	delete(_ImgProcLine);
	delete(ui_Adjust);

	if (m_AdjustShow != nullptr)
	{
		setParent(nullptr);
		delete(m_AdjustShow);
	}
}

void GuiChoiceLine::mousePressEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::MidButton) == Qt::MidButton)
	{
		setDragMode(QGraphicsView::ScrollHandDrag);
		Qt::MouseButtons mb = e->buttons();
		mb |= Qt::MouseButton::LeftButton;

		QMouseEvent *QE = new QMouseEvent(e->type(),e->pos(),e->globalPos(),\
			Qt::MouseButton::LeftButton,mb,e->modifiers());

		QGraphicsView::mousePressEvent(QE);
		return;
	}
	else
	{
		if ((e->buttons() & Qt::LeftButton) == Qt::LeftButton)
		{
			if (modeStye == 1)  //画图模式
			{
				if (selectGuiLine == nullptr)
				{
					startPoint = this->mapToScene(e->pos());
					GuiChoiceLine_line *newLine = new GuiChoiceLine_line(QLineF(startPoint,startPoint),(ui_Adjust->spinBox_H->value() + ui_Adjust->spinBox_V->value())/2);
					this->scene()->addItem(newLine);
					selectGuiLine = newLine;
				}
				else
				{
					CvLine Globle = CvLine(selectGuiLine->line());

					double r = 0;
					double t = 0;

					Globle.getPloar(r,t);
					Globle = CvLine::Polar2Descartes(cvSize(backGround->boundingRect().width(),backGround->boundingRect().height()),r,t);

					if (Globle.checked)
					{
						
						QMessageBox::information(nullptr,U8c("提示"),U8c("画线失败"));

						this->scene()->removeItem(selectGuiLine);

					}else
					{
						selectGuiLine->setLine(Globle.getLine( ));

						GuiChoiceLine_line* gl = (GuiChoiceLine_line*) selectGuiLine;

						gl->setVH(!Globle.isHorizontal( ));
					}
					
					selectGuiLine = nullptr;
				}
			}
		}

		QGraphicsView::mousePressEvent(e);
	}

}

void GuiChoiceLine::mouseReleaseEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::MidButton) != Qt::MidButton)  //中键抬起
	{
		setDragMode(QGraphicsView::NoDrag);
	}

	QGraphicsView::mouseReleaseEvent(e);
}

void GuiChoiceLine::mouseMoveEvent(QMouseEvent *e)
{
	if (modeStye == 1)
	{
		if (selectGuiLine != nullptr)
		{
			endPoint = this->mapToScene(e->pos());
			selectGuiLine->setLine(QLineF(startPoint,endPoint));
		}
	}
	QGraphicsView::mouseMoveEvent(e);
}

void GuiChoiceLine::wheelEvent(QWheelEvent *e)
{
	int delta = e->delta();
	scaleRoit = 1 + 0.001 * delta;
	QGraphicsView::scale(scaleRoit,scaleRoit);
}

void GuiChoiceLine::paintEvent(QPaintEvent *e)
{
	QGraphicsView::paintEvent(e);
}

void GuiChoiceLine::Slot_changeStye(int stye)
{
	modeStye = stye;
}

void GuiChoiceLine::Slot_BnDrawLine(bool bl)
{
	if (bl)  //按下
	{
		Slot_changeStye(1);
	}else
	{
		Slot_changeStye(0);  //抬起
	}
}

void GuiChoiceLine::setDrawing(DrawingAbstruct* Drawing)
{
	if (Drawing != nullptr)
	{
		Ptr_DrawingTarget = Drawing;
		setMat(Drawing->getMat(3));

		/*
		if (! Ptr_DrawingTarget->getDrawingSheet()->flags_Process.getProcess(DwSt_ProcFlags_线处理))
		{
		
		}else 
		{
			Slot_InitShow(false);
		}
		*/

		Slot_ImgProcLine( );
		Slot_InitShow(true);

	}else
	{
		qFatal(U8c("输入错误"));
	}
}

void GuiChoiceLine::AcceptWriteIn()//将线识别结果写入 DrawingSheet中
{
	if (Ptr_DrawingTarget != nullptr)
	{
		//将线从场景中取出,按横竖线分类
		scene( )->items( );  //所有的线性

		std::vector<CvLine> Hline;
		std::vector<CvLine> Vline;

		int s = scene()->items().size();
		for(int i = 0 ;i < s ;++i)  //items 是个栈 
		{
			if (scence->items()[i]->type() == g_GraphicsLineItemH || scence->items()[i]->type() == g_GraphicsLineItemV)
			{
				GuiChoiceLine_line *gl = (GuiChoiceLine_line *)scene( )->items( )[i];

				CvLine cl = CvLine(gl->line( )) ;

				/*
				if (cl.isHorizontal( ))  //保存一下老算法
				{
					Hline.push_back(cl);
				}else
				{
					Vline.push_back(cl);
				}*/
				
				if (gl->type() == g_GraphicsLineItemH)
				{
					Hline.push_back(cl);
				}else
				{
					Vline.push_back(cl);
				}
			}
		}
		Ptr_DrawingTarget->setImgProcLiner(Vline,Hline);
	}
	
	emit(signal_flushButton( ));
}

void GuiChoiceLine::setMat(cv::Mat &_inputMat)
{
	if (backGround != nullptr)  
	{
		delete(backGround);
	}

	m_SourceMat = _inputMat;

	QPixmap Qp;
	Qp.convertFromImage(QtCv::cvMat2Qimg( _inputMat ));
	backGround = new QGraphicsPixmapItem( Qp );

	this->resize(Qp.size( ));

}

void GuiChoiceLine::Slot_ImgProcLine( )
{
	_ImgProcLine->ImgProc(m_SourceMat);  //计算一下
}

void GuiChoiceLine::Slot_InitShow(bool useCal)  //之后升级一下区分是否第一次显示
{
	scene( )->clear( );
	scene( )->addItem(backGround);

	if (useCal)
	{
		//将处理结果输入显示场景当中
		int s = _ImgProcLine->HLine.size();
		for (int i = 0;i < s; ++i)
		{
			GuiChoiceLine_line* line = new GuiChoiceLine_line(_ImgProcLine->HLine[i],ui_Adjust->spinBox_H->value());
			line->setColor(HColor);
			line->setVH(g_Horizontal);
			scene()->addItem(line);
		}

		s = _ImgProcLine->VLine.size();
		for (int i = 0;i < s; ++i)
		{
			GuiChoiceLine_line* line = new GuiChoiceLine_line(_ImgProcLine->VLine[i],ui_Adjust->spinBox_V->value());
			line->setColor(VColor);
			line->setVH(g_Vertical);
			scene()->addItem(line);
		}


	}else
	{
		//从DrawingSheet中取线
	    DrawingSheet *ds = Ptr_DrawingTarget->getDrawingSheet();

		int s = ds->HLines.size();
		for (int i = 0;i < s; ++i)
		{
			GuiChoiceLine_line* line = new GuiChoiceLine_line(ds->HLines[i],ui_Adjust->spinBox_H->value());
			line->setColor(HColor);
			scene()->addItem(line);
		}

		s = ds->VLines.size();
		for (int i = 0;i < s; ++i)
		{
			GuiChoiceLine_line* line = new GuiChoiceLine_line(ds->VLines[i],ui_Adjust->spinBox_V->value());
			line->setColor(VColor);
			scene()->addItem(line);
		}
	}


}

void GuiChoiceLine::Slot_ChangeVLineWidth(int LineW)
{
	int s = scene()->items().size();
	for(int i = 0 ;i < s ;++i)  
	{
		if (scence->items()[i]->type() == g_GraphicsLineItemV)
		{
			GuiChoiceLine_line *gl = (GuiChoiceLine_line *)scene( )->items( )[i];
			gl->setLineWidth(LineW);
		}
	}
	 qDebug() << "changeVLineWidth" << LineW;
	update();
}

void GuiChoiceLine::Slot_ChangeHLineWidth(int LineW)
{
	int s = scene()->items().size();
	for(int i = 0 ;i < s ;++i)  
	{
		if (scence->items()[i]->type() == g_GraphicsLineItemH)
		{
			GuiChoiceLine_line *gl = (GuiChoiceLine_line *)scene( )->items( )[i];

			gl->setLineWidth(LineW);
		}
	}
	qDebug() << "changeHLineWidth" << LineW;
	update();
}

void GuiChoiceLine::SLot_ChangeVLineColor()
{
	 QColor color = QColorDialog::getColor(VColor);

	 if (color.isValid())
	 {

		 int s = scene()->items().size();
		 for(int i = 0 ;i < s ;++i)  
		 {
			 if (scence->items()[i]->type() == g_GraphicsLineItemV)
			 {
				 GuiChoiceLine_line *gl = (GuiChoiceLine_line *)scene( )->items( )[i];

				 gl->setColor(color);
			 }
		 }

	 }

	 VColor = color;

	 qDebug() << "changeVColor" << color;

	 update();
}

void GuiChoiceLine::SLot_ChangeHLineColor()
{
	QColor color = QColorDialog::getColor(HColor);

	if (color.isValid())
	{

		int s = scene()->items().size();
		for(int i = 0 ;i < s ;++i)  
		{
			if (scence->items()[i]->type() == g_GraphicsLineItemH)
			{
				GuiChoiceLine_line *gl = (GuiChoiceLine_line *)scene( )->items( )[i];

				gl->setColor(color);
			}
		}

	}

	HColor = color;

	qDebug() << "changeHColor" << color;

	update();
}

/////////////////////////////////////////////////////////

GuiChoiceLine_line::GuiChoiceLine_line(const QLineF &Line,int LineWidth , QGraphicsItem *parent /*= 0*/, QGraphicsScene *scene /*= 0*/)
	: GuiLine(Line,parent,scene)
{
	pen.setWidth(LineWidth);
}

GuiChoiceLine_line::GuiChoiceLine_line(const CvLine &Line,int LineWidth ,QGraphicsItem *parent /*= 0*/, QGraphicsScene *scene /*= 0*/)
	: GuiLine(Line,parent,scene)
{
	pen.setWidth(LineWidth);
}

int GuiChoiceLine_line::type( ) const
{
	if (LineType)
	{
		return g_GraphicsLineItemV;
	}else
	{
		return g_GraphicsLineItemH;
	}
}

//设定线的竖横
void GuiChoiceLine_line::setVH(bool lineType)
{
	LineType = lineType;
}





