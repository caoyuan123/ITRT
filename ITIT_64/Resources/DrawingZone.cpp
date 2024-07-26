#include "DrawingZone.h"
#include "DrawingBook.h"
#include "DrawingSheet.h"
#include "DrawingPart.h"
#include "DrawingType.h"
#include "GuiChoiceZone.h"

DrawingZone::DrawingZone(void)
{
	ZoneNum = 0xff;
	m_Header = CBITable::unKnow;
}

DrawingZone::DrawingZone(GuiZone* _GuiZone,DrawingSheet* parent)
{
	if (_GuiZone != nullptr)
	{

		ZoneNum = _GuiZone->getNum();
		m_Header = _GuiZone->getZoneType();
		Parent_Ds = parent;

	}else
	{

		ZoneNum = 0xff;
		m_Header = CBITable::unKnow;
		Parent_Ds = parent;

	}
}


DrawingZone::~DrawingZone(void)
{

	int S = AlternativeWords.size();
	for (int i = 0 ; i < S ; ++i)
	{
		delete(AlternativeWords[i]);
	}
	
	 S = All_OCRLine.size();
	for (int i = 0 ; i < S ; ++i)
	{
		delete(All_OCRLine[i]);
	}
}



cv::Mat DrawingZone::getMat(int step /*= 0*/)
{
	return getDrawingSheet()->getMat()(getDrawingSheet()->Rects[ZoneNum]) ;
}

DrawingBook* DrawingZone::getDrawingBook() 
{
	return getDrawingSheet()->getDrawingBook();
}

DrawingSheet* DrawingZone::getDrawingSheet() 
{	
	return Parent_Ds;
}

DrawingType* DrawingZone::getDrawingType() 
{
	return getDrawingSheet()->getDrawingType();
}

unsigned char DrawingZone::getDrawingTableType() 
{
	return getDrawingSheet()->getDrawingTableType();
}

cv::Rect DrawingZone::getRect()
{
	return Parent_Ds->Rects[ZoneNum];
}

void DrawingZone::read(const cv::FileNode &node)
{
	node["ZoneNum"] >> ZoneNum;
	std::string Str = node["m_Header"];

	int idx = 0;
	for( ; idx < 20 ; idx++)
	{
		if ( Str == CBITable::HeadersChinese[idx])
		{
			m_Header = (CBITable::Headers) idx;
			break;
		}
	}

}

void DrawingZone::write(cv::FileStorage &fs) const
{
	fs << "{";
	{
		fs << "ZoneNum" << ZoneNum;
		fs << "m_Header" << CBITable::HeadersChinese[m_Header];

		//剩下的结果都没写

	}
	fs << "}";
}



void DrawingZone::drawResult(cv::Mat &M)
{
	qFatal("not coding!");



}



void read(const cv::FileNode& node, DrawingZone& x,const DrawingZone& default_value /*= DrawingZone(nullptr,nullptr)*/)
{
	if (node.empty())
	{
		qWarning("DrawingPart Node读取空");
		x = default_value;
	}else
	{
		x.read(node);
	}
}

void write(cv::FileStorage& fs, const std::string& name, const DrawingZone& x)
{
	x.write(fs);
}
