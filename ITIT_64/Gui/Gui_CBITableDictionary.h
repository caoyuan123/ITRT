#ifndef GUIDICTIONARY_H
#define GUIDICTIONARY_H

#include <qdialog.h>
#include "CBi_TableInterface.h"


namespace Ui {
	class DictionaryDialog;
} // namespace Ui

class CBI_TableDictionary;  //操作对象

class QString;

//字典界面顺便负责控显读取的加载
class Gui_CBITableDictionary : public QDialog, public IfDictionary
{
	Q_OBJECT
protected:
	bool selfPtr;   //指示指针是否为私有
	CBI_TableDictionary *Ptr_CbiTbDictionary;  //存储的操作对象的指针
	

public:
	Gui_CBITableDictionary(QWidget *parent);
	~Gui_CBITableDictionary();

public:
	
	CBI_TableDictionary* DictionaryPtr();
	void setDictionaryPtr(CBI_TableDictionary *Ptr);  //设置后 指针为外源

	virtual std::vector<std::string> getDistorage(int num );
	virtual QString getDistorageQ(int num);
	
protected slots:
	void testButtom( );    //方便测试
	void setDistorage(int num,const std::vector<std::string> &input);
	void setDistorageQ(int num,const QString &input);
	

	//选择的配合项目
	void slot_OtherLockSelect(int num);   //其他联锁的配置
	void slot_signalLightSet(int num);          //灯显示的配置
	void slot_fileDir(); //按钮自动寻找
	void slot_setConditionSingleSwitch(int);  //设置条件道岔单动

public slots:

	void accept();
	virtual void slot_GenerateButtom();
	virtual void slot_readDev(std::string configPath = "");

	void TableToInterface();  //将内部存储的Table显示到界面上
	void InterfaceToTable();  //显示界面存储到Table上

	

Q_SIGNALS:

private:
	Ui::DictionaryDialog *ui;
};

#endif // GUIDICTIONARY_H





