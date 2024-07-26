#pragma once

#include <vector>
#include <string>
#include "DrawingDef.h"
#include "CBITableDef.h"
#include <cxcore.hpp>


#include "OCRDef.h"

class GuiZone;
class DrawingZone : DrawingAbstruct
{

public:  //原本属性
	unsigned char ZoneNum; //区块号
	CBITable::Headers m_Header;  //所属联锁表头类型

	//整体的识别结果按行集合
	DrawingSheet *Parent_Ds;  //父母类

public:  //识别所用的属性	
	OCROrlResults m_OrlResults;  //所有的原始识别点
	std::vector<OCRAlternativePoint*> AlternativeWords;  //带有备选项目的以点作为排序的结果
	std::vector<OCRLine*> All_OCRLine;    //所有行的所有结果（整理后的结果）

public:
	DrawingZone(void);
	DrawingZone(GuiZone* _GuiZone,DrawingSheet* parent);
	~DrawingZone(void);

	std::vector<std::string> getZoneResult();  //获取全部的集合 
public:

	cv::Rect getRect();

public:

	virtual cv::Mat getMat(int step = 0);

	virtual DrawingBook* getDrawingBook() ;
	virtual DrawingSheet* getDrawingSheet() ;
	virtual DrawingType* getDrawingType() ;
	virtual unsigned char getDrawingTableType() ;

	void drawResult(cv::Mat &M);  //将结果图画在上面

public:
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;

};


extern void read(const cv::FileNode& node, DrawingZone& x,const DrawingZone& default_value = DrawingZone(nullptr,nullptr));

extern void write(cv::FileStorage& fs, const std::string& name, const DrawingZone& x);



