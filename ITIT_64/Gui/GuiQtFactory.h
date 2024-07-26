#ifndef GUIQTFACTORY_H
#define GUIQTFACTORY_H

#include <QGraphicsView>
#include <QGraphicsItem>

#include <cxcore.h>


//һ����������
class GuiQtFactory : public QGraphicsView
{
	Q_OBJECT
private:
	

	double scaleRoit;  //�Ŵ���

	QGraphicsPixmapItem *backGround;

protected:
	int m_modeStye;
	cv::Mat m_MatBackGround;  //����ͼ
	QGraphicsScene* m_Scene;  //����

public:
	explicit GuiQtFactory(QWidget *parent = nullptr);
	~GuiQtFactory( );

	void setBackGround(const cv::Mat &_mat);  //���ñ���
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
