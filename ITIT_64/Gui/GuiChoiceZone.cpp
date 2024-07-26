#include "GuiChoiceZone.h"
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>

#include "ImgProc.h"
#include <cv.hpp>
#include "QtCv.h"
#include "DrawingDef.h"
#include "DrawingSheet.h"
#include "DrawingZone.h"
#include "StdAfx.h"

#include "MessyCodeTerminator.h"

HeaderAction::HeaderAction(int num,QObject* parent) 
	:  QAction(parent)
{
	m_iNum = num;
	m_iSelectZone = -1;
	
}

HeaderAction::HeaderAction(int num,const QString &text, QObject* parent) 
:  QAction(text,parent)
{
	m_iNum = num;
	m_iSelectZone = -1;
}

HeaderAction::HeaderAction(int num,const QIcon &icon, const QString &text, QObject* parent)
	 : QAction(icon,text,parent)
{
	m_iNum = num;
	m_iSelectZone = -1;
}





////////////////////////

ZoneSelect::ZoneSelect(int Nums,GuiChoiceZone* parent /*= nullptr*/)
{

	for (int i =0;i < Nums;i++)
	{
		ZoneToSelect.push_back(CBITable::unKnow);
	}

	for (int i = 0;i < MaxHeaders; i++)
	{
		headerListAction[i] = new HeaderAction(i,U8s(CBITable::HeadersChinese[i].data()),nullptr);

		headerListAction[i]->setCheckable(true);
	}

	m_Parent = parent;
}

CBITable::Headers ZoneSelect::getZoneType(int num) const
{
	if (num > ZoneToSelect.size() || num < 0)
	{
		return CBITable::unKnow;
	}
	else
	{
		return ZoneToSelect[num];
	}
}

bool ZoneSelect::isChanged() const
{
	return changed.size() > 0 ? true : false;
}

std::vector<int> ZoneSelect::getChanged(bool refresh )
{
	if (refresh)
	{
		std::vector<int> rt = changed;
		changed.clear();
		return rt;
	}else
	{
		return changed;
	}
}

ZoneSelect::~ZoneSelect()
{
	for (int i = 0;i < MaxHeaders; i++)
	{
		delete(headerListAction[i]);
	}
}








////////////////////////////////////////////////////////////////////

//1.添加进入主屏幕
//2.

GuiChoiceZone::GuiChoiceZone(QWidget *parent /*= nullptr*/)
	: QGraphicsView(parent)
{
	this->setStyleSheet("padding: 0px; border: 0px;");
	scaleRoit = 1;

	backGround = nullptr;
	Ptr_DrawingTarget = nullptr;
	Ptr_ZoneSelectActions = nullptr;

	Qscene = new QGraphicsScene();
	setScene(Qscene);

	this->setBackgroundBrush(QPixmap(":/ITIT/bj.png"));

}

void GuiChoiceZone::setRect(const std::vector<cv::Rect> &_Rects)
{
	//Qscene->clear();
	
	//清除没有写
	Qscene->clear();
	Qscene->addItem(backGround);  //重新添加背景图


	for (int i =0;i < _Rects.size();i++)
	{
		GuiZone *Gz = new GuiZone(_Rects[i],this);
		Gz->setNum(i);
		Qscene->addItem(Gz);
		_Zones.push_back(Gz);
	}

}

void GuiChoiceZone::wheelEvent(QWheelEvent *e)
{
	int delta = e->delta();
	scaleRoit = 1 + 0.001 * delta;
	scale(scaleRoit,scaleRoit);
}

GuiChoiceZone::~GuiChoiceZone()
{
	Qscene->clear();
	if (Ptr_ZoneSelectActions != nullptr)
	{
		delete(Ptr_ZoneSelectActions);
	}
	
	//GuiZone 在Qscene的时候析构
	//背景图 在Qscene的时候析构

	_Zones.clear();
}

void GuiChoiceZone::setMat(const cv::Mat &_inputMat)
{
	if (backGround != nullptr)  
	{
		delete(backGround);
	}
	
	QPixmap Qp;
	Qp.convertFromImage(QtCv::cvMat2Qimg(_inputMat));
	backGround = new QGraphicsPixmapItem(Qp);

	this->resize(Qp.size());
}

