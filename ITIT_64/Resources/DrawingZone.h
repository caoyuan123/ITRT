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

public:  //ԭ������
	unsigned char ZoneNum; //�����
	CBITable::Headers m_Header;  //����������ͷ����

	//�����ʶ�������м���
	DrawingSheet *Parent_Ds;  //��ĸ��

public:  //ʶ�����õ�����	
	OCROrlResults m_OrlResults;  //���е�ԭʼʶ���
	std::vector<OCRAlternativePoint*> AlternativeWords;  //���б�ѡ��Ŀ���Ե���Ϊ����Ľ��
	std::vector<OCRLine*> All_OCRLine;    //�����е����н���������Ľ����

public:
	DrawingZone(void);
	DrawingZone(GuiZone* _GuiZone,DrawingSheet* parent);
	~DrawingZone(void);

	std::vector<std::string> getZoneResult();  //��ȡȫ���ļ��� 
public:

	cv::Rect getRect();

public:

	virtual cv::Mat getMat(int step = 0);

	virtual DrawingBook* getDrawingBook() ;
	virtual DrawingSheet* getDrawingSheet() ;
	virtual DrawingType* getDrawingType() ;
	virtual unsigned char getDrawingTableType() ;

	void drawResult(cv::Mat &M);  //�����ͼ��������

public:
	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;

};


extern void read(const cv::FileNode& node, DrawingZone& x,const DrawingZone& default_value = DrawingZone(nullptr,nullptr));

extern void write(cv::FileStorage& fs, const std::string& name, const DrawingZone& x);



