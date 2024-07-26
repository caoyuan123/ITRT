#pragma once
#include "InterfaceTable.h"
#include <ActiveQt/QAxObject>
#include <vector>

//���� ��� û��д�� ֱ�Ӹ���
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
	//���ļ�
	bool open(QString path);
	//�ر��ļ�
	bool close();
	//��ȡ����������
	int getSheetsCount();
	//�������ƴ���������
	QAxObject* addSheet(QString name);
	//��������ɾ��������
	bool delSheet(QString name);
	//���ݱ��ɾ��������
	bool delSheet(int index);
	//�������ƻ�ȡ������
	QAxObject* getSheet(QString name);
	//���ݱ�Ż�ȡ������
	QAxObject* getSheet(int index);
	//��ȡ�ж���
	QAxObject* getRows(QAxObject* pSheet);
	//��ȡ����
	int getRowsCount(QAxObject* pSheet);
	//��ȡ�ж���
	QAxObject* getColumns(QAxObject* pSheet);
	//��ȡ����
	int getColumnsCount(QAxObject* pSheet);
	//��������ֵ��ȡ��Ԫ��ֵ, ��: 3�У�5��
	QString getCell(QAxObject* pSheet, int row, int column);
	//�������б�Ż�ȡ��Ԫ��ֵ, ��: "F6"
	QString getCell(QAxObject* pSheet, QString number);
	//��������ֵ���õ�Ԫ��ֵ
	bool setCell(QAxObject* pSheet, int row, int column, QString value);
	//�������б�����õ�Ԫ��ֵ
	bool setCell(QAxObject* pSheet, QString number, QString value);

	//��д�������� 
	bool setCell(QAxObject* pSheet,QString C1,QString C2,QVariant value);

signals:

	public slots:

};



