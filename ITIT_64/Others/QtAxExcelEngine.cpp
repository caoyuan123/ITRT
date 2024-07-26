#include "QtAxExcelEngine.h"
#include <memory>
#include <windows.h>
#include <QtGui/QtGui>
#include <QtWidgets/QtWidgets>

QtAxExcelEngine::QtAxExcelEngine()
{
	 com_init_byself_ = false;

	 excel_instance_ = nullptr;
	 work_books_ = nullptr;
	 active_book_ = nullptr;
	 work_sheets_ = nullptr;
	 active_sheet_ = nullptr;

	 is_visible_ = false;

	 row_count_ = 0;
	 column_count_ = 0;

	 start_row_ = 1;
	 start_column_ = 1;

	 is_open_ = false;
	 is_newfile_ = false;
}

QtAxExcelEngine::~QtAxExcelEngine()
{
    dispose();
}


//��ʼ��EXCEL OLE���󣬴�EXCEL ���̣�
bool QtAxExcelEngine::initialize(bool visible)
{
	/*
    HRESULT r = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (r == S_OK )
    {
		com_init_byself_ = true;
    }
	else
	{
		
		if (r == S_FALSE || r == RPC_E_CHANGED_MODE)
		{
			//Excel ���ڱ���ģʽ�� �޷����� ������
		}else
		{
			qDebug("QtActiveX: Could not initialize OLE CoInitializeEx (ret %x error %x) .\n",(unsigned int)r,::GetLastError());
		}
	}*/   //��ʹ������ԭ��Excelģʽ

    is_visible_ = visible;
    //
    if (nullptr == excel_instance_)
    {
		//OLE����EXCEL �����ױ����������ƻ���ʵ��û�з��ӡ������㰲װ��WPS,
        excel_instance_ = new QAxObject("Excel.Application");
        if (excel_instance_->isNull())
		{
			qWarning("New OLE Excel.Application fail, ensure your computer install EXCEL.\n");

            is_open_ = false;
            return is_open_;
        }
		//�ڲ��ֻ�����DEBUG�汾�����ж��Դ��󣬲���������һ�£�ԭ��δ֪��
        excel_instance_->dynamicCall("SetVisible(bool )", is_visible_);
    }
    return true;
}


//ע��
void QtAxExcelEngine::dispose()
{
    if (excel_instance_ )
    {
		if (excel_instance_->isNull() == false)
		{
			if (is_open_)
			{
				close();
			}
			excel_instance_->dynamicCall("Quit()");
		}

        delete excel_instance_;
        excel_instance_ = nullptr;

        is_open_ = false;
		is_newfile_ = false;

		row_count_ = 0;
		column_count_ = 0;
		start_row_ = 1;
		start_column_ = 1;

		xls_file_.clear();
    }
	//�����Լ���ʼ���ģ������٣�����������
	if (com_init_byself_)
	{
		::CoUninitialize();
	}
    
}

//��EXCEL�ļ�
bool QtAxExcelEngine::open(const QString &xls_file,
						   bool not_exist_new)
{
	bool new_file = false;
    //���ָ����ļ������ڣ�����Ҫ�½�һ��
    QFileInfo fi(xls_file);
    if ( false == fi.exists())
    {
		if (false == not_exist_new)
		{
			return false;
		}
		new_file = true;
    }
    
	//�õ�����·������ΪActiveXֻ֧�־���·��������Open������Saveas,��������þ���·��������Ҫʵ��
	// ԭ����·���ָ���� '\'
	xls_file_ = QDir::toNativeSeparators(fi.absoluteFilePath());

	return opennew_internal(new_file);
}

//�½�һ��XLS�ļ�
bool QtAxExcelEngine::newBook()
{
	return opennew_internal(true);
}

