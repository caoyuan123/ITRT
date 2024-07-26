#ifndef GUIDICTIONARY_H
#define GUIDICTIONARY_H

#include <qdialog.h>
#include "CBi_TableInterface.h"


namespace Ui {
	class DictionaryDialog;
} // namespace Ui

class CBI_TableDictionary;  //��������

class QString;

//�ֵ����˳�㸺����Զ�ȡ�ļ���
class Gui_CBITableDictionary : public QDialog, public IfDictionary
{
	Q_OBJECT
protected:
	bool selfPtr;   //ָʾָ���Ƿ�Ϊ˽��
	CBI_TableDictionary *Ptr_CbiTbDictionary;  //�洢�Ĳ��������ָ��
	

public:
	Gui_CBITableDictionary(QWidget *parent);
	~Gui_CBITableDictionary();

public:
	
	CBI_TableDictionary* DictionaryPtr();
	void setDictionaryPtr(CBI_TableDictionary *Ptr);  //���ú� ָ��Ϊ��Դ

	virtual std::vector<std::string> getDistorage(int num );
	virtual QString getDistorageQ(int num);
	
protected slots:
	void testButtom( );    //�������
	void setDistorage(int num,const std::vector<std::string> &input);
	void setDistorageQ(int num,const QString &input);
	

	//ѡ��������Ŀ
	void slot_OtherLockSelect(int num);   //��������������
	void slot_signalLightSet(int num);          //����ʾ������
	void slot_fileDir(); //��ť�Զ�Ѱ��
	void slot_setConditionSingleSwitch(int);  //��������������

public slots:

	void accept();
	virtual void slot_GenerateButtom();
	virtual void slot_readDev(std::string configPath = "");

	void TableToInterface();  //���ڲ��洢��Table��ʾ��������
	void InterfaceToTable();  //��ʾ����洢��Table��

	

Q_SIGNALS:

private:
	Ui::DictionaryDialog *ui;
};

#endif // GUIDICTIONARY_H





