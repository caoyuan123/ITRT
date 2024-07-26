#include "QxlsxEngine.h"

#include "xlsxdocument.h"

#include "MessyCodeTerminator.h"

#ifdef _DEBUG

#pragma comment(lib,"QXlsxd.lib")

#else

#pragma comment(lib,"QXlsx.lib")

#endif

using namespace QXlsx;

QxlsxEngine::QxlsxEngine(void)
{
	Excel_Doc = new QXlsx::Document();
}


QxlsxEngine::~QxlsxEngine(void)
{
	if (Excel_Doc != nullptr)
	{
		delete(Excel_Doc);
	}

}

bool QxlsxEngine::initialize(bool visible)
{
	return true; //QXlsx 
}

void QxlsxEngine::dispose()
{
	delete(Excel_Doc);
	Excel_Doc = nullptr;
}

bool QxlsxEngine::open(const QString &xls_file, bool not_exist_new /*= true*/)
{
	delete(Excel_Doc);
	
	Excel_Doc = new QXlsx::Document(xls_file);

	if (!Excel_Doc->isLoadPackage()) //加载不成功的话
	{
		if (not_exist_new)
		{
			Excel_Doc->saveAs(xls_file);  //没有既有则保存一个新的
		}else
		{
			return false;  //没有找到已有的而且 不让新建的话
		}
	}


	return true;

}

void QxlsxEngine::save()
{
	Excel_Doc->save();
}

void QxlsxEngine::saveAs(const QString &xls_file)
{
	Excel_Doc->saveAs(xls_file);
}

void QxlsxEngine::close()
{
	//不需要 close


	delete(Excel_Doc);

	Excel_Doc = new QXlsx::Document();
	
}

void QxlsxEngine::addSheet(const QString &sheet_name)
{
	//检查插入的页面是否有 原来的 ，如果有则 先
	if (Excel_Doc->selectSheet(sheet_name))
	{
		Excel_Doc->deleteSheet(sheet_name);

		Excel_Doc->addSheet(sheet_name);
	}else
	{
		Excel_Doc->addSheet(sheet_name);
	}

}

bool QxlsxEngine::loadSheet(int sheet_index,bool pre_read /*= false*/)
{
	Excel_Doc->selectSheet(Excel_Doc->sheetNames()[sheet_index]);

	return true;
}

bool QxlsxEngine::loadSheet(const QString &sheet_name, bool pre_read /*= false*/)
{
	return Excel_Doc->selectSheet(sheet_name);
}

bool QxlsxEngine::setRangeCell(int cell1_row,int cell1_column, int cell2_row,int cell2_column, const QVariantList &data_table)
{
	
	QList<QStringList> data;

	for (int i =0;i < data_table.size();++i)
	{
		data.push_back(data_table[i].toStringList());
	}

	return setRangeCell(cell1_row,cell1_column,data);

}

bool QxlsxEngine::setRangeCell(int cell1_row,int cell1_column, const QList<QStringList> &data_table)
{
	Worksheet *sheet = Excel_Doc->currentWorksheet();

	int lineSZ = data_table.size();

	for (int i_row = 0;i_row < lineSZ;++i_row)
	{
		int numSZ = data_table[i_row].size();

		for (int j = 0;j < numSZ;++j)
		{
			sheet->writeString(i_row + cell1_row,j + cell1_column,data_table[i_row][j]);
		}
	}

	return true;
}

int QxlsxEngine::EngineType()
{
	return 1;
}

bool QxlsxEngine::outPutStdTableHead92(const QString &sheet_name)
{
	//输出标准表格到 sheet1中然后 从sheet1中复制即可

	QXlsx::Worksheet *Sheet1;

	Sheet1 = (Worksheet*)Excel_Doc->sheet(sheet_name);

	Sheet1->writeString(1,1,U8c("进路类型"));
	Sheet1->writeString(1,2,U8c("进路号码"));
	Sheet1->writeString(1,3,U8c("进路"));
	Sheet1->writeString(1,4,U8c("进路方式"));
	Sheet1->writeString(1,5,U8c("排列进路按下按钮"));
	Sheet1->writeString(1,6,U8c("确定运行方向道岔"));
	Sheet1->writeString(1,7,U8c("信号机"));
	Sheet1->writeString(2,7,U8c("名称"));
	Sheet1->writeString(2,8,U8c("显示"));
	Sheet1->writeString(2,9,U8c("表示"));
	Sheet1->writeString(1,10,U8c("道岔"));
	Sheet1->writeString(1,11,U8c("敌对信号"));
	Sheet1->writeString(1,12,U8c("轨道区段"));
	Sheet1->writeString(1,13,U8c("迎面进路"));
	Sheet1->writeString(2,13,U8c("列车"));
	Sheet1->writeString(2,14,U8c("调车"));
	Sheet1->writeString(1,15,U8c("其他联锁"));

	Sheet1->mergeCells(CellRange(1,1,2,1));   
	Sheet1->mergeCells(CellRange(1,2,2,2)); 
	Sheet1->mergeCells(CellRange(1,3,2,3));
	Sheet1->mergeCells(CellRange(1,4,2,4));
	Sheet1->mergeCells(CellRange(1,5,2,5));
	Sheet1->mergeCells(CellRange(1,6,2,6));
	Sheet1->mergeCells(CellRange(1,7,1,9));
	Sheet1->mergeCells(CellRange(1,10,2,10));
	Sheet1->mergeCells(CellRange(1,11,2,11));
	Sheet1->mergeCells(CellRange(1,12,2,12));
	Sheet1->mergeCells(CellRange(1,13,1,14));
	Sheet1->mergeCells(CellRange(1,15,2,15)); 

	return true;
}

bool QxlsxEngine::setCell(int row, int column, const QVariant &data)
{
	Worksheet *sheet = Excel_Doc->currentWorksheet();
	return sheet->writeString(row,column,data.toString());
}