//!�򿪣��½����ļ����ڲ�����ʵ��
bool QtAxExcelEngine::opennew_internal(bool new_file)
{
	if (excel_instance_ == nullptr || excel_instance_->isNull())
	{
		return false;
	}

	if (is_open_)
	{
		close();
	}

	work_books_ = excel_instance_->querySubObject("WorkBooks");
	if (!new_file)
	{
		//���ԭ���Ĵ��������������ģ��������ҵ���̨�����������˷���Ϊnullptr�����⣬���һ������ʾ����
		//����һ̨���ҷ������ں�������������һ̨����ԣ�
		//QAxBase: error calling idispatch member open: unknown error
		//����������£�
		//active_book_ = excel_instance_->querySubObject("Open(const QString&)",xls_file_);
		//����ʵ��Ӧ�����ⲻ��,����˵��������������ȥ�����ǰѴ���ĳ����£������������⣬����Ȼ����ʾ

		//��xls��Ӧ�ģ���ȡ������,ע�⣬Ҫ�þ���·��
		work_books_->dynamicCall("Open(const QString&)", xls_file_);
		active_book_ = excel_instance_->querySubObject("ActiveWorkBook");
	}
	else
	{
		//�½�һ��xls�����һ���µĹ�����
		work_books_->dynamicCall("Add()");
		active_book_ = excel_instance_->querySubObject("ActiveWorkBook");
		is_newfile_ = true;
	}

	if (!active_book_)
	{
		return false;
	}
	work_sheets_ = active_book_->querySubObject("Worksheets");
	is_open_ = true;
	return true;
}

//���������ݣ�������д���ļ�
void QtAxExcelEngine::save()
{
    if (!active_book_)
    {
		return;
    }
	if (xls_file_.isEmpty())
	{
		return;
	}
	//����Ѿ�����
	if (isSaved())
	{
		return;
	}
	if (is_newfile_)
	{
		active_book_->dynamicCall("SaveAs(const QString&)", xls_file_);
		is_newfile_ = false;
	}
	else
	{
		active_book_->dynamicCall("Save()");
	}
}

//���Ϊ
void QtAxExcelEngine::saveAs(const QString &xls_file)
{
	QFileInfo fi(xls_file);

	//�õ�����·������ΪActiveXֻ֧�־���·��������Open������Saveas,��������þ���·��������Ҫʵ��
	// ԭ����·���ָ���� '\'
	xls_file_ = QDir::toNativeSeparators(fi.absoluteFilePath());
	/*������ĵ����½������ģ���ʹ�����ΪCOM�ӿ�*/
	active_book_->dynamicCall("SaveAs(const QString&)", xls_file_);
	is_newfile_ = false;
}


//�رմ򿪵�EXCEL,
void QtAxExcelEngine::close()
{
    //�ر�ǰ�ȱ�������
    save();
    if ( excel_instance_ && active_book_ )
    {
        active_book_->dynamicCall("Close(bool)", true);

        is_open_     = false;
		xls_file_.clear();
    }
}

//
int QtAxExcelEngine::getSheetsCounts()
{
    return work_sheets_->property("Count").toInt();
}

//�õ�ĳ��sheet������
bool QtAxExcelEngine::getSheetName(int sheet_index, QString &sheet_name)
{
    QAxObject *sheet_tmp = active_book_->querySubObject("WorkSheets(int)", sheet_index);
    if (!sheet_tmp)
    {
        return false;
    }
    sheet_name = sheet_tmp->property("Name").toString();
    return true;
}


bool QtAxExcelEngine::loadSheet(int sheet_index,bool pre_read)
{
    active_sheet_ = active_book_->querySubObject("WorkSheets(int)", sheet_index);

    //���û�д򿪣�
    if (!active_sheet_)
    {
        return false;
    }
    loadSheet_internal(pre_read);
    return true;
}



//����sheet���ּ���Sheet���,
bool QtAxExcelEngine::loadSheet(const QString &sheet_name,
								bool pre_read)
{
    active_sheet_ = active_book_->querySubObject("WorkSheets(QString)", sheet_name);
    //���û�д򿪣�
    if (!active_sheet_)
    {
        return false;
    }
    loadSheet_internal(pre_read);
    return true;
}

