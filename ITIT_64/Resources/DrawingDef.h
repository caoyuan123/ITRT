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
		混合 = 1,
		列车联锁表 = 2,
		调车联锁表 = 3,
		组合进路联锁表 = 4,
		延续进路联锁表 = 5,
		非进路调车联锁表 = 6,
		
	};
}


class DrawingAbstruct
{
public:
	
	virtual cv::Mat getMat(int step = 0) = 0;
	
	virtual DrawingBook* getDrawingBook()  = 0;   //获取所属book
	virtual DrawingSheet* getDrawingSheet()  = 0;     //获取所属Sheet
	virtual DrawingType* getDrawingType()  = 0;      //获取所属Sheet
	virtual unsigned char getDrawingTableType()  = 0;      //获取联锁表类型 1992/2018/


	virtual void setImgProcCorrect( CvPoint Area[4],cv::InputArray _Crt){throw new std::exception("fatal Error");} //写入图像处理矫正的参数
	virtual void setImgProcCorrect( cv::Point Area[4],cv::InputArray _Crt){throw new std::exception("fatal Error");} //写入图像处理矫正的参数
	virtual void setImgProcFilter(std::vector<cv::Point> _fltPoints ,cv::InputArray _Crt){throw new std::exception("fatal Error");} //写入图像处理矫正的参数
	virtual void setImgProcLiner(std::vector<CvLine> &_VLine,std::vector<CvLine> &_Hline){throw new std::exception("fatal Error");} //写入线识别处理的参数
	virtual void setImgProcZone(std::vector<GuiZone*> _GuiZone ){throw new std::exception("fatal Error");} //写入区域选择的结果;

};



