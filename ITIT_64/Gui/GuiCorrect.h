#ifndef GUICORRECT_H
#define GUICORRECT_H

#include <QWidget>


	
class GuiShowImg;
class GuiCvFactory;
class ImgProcCorrect;
class DrawingAbstruct;
class QCloseEvent;
class IfGUI;

namespace Ui {
	class Correct;
} // namespace Ui


class GuiCorrect : public QWidget
{
	Q_OBJECT
protected:
	GuiCvFactory* PageSrc;    //主图
	GuiCvFactory* PageDst;	  //附图
	
	ImgProcCorrect* m_ImgProcCorrect;  //这里面有待处理的图,构造的时候加载

	GuiShowImg* m_showImg;             //单纯显示
    
	DrawingAbstruct* Ptr_DrawingTarget;   //处理对象（DrawingSheet/DrawingPart）
	
public:
	GuiCorrect(QWidget *parent = 0,DrawingAbstruct* Drawing = 0);
	~GuiCorrect();

public slots:
	void bB_Page1_Clicked();
	void bB_Page2_Clicked();
	
	void setDrawing(DrawingAbstruct* Drawing);   //设置被处理的DrawingSheet的接口
	void AcceptWriteIn();   //确认结果写入DrawingAbstruct


signals:
	void accept();
	void signal_flushButton();
public:

private:
	Ui::Correct* ui;

};

#endif // GUICORRECT_H