//
bool QtAxExcelEngine::isHasSheet(const QString &sheet_name)
{
    QAxObject *temp_sheet = active_book_->querySubObject("WorkSheets(QString)", 
														 sheet_name);
    if (!temp_sheet)
    {
        return false;
    }
    return true;
}

void QtAxExcelEngine::loadSheet_internal(bool pre_read)
{
    //��ȡ��sheet��ʹ�÷�Χ����
    QAxObject *used_range = active_sheet_->querySubObject("UsedRange");
	if (used_range)
	{
		QAxObject *rows = used_range->querySubObject("Rows");
		QAxObject *columns = used_range->querySubObject("Columns");

		//��Ϊexcel���Դ��������������ݶ���һ���Ǵ�1,1��ʼ�����Ҫ��ȡ�������±�
		//��һ�е���ʼλ��
		start_row_ = used_range->property("Row").toInt();
		//��һ�е���ʼλ��
		start_column_ = used_range->property("Column").toInt();

		//��ȡ������������⣬Ҳ�����˿��У�������ֵط�������⻹��һ�¡�
		//rows->property("Count").toInt()���ص�����ʵʹ���˵�����
		row_count_ = rows->property("Count").toInt() + start_row_ - 1;
		//��ȡ����
		column_count_ = columns->property("Count").toInt() + start_column_ - 1;

		preload_sheet_data_.clear();
		preload_sheet_data_.reserve(row_count_ * column_count_);
		//Ԥ���ص����ݣ����
		if (pre_read)
		{
			//Value2 and Value ��������
			QVariantList row_list = used_range->property("Value").toList();
			//��һ��ת���õ��������ݣ���Ҫ��ȡһ��.ʵ�ʲ��ԣ����ֻ��1,1һ�����ݣ�Ҳ�������Ԥ���ش���
			for (int i = 0; i < row_list.size(); ++i)
			{
				preload_sheet_data_ += row_list.at(i).toList();
			}
		}

		delete rows;
		delete columns;
		delete used_range;
	}
    return;
}

//����һ��SHEET
void QtAxExcelEngine::addSheet(const QString &sheet_name)
{
	//��鱻�����Sheet�Ƿ����
	bool ishas = isHasSheet(sheet_name);

	//����Ѿ������Ļ�,�Ͱ�֮ǰ���ڵ��Ǹ��ɵ�
	if (ishas)
	{
		loadSheet(sheet_name);

		active_sheet_->dynamicCall("Delete");
	}

	active_sheet_ = work_sheets_->querySubObject("Add()");
	if (!active_sheet_)
	{
		qWarning("Excel ����ʧ��");
		return;
	}
	active_sheet_->setProperty("Name", sheet_name);

	row_count_ = 0;
	column_count_ = 0;
	start_row_ = 1;
	start_column_ = 1;
}

//�򿪵�xls�ļ�����
QString QtAxExcelEngine::getFilename() const
{
    return xls_file_;
}

//����
void QtAxExcelEngine::renameSheet(const QString & new_name)
{
	active_sheet_->setProperty("Name", new_name);
}


