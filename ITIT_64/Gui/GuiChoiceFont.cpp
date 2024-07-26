#include "GuiChoiceFont.h"
#include "ui_GuiFontChoice.h"

#include "FontPictureDef.hpp"
#include <QtGui/QtGui>
#include <QFontDialog>
#include <QMessageBox>
#include "DrawingBook.h"
#include "DrawingSheet.h"
#include "DrawingType.h"

#include "FontGeneraterQ.h"
#include "CBI_TableDictionary.h"
#include <cxcore.hpp>
#include "GuiSimilar.h"

#include "FontSimilar.h"
#include "MessyCodeTerminator.h"
#include "FontGenerater.h"
#include "ImgProc.h"
#include "QtCv.h"
#include "FontSimilar.h"

GuiChoiceFont::GuiChoiceFont(QWidget *parent)
	: QDialog(parent)
{
	ui = new Ui::IkChoiceFontClass;
	ui->setupUi(this);
	
	selfProprety = true;
	m_WordProperty = new FontWordProprety( );

	m_WordProperty->FontCompressRatio = 0.7;
	m_WordProperty->FontSpacing = 1.01;

	m_FontGeneraterFull = new FontPictureGenerater( );

	Ptr_DrawingBook = nullptr;
	Ptr_DwTypeList = nullptr;

	m_GuiSimilar = new GuiSimilar( );
	
	ui->frame_2->setHidden(true);
	ui->frame_3->setHidden(true);

	ImgProcInit( );
	GuiInit( );

	connect(ui->comboBox_DwType,SIGNAL(activated(int)),this,SLOT(slot_usingDrawingType(int)));
	connect(ui->pushButton_Similar,SIGNAL(clicked( )), this , SLOT(slot_simClick( )));
	connect(m_GuiSimilar,SIGNAL(signal_calResult(void*)),this,SLOT(slot_writeFontWordPrep(void*)));

	m_Sample = cv::noArray().getMat();
	m_Tample = cv::noArray().getMat();
	m_StrSample = "";

	slot_Prop2SetGui( );
	slot_updateAllImg( );
}

void GuiChoiceFont::GuiInit()
{
	m_FontDialog = new QFontDialog(m_Font,this);

	m_FontDialog->setModal(true);

	ui->verticalFrame_3->setVisible(false);
	ui->verticalFrame_4->setVisible(false);

	connect(ui->pushButton_DicShow,SIGNAL(clicked()),this,SIGNAL(signal_calDictionayDialog()));

	connect(ui->textSemple1,SIGNAL(textEdited(QString)),this,SLOT(slot_ShowString1(QString)));
	connect(ui->textSemple2,SIGNAL(textEdited(QString)),this,SLOT(slot_ShowString2(QString)));
	connect(ui->textSemple3,SIGNAL(textEdited(QString)),this,SLOT(slot_ShowString3(QString)));
	connect(ui->textSemple4,SIGNAL(textEdited(QString)),this,SLOT(slot_ShowString4(QString)));

	connect(ui->doubleSpinBox_Stretch,SIGNAL(valueChanged(double)),this,SLOT(slot_setStretch(double)));
	connect(ui->doubleSpinBox_Space,SIGNAL(valueChanged(double)),this,SLOT(slot_setWordSpacing(double)));

	connect(ui->IkBtmFontDia,SIGNAL(clicked()),this,SLOT(slot_FontDialog()));
	connect(m_FontDialog,SIGNAL(accepted()),this,SLOT(slot_FontDialog_Accept()));

	connect(ui->pushButtom_SaveNewType,SIGNAL(clicked()),this,SLOT(slot_saveNewType()));

	//connect(ui->pushButton_MakeAll,SIGNAL(clicked( )),this,SLOT(slot_GenerateFontPicture( )));
}

void GuiChoiceFont::ImgProcInit()
{
	m_Font.setFamily(tr("txt"));
	m_Font.setPointSize(36);
	m_Font.setBold(true);


	m_FontGenerater = new FontGeneraterQ( );  //��ʱ��ֻ֧��Q��һ��
}

GuiChoiceFont::~GuiChoiceFont()
{

	if (selfProprety)
	{
		if (m_WordProperty != nullptr)
		{
			delete(m_WordProperty);
		}
	}

	if (m_FontGenerater != nullptr)
	{
		delete(m_FontGenerater);
	}

	if (m_GuiSimilar != nullptr)
	{
		delete(m_GuiSimilar);
	}

	if (m_FontGeneraterFull != nullptr)
	{
		delete(m_FontGeneraterFull);
	}

	delete(ui);
}

