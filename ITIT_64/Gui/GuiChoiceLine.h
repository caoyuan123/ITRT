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

//�Լ����ƵĻ�ͼ����
class GuiChoiceLine_line : public GuiLine
{

	bool LineType; //true ��V���� H����

public:
	GuiChoiceLine_line(const QLineF &Line,int LineWidth = 6, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);
	GuiChoiceLine_line(const CvLine &Line,int LineWidth = 6, QGraphicsItem *parent = 0, QGraphicsScene *scene = 0);

	virtual int type() const;

	void setVH(bool lineType);
};


//����ѡ����
class GuiChoiceLine : public QGraphicsView
{
	Q_OBJECT
protected:
	QPointF startPoint;
	QPointF endPoint;

	int modeStye;   //ģʽ���� 0 - ��ͨ,1 - ����ģʽ

	ImgProcLine* _ImgProcLine;   //���㵥Ԫ
	cv::Mat m_SourceMat;
	QGraphicsLineItem *selectGuiLine;  //��ѡ�е�GuiLine  

	double scaleRoit;

	DrawingAbstruct* Ptr_DrawingTarget;   //�������DrawingSheet/DrawingPart��

	QGraphicsScene *scence;  //����

	QColor HColor;  //������ɫ
	QColor VColor;  //������ɫ

	
	Ui::GuiChoiceLineAdjust* ui_Adjust;        //������С����
public:
	QWidget* m_AdjustShow;			 //Tool�������ʾ

	QGraphicsPixmapItem *backGround;  //����ͼƬ

	explicit GuiChoiceLine(QWidget *parent = nullptr);
	~GuiChoiceLine();

	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void wheelEvent(QWheelEvent *e);
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void paintEvent(QPaintEvent *e);

public:
	void setMat(cv::Mat &_inputMat);   //�趨����ͼ
	void setDrawing(DrawingAbstruct* Drawing);   //���ñ������DrawingSheet�Ľӿ�
	void AcceptWriteIn();   //ȷ�Ͻ��д��DrawingAbstruct

public slots:
	void Slot_changeStye(int stye);   //�л�����ģʽ
	void Slot_BnDrawLine(bool bl);   //С��ʾ�еĵ�������������
	void Slot_ImgProcLine( );
	void Slot_InitShow(bool useCal = true ); //��ʼ����ʾ 

	void Slot_ChangeVLineWidth(int LineW);    //��������ڵ�����
	void Slot_ChangeHLineWidth(int LineW);    //��������ڵĺ���

	void SLot_ChangeVLineColor();  //�����ɫ
	void SLot_ChangeHLineColor();  //�����ɫ

signals:
	void signal_flushButton();

private:

};





