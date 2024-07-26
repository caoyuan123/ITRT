#pragma once
#include <string>
#include <QString>
#include <vector>
#include <QObject>


#include "CBITableDef.h"
#include "DrawingDef.h"
#include "DrawingType.h"


class QString;
class CBI_TableDictionary;


//定义一个识别任务所有相关数据的主存储结构,新建project就是新建的这个东西
class DrawingBook : public QObject,  public DrawingAbstruct //一本联锁表
{
	Q_OBJECT
private:
	//执行Ocr识别的时候会复制 保护的字典文件
	CBI_TableDictionary *m_DictionaryLock; 
	

public:
	volatile bool isDictionary4OCR();   //为OCR做准备拷贝图片,如果没有准备好则开始准备
	CBI_TableDictionary * getOcrDictionaryLock();  //获取备份的字典文件

	DrawingType *  getOcrDwType();  //获取设定的 图纸样式文件

public:
	QString readPath;    //图片文件路径-取值符合windows路径规范，以""作为空值
	QString savePath;    //保存输出的路径-取值符合windows路径规范，以""作为空值
	QString StationName; //用于输出联锁表时的名称。读取时写入图片文件夹名称,同时也在设置中可以更改，比如“地安门站1802”

public:
	CBI_TableDictionary *m_Dictionary;  //字典
	DrawingType m_DrawingType;  //图纸种类
	std::vector<DrawingSheet*> m_Pages; //所有联锁表

public:
//	void usingDrawingType(const DrawingType& newType); //使用独有的图纸类型

	int readRoteType; //读入的时候的旋转方法
	void readBooks(QString Path); //读取目录下所有按序号排序的文件,读入_Pages中

public:
	DrawingBook(QObject* parent = nullptr);
	~DrawingBook();

	virtual cv::Mat getMat(int step = 0);
	virtual DrawingBook* getDrawingBook() ;
	virtual DrawingSheet* getDrawingSheet() ;
	virtual DrawingType* getDrawingType() ;
	virtual unsigned char getDrawingTableType() ;

	// void read() = 0;   //从xml中恢复
	// void write() = 0;  //写入xml中

Q_SIGNALS:
	void ReadMessage(QString Str);
	void ReadNewSheet(DrawingSheet* newSheet);


public:

	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const ;
};



extern void read(const cv::FileNode& node, DrawingBook& x,const DrawingBook& default_value = DrawingBook());

extern void write(cv::FileStorage& fs, const std::string& name, const DrawingBook& x);