void GuiChoiceFont::slot_setDrawingBook(DrawingBook* _Tar)
{
	if (_Tar != nullptr)
	{
	
		Ptr_DrawingBook = _Tar;

		if (selfProprety)
		{
			delete(m_WordProperty);
			selfProprety = false;
		}

		m_WordProperty = nullptr; //��Դ��Ҳ���

		slot_Prop2SetGui();
	}else
	{
		Ptr_DrawingBook = nullptr;

		if (selfProprety == false)
		{
			selfProprety = true;
			m_WordProperty = new FontWordProprety;

			slot_Prop2SetGui();
		}
	}

	calSheetList(_Tar);
}

void GuiChoiceFont::slot_setDrawingTypeList(DrawingTypeList *_Tar)
{
	if (_Tar != nullptr)
	{
		Ptr_DwTypeList = _Tar;

		ui->comboBox_DwType->clear();

		int sz = Ptr_DwTypeList->size();

		for (int i = 0;i < sz; ++i)
		{
			QString qstr = U8s((*Ptr_DwTypeList)[i].strDesignInstitute.data()) ;
			qstr += "-";
			qstr += U8s((*Ptr_DwTypeList)[i].strLineName.data()) ;
			ui->comboBox_DwType->addItem(qstr);
		}
	}
}

void GuiChoiceFont::slot_usingDrawingType(int choiceNum)
{
	if (Ptr_DrawingBook != nullptr)
	{
		Ptr_DrawingBook->m_DrawingType = (*Ptr_DwTypeList)[choiceNum];
	}

	setGui((*Ptr_DwTypeList)[choiceNum]);
}


void GuiChoiceFont::slot_setFontWord(FontWordProprety* FontProprety)
{
	if (FontProprety == nullptr)
	{
		if (!selfProprety)
		{

			selfProprety = true;
			m_WordProperty = new FontWordProprety();
			slot_updateAllWordProp();

		}else;  //�Ѿ����Լ��ľ�����ν��

	}else
	{

		if (selfProprety)
		{
			if (m_WordProperty != nullptr)
			{
				delete(m_WordProperty);
				m_WordProperty = nullptr;
			}
		} else;
		

		m_WordProperty = FontProprety;
		slot_updateAllWordProp();
	}
}

void GuiChoiceFont::slot_updateAllWordProp()
{
	//ͬ��ȫ�����


	//update();
}

void GuiChoiceFont::slot_updateAllImg()
{
	//����������������ͼƬ
	m_FontGenerater->setFontProperty(*getWordProprety());

	slot_ShowString1(ui->textSemple1->text());
	slot_ShowString2(ui->textSemple2->text());
	slot_ShowString3(ui->textSemple3->text());
	slot_ShowString4(ui->textSemple4->text());

	slot_updateSimShow();
}

void GuiChoiceFont::slot_FontDialog()
{
	m_FontDialog->setCurrentFont(m_Font);
	m_FontDialog->show();

	
}

void GuiChoiceFont::slot_FontDialog_Accept()
{

	m_Font = m_FontDialog->currentFont();

}

void GuiChoiceFont::slot_setStretch(double Roit)
{
	if (Roit > 0 && Roit < 2)
	{
		//m_Font.setStretch(Roit * 100);
		getWordProprety( )->FontCompressRatio = Roit;
		
		slot_updateAllImg( );
	}
}

void GuiChoiceFont::slot_setWordSpacing(double Spacing)
{
	if (Spacing > 0 && Spacing < 2)
	{
		//m_Font.setLetterSpacing(QFont::PercentageSpacing,Spacing * 100);
		getWordProprety( )->FontSpacing = Spacing;

		slot_updateAllImg( );
	}
}

FontWordProprety* GuiChoiceFont::getWordProprety()
{
	if (selfProprety)
	{
		return m_WordProperty;
	}
	else if (Ptr_DrawingBook != nullptr)
	{
		return &(Ptr_DrawingBook->getDrawingType( )->_FontProprety);
	}
	else if (m_WordProperty != nullptr)
	{
		return m_WordProperty;
	}
	else
	{
		qFatal("getWordProprety ERROR!!");
		return nullptr;
	}
}

void GuiChoiceFont::slot_ShowString1(QString str)
{
	ui->label_S1->setPixmap(QPixmap::fromImage(m_FontGenerater->getOrlFontImageQt(str)));
}

void GuiChoiceFont::slot_ShowString2(QString str)
{
	ui->label_S2->setPixmap(QPixmap::fromImage(m_FontGenerater->getOrlFontImageQt(str)));
}

void GuiChoiceFont::slot_ShowString3(QString str)
{
	ui->label_S3->setPixmap(QPixmap::fromImage(m_FontGenerater->getOrlFontImageQt(str)));
}

