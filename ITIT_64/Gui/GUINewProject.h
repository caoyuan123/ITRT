#pragma once
#include <QDialog>

class DrawingBook;

namespace Ui {
	class DigNewProject;
} // namespace Ui

class GUINewProjectDialog : public QDialog
{
	Q_OBJECT

public:
	GUINewProjectDialog(QWidget *parent = 0);
	~GUINewProjectDialog(void);

	DrawingBook* ptr_DrawingBook;

	void setDrawingBook(DrawingBook* db);  //
	

public slots:
	void testButtomDown();  //���԰�ť
	void readImgFile();  //��ȡ��ť

	void showStr(QString qStr);
	void setStartButton(bool able = true);  //���ÿ�ʼ��ť
	
	void slot_fileLoad(); //�ļ���Ŀ¼׷��

signals:
	void newProject(void);


private:
	Ui::DigNewProject* ui;
};

