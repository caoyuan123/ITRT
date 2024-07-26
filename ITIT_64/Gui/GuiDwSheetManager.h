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




//显示图的心
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
		hide = 0,      //没有开始
		prepered = 1,  //准备中
		going = 2,     //识别中
		waiting = 3,   //排队等待中
		pause = 4,		//暂停
		unOutput = 5,     //完成待输出
		finish = 6,    //所有完成
	};

public:
	void slot_changedBackImg();  //更换背景图
	void slot_setDrawingSheet(DrawingSheet* ds);  //设置DrawingSheet
	DrawingSheet* getDrawingSheet(){return Ptr_DrawingSheet;}

public slots:
	void slot_setOcrStatus(int stats, int queueNum = 0); //状态码,排队序号

private:
	Ui::GuiDwSt *ui;

protected:
	virtual void paintEvent(QPaintEvent *);

};

struct IfGuiDwStManagerCtrl  //对ITIT按钮控制结构体接口数据
{
#define BtnMax 9
	char BtnStatus[BtnMax];   //状态存储


	char ShowStuts();  //显示状态 0-卡死 1-自由缩放 2-ROI

	char Btn_Correct();     //1按钮状态 统一为 bit1 = 是否允许按下 2 = 对于可以锁按钮 存储按下状态
	char Btn_Fliter();		//2
	char Btn_Merge();		//3
	char Btn_Line();		//4
	char Btn_Zone();		//5
	
	char Btn_Wizard();		//6  下一步按钮的状态 0-初始 1-下一步 2-完成
	char Btn_Begin();		//7
	char Btn_Output();		//8
	
	void setBasicStatus(DrawingSheet *dwSt);  //通过页面处理状态决定按钮是否是灰色
	void setQAction(int num,QAction* pButton) const;   //设定别人的按钮状态

	void saveQActionSate(int num);		//保存页面的按钮状态,给-1表示 处理流程按钮为全抬起

	IfGuiDwStManagerCtrl();

};

//图纸页面识别流程管理器
class GuiDwSheetManager : public QFrame
{
	Q_OBJECT
	Ui_GuiDwSheet* GuiThumbNail;  //显示的小缩略图
	QVBoxLayout* vlayout;         //内显示区域
	unsigned int ResizeNum;       //缩放指数
	IfGuiDwStManagerCtrl *m_ButtnStatus; //按钮状态
	
	ITIT *m_Parent;						 //父类
	unsigned char m_fgSheetStatus;		//Sheet的处理状态 0-初始 1- 读完图 2-6 4个按钮谁按下了

	//QSize miniSize;
	//QSize maxSize;

	GuiMdiCoreShow* GuiChildWindow;       //Mdi区域中的显示单元窗口
	QMdiSubWindow* m_MdiSubWindow;         //显示的那个子窗口-Qt的SubWindow

	//1. 核心展示用：
	GuiShowImg* m_ShowImg;  //普通的展示图片
	GuiCvFactory* m_CvFactoryMain;     //主界面显示的 
	
	//2.设置矫正用：
	GuiCorrect* m_imgProcCorrect;

	//3.滤波器用：
	GuiFilter* m_imgProcGuiFilter;

	//4.识别线：
	GuiChoiceLine* m_imgProcChoiceLine;

	//5.选择区域：
	GuiChoiceZone* m_imgProcChoiceZone;

public:
	GuiDwSheetManager(ITIT *parent);
	~GuiDwSheetManager();


public slots:
	void slot_changedBackImg();  //更换背景图
	void slot_setDrawingSheet(DrawingSheet* ds);  //设置DrawingSheet
	void slot_selected(bool isSelect = false); //图像控显被选中
	void slot_showImg(); //0.最普通的图像展示
	void slot_disposeShowWidget(); //释放普通图片展示


	///////////////OCR///////////////
	void slot_setOcrStatus(int stats, int queueNum = 0); //状态码,排队序号
	void slot_setProgressBarMax(int Max);
	void slot_setProgressBarValue(int value = -1);  //正数有效,负数为默认加一
	///////////////OCR///////////////

	 //保存暂存结果
	void slot_saveTemp();  
signals:
	void signal_clicked( );   //被点击事件
	void signal_focus( );
	void signal_flushButton();
	void signal_checkOcrStart(bool bl,QString reason);  //是否可以开始OCR / 原因

public: 
	DrawingSheet* getDrawingSheet();  //获取从属的DrawingSheet

	void bnCorrectDown();  //矫正按钮被按下
	void bnFliterDown();   //滤波按钮被按下
	void bnAcceptDown();   //接受按钮被按下
	void bnLineDown();     //线识别按钮被按下
	void bnZoneDown();     //区域按钮被按下
	void bnOutPutDown();     //输出按钮被按下

	bool getMiniWindowShow();

	IfGuiDwStManagerCtrl* getbuttonSate();  //获取主界面按钮状态

	int checkOcrReady(); //检查是否可以进行OCR是否准备好

	int checkNextButton(); //检查下一步按钮应该是啥

private:
	
protected:
	virtual void mousePressEvent(QMouseEvent *);
	virtual void focusInEvent(QFocusEvent *);
	virtual void focusOutEvent(QFocusEvent *);

};



#endif // GUIDWSHEET_H




