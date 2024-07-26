#include "GlobalSettingQt.h"
#include <opencv.hpp>
#include <QSettings>
#include <QtGui/QtGui>

using namespace std;


SettingQt::SettingQt()
{
	g_qstrIntrfcStyle = "windowsXp";

	g_blToolBarMovable = true;
	g_numToolBarSize = 48;

	g_blGpcWgtBackground = true;
	g_qstrBlockBackgroundImgPath = ".\\bj.jpg";

	g_qclrGpcFtColor = QColor(255,255,255);
	g_qclrGpcBkColor = QColor(255,255,255);

	g_szDwStGui = QSize(300,300);
	g_szGuiMdiMini = QSize(200,200);
	g_nGuiMdiNormalMode = 2;

	g_nGpcFtDepth = 0;
	g_nGpcBkDepth = 0;

	g_blFltAuto = true;
}

QColor SettingQt::getGpcFtColor() const
{
	return QColor(g_qclrGpcFtColor.red(),
					g_qclrGpcFtColor.green(),
					g_qclrGpcFtColor.blue(),
					g_nGpcFtDepth);
}

QColor SettingQt::getGpcBkColor() const
{
	return QColor(g_qclrGpcBkColor.red(),
		g_qclrGpcBkColor.green(),
		g_qclrGpcBkColor.blue(),
		g_nGpcBkDepth);
}

///////////////////////////////////////////////////////////



SettingGlobal::SettingGlobal()
{
	readType.push_back("*.tif");
	readType.push_back("*.tiff");

	readType.push_back("*.jpg");
	readType.push_back("*.png");

	readType.push_back("*.bmp");
	readType.push_back("*.jpeg");

	readType.push_back("*.gif");

	SplitChar = "-";

	SpliterOpen = false;
}



///////////////////////////////////////////////////////////////


bool SettingSaveTable::AutoOutPut = false;
QString SettingSaveTable::ExcelName = "Result.xlsx";
bool SettingSaveTable::ResultPictureOutput = false;


SettingSaveTable::SettingSaveTable()
{
	//编译器不让写任何东西,但是为了安全编程必须有
}

SettingSaveTable::~SettingSaveTable()
{
	
}


#define ConfigPath (QCoreApplication::applicationDirPath( ) + "\\Config.ini")

void SettingSaveTable::read()
{
	QSettings Qset(ConfigPath,QSettings::Format::IniFormat);
	Qset.beginGroup("Output");
	
	AutoOutPut = Qset.value("AutoOutput",false).toBool();

	ResultPictureOutput = Qset.value("ResultPicture",false).toBool();

	Qset.endGroup();
}

void SettingSaveTable::write()
{
	QSettings Qset(ConfigPath,QSettings::Format::IniFormat);
	Qset.beginGroup("Output");

	Qset.setValue("AutoOutput",AutoOutPut);

	Qset.endGroup();
}






/////////////////////////////////////////////////////////////////
SettingDictionary::SettingDictionary()
{
	OtherLockPinYinSample += "CL\r\n";
	OtherLockPinYinSample += "BS\r\n";
	OtherLockPinYinSample += "Y\r\n";

	OtherLockPinYinFull += "CL\r\n";
	OtherLockPinYinFull += "BS\r\n";
	OtherLockPinYinFull += "Y\r\n";

	OtherLockPinYinFull += "CJLX\r\n";
	OtherLockPinYinFull += "ZDBS\r\n";
	OtherLockPinYinFull += "YX\r\n";

	OtherLockChinese += "场联\r\n";
	OtherLockChinese += "延续\r\n";
	OtherLockChinese += "闭塞\r\n";
	OtherLockChinese += "场间联系\r\n";
	OtherLockChinese += "延续进路\r\n";
	OtherLockChinese += "自动闭塞\r\n";


	LightSetStandard += "L\r\n";
	LightSetStandard += "LU\r\n";
	LightSetStandard += "U\r\n";
	LightSetStandard += "UU\r\n";
	LightSetStandard += "USU\r\n";
//	LightSetStandard += "LL\r\n";
	LightSetStandard += "B\r\n";

	LightSetStandard += "L\\L\r\n";
	LightSetStandard += "L\\LU\\U\r\n";

	LightSetStandard += "L/L\r\n";
	LightSetStandard += "L/LU/U\r\n";
	
	LightSetChinese = "L或LU或U\r\n";
}


