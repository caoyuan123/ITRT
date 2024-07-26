#include "StdAfx.h"
#include <string>
#include <vector>
#include <QtGui/QtGui>
#include "Gui_CBITableDictionary.h"
#include "ui_GuiDictionary.h"
#include "CBI_TableDictionary.h"
#include "GlobalSettingQt.h"
#include "MessyCodeTerminator.h"
#include <QFileDialog>
#include <QMessageBox>

using namespace std;
using namespace cv;


Gui_CBITableDictionary::Gui_CBITableDictionary(QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::DictionaryDialog;
	ui->setupUi(this);
	ui->Button_ProductButton->hide();

	ui->label_9->hide();
	ui->label_10->hide();
	ui->textEdit_SpSwitch->hide();
	ui->textEdit_SpSection->hide();
	
	connect(ui->pushButton_Read,SIGNAL(clicked( )),this,SLOT(slot_readDev( )));
	
#ifdef _DEBUG
	
	connect(ui->Button_Test,SIGNAL(clicked()),this,SLOT(testButtom()));
	connect(ui->Button_SaveSelf,SIGNAL(clicked()),this,SLOT(InterfaceToTable()));
#else
	ui->Button_Test->hide();
	ui->Button_SaveSelf->hide();

#endif

	connect(ui->comboBox_OtherLockChoice,SIGNAL(activated(int)),\
			this,SLOT(slot_OtherLockSelect(int)));

	connect(ui->comboBox_LightSet,SIGNAL(activated(int)),\
			this,SLOT(slot_signalLightSet(int)));

	connect(ui->comboBox_PointSingalCondition,SIGNAL(currentIndexChanged(int)),\
			this,SLOT(slot_setConditionSingleSwitch(int)));

	//slot_OtherLockSelect(0);
	//slot_LightSet(0);

	Ptr_CbiTbDictionary = new CBI_TableDictionary(nullptr);
	selfPtr = true;


}

Gui_CBITableDictionary::~Gui_CBITableDictionary()
{
	if (selfPtr == true)
	{
		delete(Ptr_CbiTbDictionary);
	}


	delete(ui);
}

void Gui_CBITableDictionary::setDictionaryPtr(CBI_TableDictionary *Ptr)
{
	if (selfPtr == true)
	{
		delete(Ptr_CbiTbDictionary);
		selfPtr = false;
	}
	
	if (Ptr_CbiTbDictionary != Ptr)
	{
		Ptr_CbiTbDictionary = Ptr;

		
	}
	TableToInterface();
}

CBI_TableDictionary* Gui_CBITableDictionary::DictionaryPtr()
{
	return Ptr_CbiTbDictionary;
}



/////////SLOTS/////////
void Gui_CBITableDictionary::testButtom()
{
	ui->lineEdit_ConfigPath->setText(U8s("D:\\������ѧϰ\\��2019��\\05\\�Ϸ�\\1�� ��\\�Ϸ�LS-V3.0.1-1.7.10 KX-V3.7.9-1.1.11 JC-V1.2.5-1.9.10\\S\\KX-V3.7.9-1.1.11\\config"));
}

void Gui_CBITableDictionary::accept()
{
	InterfaceToTable();
}

void Gui_CBITableDictionary::slot_GenerateButtom()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void Gui_CBITableDictionary::slot_readDev(std::string configPath)
{
	std::string Path = "";

	if (configPath == "")
	{
		Path = ui->lineEdit_ConfigPath->text().toStdString();
	}else
	{
		Path = configPath;
	}

	if (Path == "")
	{
		QString s = QFileDialog::getExistingDirectory(this,U8s("ѡ��Config�ļ���"),"/");

		if (!s.isEmpty())
		{
			ui->lineEdit_ConfigPath->setText(s);
		}

		Path = s.toStdString();

	}
	
	Ptr_CbiTbDictionary->slot_readDev(Path);

	TableToInterface();
	slot_signalLightSet(0);
	slot_OtherLockSelect(1);
	
	
}


