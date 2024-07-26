#include "GlobalSetting.h"
#include <QSettings>
#include <QTextCodec>
#include "MessyCodeTerminator.h"
#include <QtGui/QtGui>
#include <omp.h>
#include "GlobalSet.h"


SettingMp::SettingMp( )
{
	useMp = false;    //默认关闭
	MpNums = -1;     //默认
	mainThreadRetain = true;
}

void set_mp_num_Threads(int num)
{
	if (num == 0)
	{
		num = 1;
	}

	omp_set_num_threads(num);

	qDebug() << "Mp set =" << num;
}

void SettingMp::read()
{
	
	QSettings Qset(QCoreApplication::applicationDirPath() + "\\Base.ini",QSettings::Format::IniFormat);
	
	//Qset->setIniCodec(QTextCodec::codecForName("UTF-8"));
	Qset.beginGroup("MultiProcessing");

	useMp = Qset.value(U8c("UseOpenMp"),false).toBool();
	MpNums = Qset.value(U8c("MaxThreadNums"),1).toInt();
	mainThreadRetain = Qset.value(U8c("MainCoreRetain"),true).toBool();

//	Qset.setValue("test",3333);

	Qset.endGroup( );

	if (useMp)
	{
		int MaxThread = omp_get_max_threads();
		
		if (MpNums >= MaxThread || MpNums < 1)
		{
			if (mainThreadRetain)
			{
				set_mp_num_Threads(MaxThread - 1);
			}else
			{
				set_mp_num_Threads(MaxThread);
			}
		}
		else
		{
			set_mp_num_Threads(MpNums);
		}
	}else
	{
		set_mp_num_Threads(1);
	}

}

void SettingMp::write()
{

}

//////////////////////////////////////////////

SettingOpenCV::SettingOpenCV()
{
	cpuType = 0;

	g_blCpuSp     = false;
	g_blSetMMX    = false;
	g_blSetAVX    = false;
	g_blSetAVX2   = false;
	g_blSetSEE2   = false;
	g_blSetSEE4_1 = false;
	g_blSetSEE4_2 = false;
}




SettingCvOCR::SettingCvOCR()
{
	 g_fWidthRoit = 0.9f;  

	 g_fLineRoit = 0.75f;  

	 g_fWordRoit = 0.30f; 

	 g_fMinThresholdRoit = 0.4f;  

	 g_blSaveTempResult = false;

	 g_charSplit = ',';

	 g_blWordPosJudge = true;
}

void SettingCvOCR::read()
{
	QSettings Qset(QCoreApplication::applicationDirPath( ) + "\\Base.ini",QSettings::Format::IniFormat);

	Qset.beginGroup("OCR");

//	Qset.setValue("test",',');

	g_charSplit  = Qset.value("SplitComa",44).toInt();

	qDebug() << U8c("默认输出分隔符号") << g_charSplit;

	Qset.endGroup();
}





