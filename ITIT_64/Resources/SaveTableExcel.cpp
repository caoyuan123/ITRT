#include "SaveTableExcel.h"
#include <QtGui/QtGui>


#include <objbase.h>

SaveTableExcel::SaveTableExcel(void)
{
	s_QExcel = new QAxObject("Excel.Application");

	workbook = s_QExcel->querySubObject("Workbooks");

	s_QExcel->setProperty("DisplayAlerts", false);  //����ʾ������Ϣ
}


SaveTableExcel::~SaveTableExcel(void)
{
	quit();
	
}

void SaveTableExcel::Open(std::string path)
{
	QString Str = path.data();

	workbook->querySubObject("Open(const QString&)",Str);
}

void SaveTableExcel::NewBook(std::string newBookname)
{
	
}

void SaveTableExcel::NewSheet(std::string newSheetName)
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool SaveTableExcel::DeleteSheet(std::string sheetName)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void SaveTableExcel::CopyBook(std::string Src,std::string Dst)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void SaveTableExcel::Copy2NewBook(std::string Src,std::string newBookName)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void SaveTableExcel::CopySheet(std::string srcSheetName,std::string dstSheetName)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void SaveTableExcel::CopyRange(std::string srcSheetName, std::string dstSheetName, std::string srcStartCell, std::string srcEndCell, std::string dstStartCell, std::string dstEndCell)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void SaveTableExcel::SaveAs(std::string path)
{
	workbook->dynamicCall("SaveAs(const QString&)",  QDir::toNativeSeparators(path.data()));

	workbook->dynamicCall("Close(Boolean)", false);

	quit();
}

void SaveTableExcel::ReNameSheet(std::string oldName, std::string newName)
{
	throw std::logic_error("The method or operation is not implemented.");
}

int SaveTableExcel::GetSheetIndexByName(std::string sheetName)
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool SaveTableExcel::SetCellValue(std::string sheetName,int row,int column,std::string value)
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool SaveTableExcel::SetRangeValue(std::string sheetName, int startRow, int startColumn, int endRow,
	int endColumn, std::vector<std::vector<std::string>> dataArray)
{
	
	return false;
}


bool SaveTableExcel::SetRangeValue(QString sheetName, int startRow, int startColumn, 
	int endRow, int endColumn, QList<QList<QString>> dataArray)
{
	//QAxObject *worksheet = 



	return false;
}


void SaveTableExcel::show()
{
	s_QExcel->dynamicCall("SetVisible(bool Visible)","true");
}

void SaveTableExcel::hide()
{
	s_QExcel->dynamicCall("SetVisible(bool Visible)","false");
}

void SaveTableExcel::quit()
{
	s_QExcel->dynamicCall("Quit(void)"); 
}


void SaveTableExcel::GetAllSheets()
{
	worksheets = workbook->querySubObject("Sheets");
}

///////////////////////////////////////////////

ExcelOperator::ExcelOperator(QObject *parent) : QObject(parent)
	, m_pExcel(nullptr)
	, m_pWorksheets(nullptr)
	, m_pWorkbook(nullptr)
{

}

ExcelOperator::~ExcelOperator()
{
	close();
}

bool ExcelOperator::open(QString path)
{
	m_strPath = path;
	QAxObject *pWorkbooks = nullptr;
	CoInitializeEx(nullptr, COINIT_MULTITHREADED);
	m_pExcel = new(std::nothrow) QAxObject();
	if (nullptr == m_pExcel) {
		qCritical()<<"����Excel����ʧ��...";
		return false;
	}
	try {
		m_pExcel->setControl("Excel.Application");
		m_pExcel->dynamicCall("SetVisible(bool)", false); //true ��ʾ�����ļ�ʱ�ɼ���false��ʾΪ���ɼ�
		m_pExcel->setProperty("DisplayAlerts", false);
		pWorkbooks = m_pExcel->querySubObject("WorkBooks");
		pWorkbooks->dynamicCall("Add");
		m_pWorkbook = m_pExcel->querySubObject("ActiveWorkBook");
		qDebug()<<"excel path: "<<m_strPath;

		// ��ȡ�򿪵�excel�ļ������еĹ���sheet
		m_pWorksheets = m_pWorkbook->querySubObject("WorkSheets");
	} catch (...) {
		qCritical()<<"���ļ�ʧ��...";
		return false;
	}

	return true;
}

bool ExcelOperator::close()
{
	qDebug()<<"excel close...";
	if (m_pExcel)
	{
		qDebug()<<"closing...";
		m_pWorkbook->dynamicCall("SaveAs(const QString&)", QDir::toNativeSeparators(m_strPath));
		m_pWorkbook->dynamicCall("Close()");
		m_pExcel->dynamicCall("Quit()");
		delete m_pExcel;
		m_pExcel = nullptr;
	}
	return true;
}

int ExcelOperator::getSheetsCount()
{
	int count =  0;
	count = m_pWorksheets->property("Count").toInt();
	return count;
}


