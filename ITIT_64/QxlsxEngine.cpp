#include "QxlsxEngine.h"
#include <QtCore>
#include <QDebug>

#include <xlsxdocument.h>


using namespace std;
using namespace QXlsx;


#ifdef _DEBUG

#pragma comment(lib,"QXlsxd.lib")

#else

#pragma comment(lib,"QXlsx.lib")

#endif


QxlsxEngine::QxlsxEngine(void)
{
	Excel_Doc = new QXlsx::Document();
}


QxlsxEngine::~QxlsxEngine(void)
{
	delete(Excel_Doc);
}

bool QxlsxEngine::initialize(bool visible)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void QxlsxEngine::dispose()
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool QxlsxEngine::open(const QString &xls_file, bool not_exist_new /*= true*/)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void QxlsxEngine::save()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void QxlsxEngine::saveAs(const QString &xls_file)
{
	throw std::logic_error("The method or operation is not implemented.");
}

void QxlsxEngine::close()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void QxlsxEngine::insertSheet(const QString &sheet_name)
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool QxlsxEngine::loadSheet(int sheet_index,bool pre_read /*= false*/)
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool QxlsxEngine::loadSheet(const QString &sheet_name, bool pre_read /*= false*/)
{
	throw std::logic_error("The method or operation is not implemented.");
}

bool QxlsxEngine::setRangeCell(int cell1_row,int cell1_column, int cell2_row,int cell2_column, const QVariantList &data_table)
{
	throw std::logic_error("The method or operation is not implemented.");
}
