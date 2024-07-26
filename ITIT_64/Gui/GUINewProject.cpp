#include "GUINewProject.h"
#include <QtGui/QtGui>
#include "DrawingBook.h"
#include "ui_GuiNewProject.h"
#include "StdAfx.h"
#include "MessyCodeTerminator.h"
#include <QFileDialog>
#include <QMessageBox>

class ReadThread :public QThread
{
	
public:
	DrawingBook* Target;
	QString path;

	virtual void run()
	{
		Target->readBooks(path);
	}

};

GUINewProjectDialog::GUINewProjectDialog(QWidget *parent /*= 0*/)
	:  QDialog(parent)
{
	ptr_DrawingBook = nullptr;

	ui = new Ui::DigNewProject;
	ui->setupUi(this);

	ui->pushButton_accept->hide();
	
#ifdef _DEBUG
#else
	ui->pushButton_Test->hide();
#endif

	//分隔先不做了
	ui->label_2->hide();
	ui->lineEdit_Split->hide();

	connect(this->ui->pushButton_Test,SIGNAL(clicked()),this,SLOT(testButtomDown()));
	connect(this->ui->pushButton,SIGNAL(clicked()),this,SLOT(readImgFile()));
	connect(this->ui->toolButton,SIGNAL(clicked()),this,SLOT(slot_fileLoad()));
	connect(this->ui->pushButton_accept,SIGNAL(clicked()),this,SLOT(close()));
}

GUINewProjectDialog::~GUINewProjectDialog(void)
{
	
	disconnect(ptr_DrawingBook,SIGNAL(ReadMessage(QString)),this,SLOT(showStr(QString)));

}

void GUINewProjectDialog::testButtomDown()
{
	
	ui->lineEdit->setText(QString::fromLocal8Bit("E:\\ITIT_project\\打包范例\\通号南站") );
}

static bool CheckPath(QString path)
{
	QFile F(path);


	return F.exists();
}

void GUINewProjectDialog::readImgFile()
{
	emit(newProject());

	//检查一下路径是否合法,合法才能读取
	QString str = ui->lineEdit->text();

	if (str.isEmpty())
	{

		QMessageBox mb(U8s("请输入路径"),U8s("请输入路径") ,QMessageBox::Icon::Warning,QMessageBox::StandardButton::Ok,0,0);

		mb.exec( );


	}else if (!CheckPath(str))
	{
		QMessageBox mb(U8s("请检查输入路径"),U8s("输入路径不对") ,QMessageBox::Icon::Warning,QMessageBox::StandardButton::Ok,0,0);

		mb.exec( );

	}else


	if (ptr_DrawingBook != nullptr)
	{

		ReadThread* Rt = new ReadThread( );
		setStartButton(false);

		connect(Rt,SIGNAL(finished()),Rt,SLOT(deleteLater()));      //线程结束自己删除
		connect(Rt,SIGNAL(finished()),this,SLOT(setStartButton())); //线程结束开放按钮

		Rt->Target = ptr_DrawingBook;
		ptr_DrawingBook->readRoteType = ui->comboBox_Rote->currentIndex();
		Rt->path = ui->lineEdit->text();
		Rt->start();

		
	}

	//ptr_DrawingBook->readBook(ui.lineEdit->text());  //tiaoshi

}

//展示QString
void GUINewProjectDialog::showStr(QString qStr)
{
	ui->textEditer_shower->insertPlainText(qStr + "\r\n");
	ui->textEditer_shower->moveCursor(QTextCursor::End);

	//ui->textEditer_shower->repaint();
}

void GUINewProjectDialog::setDrawingBook(DrawingBook* db)
{
	if (ptr_DrawingBook != db)
	{
		ptr_DrawingBook = db;
		connect(ptr_DrawingBook,SIGNAL(ReadMessage(QString)),this,SLOT(showStr(QString)),\
			Qt::ConnectionType::QueuedConnection);
	}

}

void GUINewProjectDialog::setStartButton(bool able)
{
	ui->pushButton->setEnabled(able);

	if (able)
	{
		ui->pushButton_accept->show();
	}else
	{
		ui->pushButton_accept->hide();
	}
	
}

void GUINewProjectDialog::slot_fileLoad()
{
	QString s = QFileDialog::getExistingDirectory(this,U8s("选择联锁表图片文件夹"),"/");

	if (!s.isEmpty())
	{
		ui->lineEdit->setText(s);
	}
}




