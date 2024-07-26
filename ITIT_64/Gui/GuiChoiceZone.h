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

//每一个字菜单项目
class HeaderAction : public QAction
{
	Q_OBJECT

protected:
	int m_iNum;         //序号

public:
	int m_iSelectZone;  //那个区段当前选中我的 

	int getNum() {return m_iNum;};
	explicit HeaderAction(int num,QObject* parent);
	HeaderAction(int num,const QString &text, QObject* parent);
	HeaderAction(int num,const QIcon &icon, const QString &text, QObject* parent);

};

//区域备选的那个综合菜单存储体
struct ZoneSelect
{
protected:
	std::vector<CBITable::Headers> ZoneToSelect;  //每个区域对应的表格区域是谁
	std::vector<int> changed;    //变更情况


public:
	HeaderAction* headerListAction[MaxHeaders];
	GuiChoiceZone *m_Parent;  


public:
	ZoneSelect(int Nums = 0,GuiChoiceZone* parent = nullptr);  //一共有多少
	~ZoneSelect();

	CBITable::Headers getZoneType(int num) const;                             //获取某一个区域的状态
	bool isChanged() const;                              //有变更么
	std::vector<int> getChanged(bool refresh = true);    //变的是那些,读取之后是否重置变更
};

class QCheckBox;
class QVBoxLayout;

//整个选择的主界面
class GuiChoiceZone : public QGraphicsView
{
friend GuiZone;
	Q_OBJECT

protected:
	double scaleRoit;           //放大倍率
	GuiZone *selectGuiZone;	    //被选中的GuiLine  
	ZoneSelect *Ptr_ZoneSelectActions;     //所有的区域集合
	QGraphicsScene *Qscene;		//被控场景

	std::vector<GuiZone*> _Zones;



public:
	//QWidget* m_toolShow;
	QVBoxLayout* m_toolShowLayout; //工具选项条子

public:
	explicit GuiChoiceZone(QWidget *parent = nullptr);
	~GuiChoiceZone();

	virtual void wheelEvent(QWheelEvent *e);
	QGraphicsPixmapItem *backGround;  //背景图片

public:  //对界面与数据接口
	DrawingAbstruct* Ptr_DrawingTarget;   //处理对象（DrawingSheet）
	void setRect(const std::vector<cv::Rect> &_Rects);

public slots:
	void setMat(const cv::Mat &_inputMat);   //设定背景图
	void setDrawing(DrawingAbstruct* Drawing);   //设置被处理的DrawingSheet的接口
	void AcceptWriteIn( );   //确认结果写入DrawingAbstruct

	void slot_AutoSelect( );

signals:
	void signal_flushButton( );

public:
	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void mouseMoveEvent(QMouseEvent *);


private:

};





//每一个子区域
class GuiZone :  public QObject , public QGraphicsRectItem
{
	Q_OBJECT

private:
	void init(GuiChoiceZone *zoneChoice);

protected:
	CBITable::Headers ZoneType;   //这个区域被选择的区域种类 0-未知就是没选
	GuiChoiceZone *_GuiZoneChoice;        //从属
	
	QColor zoneColor;						 //区域内颜色
	QColor zoneSelect;                       //被选中的颜色

	bool select;       //选中判定
	QPen _pen;         //画笔

	int Num;                                 //区域序号,从0开始

public:
	GuiZone(const QRectF &rect,GuiChoiceZone *scene, QGraphicsItem *parent = 0);
	GuiZone(const CvRect &rect,GuiChoiceZone *scene, QGraphicsItem *parent = 0);
	GuiZone(qreal x, qreal y, qreal w, qreal h,GuiChoiceZone *scene, QGraphicsItem *parent = 0);
	GuiZone(GuiChoiceZone *scene ,QGraphicsItem *parent = 0);

	void setColor(const QColor &colorNormal,const QColor& colorSelect);  //设置普通颜色和选中颜色
	void setColor(const QColor &colorNormal);  //设置普通颜色

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
	void selectZoneType(int T);   //发射自己被选区域类型的信号,来触发其他动作

	void signal_AutoSelectZoneType(int T);

};





