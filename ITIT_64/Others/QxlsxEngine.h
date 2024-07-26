#pragma once
#include "InterfaceTable.h"


namespace QXlsx
{
	class Document;
}

class QxlsxEngine : public IfExcel
{
private:
	
	QXlsx::Document * Excel_Doc;

public:
	QxlsxEngine(void);
	~QxlsxEngine(void);

	virtual bool initialize(bool visible);

	virtual void dispose();

	virtual bool open(const QString &xls_file, bool not_exist_new = true);

	 

	virtual void save();

	virtual void saveAs(const QString &xls_file);

	virtual void close();

	virtual void addSheet(const QString &sheet_name);



	virtual bool loadSheet(int sheet_index,bool pre_read = false);

	virtual bool loadSheet(const QString &sheet_name, bool pre_read = false);

	virtual bool setRangeCell(int cell1_row,int cell1_column, int cell2_row,int cell2_column, const QVariantList &data_table);

	virtual bool setRangeCell(int cell1_row,int cell1_column, const QList<QStringList> &data_table);

	virtual bool setCell(int row, int column, const QVariant &data);

	bool outPutStdTableHead92(const QString &sheet_name);  //输出标准联锁表格式92



	virtual int EngineType();

	

};