void GuiChoiceFont::slot_ShowString4(QString str)
{
	ui->label_S4->setPixmap(QPixmap::fromImage(m_FontGenerater->getOrlFontImageQt(str)));
}

void GuiChoiceFont::slot_SetGui2Prop()
{
	getWordProprety()->setFormQfont(m_Font);


	//����ͳһ һ�µ�
	//getWordProprety()->MarginX = ui->doubleSpinBox_offsetX;  
	//getWordProprety()->MarginY = ui->spinBox_offsetY;

}

void GuiChoiceFont::slot_Prop2SetGui()
{
	getWordProprety()->setQfont(m_Font);

	ui->doubleSpinBox_Space->setValue(getWordProprety()->FontSpacing);
	ui->doubleSpinBox_Stretch->setValue(getWordProprety()->FontCompressRatio);
}

void GuiChoiceFont::slot_GenerateFontPicture()
{
	Ptr_DrawingBook->m_Dictionary->slot_GenerateFontPicture();
}

void GuiChoiceFont::setGui(const DrawingType & _dt)
{
	const FontWordProprety *fwp = & _dt._FontProprety;
	const FontGraphicTransTemp *fgtt = & _dt._GraphicTransTemp;

	//���������õĽ�������
	ui->doubleSpinBox_Stretch->setValue(fwp->FontCompressRatio);
	ui->doubleSpinBox_Space->setValue(fwp->FontSpacing);

	_dt._FontProprety.setQfont(m_Font);

	ui->spinBox_offsetX->setValue(fwp->MarginX);
	ui->spinBox_offsetY->setValue(fwp->MarginY);

	ui->spinBox_E->setValue(fgtt->ErodeRoit);
	ui->spinBox_G->setValue(fgtt->GlussRoit);
	ui->spinBox_P->setValue(fgtt->PowRoit);
	ui->doubleSpinBox_R->setValue(fgtt->Roit);
	

	//update();
}

void GuiChoiceFont::getGui(DrawingType& _dt)
{

	_dt._FontProprety.setFormQfont(m_Font);

	_dt._FontProprety.FontCompressRatio  = ui->doubleSpinBox_Stretch->value();
	_dt._FontProprety.FontSpacing  = ui->doubleSpinBox_Space->value();

	_dt._FontProprety.MarginX = ui->spinBox_offsetX->value();
	_dt._FontProprety.MarginY = ui->spinBox_offsetY->value();

	_dt._GraphicTransTemp.ErodeRoit = ui->spinBox_E->value();
	_dt._GraphicTransTemp.GlussRoit = ui->spinBox_G->value();
	_dt._GraphicTransTemp.PowRoit = ui->spinBox_P->value();
	_dt._GraphicTransTemp.Roit = ui->doubleSpinBox_R->value();
	

	//����һ����������,����һ��j�ĳ�����Ϊ �ָ߱�׼���������ȡ�ָߵ� 0.3��

}

void GuiChoiceFont::calSheetList(DrawingBook * ptr)
{
	if (ptr == nullptr)
	{
		//���ѡ��
		ui->comboBox_SheetSelect->clear();

	}else
	{
		//��Sheetҳ��
		ui->comboBox_SheetSelect->clear();

		int Page_sz = ptr->m_Pages.size();

		for (int i = 0;i < Page_sz; i++)
		{
			ui->comboBox_SheetSelect->addItem(ptr->m_Pages[i]->getName());
		}
		
	}
}

void GuiChoiceFont::slot_simClick( ) //���
{
	//��ѡ��ĺ�д����
	int idx = ui->comboBox_SheetSelect->currentIndex( );

	if (Ptr_DrawingBook != nullptr)
	{

		if (Ptr_DrawingBook->m_Pages[idx]->flags_Process.getProcess(DwSt_ProcFlags_�˲�))
		{
			cv::Mat m = Ptr_DrawingBook->m_Pages[idx]->getMat( );

			m_GuiSimilar->setBackGround(m);
			m_GuiSimilar->slot_InitShow( );
			//m_GuiSimilar->show();

			m_GuiSimilar->setManualFontName(m_Font.family(), m_Font.bold());

			m_GuiSimilar->m_Tools->show();

			//�����д��̨��
			//	connect(m_GuiSimilar,SIGNAL(),this,SLOT(slot_writeFontWordPrep()));

		}else
		{
			QMessageBox::warning(nullptr,U8s("����"),U8s("����ͼƬ��δ�����˲������޷�������ϣ�"));
		}
	}
}

