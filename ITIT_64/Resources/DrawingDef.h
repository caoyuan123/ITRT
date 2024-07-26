#pragma once
#include <cxcore.hpp>
//#include <qdebug.h>

class DrawingBook;
class DrawingSheet;
class DrawingType;
class DrawingPart;
class DrawingZone;
class GuiZone;
class CvLine;


namespace Drawing
{
	enum DrawingSheetType
	{
		unKnow = 0,
		��� = 1,
		�г������� = 2,
		���������� = 3,
		��Ͻ�·������ = 4,
		������·������ = 5,
		�ǽ�·���������� = 6,
		
	};
}


class DrawingAbstruct
{
public:
	
	virtual cv::Mat getMat(int step = 0) = 0;
	
	virtual DrawingBook* getDrawingBook()  = 0;   //��ȡ����book
	virtual DrawingSheet* getDrawingSheet()  = 0;     //��ȡ����Sheet
	virtual DrawingType* getDrawingType()  = 0;      //��ȡ����Sheet
	virtual unsigned char getDrawingTableType()  = 0;      //��ȡ���������� 1992/2018/


	virtual void setImgProcCorrect( CvPoint Area[4],cv::InputArray _Crt){throw new std::exception("fatal Error");} //д��ͼ��������Ĳ���
	virtual void setImgProcCorrect( cv::Point Area[4],cv::InputArray _Crt){throw new std::exception("fatal Error");} //д��ͼ��������Ĳ���
	virtual void setImgProcFilter(std::vector<cv::Point> _fltPoints ,cv::InputArray _Crt){throw new std::exception("fatal Error");} //д��ͼ��������Ĳ���
	virtual void setImgProcLiner(std::vector<CvLine> &_VLine,std::vector<CvLine> &_Hline){throw new std::exception("fatal Error");} //д����ʶ����Ĳ���
	virtual void setImgProcZone(std::vector<GuiZone*> _GuiZone ){throw new std::exception("fatal Error");} //д������ѡ��Ľ��;

};



