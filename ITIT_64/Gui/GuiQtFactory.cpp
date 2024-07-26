#include "GuiQtFactory.h"
#include <QtGui/QtGui>
#include "QtCv.h"

GuiQtFactory::GuiQtFactory(QWidget *parent /*= nullptr*/)
	: QGraphicsView(parent)
{

	backGround = nullptr;
	this->setStyleSheet("padding: 0px; border: 0px;");

	m_Scene = new QGraphicsScene(this);
	QGraphicsView::setScene(m_Scene);

//	modeStye = 0;
	scaleRoit = 1;

	this->setBackgroundBrush(QPixmap(":/ITIT/bj.png"));

}

GuiQtFactory::~GuiQtFactory( )
{

}

void GuiQtFactory::setBackGround(const cv::Mat &_mat)
{
	if (backGround != nullptr)  
	{
		delete(backGround);
	}

	m_MatBackGround = _mat;

	QPixmap p = QPixmap::fromImage(QtCv::cvMat2Qimg(m_MatBackGround));
	backGround = new QGraphicsPixmapItem(p);

	//this->resize(p.size( )); //这句话会导致需要刷边框的时候刷不出来



}

void GuiQtFactory::mousePressEvent(QMouseEvent *e)
{
	if ((e->buttons( ) & Qt::MidButton) == Qt::MidButton)
	{
		QGraphicsView::setDragMode(QGraphicsView::ScrollHandDrag);
		Qt::MouseButtons mb = e->buttons( );
		mb |= Qt::MouseButton::LeftButton;

		QMouseEvent *QE = new QMouseEvent(e->type( ),e->pos( ),\
			e->globalPos( ),Qt::MouseButton::LeftButton,mb,e->modifiers( ));
		QGraphicsView::mousePressEvent(QE);

		return;
	}else
	{
		QGraphicsView::mousePressEvent(e);
	}
}

void GuiQtFactory::mouseReleaseEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::MidButton) != Qt::MidButton)  //中键抬起
	{
		setDragMode(QGraphicsView::NoDrag);
	}

	QGraphicsView::mouseReleaseEvent(e);
}

void GuiQtFactory::wheelEvent(QWheelEvent *e)
{
	int delta = e->delta();
	scaleRoit = 1 + 0.001 * delta;
	QGraphicsView::scale(scaleRoit,scaleRoit);
}

void GuiQtFactory::slot_InitShow()
{
	QGraphicsView::scene()->clear();
	QGraphicsView::scene()->addItem(backGround);

}

void GuiQtFactory::changeBackGround(const cv::Mat &_mat)
{
	m_MatBackGround = _mat;

	QPixmap p = QPixmap::fromImage(QtCv::cvMat2Qimg(m_MatBackGround));
	backGround->setPixmap(p);

	this->setSceneRect( 0 , 0 , p.size( ).width( ) , p.size( ).height( ) );
//	this->resize( p.size( ) );
	

	/*
	m_Scene->clear( );
	backGround = nullptr;
	setBackGround(_mat);
	update( );*/
}

void GuiQtFactory::keyPressEvent(QKeyEvent *e)
{
	QGraphicsView::keyPressEvent(e);
}





