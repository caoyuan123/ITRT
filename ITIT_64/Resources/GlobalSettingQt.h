#pragma once
#include "StdAfx.h"
#include <QString>
#include <QColor>
#include <QWidget>
#include <QStringList>

static class SettingQt  //Qt�����������
{
public:
	QString g_qstrIntrfcStyle;   //������
	bool g_blToolBarMovable;  //����������
	unsigned char g_numToolBarSize ; //��������С Ĭ�� 48


	bool g_blGpcWgtBackground;  //tureΪʹ��ͼƬ
	QString g_qstrBlockBackgroundImgPath;  //�հױ���ͼƬ�洢��ַ

	QColor g_qclrGpcFtColor; // ͼ��ؼ�ǰ����ɫ 
	QColor g_qclrGpcBkColor; // ͼ��ؼ�����ɫ

	byte g_nGpcFtDepth;   //ͼ��ؼ�ǰ�����
	byte g_nGpcBkDepth;   //ͼ��ؼ������


public:
	QSize g_szDwStGui;   //������Sheet����ͼ��С
	QSize g_szGuiMdiMini;  //������mdi�д�����С��С
	byte  g_nGuiMdiNormalMode; //����Ĭ�Ͽ���ģʽ 0-��С 1-���ڻ���С 2-���

public:

	bool g_blFltAuto;   //�˲����Ƿ��Զ��˲�

public:
	QColor getGpcFtColor() const;
	QColor getGpcBkColor() const;
	SettingQt();

}u_setQt;


static class SettingGlobal  //ȫ������
{
public:
	QStringList readType;   //��ȡ��ͼ������
	QString SplitChar;      //�ָ��ַ�������
	bool SpliterOpen;       //֧�ַָ�ͼƬ

	SettingGlobal();
}u_setGlobal;


static class SettingSaveTable  //ocr�������
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


static class SettingDictionary  //ocr�������
{
public:
	QString OtherLockPinYinSample;   //����������ƴ�����Ŀ¼
	QString OtherLockPinYinFull;     //����������ƴ��ȫ��Ŀ¼
	std::string OtherLockChinese;       //�������������ĵ�Ŀ¼

	QString LightSetStandard;   //����ʾ�ı�׼ģʽ
	std::string LightSetChinese;   //����ʾ�ĺ�����ģʽ

	SettingDictionary();



}u_setDic;



