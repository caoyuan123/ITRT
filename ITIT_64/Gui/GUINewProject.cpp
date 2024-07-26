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

	//�ָ��Ȳ�����
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
	
	ui->lineEdit->setText(QString::fromLocal8Bit("E:\\ITIT_project\\�������\\ͨ����վ") );
}

static bool CheckPath(QString path)
{
	QFile F(path);


	return F.exists();
}

void GUINewProjectDialog::readImgFile()
{
	emit(newProject());

	//���һ��·���Ƿ�Ϸ�,�Ϸ����ܶ�ȡ
	QString str = ui->lineEdit->text();

	if (str.isEmpty())
	{

		QMessageBox mb(U8s("������·��"),U8s("������·��") ,QMessageBox::Icon::Warning,QMessageBox::StandardButton::Ok,0,0);

		mb.exec( );


	}else if (!CheckPath(str))
	{
		QMessageBox mb(U8s("��������·��"),U8s("����·������") ,QMessageBox::Icon::Warning,QMessageBox::StandardButton::Ok,0,0);

		mb.exec( );

	}else


	if (ptr_DrawingBook != nullptr)
	{

		ReadThread* Rt = new ReadThread( );
		setStartButton(false);

		connect(Rt,SIGNAL(finished()),Rt,SLOT(deleteLater()));      //�߳̽����Լ�ɾ��
		connect(Rt,SIGNAL(finished()),this,SLOT(setStartButton())); //�߳̽������Ű�ť

		Rt->Target = ptr_DrawingBook;
		ptr_DrawingBook->readRoteType = ui->comboBox_Rote->currentIndex();
		Rt->path = ui->lineEdit->text();
		Rt->start();

		
	}

	//ptr_DrawingBook->readBook(ui.lineEdit->text());  //tiaoshi

}

//չʾQString
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
	QString s = QFileDialog::getExistingDirectory(this,U8s("ѡ��������ͼƬ�ļ���"),"/");

	if (!s.isEmpty())
	{
		ui->lineEdit->setText(s);
	}
}




