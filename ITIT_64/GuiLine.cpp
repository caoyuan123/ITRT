#include "GuiLine.h"
#include "QtGui/QtGui"
#include <QtWidgets/QtWidgets>
#include "CvLine.h"
#include "MessyCodeTerminator.h"

////////////////////////////////////////////////////////////////////////////////////////////////////////////

GuiLine::GuiLine(const QLineF &line, QGraphicsItem *parent , QGraphicsScene *scene )
	:  QGraphicsLineItem(line,parent)
{
	pen.setWidth(1);

	zoneColor = Qt::green;
	zoneSelect = Qt::darkGreen;

	pen.setColor(zoneColor);

	setFlag(QGraphicsItem::ItemIsFocusable);
	//setFlag(QGraphicsItem::ItemIsMovable);
	setAcceptHoverEvents(true);

	setPen(pen);
	select = false;
}

GuiLine::GuiLine(const CvLine &Line, QGraphicsItem *parent /*= 0*/, QGraphicsScene *scene /*= 0*/)
	:  QGraphicsLineItem(QLineF(Line.Point1.x,Line.Point1.y,Line.Point2.x,Line.Point2.y),parent)
{
	pen.setWidth(1);

	zoneColor = Qt::green;
	zoneSelect = Qt::darkGreen;

	pen.setColor(zoneColor);

	setFlag(QGraphicsItem::ItemIsFocusable);
	//	setFlag(QGraphicsItem::ItemIsMovable);
	setAcceptHoverEvents(true);

	setPen(pen);
	select = false;
}

void GuiLine::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= 0*/)
{
	if (select)
	{
		pen.setColor(zoneSelect);
	}else
	{
		pen.setColor(zoneColor);
	}

	setPen(pen);

	QGraphicsLineItem::paint(painter,option,widget);
}

void GuiLine::setColor(const QColor &colorNormal,const QColor& colorSelect)
{
	zoneColor = colorNormal;
	zoneSelect = colorSelect;
}

void GuiLine::setColor(const QColor &colorNormal)
{
	zoneColor = colorNormal;
	zoneSelect = QColor(colorNormal.red() / 2,colorNormal.green() /2 ,colorNormal.blue() /2);
}

int GuiLine::getLineWidth() const
{
	return pen.width();
}

void GuiLine::setLineWidth(int val)
{
	pen.setWidth(val);
}

void GuiLine::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
	QMenu menu;
	QAction *RemoveAction = menu.addAction(U8s("É¾³ý"));
	QAction *selectedAction = menu.exec(event->screenPos());
	if (selectedAction == RemoveAction)
	{
		this->scene()->removeItem(this);	
	}
}

void GuiLine::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
	select = true;
	setFocus();
}

void GuiLine::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
	select = false;
}

void GuiLine::keyPressEvent(QKeyEvent *event)
{
	switch(event->key())
	{
	case Qt::Key_Up : 
		{
			moveBy(0,-1);
		}break;

	case Qt::Key_Left : 
		{
			moveBy(-1,0);
		}break;

	case Qt::Key_Right : 
		{
			moveBy(1,0);
		}break;

	case Qt::Key_Down : 
		{
			moveBy(0,1);
		}break;
	}

	QGraphicsLineItem::keyPressEvent(event);
}




