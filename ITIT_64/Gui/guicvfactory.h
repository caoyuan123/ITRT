#ifndef GUICVFACTORY_H
#define GUICVFACTORY_H


#include <QImage>
#include <QWidget>
#include <cxcore.hpp>



class IfGUI;

class GuiCvFactory : public QWidget
{
	Q_OBJECT

protected:

	IfGUI * _GraphicInterFace;  //ͼ��ӿ�
	IfGUI * _slefGui;           //�Լ�������

	bool m_colored;  //�ڰ�ģʽ���ǲ�ɫģʽ

	bool debugShow;

	//��������ͼƬ������ͼƬͷ��һ���洢�塣�洢���Qt����BGRA��ARGB32����GRAY��ʽ
	//���������ķ�Χ��512mb�ڴ��С��ͼƬ��������Ŵ�16����,������cv���ֲ��ᰴ����ͼ������qt��ʾʧ��
	QImage m_qt_img;         //�ڴ���ʾ�����ã���Ҫ��
	cv::Mat m_cv_img;

	cv::Mat ReSizeTemp;       //�Ŵ������ͼƬ
	double zoomRoit;          //�Ŵ����

	virtual void paintEvent(QPaintEvent * q);       
	void _copyImage(void);                  //cv��ʽתQt��ʽ

	void _initCopyImage(bool force = false);  //��ʼ��ͼƬ,ǿ�ƣ�

	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);

	virtual void wheelEvent(QWheelEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

	bool firstRoi;
	cv::Point RoiMoveBegin;  //ԭʼ������
	cv::Point RoiMoveStart;  //ԭʼ�����
	bool RoiMoveButtonDown;  //����м�������
	bool RoiMove;            //���ƶ���
	cv::Rect RoiZone; //��ǰ�Ĺ۲ⴰ��,����ĵ������Ͻ���

#define NoneZoom 0
#define	WidgetZoom 1
#define	QtRoiZoom 2
#define CvRoiZoom 3 //���ͼƬ̫���Ҫд�����
	int zoomMode; //�Ŵ�ģʽ - 0Ĭ�Ͽ������Ŵ�,1����Ŵ�,2������ROIģʽ,�ǵø�setzoomMode�еļ��

public:
	GuiCvFactory(QWidget *parent = nullptr,IfGUI* GraphicInterFace = nullptr);
	~GuiCvFactory();

	virtual void imgProc(cv::InputArray src,cv::OutputArray dst);
	
public slots:
		

	//ͼƬ��ʾ�Ŵ���
	void zoomResize(double newSize = 1.000000f);  //100����ԭ��С 1����С0.01, 102�ǷŴ�1.02 
	void zoomResizeStep(double newStep);   //����ʽ����
	void setZoomMode(int Mode);            //��������ģʽ
	void setIfGui(IfGUI* newShow);         //�����µ���ʾ

signals:


};

#endif // GUICVFACTORY_H
