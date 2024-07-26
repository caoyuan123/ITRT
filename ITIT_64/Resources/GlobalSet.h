#pragma once

#include <QFrame>
#include <vector>
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include "GlobalSetting.h"
#include "GlobalSettingQt.h"
#include <QSettings>

using namespace std;
class GlobalSet;


namespace cv
{
	class FileStorage;
	class FileNode;
}

extern bool IniReadFunc(QIODevice &device, QSettings::SettingsMap &settingsMap);
bool IniWriteFunc(QIODevice &device, const QSettings::SettingsMap &settingsMap);

class Config
{
public:
	QString settingFileName;  //�����ļ�������

	void readSettingQt();

	Config();
};




class GUIListSet    //��װ�õ�һ����Ŀ��ѡ��
{
	friend class GlobalSet;
protected:
	std::string XmlSet;                              //��Xml�ĵ��ж����Ĭ��ѡ��
	 
	QLabel* labelName;
	QComboBox* comboBox;
	QHBoxLayout* boxlayout;

	QWidget* widget; //�������װ�õ�
public:
	void addItem(std::string item);        //���ñ�ѡ��Ŀ
	void setDefault(std::string Default);            //����Ĭ��ѡ��
	
	 //���Ǳ�ǩ������Ҳ��Qobject������
	GUIListSet(std::string LabelName,QWidget* Parent = nullptr);
	~GUIListSet();

	virtual void setParent(QWidget* Parent);
	virtual QWidget* getGui();

	virtual bool getNextButtom();
	virtual QString getReason();

	virtual void write(cv::FileStorage &fs) const;
	virtual void read(const cv::FileNode &node);    //��ȡĬ��ֵ
};