void GuiChoiceFont::slot_writeFontWordPrep(void* vd)
{
	FontSimilar* fontSim = (FontSimilar*)vd; 
	
	DrawingType dt;

	fontSim->FontPropertyFinal.setDrawingType(dt);

	setGui(dt);

	m_Sample = fontSim->List_FontSelect[0]._Mat8u1c;
	m_StrSample = U8c(fontSim->List_FontSelect[0].words.data());

	if (Ptr_DrawingBook != nullptr)
	{
		Ptr_DrawingBook->m_DrawingType = dt;
	}

}

void GuiChoiceFont::slot_saveNewType( )
{
	QString designName = ui->lineEdit_DesignName->text().trimmed();
	QString lineName = ui->lineEdit_LineName->text().trimmed();

	//1.Ҫ�� ���Ժ���� �� ��·���� �������
	if (designName == "" || lineName == "")
	{

		QMessageBox mb(U8c("δ�ܱ���"),U8c("���Ժ���ƺ���·���Ʊ�����д") ,QMessageBox::Icon::Warning,QMessageBox::StandardButton::Ok,0,0);

		mb.exec( );
	}
	//2.����֮ǰ������
	else if (Ptr_DwTypeList->checkRepeatName(designName,lineName))
	{
	
		QMessageBox mb(U8c("δ�ܱ���"),U8c("��֮ǰԤ���������,���ֹ�ɾ������yaml�е�Ԥ������") ,QMessageBox::Icon::Warning,QMessageBox::StandardButton::Ok,0,0);

		mb.exec( );
	}
	//3.���Ժ�����в�Ӧ���������ַ�
	else if (false)
	{

		//��û��

	}else
	{

	//4.��ӵ�Ԥ���� ����������һ�� typeList


		if (Ptr_DrawingBook != nullptr)
		{

			if (Ptr_DrawingBook->getDrawingType()->_GraphicTransTemp.CompareRoit > 0.2373)  //��Ҫ75������
			{
				DrawingType drawType;

				drawType = Ptr_DrawingBook->m_DrawingType;
				
				getGui(drawType);

				drawType.strDesignInstitute = designName.toLocal8Bit();
				drawType.strLineName = lineName.toLocal8Bit();

				Ptr_DwTypeList->append(drawType);
				Ptr_DwTypeList->save();

				QMessageBox mb(U8c("����ɹ�"),U8c("����ɹ�!") ,QMessageBox::Icon::Information,QMessageBox::StandardButton::Ok,0,0);

				mb.exec( );

				//ȱ��һ��ˢ��List��
				slot_setDrawingTypeList(Ptr_DwTypeList);
			
			}
			else
			{
				QMessageBox mb(U8c("δ�ܱ���"),U8c("��Ϻ���ܱ������") ,QMessageBox::Icon::Warning,QMessageBox::StandardButton::Ok,0,0);
				mb.exec( );
			}

		}
		else
		{
			QMessageBox mb(U8c("δ�ܱ���"),U8c("�������Ŀ�ļ�") ,QMessageBox::Icon::Warning,QMessageBox::StandardButton::Ok,0,0);
			mb.exec( );
		}



	}


}

void GuiChoiceFont::slot_updateSimShow()
{

#ifdef _DEBUG
	if (!m_Sample.empty( ))
	{
		if (m_StrSample.trimmed( ) != "")
		{
			//���ݵ�ǰ��������������
				
			DrawingType dt;

			getGui(dt);

			cv::Mat wordTemp;

			//��GUI��ʾ�г�ȡ����Ҫ�Ĳ��� //���ظ�������������
					
			m_FontGeneraterFull->setDrawingType(dt);
				//Ԥ��Ľ�ѡͼ

			wordTemp = m_FontGeneraterFull->getFontImageCv(m_StrSample);
			

			//��ѡȡ�ӿ�
			cv::Mat M_big;

			double rs = ImgProc::MatCompare(m_Sample,wordTemp);

			//�����ʾ����������

			double orlRs = 0.00f;
			orlRs = pow(rs,0.2);

			orlRs *= 100;

			char strC[20]; 

			//printf("%2f",orlRs);

			sprintf(strC,"%.2f%",orlRs);
			

			ui->label_Similar->setText(U8c(strC));

			ui->label_selectSample->setPixmap(QPixmap::fromImage(QtCv::cvMat2Qimg(m_Sample)));
			ui->label_selectTemp->setPixmap(QPixmap::fromImage(QtCv::cvMat2Qimg(wordTemp)));
		}
	}

#else

#endif



}

void GuiChoiceFont::slot_pushOKButton()
{
	//�������еĲ���ȡ��


	//д����Ŀ��


}





