#pragma once



#include <QMainWindow>

#include <vector>

class DrawingBook;
class DrawingSheet;
class GuiDwSheetManager;
class QVBoxLayout;
class GUINewProjectDialog;
class QLabel;
class Gui_CBITableDictionary;
class QMdiArea;
class QMdiSubWindow;
class GuiChoiceFont;
struct IfGuiDwStManagerCtrl;
class DrawingTypeList;
class OCREngineThread;
class IfExcel;
class GuiSimilar;

namespace Ui {
	class ITITClass;
} // namespace Ui



class ITIT : public QMainWindow
{
	Q_OBJECT

friend class GuiDwSheetManager;

	QVBoxLayout* ScrollLayout;
	QWidget* m_miniWinshow;

	GUINewProjectDialog *m_NewProjectDialog;    //新建项目对话框
	GuiChoiceFont* m_ChoiceFont;

	DrawingTypeList *g_DrawingTypeList;  //备选图纸项目

	Gui_CBITableDictionary* Gui_Dictionary;   //字典设置界面

	QLabel* StatusLabel; //状态栏显示
	QLabel* TipsLabel; //状态栏显示

	QMdiSubWindow* m_subwindowActiveLast;   //上次的激活窗口指针
	GuiDwSheetManager* nowActiveSheetManager;  //当前选中的窗口指针

	IfExcel * m_ExcelEngine; //excel输出的驱动

private: //项目有关 退项目的时候退的
	DrawingBook *DrawingBook_Project;  //主项目文件
	std::vector<GuiDwSheetManager*> SheetList;   //显示小图-同时也是Sheet的流程管理器

private: //识别有关
	
	OCREngineThread* m_OCREngineThread;  //异步的OCR引擎

private:
	void CreateStatusBar( );  //设置状态栏
	void CreateAction( ); //设置菜单栏

	void setButtonStatus(IfGuiDwStManagerCtrl *buttonCtrl);  //设置按钮状态

public:
	ITIT(QWidget *parent = 0, Qt::WindowFlags flags = 0);
	~ITIT( );

	QMdiArea* getMdiArea();
	
	IfExcel* getExcelEngine();

public slots:
	 void update( );
	 void slot_newProjectDialog( );  //新建项目对话框
	 void slot_NewProject( ); //新建项目
	 void slot_CloseProject( ); //关闭项目
	 void slot_addSheetFrame(DrawingSheet* newSheet);     //新建DrawingSheet项目
	
	 void slot_DictionaryDialog();  //设置字典
	 void slot_ChoiceFontDialog();  //设置字体对话框

	 void slot_SwitchSubWindow(QMdiSubWindow* subWindow);   //子窗口切换
	 void slot_setToolShow(QWidget* qw = nullptr);  //设置小窗口显示,给nullptr为退掉当前显示
	
	 void slot_bnCorrectDown(bool pushDown);   //矫正按钮按下
	 void slot_bnFliterDown(bool pushDown);   //滤波按钮按下
	 void slot_bnLineDown(bool pushDown);   //矫正线按钮按下
	 void slot_bnZoneDown(bool pushDown);   //矫正区域按钮按下
	 void slot_bnBeginDown(bool pushDown);  //开始识别按钮按下
	 void slot_bnOutPutDown(bool pushDown);  //输出按钮按下
	 void slot_bnNextDown( );                //下一步按钮按下

	 void slot_bnZoomOutDown( );  //区域放大按钮
	 void slot_bnZoomInDown( );   //区域缩小按钮
	 void slot_bnHandMoveDown( );  //鼠标左键拖拽

	 void slot_flushButton( );  //更新按钮状态

	 void slot_SaveProject( );  //调试用的保存按钮

	 void slot_ShowAbout( ); //展示关于界面

	 virtual void closeEvent(QCloseEvent *);

public:
	Ui::ITITClass *ui;

private:
	
};


