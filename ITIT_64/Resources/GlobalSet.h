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
	QString settingFileName;  //配置文件的名称

	void readSettingQt();

	Config();
};




class GUIListSet    //封装好的一个项目带选项
{
	friend class GlobalSet;
protected:
	std::string XmlSet;                              //从Xml文档中读入的默认选项
	 
	QLabel* labelName;
	QComboBox* comboBox;
	QHBoxLayout* boxlayout;

	QWidget* widget; //最外面封装用的
public:
	void addItem(std::string item);        //设置备选项目
	void setDefault(std::string Default);            //设置默认选项
	
	 //就是标签的名称也是Qobject的名称
	GUIListSet(std::string LabelName,QWidget* Parent = nullptr);
	~GUIListSet();

	virtual void setParent(QWidget* Parent);
	virtual QWidget* getGui();

	virtual bool getNextButtom();
	virtual QString getReason();

	virtual void write(cv::FileStorage &fs) const;
	virtual void read(const cv::FileNode &node);    //读取默认值
};






