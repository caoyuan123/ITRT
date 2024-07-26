#pragma once
#include "StdAfx.h"
#include <QString>
#include <QColor>
#include <QWidget>
#include <QStringList>

static class SettingQt  //Qt界面相关设置
{
public:
	QString g_qstrIntrfcStyle;   //界面风格
	bool g_blToolBarMovable;  //工具条锁定
	unsigned char g_numToolBarSize ; //工具栏大小 默认 48


	bool g_blGpcWgtBackground;  //ture为使用图片
	QString g_qstrBlockBackgroundImgPath;  //空白背景图片存储地址

	QColor g_qclrGpcFtColor; // 图像控件前景颜色 
	QColor g_qclrGpcBkColor; // 图像控件后景颜色

	byte g_nGpcFtDepth;   //图像控件前景深度
	byte g_nGpcBkDepth;   //图像控件后景深度


public:
	QSize g_szDwStGui;   //主界面Sheet缩略图大小
	QSize g_szGuiMdiMini;  //主界面mdi中窗口最小大小
	byte  g_nGuiMdiNormalMode; //窗口默认开启模式 0-最小 1-窗口化最小 2-最大化

public:

	bool g_blFltAuto;   //滤波器是否自动滤波

public:
	QColor getGpcFtColor() const;
	QColor getGpcBkColor() const;
	SettingQt();

}u_setQt;


static class SettingGlobal  //全局设置
{
public:
	QStringList readType;   //读取的图像类型
	QString SplitChar;      //分割字符串类型
	bool SpliterOpen;       //支持分隔图片

	SettingGlobal();
}u_setGlobal;


static class SettingSaveTable  //ocr相关设置
{
public:
	static QString ExcelName;

	static bool AutoOutPut;

	static bool ResultPictureOutput;

	SettingSaveTable();
	~SettingSaveTable();

	static void read( );
	static void write( );


}u_setSaveTable;


static class SettingDictionary  //ocr相关设置
{
public:
	QString OtherLockPinYinSample;   //其他联锁的拼音简称目录
	QString OtherLockPinYinFull;     //其他联锁的拼音全称目录
	std::string OtherLockChinese;       //其他联锁的中文的目录

	QString LightSetStandard;   //灯显示的标准模式
	std::string LightSetChinese;   //灯显示的含中文模式

	SettingDictionary();



}u_setDic;



