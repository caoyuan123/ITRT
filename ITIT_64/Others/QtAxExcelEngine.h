

#ifndef QT_ACTIVEX_EXCELENGINE_H
#define QT_ACTIVEX_EXCELENGINE_H


/*
#ifndef _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_NONSTDC_NO_DEPRECATE 1
#endif

#ifndef _CRT_NONSTDC_NO_WARNINGS
#define _CRT_NONSTDC_NO_WARNINGS  1
#endif

#ifndef _WINSOCK_DEPRECATED_NO_WARNINGS
#define _WINSOCK_DEPRECATED_NO_WARNINGS 1
#endif


#ifndef _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS   1
#endif

#ifndef _CRT_SECURE_NO_DEPRECATE
#define _CRT_SECURE_NO_DEPRECATE  1
#endif
*/
#include "InterfaceTable.h"
#pragma warning ( push )
#pragma warning ( disable : 4127)

#include <ActiveQt/QAxObject>
#include <QtCore/QStringList>

#pragma warning ( pop )




class QTableWidget;

class QtAxExcelEngine : public IfExcel
{

protected:
	void loadSheet_internal(bool pre_load);  //!加载，内部函数，以后可以考虑增加一个预加载，加快读取速度。
	bool opennew_internal(bool new_file);  //!打开（新建）文件

public:
	static QString columnName(int column_no);   	//数字算列名称
	static QString cellsName(int row_no, int column_no);   	//组合出单元名称

public:
    QtAxExcelEngine();
    ~QtAxExcelEngine();

public:  //book级操作

    bool initialize(bool visible);
    void dispose();    //!释放退出，关闭EXCEL进程

	bool open(const QString &xls_file, bool not_exist_new = true );  //路径/是否新建
	
	bool newBook(); //!新建一个XLS文件

	void save();   	//保存xls
	void saveAs(const QString &xls_file); //另存为
    void close();  //关闭xls

    int  getSheetsCounts();   //得到sheet的总数
    bool getSheetName(int sheet_index, QString &sheet_name);   //得到某个sheet的名字

    bool isHasSheet(const QString &sheet_name); //判断是否存在某页
	void addSheet(const QString &sheet_name); //插入一页
	void renameSheet(const QString &new_name);  //更改当前活动页面名称
	bool copySheet(const QString &sheet_name);  //将当前页拷贝至目标名称,bool 返回是否成功。默认不改变当前Active页面

	QString getFilename() const;  //当前文档名称

	bool isOpen();   	//!是否打开了一个EXCEL
	bool isSaved();  	//!当前book 是否保存了。

public:  //sheet级操作

	bool loadSheet(int sheet_index,bool pre_read = false);  //将某个Sheet设为主
	bool loadSheet(const QString &sheet_name, bool pre_read = false);

	//读写某格数据
    QVariant getCell(int row, int column);
    bool  setCell(int row, int column, const QVariant &data);

	//获取区域数据
	void getRangecell(int cell1_row,int cell1_column,
					  int cell2_row,int cell2_column, 
					  QVariantList &data_table);

	//向一个RANGE中写入sheet的数据，QVariantList是一个table，也就是List套List
	bool setRangeCell(int cell1_row,int cell1_column,
					  int cell2_row,int cell2_column,
					  const QVariantList &data_table);


	bool writeTableWidget(QTableWidget *table_widget);  //将Table中的保存至xls
	bool readTableWidget(QTableWidget *table_widget);  //将xls保存至Table
  
	int  getRowCount() const;    //!当前Sheet的行数,包括多余空行
	int  getColumnCount() const;      //!当前Sheet的列数,包括空行 

	int getBeginRow() const;  	//!当前的Sheet的起始行数，比如：第1,2行是空，没有数据，那么返回3
	int getBeginColumn() const;  	//!当前的Sheet的起始列数，比如:第1,2,3列是空，没有数据，那么返回4

	virtual int EngineType();  //返回类型

private:	
	bool com_init_byself_;  //是不是自己加载的com
    
	QAxObject *excel_instance_;     //!指向整个excel应用程序
    QAxObject *work_books_;      //!指向工作簿集,excel有很多工作簿，你可以简单理解EXCEL会打开很多个文件
    QAxObject *active_book_;     //!指向sXlsFile对应的工作簿

	QAxObject *work_sheets_;     //!
    QAxObject *active_sheet_;       //指向工作簿中的某个sheet表单

    QString xls_file_;      //!xls文件路径
	QVariantList preload_sheet_data_;  	//预加载的表的数据
    bool is_visible_;      //!excel是否可见
    int row_count_;      //!行数,包括起始的空行
    int column_count_;     //!列数,包括起始的空列
    int start_row_ ;      //!开始有数据的行下标值
    int start_column_ ;      //!开始有数据的列下标值
    bool is_open_ ;      //!是否已打开
	bool is_newfile_;  	//是否是一个新建的文件

};

#endif // QT_EXCELENGINE_H