void GuiChoiceZone::setDrawing(DrawingAbstruct* Drawing)
{
	if (Drawing != nullptr)
	{
		Ptr_DrawingTarget = Drawing;
		//设定背景图
		setMat(Drawing->getMat(3));
		//初始化各个矩形框
		setRect(Drawing->getDrawingSheet()->Rects);

		if (Ptr_ZoneSelectActions != nullptr)
		{
			delete(Ptr_ZoneSelectActions);
		}

		//初始化所有菜单
		Ptr_ZoneSelectActions = new ZoneSelect(_Zones.size(),this);


		//如果已经有既有的,加载既有显示
		if (Drawing->getDrawingSheet()->flags_Process.getProcess(DwSt_ProcFlags_区域人选))
		{
			int s = Drawing->getDrawingSheet()->m_ZoneBeSelected.size();
			for (int i = 0;i < s; ++i)
			{
				//提取号码
				int Num = Drawing->getDrawingSheet()->m_ZoneBeSelected[i]->ZoneNum;

				_Zones[Num]->setZoneType(Drawing->getDrawingSheet()->m_ZoneBeSelected[i]->m_Header);
			}

			//这里有个BUG就是重新调整线后会乱
		}

	}else
	{
		Ptr_DrawingTarget = nullptr;
		setMat(cv::Mat::eye(100,100,1));
	}
}

void GuiChoiceZone::AcceptWriteIn()
{
	if (Ptr_DrawingTarget != nullptr)
	{
		Ptr_DrawingTarget->setImgProcZone(_Zones);
	}

	emit(signal_flushButton());
}

void GuiChoiceZone::mousePressEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::MidButton) == Qt::MidButton)
	{
		setDragMode(QGraphicsView::ScrollHandDrag);
		Qt::MouseButtons mb = e->buttons();
		mb |= Qt::MouseButton::LeftButton;

		QMouseEvent *QE = new QMouseEvent(e->type(),e->pos(),e->globalPos(),Qt::MouseButton::LeftButton,mb,e->modifiers());

		QGraphicsView::mousePressEvent(QE);
		return;
	}
}

void GuiChoiceZone::mouseReleaseEvent(QMouseEvent *e)
{
	if ((e->buttons() & Qt::MidButton) != Qt::MidButton)  //中键抬起
	{
		setDragMode(QGraphicsView::NoDrag);
	}

	QGraphicsView::mouseReleaseEvent(e);
}

void GuiChoiceZone::mouseMoveEvent(QMouseEvent *e)
{
	QGraphicsView::mouseMoveEvent(e);
}

void GuiChoiceZone::slot_AutoSelect()
{

	//判断当前除了备选的选项列以外都是 未知状态，也就是初始状态


}


///////////////////////////////////////////////////////////////////////////////////////////////////////////
void GuiZone::init(GuiChoiceZone *zoneChoice)
{
	ZoneType = CBITable::unKnow;

	_pen.setWidth(4);
	
	zoneColor = QColor(0,255,0,100);
	zoneSelect = QColor(0,255,0,200);

	_pen.setColor(zoneColor);
	
	setFlag(QGraphicsItem::ItemIsFocusable);
	//setFlag(QGraphicsItem::ItemIsMovable);

	setAcceptHoverEvents(true);

	_GuiZoneChoice = zoneChoice;

	setPen(_pen);
	select = false;
}

GuiZone::GuiZone(const QRectF &rect,GuiChoiceZone *scene, QGraphicsItem *parent )
	: QGraphicsRectItem(rect,parent)
{
	init(scene);
}

GuiZone::GuiZone(const CvRect &rect,GuiChoiceZone *scene, QGraphicsItem *parent)
	: QGraphicsRectItem(QRectF(rect.x,rect.y,rect.width,rect.height),parent)
{
	init(scene);
}

GuiZone::GuiZone(qreal x, qreal y, qreal w, qreal h,GuiChoiceZone *scene, QGraphicsItem *parent)
	:QGraphicsRectItem(x,y,w,h,parent)
{
	init(scene);
}

GuiZone::GuiZone(GuiChoiceZone *scene, QGraphicsItem *parent)
	:QGraphicsRectItem(parent)
{
	init(scene);
}


void GuiZone::setColor(const QColor &colorNormal,const QColor& colorSelect)
{
	zoneColor = colorNormal;
	zoneSelect = colorSelect;
}