QAxObject* ExcelOperator::addSheet(QString name)
{
	QAxObject *pWorkSheet = nullptr;
	try {
		int count = m_pWorksheets->property("Count").toInt();  //��ȡ��������Ŀ
		QAxObject *pLastSheet = m_pWorksheets->querySubObject("Item(int)", count);
		pWorkSheet = m_pWorksheets->querySubObject("Add(QVariant)", pLastSheet->asVariant());
		pLastSheet->dynamicCall("Move(QVariant)", pWorkSheet->asVariant());
		pWorkSheet->setProperty("Name", name);  //���ù���������
	} catch (...) {
		qCritical()<<"����sheetʧ��...";
	}
	return pWorkSheet;
}

bool ExcelOperator::delSheet(QString name)
{
	try {
		QAxObject *pFirstSheet = m_pWorksheets->querySubObject("Item(QString)", name);
		pFirstSheet->dynamicCall("delete");
	} catch (...) {
		qCritical()<<"ɾ��sheetʧ��...";
		return false;
	}
	return true;
}

bool ExcelOperator::delSheet(int index)
{
	try {
		QAxObject *pFirstSheet = m_pWorksheets->querySubObject("Item(int)", index);
		pFirstSheet->dynamicCall("delete");
	} catch (...) {
		qCritical()<<"ɾ��sheetʧ��...";
		return false;
	}
	return true;
}

QAxObject* ExcelOperator::getSheet(QString name)
{
	QAxObject* pWorkSheet = nullptr;
	try {
		pWorkSheet = m_pWorksheets->querySubObject("Item(QString)", name);
	} catch (...) {
		qCritical()<<"��ȡsheetʧ��...";
	}
	return pWorkSheet;
}

QAxObject* ExcelOperator::getSheet(int index)
{
	QAxObject* pWorkSheet = nullptr;
	try {
		pWorkSheet = m_pWorksheets->querySubObject("Item(int)", index);
	} catch (...) {
		qCritical()<<"��ȡsheetʧ��...";
	}
	return pWorkSheet;
}

QAxObject* ExcelOperator::getRows(QAxObject* pSheet)
{
	QAxObject* pRows = nullptr;
	try {
		pRows = pSheet->querySubObject("Rows");
	} catch (...) {
		qCritical()<<"��ȡ��ʧ��...";
	}
	return pRows;
}

int ExcelOperator::getRowsCount(QAxObject* pSheet)
{
	int rows = 0;
	try {
		QAxObject* pRows = getRows(pSheet);
		rows = pRows->property("Count").toInt();
	} catch (...) {
		qCritical()<<"��ȡ����ʧ��...";
	}
	return rows;
}

QAxObject* ExcelOperator::getColumns(QAxObject* pSheet)
{
	QAxObject* pColumns = nullptr;
	try {
		pColumns = pSheet->querySubObject("Columns");
	} catch (...) {
		qCritical()<<"��ȡ��ʧ��...";
	}
	return pColumns;
}

int ExcelOperator::getColumnsCount(QAxObject* pSheet)
{
	int columns = 0;
	try {
		QAxObject* pColumns = getColumns(pSheet);
		columns = pColumns->property("Count").toInt();
	} catch (...) {
		qCritical()<<"��ȡ����ʧ��...";
	}
	return columns;
}

QString ExcelOperator::getCell(QAxObject* pSheet, int row, int column)
{
	QString strCell = "";
	try {
		QAxObject* pCell = pSheet->querySubObject("Cells(int, int)", row, column);
		strCell = pCell->property("Value").toString();
	} catch (...) {
		qCritical()<<"��ȡ��Ԫ����Ϣʧ��...";
	}

	return strCell;
}

QString ExcelOperator::getCell(QAxObject* pSheet, QString number)
{
	QString strCell = "";
	try {
		QAxObject* pCell = pSheet->querySubObject("Range(QString)", number);
		strCell = pCell->property("Value").toString();
	} catch (...) {
		qCritical()<<"��ȡ��Ԫ����Ϣʧ��...";
	}

	return strCell;
}

bool ExcelOperator::setCell(QAxObject* pSheet, int row, int column, QString value)
{
	try {
		QAxObject* pCell = pSheet->querySubObject("Cells(int, int)", row, column);
		pCell->setProperty("Value", value);
	} catch (...) {
		qCritical()<<"д�뵥Ԫ����Ϣʧ��...";
		return false;
	}
	return true;
}

bool ExcelOperator::setCell(QAxObject* pSheet, QString number, QString value)
{
	try {
		QAxObject* pCell = pSheet->querySubObject("Range(QString)", number);
		pCell->setProperty("Value", value);
	} catch (...) {
		qCritical()<<"д�뵥Ԫ����Ϣʧ��...";
		return false;
	}
	return true;
}

bool ExcelOperator::setCell(QAxObject* pSheet,QString C1,QString C2,QVariant value)
{
	try {
		
		QAxObject *rang = pSheet->querySubObject("Rang(const QString)",C1 + ":" + C2);

		rang->setProperty("Value",value);

		

	} catch (...) {
		qCritical()<<"д�뵥Ԫ����Ϣʧ��...";
		return false;
	}
	return true;
}
