#pragma once
#include "InterfaceTable.h"
#include <ActiveQt/QAxObject>
#include <vector>

//别看了 这个 没有写完 直接搞了
class SaveTableExcel 
{
protected:
	QAxObject *s_QExcel;

	QAxObject *workbook;

	QAxObject *worksheets;

public:
	SaveTableExcel(void);
	~SaveTableExcel(void);

	virtual void GetAllSheets( );

	virtual void Open(std::string path);

	virtual void NewBook(std::string newBookname);

	virtual void NewSheet(std::string newSheetName);

	virtual bool DeleteSheet(std::string sheetName);

	virtual void CopyBook(std::string Src,std::string Dst);

	virtual void Copy2NewBook(std::string Src,std::string newBookName);

	virtual void CopySheet(std::string srcSheetName,std::string dstSheetName);

	virtual void CopyRange(std::string srcSheetName, std::string dstSheetName, std::string srcStartCell, 
		std::string srcEndCell, std::string dstStartCell, std::string dstEndCell);

	virtual void SaveAs(std::string path);

	virtual void ReNameSheet(std::string oldName, std::string newName);

	virtual int GetSheetIndexByName(std::string sheetName);

	virtual bool SetCellValue(std::string sheetName,int row,int column,std::string value);

	virtual bool SetRangeValue(std::string sheetName, int startRow, int startColumn, 
		int endRow, int endColumn, std::vector<std::vector<std::string>> dataArray);

	virtual bool SetRangeValue(QString sheetName, int startRow, int startColumn,
		int endRow, int endColumn, QList<QList<QString>> dataArray);

	virtual void show();

	virtual void hide();

	virtual void quit();



};



class ExcelOperator : public QObject
{
	Q_OBJECT
private:
	QAxObject*      m_pExcel;
	QAxObject*      m_pWorksheets;
	QAxObject*      m_pWorkbook;
	QString         m_strPath;

public:
	explicit ExcelOperator(QObject *parent = nullptr);
	~ExcelOperator();
	//打开文件
	bool open(QString path);
	//关闭文件
	bool close();
	//获取工作表数量
	int getSheetsCount();
	//根据名称创建工作表
	QAxObject* addSheet(QString name);
	//根据名称删除工作表
	bool delSheet(QString name);
	//根据编号删除工作表
	bool delSheet(int index);
	//根据名称获取工作表
	QAxObject* getSheet(QString name);
	//根据编号获取工作表
	QAxObject* getSheet(int index);
	//获取行对象
	QAxObject* getRows(QAxObject* pSheet);
	//获取行数
	int getRowsCount(QAxObject* pSheet);
	//获取列对象
	QAxObject* getColumns(QAxObject* pSheet);
	//获取列数
	int getColumnsCount(QAxObject* pSheet);
	//根据行列值获取单元格值, 如: 3行，5列
	QString getCell(QAxObject* pSheet, int row, int column);
	//根据行列编号获取单元格值, 如: "F6"
	QString getCell(QAxObject* pSheet, QString number);
	//根据行列值设置单元格值
	bool setCell(QAxObject* pSheet, int row, int column, QString value);
	//根据行列编号设置单元格值
	bool setCell(QAxObject* pSheet, QString number, QString value);

	//读写大量数据 
	bool setCell(QAxObject* pSheet,QString C1,QString C2,QVariant value);

signals:

	public slots:

};



