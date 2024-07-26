#ifndef GUIFILTER_H
#define GUIFILTER_H

#include <QObject>
#include "guicvfactory.h"



class ImgProcFilter;
class GuiShowImg;
class QPushButton;
class QWidget;
class DrawingAbstruct;


namespace Ui {
	class GuiFilterAdjust;
} // namespace Ui

//代理操控 处理滤波器的控件
class GuiFilter : public GuiCvFactory
{
	Q_OBJECT

protected:
	ImgProcFilter* m_ImgProcFilter;   //图像处理滤波,主要核心部分
	GuiShowImg* m_ShowImg;  //主显示的单元	
	DrawingAbstruct* Ptr_DrawingTarget;   //处理对象（DrawingSheet/DrawingPart）
	Ui::GuiFilterAdjust *ui;  //
	cv::Mat src;    //被处理对象
	cv::Mat dst;	//被处理对象
	
public:
	GuiFilter(QWidget* adjustWidget = nullptr ,QWidget* showMainWindow = nullptr);  //调整控件,展示窗口
	~GuiFilter();

	QWidget* m_AdjustShow;			 //外面包装的显示区域
	GuiCvFactory* m_ShowMainWindow;  //主窗口显示


public Q_SLOTS:
	 void show() ;
	 void hide() ;

	 virtual void paintEvent(QPaintEvent * q);

	 void setMat(cv::Mat &source);
	 void setDrawing(DrawingAbstruct* Drawing);   //设置被处理的DrawingSheet的接口
	 void AcceptWriteIn();   //确认结果写入DrawingAbstruct

	 void slot_accept();  //确认按键按下
	 void slot_skip();  //还原
	 void slot_Auto();  //自动处理


signals:
	 void accept();  //接收信号
	 void signal_flushButton();


signals:
	


};



#endif // GUIFILTER_H
