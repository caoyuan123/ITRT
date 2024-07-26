#pragma once
#include <QWidget>



class GuiDwSheetManager;
class IfGuiDwStManagerCtrl;
class QVBoxLayout;
///Mdi区域中的核心显示部分,
class GuiMdiCoreShow :public QWidget
{
	Q_OBJECT

	QWidget* ShowWidget;  //当前显示的控件
	GuiDwSheetManager *m_Parent; //父类
	QVBoxLayout* vlayout;         //内显示区域

public:
	bool m_blClose; //是否真正关闭

	GuiMdiCoreShow(GuiDwSheetManager* parent);
	~GuiMdiCoreShow();

	virtual void closeEvent(QCloseEvent *);

	GuiDwSheetManager * getParent();
	void setShowWidget(QWidget* subWidget);
	void removeShowWidget();   //推掉中央显示单元的展示控件


signals:
	void signalButtonStatus(IfGuiDwStManagerCtrl *ButtomSt);  //按钮状态信号
	void hideAccept(); //隐藏请求

};