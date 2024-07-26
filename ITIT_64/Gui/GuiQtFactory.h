#ifndef GUIQTFACTORY_H
#define GUIQTFACTORY_H

#include <QGraphicsView>
#include <QGraphicsItem>

#include <cxcore.h>


//一个基本类型
class GuiQtFactory : public QGraphicsView
{
	Q_OBJECT
private:
	

	double scaleRoit;  //放大倍率

	QGraphicsPixmapItem *backGround;

protected:
	int m_modeStye;
	cv::Mat m_MatBackGround;  //背景图
	QGraphicsScene* m_Scene;  //场景

public:
	explicit GuiQtFactory(QWidget *parent = nullptr);
	~GuiQtFactory( );

	void setBackGround(const cv::Mat &_mat);  //设置背景
	void changeBackGround(const cv::Mat &_mat);


	virtual void mousePressEvent(QMouseEvent *);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual void wheelEvent(QWheelEvent *);
	



public slots:
	void slot_InitShow();

	virtual void keyPressEvent(QKeyEvent *);

	



private:

};

#endif // GUIQTFACTORY_H