//��tableWidget�е����ݱ��浽excel��
bool QtAxExcelEngine::writeTableWidget(QTableWidget *table_widget)
{
	if (nullptr == table_widget)
	{
		return false;
	}
	if (!is_open_)
	{
		return false;
	}
	int table_row = table_widget->rowCount();
	int table_column = table_widget->columnCount();

    //д�����ݶ�ȡ��data_table����
    QVariantList data_table;
    QVariantList header_list;
    data_table.reserve(table_row + 1);
    header_list.reserve(table_column);
    //�ȶ�ȡ������
	for (int i = 0; i < table_column; i++)
	{
		if (table_widget->horizontalHeaderItem(i) != nullptr)
		{
            header_list.push_back(table_widget->horizontalHeaderItem(i)->text()) ;
		}
		else
		{
            header_list.push_back(QVariant());
		}
	}
    data_table.push_back(header_list);
    //�ٶ�ȡ������
	for (int i = 0; i < table_row; i++)
	{
        QVariantList data_list;
        data_list.reserve(table_column);
		for (int j = 0; j < table_column; j++)
		{
			if (table_widget->item(i, j) != nullptr)
			{
				data_list.push_back(table_widget->item(i, j)->text());
			}
			else
			{
				data_list.push_back(QVariant());
			}
		}
        data_table.push_back(data_list);
	}

    //���Դ���
    //qDebug("table has row=%d colum=%d", data_table.size(), header_list.size());
    //for (int i = 0; i < data_table.size(); i++)
    //{
    //    QVariantList data_list = data_table.at(i).toList();
    //    for (int j = 0; j < data_list.size(); j++)
    //    {
    //        qDebug("table has row colum[%d,%d] data %s", i,j,data_list.at(j).toString().toStdString().c_str());
    //    }
    //}

    //д��EXCEL
	setRangeCell(1, 1, table_row + 1, table_column , data_table);
	return true;
}

/**
  *@brief ��ָ����xls�ļ��а����ݵ��뵽tableWidget��
  *@param tableWidget : ִ��Ҫ���뵽��tablewidgetָ��
  *@return ����ɹ���� true : �ɹ�
  *                   false: ʧ��
  */
bool QtAxExcelEngine::readTableWidget(QTableWidget *table_widget)
{
    if ( nullptr == table_widget )
    {
        return false;
    }

    //�Ȱ�table���������
    int tableColumn = table_widget->columnCount();
    table_widget->clear();
    for (int n = 0; n < tableColumn; n++)
    {
        table_widget->removeColumn(0);
    }

    //��ȡexcel�еĵ�һ��������Ϊ��ͷ
    QStringList headerList;
    for (int n = start_column_; n <= column_count_; n++ )
    {
		headerList << getCell(start_row_, n).toString();
    }

    //���´�����ͷ
    table_widget->setColumnCount(column_count_ - start_column_ +1);
    table_widget->setHorizontalHeaderLabels(headerList);

    //����������
	//��
    for (int i = start_row_ + 1, r = 0; i <= row_count_; i++, r++ )   
    {
		//��������
        table_widget->insertRow(r); 
		//��
        for (int j = start_column_, c = 0; j <= column_count_; j++, c++ )   
        {
			table_widget->setItem(r, c, new QTableWidgetItem(getCell(i, j).toString()));
        }
    }

    return true;
}

//Dim rng As Range : Set rng = Application.Range("$A$1")
//Debug.Print rng.Value             0.1429
//Debug.Print rng.Value2            0.142857142857143
//Debug.Print rng.Text              $0.14


QVariant QtAxExcelEngine::getCell(int row, int column)
{
	//���Ԥ����������,
	if (preload_sheet_data_.size() > 0)
	{
		//������Χ���ؿ�����
		if (row > row_count_ || column > column_count_ || row < start_row_ 	|| column < start_column_)
		{
			return QVariant();
		}
		else
		{
			return preload_sheet_data_.at((row - start_row_)*(column_count_ - start_column_+1)
										  + column - start_column_);
		}
	}
	//���û��Ԥ��������
	else
	{
		//��ȡ��Ԫ�����
		QAxObject *cell = active_sheet_->querySubObject("Cells(int,int)", row, column);
		std::unique_ptr<QAxObject> del_it(cell);
		if (cell)
		{
			return cell->property("Value");
		}
		else
		{
			return QVariant();
		}
	}
}


