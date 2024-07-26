#pragma once

#include <QGraphicsItem>
#include <QPointF>
#include <QGraphicsView>
#include <cxcore.hpp>
#include <GuiLine.h>

namespace cv
{
	class Mat;
}

namespace Ui {
	class GuiChoiceLineAdjust;
} // namespace Ui


class DrawingAbstruct;
class ImgProcLine;

//自己定制的绘图线性
class GuiChoiceLine_line : public GuiLine
{

	bool LineType; //true 是V竖线 H横线

public:
	GuiChoiceLine_line(const QLineF &Line,int LineWidth = 6, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
	GuiChoiceLine_line(const CvLine &Line,int LineWidth = 6, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

	virtual int type() const;

	void setVH(bool lineType);
};


//处理选择线
class GuiChoiceLine : public QGraphicsView
{
	Q_OBJECT
protected:
	QPointF startPoint;
	QPointF endPoint;

	int modeStye;   //模式方法 0 - 普通,1 - 画线模式

	ImgProcLine* _ImgProcLine;   //计算单元
	cv::Mat m_SourceMat;
	QGraphicsLineItem *selectGuiLine;  //被选中的GuiLine  

	double scaleRoit;

	DrawingAbstruct* Ptr_DrawingTarget;   //处理对象（DrawingSheet/DrawingPart）

	QGraphicsScene *scence;  //场景

	QColor HColor;  //横线颜色
	QColor VColor;  //竖线颜色

	
	Ui::GuiChoiceLineAdjust* ui_Adjust;        //调整的小窗口
public:
	QWidget* m_AdjustShow;			 //Tool区域的显示

	QGraphicsPixmapItem *backGround;  //背景图片

	explicit GuiChoiceLine(QWidget *parent = nullptr);
	~GuiChoiceLine();

	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void wheelEvent(QWheelEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void paintEvent(QPaintEvent *e);

public:
	void setMat(cv::Mat &_inputMat);   //设定背景图
	void setDrawing(DrawingAbstruct* Drawing);   //设置被处理的DrawingSheet的接口
	void AcceptWriteIn();   //确认结果写入DrawingAbstruct

public slots:
	void Slot_changeStye(int stye);   //切换画线模式
	void Slot_BnDrawLine(bool bl);   //小显示中的调整按键被按下
	void Slot_ImgProcLine( );
	void Slot_InitShow(bool useCal = true ); //初始化显示 

	void Slot_ChangeVLineWidth(int LineW);    //变更场景内的竖线
	void Slot_ChangeHLineWidth(int LineW);    //变更场景内的横线

	void SLot_ChangeVLineColor();  //变更颜色
	void SLot_ChangeHLineColor();  //变更颜色

signals:
	void signal_flushButton();

private:

};





