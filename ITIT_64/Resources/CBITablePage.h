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
	virtual void setData(DrawingZone *) = 0;  //写入数据
	virtual void setData(DrawingSheet *) = 0 ;  //写入数据 -> 实际的对外接口
	virtual QList<QVariant> getExcelValue( ) = 0;

public:

	virtual void read(const cv::FileNode &node) {;}
	virtual void write(cv::FileStorage &fs) const {;}
};





struct CBITableLine
{
private:
	int TableType;  //表格种类 1992 / 2018
	void init( );

public:
	QVector<QString> value;  //定义详见 Def.h中的定义
	int dataLine;   //行的页面内序号 在有分页的时候需要依靠这个来对应行
	
	int posYTop;    //坐标系是Sheet的坐标系
	int posYDown;   

	void setValue(int num, QString Str);
	int getType();
	bool checkInclude(int y);

	CBITableLine(int tableType);
	
	//表的种类,上界限,下界限
	CBITableLine(int tableType,int yTop,int yDown);  
};

//输出的格式文件
class CBITablePage92 : public IfCBITable
{
protected:
	DrawingSheet *Ptr_DrawingSheet;
	void calExcelValue();  //计算出最后输出的值
	uchar s_lineCalType;   //1-> 按图中行处理 2->按坐标对应处理
	int getPartNum();   //一共被拆成了几个页面

	bool updateValue;  //数据输出的时候是否要刷新
public:	

	QList<QStringList> ExcelValue;  //最后输出时发给 Excel驱动的
	QList<QList<CBITableLine>> _value; //外层对应页面的分隔数量 内层是每页的

public:	
	CBITablePage92( );
	
	virtual void setData(DrawingZone *) ;  //写入数据
	virtual void setData(DrawingSheet *) ;  //写入数据 -> 实际的对外接口
	virtual QList<QVariant> getExcelValue( );

public:
	
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;

	

	virtual int getTableType();

};

extern void read(const cv::FileNode& node, CBITablePage92& x,const CBITablePage92& default_value = CBITablePage92( ));

extern void write(cv::FileStorage& fs, const std::string& name, const CBITablePage92& x);

