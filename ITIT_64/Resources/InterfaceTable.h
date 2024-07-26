#pragma once
#include <QVariantList>
#include <QString>


struct IfExcel  //电子表格接口
{
	virtual bool initialize(bool visible) = 0;  //初始化,是否自动打开文件
	virtual void dispose() = 0;   //退出

	//路径/是否新建
	virtual bool open(const QString &xls_file, bool not_exist_new = true ) = 0;  


	virtual void save() = 0;   	//保存xls
	virtual void saveAs(const QString &xls_file) = 0; //另存为
	virtual void close() = 0;  //关闭xls
	
	virtual void addSheet(const QString &sheet_name) = 0; //插入一页
	
public:	//sheet操作
	virtual bool loadSheet(int sheet_index,bool pre_read = false) = 0;  //将某个Sheet设为主
	virtual bool loadSheet(const QString &sheet_name, bool pre_read = false) = 0;

	//向一个RANGE中写入sheet的数据，QVariantList是一个table，也就是List套List
	virtual bool setRangeCell(int cell1_row,int cell1_column,
		int cell2_row,int cell2_column,
		const QVariantList &data_table) = 0;

	virtual bool setCell(int row, int column, const QVariant &data) = 0;

	virtual int EngineType() = 0;

	virtual bool outPutStdTableHead92(const QString &sheet_name) = 0;

	//向一个RANGE中写入sheet的数据，table是自动的，只要
	virtual bool setRangeCell(int cell1_row,int cell1_column,
		const QList<QStringList> &data_table)
		{
			return false;
		};  //非必须接口
 

};



