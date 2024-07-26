#ifndef GUIDWSHEET_H
#define GUIDWSHEET_H

#include <QFrame>
#include <QMdiSubWindow>

namespace Ui {
	class GuiDwSt;
} // namespace Ui

class DrawingSheet;
class GuiMdiCoreShow;
class ITIT;
class GuiCorrect;
class QMdiSubWindow;
class GuiShowImg;
class GuiCvFactory;
class GuiFilter;
class GuiChoiceLine;
class GuiChoiceZone;
class QVBoxLayout;




//��ʾͼ����
class Ui_GuiDwSheet : public QFrame
{
	Q_OBJECT

	friend class GuiDwSheetManager;

	DrawingSheet* Ptr_DrawingSheet;
	QPixmap backGroundPage;

public:
	Ui_GuiDwSheet(QWidget *parent);
	~Ui_GuiDwSheet();
	
	enum GuiMagOcrStates
	{
		hide = 0,      //û�п�ʼ
		prepered = 1,  //׼����
		going = 2,     //ʶ����
		waiting = 3,   //�Ŷӵȴ���
		pause = 4,		//��ͣ
		unOutput = 5,     //��ɴ����
		finish = 6,    //�������
	};

public:
	void slot_changedBackImg();  //��������ͼ
	void slot_setDrawingSheet(DrawingSheet* ds);  //����DrawingSheet
	DrawingSheet* getDrawingSheet(){return Ptr_DrawingSheet;}

public slots:
	void slot_setOcrStatus(int stats, int queueNum = 0); //״̬��,�Ŷ����

private:
	Ui::GuiDwSt *ui;

protected:
	virtual void paintEvent(QPaintEvent *);

};

struct IfGuiDwStManagerCtrl  //��ITIT��ť���ƽṹ��ӿ�����
{
#define BtnMax 9
	char BtnStatus[BtnMax];   //״̬�洢


	char ShowStuts();  //��ʾ״̬ 0-���� 1-�������� 2-ROI

	char Btn_Correct();     //1��ť״̬ ͳһΪ bit1 = �Ƿ������� 2 = ���ڿ�������ť �洢����״̬
	char Btn_Fliter();		//2
	char Btn_Merge();		//3
	char Btn_Line();		//4
	char Btn_Zone();		//5
	
	char Btn_Wizard();		//6  ��һ����ť��״̬ 0-��ʼ 1-��һ�� 2-���
	char Btn_Begin();		//7
	char Btn_Output();		//8
	
	void setBasicStatus(DrawingSheet *dwSt);  //ͨ��ҳ�洦��״̬������ť�Ƿ��ǻ�ɫ
	void setQAction(int num,QAction* pButton) const;   //�趨���˵İ�ť״̬

	void saveQActionSate(int num);		//����ҳ��İ�ť״̬,��-1��ʾ �������̰�ťΪȫ̧��

	IfGuiDwStManagerCtrl();

};

//ͼֽҳ��ʶ�����̹�����
class GuiDwSheetManager : public QFrame
{
	Q_OBJECT
	Ui_GuiDwSheet* GuiThumbNail;  //��ʾ��С����ͼ
	QVBoxLayout* vlayout;         //����ʾ����
	unsigned int ResizeNum;       //����ָ��
	IfGuiDwStManagerCtrl *m_ButtnStatus; //��ť״̬
	
	ITIT *m_Parent;						 //����
	unsigned char m_fgSheetStatus;		//Sheet�Ĵ���״̬ 0-��ʼ 1- ����ͼ 2-6 4����ť˭������

	//QSize miniSize;
	//QSize maxSize;

	GuiMdiCoreShow* GuiChildWindow;       //Mdi�����е���ʾ��Ԫ����
	QMdiSubWindow* m_MdiSubWindow;         //��ʾ���Ǹ��Ӵ���-Qt��SubWindow

	//1. ����չʾ�ã�
	GuiShowImg* m_ShowImg;  //��ͨ��չʾͼƬ
	GuiCvFactory* m_CvFactoryMain;     //��������ʾ�� 
	
	//2.���ý����ã�
	GuiCorrect* m_imgProcCorrect;

	//3.�˲����ã�
	GuiFilter* m_imgProcGuiFilter;

	//4.ʶ���ߣ�
	GuiChoiceLine* m_imgProcChoiceLine;

	//5.ѡ������
	GuiChoiceZone* m_imgProcChoiceZone;

public:
	GuiDwSheetManager(ITIT *parent);
	~GuiDwSheetManager();


public slots:
	void slot_changedBackImg();  //��������ͼ
	void slot_setDrawingSheet(DrawingSheet* ds);  //����DrawingSheet
	void slot_selected(bool isSelect = false); //ͼ����Ա�ѡ��
	void slot_showImg(); //0.����ͨ��ͼ��չʾ
	void slot_disposeShowWidget(); //�ͷ���ͨͼƬչʾ


	///////////////OCR///////////////
	void slot_setOcrStatus(int stats, int queueNum = 0); //״̬��,�Ŷ����
	void slot_setProgressBarMax(int Max);
	void slot_setProgressBarValue(int value = -1);  //������Ч,����ΪĬ�ϼ�һ
	///////////////OCR///////////////

	 //�����ݴ���
	void slot_saveTemp();  
signals:
	void signal_clicked( );   //������¼�
	void signal_focus( );
	void signal_flushButton();
	void signal_checkOcrStart(bool bl,QString reason);  //�Ƿ���Կ�ʼOCR / ԭ��

public: 
	DrawingSheet* getDrawingSheet();  //��ȡ������DrawingSheet

	void bnCorrectDown();  //������ť������
	void bnFliterDown();   //�˲���ť������
	void bnAcceptDown();   //���ܰ�ť������
	void bnLineDown();     //��ʶ��ť������
	void bnZoneDown();     //����ť������
	void bnOutPutDown();     //�����ť������

	bool getMiniWindowShow();

	IfGuiDwStManagerCtrl* getbuttonSate();  //��ȡ�����水ť״̬

	int checkOcrReady(); //����Ƿ���Խ���OCR�Ƿ�׼����

	int checkNextButton(); //�����һ����ťӦ����ɶ

private:
	
protected:
	virtual void mousePressEvent(QMouseEvent *);
	virtual void focusInEvent(QFocusEvent *);
	virtual void focusOutEvent(QFocusEvent *);

};



#endif // GUIDWSHEET_H




