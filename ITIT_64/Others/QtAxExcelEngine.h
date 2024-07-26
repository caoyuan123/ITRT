

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
	void loadSheet_internal(bool pre_load);  //!���أ��ڲ��������Ժ���Կ�������һ��Ԥ���أ��ӿ��ȡ�ٶȡ�
	bool opennew_internal(bool new_file);  //!�򿪣��½����ļ�

public:
	static QString columnName(int column_no);   	//������������
	static QString cellsName(int row_no, int column_no);   	//��ϳ���Ԫ����

public:
    QtAxExcelEngine();
    ~QtAxExcelEngine();

public:  //book������

    bool initialize(bool visible);
    void dispose();    //!�ͷ��˳����ر�EXCEL����

	bool open(const QString &xls_file, bool not_exist_new = true );  //·��/�Ƿ��½�
	
	bool newBook(); //!�½�һ��XLS�ļ�

	void save();   	//����xls
	void saveAs(const QString &xls_file); //���Ϊ
    void close();  //�ر�xls

    int  getSheetsCounts();   //�õ�sheet������
    bool getSheetName(int sheet_index, QString &sheet_name);   //�õ�ĳ��sheet������

    bool isHasSheet(const QString &sheet_name); //�ж��Ƿ����ĳҳ
	void addSheet(const QString &sheet_name); //����һҳ
	void renameSheet(const QString &new_name);  //���ĵ�ǰ�ҳ������
	bool copySheet(const QString &sheet_name);  //����ǰҳ������Ŀ������,bool �����Ƿ�ɹ���Ĭ�ϲ��ı䵱ǰActiveҳ��

	QString getFilename() const;  //��ǰ�ĵ�����

	bool isOpen();   	//!�Ƿ����һ��EXCEL
	bool isSaved();  	//!��ǰbook �Ƿ񱣴��ˡ�

public:  //sheet������

	bool loadSheet(int sheet_index,bool pre_read = false);  //��ĳ��Sheet��Ϊ��
	bool loadSheet(const QString &sheet_name, bool pre_read = false);

	//��дĳ������
    QVariant getCell(int row, int column);
    bool  setCell(int row, int column, const QVariant &data);

	//��ȡ��������
	void getRangecell(int cell1_row,int cell1_column,
					  int cell2_row,int cell2_column, 
					  QVariantList &data_table);

	//��һ��RANGE��д��sheet�����ݣ�QVariantList��һ��table��Ҳ����List��List
	bool setRangeCell(int cell1_row,int cell1_column,
					  int cell2_row,int cell2_column,
					  const QVariantList &data_table);


	bool writeTableWidget(QTableWidget *table_widget);  //��Table�еı�����xls
	bool readTableWidget(QTableWidget *table_widget);  //��xls������Table
  
	int  getRowCount() const;    //!��ǰSheet������,�����������
	int  getColumnCount() const;      //!��ǰSheet������,�������� 

	int getBeginRow() const;  	//!��ǰ��Sheet����ʼ���������磺��1,2���ǿգ�û�����ݣ���ô����3
	int getBeginColumn() const;  	//!��ǰ��Sheet����ʼ����������:��1,2,3���ǿգ�û�����ݣ���ô����4

	virtual int EngineType();  //��������

private:	
	bool com_init_byself_;  //�ǲ����Լ����ص�com
    
	QAxObject *excel_instance_;     //!ָ������excelӦ�ó���
    QAxObject *work_books_;      //!ָ��������,excel�кܶ๤����������Լ����EXCEL��򿪺ܶ���ļ�
    QAxObject *active_book_;     //!ָ��sXlsFile��Ӧ�Ĺ�����

	QAxObject *work_sheets_;     //!
    QAxObject *active_sheet_;       //ָ�������е�ĳ��sheet��

    QString xls_file_;      //!xls�ļ�·��
	QVariantList preload_sheet_data_;  	//Ԥ���صı������
    bool is_visible_;      //!excel�Ƿ�ɼ�
    int row_count_;      //!����,������ʼ�Ŀ���
    int column_count_;     //!����,������ʼ�Ŀ���
    int start_row_ ;      //!��ʼ�����ݵ����±�ֵ
    int start_column_ ;      //!��ʼ�����ݵ����±�ֵ
    bool is_open_ ;      //!�Ƿ��Ѵ�
	bool is_newfile_;  	//�Ƿ���һ���½����ļ�

};

#endif // QT_EXCELENGINE_H