//�޸�ָ����Ԫ�������
bool QtAxExcelEngine::setCell(int row, int column, const QVariant &data)
{
	
	//��ȡ��Ԫ�����
	QAxObject *cell = active_sheet_->querySubObject("Cells(int,int)",
													row,
													column);
	if (!cell)
	{
		return false;
	}

	//�޸ĵ�Ԫ�������
	QString strData = data.toString();
	cell->dynamicCall("SetValue(const QVariant&)", strData);
	return true;
}

//�ж�excel�Ƿ��ѱ���
bool QtAxExcelEngine::isOpen()
{
    return is_open_;
}

bool QtAxExcelEngine::isSaved()
{
	return active_book_->property("Saved").toBool();
}

//��ȡexcel������,�������� 
int QtAxExcelEngine::getRowCount()const
{
    return row_count_ ;
}


//��ȡexcel������,��������
int QtAxExcelEngine::getColumnCount()const
{
    return column_count_;
}

//!��ǰ��Sheet����ʼ�����������1,2���ǿգ�û�����ݣ���ô����3
int QtAxExcelEngine::getBeginRow() const
{
	return start_row_;
}

//!��ǰ��Sheet����ʼ�����������1,2,3���ǿգ�û�����ݣ���ô����4
int QtAxExcelEngine::getBeginColumn() const
{
	return start_column_;
}


//ȡ���е����ƣ�����27->AA
QString QtAxExcelEngine::columnName(int column_no)
{
    char column_name[64];
    size_t str_len = 0;

    while (column_no > 0)
    {
        int num_data = column_no % 26;
        column_no /= 26;
        if (num_data == 0)
        {
            num_data = 26;
            column_no--;
        }
        //��֪������Բ���
        column_name[str_len] = (char)((num_data - 1) + ('A'));
        str_len++;
    }
    column_name[str_len] = '\0';
    //��ת
    strrev(column_name);

    return column_name;
}

//ȡ��cell�����ơ������һ��cell��A1
QString QtAxExcelEngine::cellsName(int row_no, int column_no)
{
    return QString("%1%2")
        .arg(QtAxExcelEngine::columnName(column_no))
        .arg(row_no);
}

//��һ��RANGE�ж�ȡsheet�����ݣ�QVariantList��һ��table��Ҳ����List��List
void QtAxExcelEngine::getRangecell(int cell1_row,
				  int cell1_column,
				  int cell2_row,
				  int cell2_column,
				  QVariantList &data_table)
{
    data_table.clear();
	QAxObject *range = active_sheet_->querySubObject("Range(const QString&, const QString&)",
													 QtAxExcelEngine::cellsName(cell1_row, cell1_column),
													 QtAxExcelEngine::cellsName(cell2_row, cell2_column));
	
	if (!range)
	{
		return;
	}
    data_table.reserve((cell2_row - cell1_row + 1));
    data_table = range->property("Value").toList();
}

//��һ��RANGE��д��sheet�����ݣ�QVariantList��һ��table��Ҳ����List��List
bool QtAxExcelEngine::setRangeCell(int cell1_row,
				  int cell1_column,
				  int cell2_row,
				  int cell2_column,
				  const QVariantList &data_table)
{
	qDebug("Cell1 name %s Cell2 name %s ,data size %d",
		   QtAxExcelEngine::cellsName(cell1_row, cell1_column).toStdString().c_str(),
		   QtAxExcelEngine::cellsName(cell2_row, cell2_column).toStdString().c_str(),
           data_table.size());

	QAxObject *range = active_sheet_->querySubObject("Range(const QString&, const QString&)",
													 QtAxExcelEngine::cellsName(cell1_row, cell1_column),
													 QtAxExcelEngine::cellsName(cell2_row, cell2_column));
	
	if (!range)
	{
		return false;
	}
	
	range->dynamicCall("SetValue(const QVariant&)", QVariant(data_table));
    return true;
}

bool QtAxExcelEngine::copySheet(const QString &sheet_name)
{

	
	

	return true;

}

int QtAxExcelEngine::EngineType()
{
	return 2;
}




