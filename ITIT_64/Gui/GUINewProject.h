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
	void testButtomDown();  //测试按钮
	void readImgFile();  //读取按钮

	void showStr(QString qStr);
	void setStartButton(bool able = true);  //设置开始按钮
	
	void slot_fileLoad(); //文件夹目录追踪

signals:
	void newProject(void);


private:
	Ui::DigNewProject* ui;
};