//vector<string> ��ʽתQString��ʽ
inline static QString VecStd2QString(const std::vector<std::string> &input)
{
	QString Rt = "";

	int size = input.size();

	for (int i = 0;i < size;++i)
	{
		Rt += input[i].data();
		Rt += "\r\n";
	}

	return Rt;
}

//����˼��
inline static void QstrList2VecStd(const QStringList Qsl,std::vector<std::string> &StdVec,bool PushBack = false)
{
	if (!PushBack)
	{
		StdVec.clear();
	}

	foreach(QString Str , Qsl)
	{
		std::string stdStr = Str.remove(QRegExp("\s")).toStdString();
		if (stdStr != "" && stdStr != "\r" && stdStr != "\n" && stdStr != "\t")
		{
			StdVec.push_back(stdStr);
		}

	}
}

void Gui_CBITableDictionary::TableToInterface()
{
	//1.�г��źŻ�
	ui->textEdit_TrainSingal->setText(getDistorageQ(CBI_TableDictionary::Field::�г��źŻ�����));
	ui->textEdit_ShuntingSingal->setText(getDistorageQ(CBI_TableDictionary::Field::�����źŻ�����));
	ui->textEdit_TrainButton->setText(getDistorageQ(CBI_TableDictionary::Field::�г���ť����));
	ui->textEdit_ShuntingButton->setText(getDistorageQ(CBI_TableDictionary::Field::������ť����));
	ui->textEdit_TrunOut->setText(getDistorageQ(CBI_TableDictionary::Field::��������));
	ui->textEdit_Track->setText(getDistorageQ(CBI_TableDictionary::Field::��������));
	ui->textEdit_SingalView->setText(getDistorageQ(CBI_TableDictionary::Field::����ʾ��ʽ));
	ui->textEdit_OtherLock->setText(getDistorageQ(CBI_TableDictionary::Field::��������));


	
	if (Ptr_CbiTbDictionary->spOverrun)
	{
			ui->checkBox_SpSection->setCheckState(Qt::CheckState::Checked);
			ui->textEdit_SpSwitch->setText(getDistorageQ(CBI_TableDictionary::Field::��������));
	}else
	{
			ui->checkBox_SpSection->setCheckState(Qt::CheckState::Unchecked);
	}

	if (Ptr_CbiTbDictionary->spProtectFollow)
	{
		ui->checkBox_useSpSwitch->setCheckState(Qt::CheckState::Checked);
		ui->textEdit_SpSection->setText(getDistorageQ(CBI_TableDictionary::Field::��������));
	}else
	{
		ui->checkBox_useSpSwitch->setCheckState(Qt::CheckState::Unchecked);
	}


	switch (Ptr_CbiTbDictionary->m_conditionSingleSwitch)
	{
	case 0: 
		{
			ui->comboBox_PointSingalCondition->setCurrentIndex(0);
		}break;
	case 1:
		{
			ui->comboBox_PointSingalCondition->setCurrentIndex(1);
		}break;
	case 2:
		{
			ui->comboBox_PointSingalCondition->setCurrentIndex(2);
		}break;
	default:
		{

			ui->comboBox_PointSingalCondition->setCurrentIndex(0);
		}
		break;
	}



	update();
}

