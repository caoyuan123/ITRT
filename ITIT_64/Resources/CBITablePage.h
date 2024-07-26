#pragma once

#include <QVector>
#include <QList>
#include <QString>
#include <QStringList>


class IfSaveTable;
class DrawingZone;
class DrawingSheet;

namespace cv
{
	class FileStorage;
	class FileNode;
}

class IfCBITable
{
public:
	IfCBITable( );
	~IfCBITable(void);

	virtual int  getTableType() = 0;  
	virtual void setData(DrawingZone *) = 0;  //д������
	virtual void setData(DrawingSheet *) = 0 ;  //д������ -> ʵ�ʵĶ���ӿ�
	virtual QList<QVariant> getExcelValue( ) = 0;

public:

	virtual void read(const cv::FileNode &node) {;}
	virtual void write(cv::FileStorage &fs) const {;}
};





struct CBITableLine
{
private:
	int TableType;  //������� 1992 / 2018
	void init( );

public:
	QVector<QString> value;  //������� Def.h�еĶ���
	int dataLine;   //�е�ҳ������� ���з�ҳ��ʱ����Ҫ�����������Ӧ��
	
	int posYTop;    //����ϵ��Sheet������ϵ
	int posYDown;   

	void setValue(int num, QString Str);
	int getType();
	bool checkInclude(int y);

	CBITableLine(int tableType);
	
	//�������,�Ͻ���,�½���
	CBITableLine(int tableType,int yTop,int yDown);  
};

//����ĸ�ʽ�ļ�
class CBITablePage92 : public IfCBITable
{
protected:
	DrawingSheet *Ptr_DrawingSheet;
	void calExcelValue();  //�������������ֵ
	uchar s_lineCalType;   //1-> ��ͼ���д��� 2->�������Ӧ����
	int getPartNum();   //һ��������˼���ҳ��

	bool updateValue;  //���������ʱ���Ƿ�Ҫˢ��
public:	

	QList<QStringList> ExcelValue;  //������ʱ���� Excel������
	QList<QList<CBITableLine>> _value; //����Ӧҳ��ķָ����� �ڲ���ÿҳ��

public:	
	CBITablePage92( );
	
	virtual void setData(DrawingZone *) ;  //д������
	virtual void setData(DrawingSheet *) ;  //д������ -> ʵ�ʵĶ���ӿ�
	virtual QList<QVariant> getExcelValue( );

public:
	
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;

	

	virtual int getTableType();

};

extern void read(const cv::FileNode& node, CBITablePage92& x,const CBITablePage92& default_value = CBITablePage92( ));

extern void write(cv::FileStorage& fs, const std::string& name, const CBITablePage92& x);

