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

	IfGUI * _GraphicInterFace;  //图像接口
	IfGUI * _slefGui;           //自己保护的

	bool m_colored;  //黑白模式还是彩色模式

	bool debugShow;

	//下面这俩图片是两个图片头，一个存储体。存储体归Qt管理，BGRA（ARGB32）或GRAY格式
	//可以联动的范围是512mb内存大小的图片（联锁表放大16倍）,超过后cv部分不会按连续图像处理导致qt显示失败
	QImage m_qt_img;         //内存显示计算用，不要动
	cv::Mat m_cv_img;

	cv::Mat ReSizeTemp;       //放大后的最大图片
	double zoomRoit;          //放大比例

	virtual void paintEvent(QPaintEvent * q);       
	void _copyImage(void);                  //cv格式转Qt格式

	void _initCopyImage(bool force = false);  //初始化图片,强制？

	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);

	virtual void wheelEvent(QWheelEvent *e);
	virtual void keyReleaseEvent(QKeyEvent *e);
	virtual void resizeEvent(QResizeEvent *e);

	bool firstRoi;
	cv::Point RoiMoveBegin;  //原始鼠标起点
	cv::Point RoiMoveStart;  //原始窗起点
	bool RoiMoveButtonDown;  //鼠标中键按下了
	bool RoiMove;            //窗移动了
	cv::Rect RoiZone; //当前的观测窗口,里面的点是左上焦点

#define NoneZoom 0
#define	WidgetZoom 1
#define	QtRoiZoom 2
#define CvRoiZoom 3 //如果图片太大就要写这个了
	int zoomMode; //放大模式 - 0默认卡死不放大,1跟随放大,2是区域ROI模式,记得改setzoomMode中的检查

public:
	GuiCvFactory(QWidget *parent = nullptr,IfGUI* GraphicInterFace = nullptr);
	~GuiCvFactory();

	virtual void imgProc(cv::InputArray src,cv::OutputArray dst);
	
public slots:
		

	//图片显示放大函数
	void zoomResize(double newSize = 1.000000f);  //100就是原大小 1是缩小0.01, 102是放大1.02 
	void zoomResizeStep(double newStep);   //增量式缩放
	void setZoomMode(int Mode);            //设置缩放模式
	void setIfGui(IfGUI* newShow);         //设置新的显示

signals:


};

#endif // GUICVFACTORY_H
