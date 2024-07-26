#pragma once
#include <QWidget>



class GuiDwSheetManager;
class IfGuiDwStManagerCtrl;
class QVBoxLayout;
///Mdi�����еĺ�����ʾ����,
class GuiMdiCoreShow :public QWidget
{
	Q_OBJECT

	QWidget* ShowWidget;  //��ǰ��ʾ�Ŀؼ�
	GuiDwSheetManager *m_Parent; //����
	QVBoxLayout* vlayout;         //����ʾ����

public:
	bool m_blClose; //�Ƿ������ر�

	GuiMdiCoreShow(GuiDwSheetManager* parent);
	~GuiMdiCoreShow();

	virtual void closeEvent(QCloseEvent *);

	GuiDwSheetManager * getParent();
	void setShowWidget(QWidget* subWidget);
	void removeShowWidget();   //�Ƶ�������ʾ��Ԫ��չʾ�ؼ�


signals:
	void signalButtonStatus(IfGuiDwStManagerCtrl *ButtomSt);  //��ť״̬�ź�
	void hideAccept(); //��������

};