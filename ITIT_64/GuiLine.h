#pragma once


#include <QGraphicsLineItem>
#include <QGraphicsView>

class CvLine;

struct myQColor
{

	QString name;
	QColor color;

};




class GuiLine : public QGraphicsLineItem
{
protected:
	bool select;

	QColor zoneColor;						 //区域内颜色
	QColor zoneSelect;                       //被选中的颜色

	QPen pen;
public:
	GuiLine(const QLineF &Line, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
	GuiLine(const CvLine &Line, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

	void setColor(const QColor &colorNormal,const QColor& colorSelect);  //设置普通颜色和选中颜色
	void setColor(const QColor &colorNormal);  //设置普通颜色

	int getLineWidth() const;     //画的线的线宽
	void setLineWidth(int val);


	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);

};
