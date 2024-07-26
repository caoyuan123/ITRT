#pragma once
#include <QVariantList>
#include <QString>


struct IfExcel  //���ӱ��ӿ�
{
	virtual bool initialize(bool visible) = 0;  //��ʼ��,�Ƿ��Զ����ļ�
	virtual void dispose() = 0;   //�˳�

	//·��/�Ƿ��½�
	virtual bool open(const QString &xls_file, bool not_exist_new = true ) = 0;  


	virtual void save() = 0;   	//����xls
	virtual void saveAs(const QString &xls_file) = 0; //���Ϊ
	virtual void close() = 0;  //�ر�xls
	
	virtual void addSheet(const QString &sheet_name) = 0; //����һҳ
	
public:	//sheet����
	virtual bool loadSheet(int sheet_index,bool pre_read = false) = 0;  //��ĳ��Sheet��Ϊ��
	virtual bool loadSheet(const QString &sheet_name, bool pre_read = false) = 0;

	//��һ��RANGE��д��sheet�����ݣ�QVariantList��һ��table��Ҳ����List��List
	virtual bool setRangeCell(int cell1_row,int cell1_column,
		int cell2_row,int cell2_column,
		const QVariantList &data_table) = 0;

	virtual bool setCell(int row, int column, const QVariant &data) = 0;

	virtual int EngineType() = 0;

	virtual bool outPutStdTableHead92(const QString &sheet_name) = 0;

	//��һ��RANGE��д��sheet�����ݣ�table���Զ��ģ�ֻҪ
	virtual bool setRangeCell(int cell1_row,int cell1_column,
		const QList<QStringList> &data_table)
		{
			return false;
		};  //�Ǳ���ӿ�
 

};