void Gui_CBITableDictionary::InterfaceToTable()
{
	setDistorageQ(CBI_TableDictionary::Field::�г��źŻ�����,ui->textEdit_TrainSingal->toPlainText());
	setDistorageQ(CBI_TableDictionary::Field::�����źŻ�����,ui->textEdit_ShuntingSingal->toPlainText());
	setDistorageQ(CBI_TableDictionary::Field::�г���ť����,ui->textEdit_TrainButton->toPlainText());
	setDistorageQ(CBI_TableDictionary::Field::������ť����,ui->textEdit_ShuntingButton->toPlainText());
	setDistorageQ(CBI_TableDictionary::Field::��������,ui->textEdit_TrunOut->toPlainText());
	setDistorageQ(CBI_TableDictionary::Field::��������,ui->textEdit_Track->toPlainText());
	setDistorageQ(CBI_TableDictionary::Field::����ʾ��ʽ,ui->textEdit_SingalView->toPlainText());
	setDistorageQ(CBI_TableDictionary::Field::��������,ui->textEdit_OtherLock->toPlainText());
	
	if (ui->checkBox_SpSection->checkState() == Qt::CheckState::Checked)
	{
		Ptr_CbiTbDictionary->spOverrun = true;
		setDistorageQ(CBI_TableDictionary::Field::��������,ui->textEdit_SpSection->toPlainText());
	}else
	{
		Ptr_CbiTbDictionary->spOverrun = false;
		setDistorageQ(CBI_TableDictionary::Field::��������,"");
	}
	
	if (ui->checkBox_useSpSwitch->checkState() == Qt::CheckState::Checked)
	{
		Ptr_CbiTbDictionary->spProtectFollow = true;
		setDistorageQ(CBI_TableDictionary::Field::��������,ui->textEdit_SpSwitch->toPlainText());
	}else
	{
		Ptr_CbiTbDictionary->spProtectFollow = false;
		setDistorageQ(CBI_TableDictionary::Field::��������,"");
	}

	TableToInterface();
}

std::vector<std::string> Gui_CBITableDictionary::getDistorage(int num /*= -1*/)
{
	return Ptr_CbiTbDictionary->getDistorage(num);
}

QString Gui_CBITableDictionary::getDistorageQ(int num)
{
	return VecStd2QString(Ptr_CbiTbDictionary->getDistorage(num));
}

void Gui_CBITableDictionary::setDistorage(int num,const std::vector<std::string> &input)
{
	Ptr_CbiTbDictionary->setDistorage(num,input);
}

void Gui_CBITableDictionary::setDistorageQ(int num,const QString &input)
{
	QStringList Qsl;

	Qsl = (input + "\r\n").split('\n');
	std::vector<std::string> vStr;
	
	QstrList2VecStd(Qsl,vStr);

	setDistorage(num,vStr);

}

void Gui_CBITableDictionary::slot_OtherLockSelect(int num)
{
	switch(num)
	{
		case 0:  //ƴ�����
			{
				ui->textEdit_OtherLock->setText(u_setDic.OtherLockPinYinSample);
			}
			break;

		case 1: //ƴ��ȫ��
			{
				ui->textEdit_OtherLock->setText(u_setDic.OtherLockPinYinFull);
			}break;

		case 2: //����
			{
				ui->textEdit_OtherLock->setText(U8s(u_setDic.OtherLockChinese.data()));
			}
			break;


		default:
			qFatal(U8c("û�������Ŀ")); break;
	}
}

void Gui_CBITableDictionary::slot_signalLightSet(int num)
{
	switch(num)
	{
	case 0:  //ƴ�����
		{
			ui->textEdit_SingalView->setText(u_setDic.LightSetStandard);
		
		}
		break;

	case 1: //ƴ��ȫ��
		{
			ui->textEdit_SingalView->setText(u_setDic.LightSetStandard + \
											U8s(u_setDic.LightSetChinese.data()));
		}break;

	default:
		qFatal(U8c("û�������Ŀ")); break;
	}
}

void Gui_CBITableDictionary::slot_fileDir()
{
	QString s = QFileDialog::getExistingDirectory(this,U8s("ѡ������ļ���"),"/");

	if (!s.isEmpty())
	{
		ui->lineEdit_ConfigPath->setText(s);
	}
}

void Gui_CBITableDictionary::slot_setConditionSingleSwitch(int bl)
{

	Ptr_CbiTbDictionary->setConditionSingleSwitch(bl);

}








