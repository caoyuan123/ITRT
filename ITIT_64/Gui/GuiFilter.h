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

//����ٿ� �����˲����Ŀؼ�
class GuiFilter : public GuiCvFactory
{
	Q_OBJECT

protected:
	ImgProcFilter* m_ImgProcFilter;   //ͼ�����˲�,��Ҫ���Ĳ���
	GuiShowImg* m_ShowImg;  //����ʾ�ĵ�Ԫ	
	DrawingAbstruct* Ptr_DrawingTarget;   //�������DrawingSheet/DrawingPart��
	Ui::GuiFilterAdjust *ui;  //
	cv::Mat src;    //���������
	cv::Mat dst;	//���������
	
public:
	GuiFilter(QWidget* adjustWidget = nullptr ,QWidget* showMainWindow = nullptr);  //�����ؼ�,չʾ����
	~GuiFilter();

	QWidget* m_AdjustShow;			 //�����װ����ʾ����
	GuiCvFactory* m_ShowMainWindow;  //��������ʾ


public Q_SLOTS:
	 void show() ;
	 void hide() ;

	 virtual void paintEvent(QPaintEvent * q);

	 void setMat(cv::Mat &source);
	 void setDrawing(DrawingAbstruct* Drawing);   //���ñ������DrawingSheet�Ľӿ�
	 void AcceptWriteIn();   //ȷ�Ͻ��д��DrawingAbstruct

	 void slot_accept();  //ȷ�ϰ�������
	 void slot_skip();  //��ԭ
	 void slot_Auto();  //�Զ�����


signals:
	 void accept();  //�����ź�
	 void signal_flushButton();


signals:
	


};



#endif // GUIFILTER_H
