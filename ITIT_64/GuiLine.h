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

	QColor zoneColor;						 //��������ɫ
	QColor zoneSelect;                       //��ѡ�е���ɫ

	QPen pen;
public:
	GuiLine(const QLineF &Line, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
	GuiLine(const CvLine &Line, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

	void setColor(const QColor &colorNormal,const QColor& colorSelect);  //������ͨ��ɫ��ѡ����ɫ
	void setColor(const QColor &colorNormal);  //������ͨ��ɫ

	int getLineWidth() const;     //�����ߵ��߿�
	void setLineWidth(int val);


	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
	virtual void keyPressEvent(QKeyEvent *event);

};
