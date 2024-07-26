#pragma once

#include <QGraphicsView>
#include <QGraphicsItem>
#include <qglobal.h>
#include <cxcore.hpp>

#include "CBITableDef.h"
#include "QAction"
#include "CvLine.h"

#include "GuiChoiceLine.h"

class GuiLine;
class GuiChoiceZone ;
class GuiZone;

//ÿһ���ֲ˵���Ŀ
class HeaderAction : public QAction
{
	Q_OBJECT

protected:
	int m_iNum;         //���

public:
	int m_iSelectZone;  //�Ǹ����ε�ǰѡ���ҵ� 

	int getNum() {return m_iNum;};
	explicit HeaderAction(int num,QObject* parent);
	HeaderAction(int num,const QString &text, QObject* parent);
	HeaderAction(int num,const QIcon &icon, const QString &text, QObject* parent);

};

//����ѡ���Ǹ��ۺϲ˵��洢��
struct ZoneSelect
{
protected:
	std::vector<CBITable::Headers> ZoneToSelect;  //ÿ�������Ӧ�ı��������˭
	std::vector<int> changed;    //������


public:
	HeaderAction* headerListAction[MaxHeaders];
	GuiChoiceZone *m_Parent;  


public:
	ZoneSelect(int Nums = 0,GuiChoiceZone* parent = nullptr);  //һ���ж���
	~ZoneSelect();

	CBITable::Headers getZoneType(int num) const;                             //��ȡĳһ�������״̬
	bool isChanged() const;                              //�б��ô
	std::vector<int> getChanged(bool refresh = true);    //�������Щ,��ȡ֮���Ƿ����ñ��
};

class QCheckBox;
class QVBoxLayout;

//����ѡ���������
class GuiChoiceZone : public QGraphicsView
{
friend GuiZone;
	Q_OBJECT

protected:
	double scaleRoit;           //�Ŵ���
	GuiZone *selectGuiZone;	    //��ѡ�е�GuiLine  
	ZoneSelect *Ptr_ZoneSelectActions;     //���е����򼯺�
	QGraphicsScene *Qscene;		//���س���

	std::vector<GuiZone*> _Zones;



public:
	//QWidget* m_toolShow;
	QVBoxLayout* m_toolShowLayout; //����ѡ������

public:
	explicit GuiChoiceZone(QWidget *parent = nullptr);
	~GuiChoiceZone();

	virtual void wheelEvent(QWheelEvent *e);
	QGraphicsPixmapItem *backGround;  //����ͼƬ

public:  //�Խ��������ݽӿ�
	DrawingAbstruct* Ptr_DrawingTarget;   //�������DrawingSheet��
	void setRect(const std::vector<cv::Rect> &_Rects);

public slots:
	void setMat(const cv::Mat &_inputMat);   //�趨����ͼ
	void setDrawing(DrawingAbstruct* Drawing);   //���ñ������DrawingSheet�Ľӿ�
	void AcceptWriteIn( );   //ȷ�Ͻ��д��DrawingAbstruct

	void slot_AutoSelect( );

signals:
	void signal_flushButton( );

public:
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);


private:

};





//ÿһ��������
class GuiZone :  public QObject , public QGraphicsRectItem
{
	Q_OBJECT

private:
	void init(GuiChoiceZone *zoneChoice);

protected:
	CBITable::Headers ZoneType;   //�������ѡ����������� 0-δ֪����ûѡ
	GuiChoiceZone *_GuiZoneChoice;        //����
	
	QColor zoneColor;						 //��������ɫ
	QColor zoneSelect;                       //��ѡ�е���ɫ

	bool select;       //ѡ���ж�
	QPen _pen;         //����

	int Num;                                 //�������,��0��ʼ

public:
	GuiZone(const QRectF &rect,GuiChoiceZone *scene, QGraphicsItem *parent = 0);
	GuiZone(const CvRect &rect,GuiChoiceZone *scene, QGraphicsItem *parent = 0);
	GuiZone(qreal x, qreal y, qreal w, qreal h,GuiChoiceZone *scene, QGraphicsItem *parent = 0);
	GuiZone(GuiChoiceZone *scene ,QGraphicsItem *parent = 0);

	void setColor(const QColor &colorNormal,const QColor& colorSelect);  //������ͨ��ɫ��ѡ����ɫ
	void setColor(const QColor &colorNormal);  //������ͨ��ɫ

	int getNum() const { return Num; }
	void setNum(int val) { Num = val; }
	CBITable::Headers getZoneType() const {return ZoneType;}

	void setZoneType(CBITable::Headers _TableHeader);

	virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);
	virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
	virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);
	virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

	virtual int type( ) const;


Q_SIGNALS:
	void selectZoneType(int T);   //�����Լ���ѡ�������͵��ź�,��������������

	void signal_AutoSelectZoneType(int T);

};





