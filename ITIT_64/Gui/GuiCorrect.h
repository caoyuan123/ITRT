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
	GuiCvFactory* PageSrc;    //��ͼ
	GuiCvFactory* PageDst;	  //��ͼ
	
	ImgProcCorrect* m_ImgProcCorrect;  //�������д������ͼ,�����ʱ�����

	GuiShowImg* m_showImg;             //������ʾ
    
	DrawingAbstruct* Ptr_DrawingTarget;   //�������DrawingSheet/DrawingPart��
	
public:
	GuiCorrect(QWidget *parent = 0,DrawingAbstruct* Drawing = 0);
	~GuiCorrect();

public slots:
	void bB_Page1_Clicked();
	void bB_Page2_Clicked();
	
	void setDrawing(DrawingAbstruct* Drawing);   //���ñ������DrawingSheet�Ľӿ�
	void AcceptWriteIn();   //ȷ�Ͻ��д��DrawingAbstruct


signals:
	void accept();
	void signal_flushButton();
public:

private:
	Ui::Correct* ui;

};

#endif // GUICORRECT_H