void GuiZone::setColor(const QColor &colorNormal)
{
	zoneColor = colorNormal;
	zoneSelect = QColor(colorNormal.red() ,colorNormal.green()  ,colorNormal.blue() ,colorNormal.alpha() + 50 > 255 ? 255: colorNormal.alpha() + 50);
}

void GuiZone::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QMenu menu;

	for (int i =0;i < MaxHeaders;i++)
	{
		menu.addAction(_GuiZoneChoice->Ptr_ZoneSelectActions->headerListAction[i]);
	}
	HeaderAction *selectedAction = (HeaderAction*)menu.exec(event->screenPos());
	
	if (selectedAction != nullptr)
	{
		if (selectedAction->getNum() == 0)  //清除这个选项
		{
			selectedAction->m_iSelectZone = -1;
			if (ZoneType!= CBITable::unKnow)
			{
				_GuiZoneChoice->Ptr_ZoneSelectActions->headerListAction[ZoneType]->m_iSelectZone = -1;
				_GuiZoneChoice->Ptr_ZoneSelectActions->headerListAction[ZoneType]->setChecked(false);

				ZoneType = CBITable::unKnow;
			}

			selectedAction->setChecked(false);
			update();
		}

		//判断选中的选项是否为已经选中的
		else if (selectedAction->isChecked())  
		{
			if (selectedAction->m_iSelectZone < 0)	//将自己设置为这个
			{
				
				if (ZoneType != CBITable::unKnow)  //如果本区段选过别的,先清除选过的那个
				{
					_GuiZoneChoice->Ptr_ZoneSelectActions->headerListAction[ZoneType]->m_iSelectZone = -1;
					_GuiZoneChoice->Ptr_ZoneSelectActions->headerListAction[ZoneType]->setChecked(false);
				}

				selectedAction->m_iSelectZone = Num;
				ZoneType = (CBITable::Headers)selectedAction->getNum();
				update();
	
			}else
			{
				qFatal("contextMenuEvent Bug!!!!")  ;
			}
	
		}else //清除
		{
			if (selectedAction->m_iSelectZone < 0)
			{
				qFatal("contextMenuEvent Bug !!!!") ;
			}else
			{
				(_GuiZoneChoice->_Zones[selectedAction->m_iSelectZone])->ZoneType = CBITable::unKnow;
				_GuiZoneChoice->_Zones[selectedAction->m_iSelectZone]->update();
				selectedAction->m_iSelectZone = -1;
			}
		}
		//如果没有将自己改为这个选项
	}
}

void GuiZone::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	select = true;
	setFocus( );
	update( );
}

void GuiZone::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	if (select)
	{
		_pen.setColor(zoneSelect);
		painter->setBrush(zoneSelect);
	}else
	{
		_pen.setColor(zoneColor);
		painter->setBrush(zoneColor);	
	}

	setPen(_pen);
	painter->drawRect(rect());

	QTextOption qto;
	qto.setAlignment(Qt::AlignCenter);
	
	QFont Qf;
	Qf.setBold(true);
	//Qf.setWordSpacing(50);
	Qf.setPixelSize(50);

	painter->setPen(QPen(Qt::darkRed));
	painter->setFont(Qf);

	QString str;
	QString qs = U8s(CBITable::HeadersChinese[ZoneType].data());

	for (int i = 0;i < qs.size();i++)
	{
		str += qs[i];
		str += "\r\n";
	}
	painter->drawText(rect(),str,qto);

	QGraphicsRectItem::paint(painter,option,widget);
}

void GuiZone::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	select = false;
}

int GuiZone::type( ) const
{
	return g_GraphicsZoneItem; 
}

void GuiZone::setZoneType(CBITable::Headers _TableHeader)
{
	if (ZoneType != CBITable::unKnow)  //如果本区段选过别的,先清除选过的那个
	{
		_GuiZoneChoice->Ptr_ZoneSelectActions->headerListAction[ZoneType]->m_iSelectZone = -1;
		_GuiZoneChoice->Ptr_ZoneSelectActions->headerListAction[ZoneType]->setChecked(false);
	}

	_GuiZoneChoice->Ptr_ZoneSelectActions->headerListAction[_TableHeader]->m_iSelectZone = Num;
	_GuiZoneChoice->Ptr_ZoneSelectActions->headerListAction[_TableHeader]->setChecked(true);
	ZoneType = _TableHeader;
}






