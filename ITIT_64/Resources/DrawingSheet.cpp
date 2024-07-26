#include "DrawingSheet.h"
#include <highgui.h>

#include "DrawingPart.h"
#include "DrawingBook.h"
#include "DrawingZone.h"

#include "ImgProcFilter.h"
#include "ImgProc.h"
#include <QtGui/QtGui>
#include "GlobalSettingQt.h"
#include "GuiChoiceZone.h"
#include "CBI_TableDictionary.h"
#include "MessyCodeTerminator.h"
#include "OCREngine.h"

#include <ostream>
#include "QtCv.h"

using namespace std;
using namespace cv;

void DrawingSheetFlags::setProcess(const unsigned int flags,bool bl)
{

	if (bl)
	{
		flags_Process |= flags;
	}else
	{
		flags_Process &= ~flags;
	}

}


bool DrawingSheetFlags::getProcess(const unsigned int flags) const
{
	if ((flags_Process & flags))
	{
		return true;
	}else
	{
		return false;
	}
	
}


unsigned int& DrawingSheetFlags::operator=(const unsigned int &input)
{
	flags_Process = input;
	return flags_Process;
}


DrawingSheetFlags::DrawingSheetFlags()
{
	flags_Process = 0;
}


//////////////////////////////////////////////////////////////////////////////
DrawingSheet::DrawingSheet(DrawingBook* Parent)
{
	threadLock = false;
	flags_Process = 0;
	Parent_DB = Parent;

	m_selfDic = false;

	Ptr_dictionary = nullptr;

	Str_OcrTime = "NULL TIME";
}


CBI_TableDictionary * DrawingSheet::getCbiTableDictionary()
{
	if (m_selfDic)   //没有的话返回图纸页共有的字典
	{
		qDebug() << Str_ImgName.data() << U8s("使用私有字典");

		return Ptr_dictionary;
	}else
	{
		return Parent_DB->m_Dictionary;
	}
}


DrawingSheet::~DrawingSheet()
{
	int s = _AllDrawingParts.size();
	for (int i =0;i < s;++i)
	{
		delete(_AllDrawingParts[i]);
	}

	if (m_selfDic)
	{
		delete(Ptr_dictionary);
	}

	s = m_ZoneBeSelected.size();
	for (int i =0;i < s;++i)
	{
		delete(m_ZoneBeSelected[i]);
	}

}


int DrawingSheet::getPartNum()
{
	return _AllDrawingParts.size();
}


bool DrawingSheet::compare(const DrawingSheet &spm1,const DrawingSheet &spm2)
{
	return spm1.PageNum < spm2.PageNum;
}


cv::Mat DrawingSheet::getWholeMap(int step) const
{
	if (blParts)
	{
		return cv::noArray().getMat();
	}else
	{
		return _AllDrawingParts[0]->getFinalMat( step );
	}
}


cv::Mat DrawingSheet::getshowMat() const
{
	return smallMat;
}


QPixmap DrawingSheet::getshowPix() const
{
	return QPixmap::fromImage(QtCv::cvMat2Qimg(getshowMat()));
}


QString DrawingSheet::getName() const
{
	return Str_ImgName.data();
}


void DrawingSheet::testRead(std::string Str)
{
	DrawingPart *dp = new DrawingPart(this);  //图纸分页
	dp->setOrgMat(cv::imread(Str),"test1",1);

	PageNum = 1;
	blParts = false;

	Str_ImgName = "test1";

	//GraphicFilter gf;
	//cv::Mat Flt;  //滤波后
	//gf.filterImgProc(dp->matOrg,Flt);
	//dp->setFlt(Flt);
	//dp->FltPara[0] = gf.max_val;
	//dp->FltPara[1] = gf.max_mu1;
	//dp->FltPara[2] = gf.max_mu2;
	_AllDrawingParts.push_back(dp);
	
	calSmallMap( );
}


void DrawingSheet::ReadOne(std::string Str,int num)
{
	DrawingPart *dp = new DrawingPart(this);  //图纸分页
	QString FullPath = Parent_DB->readPath + "\\" + Str.data();
	
	QString Qstr = Str.data();
	QStringList Ql = Qstr.split(".");

	Str_ImgName = Ql[0].toLocal8Bit();

	std::string str = FullPath.toLocal8Bit();
	cv::Mat read;

	if (Ql[1] == "tif" || Ql[1] == "tiff")
	{
		read = cv::imread(str);
	}else
	{
		read = cv::imread(str,cv::IMREAD_UNCHANGED);
	}
	
	switch (Parent_DB->readRoteType)
	{
	case 0: break;  //不转
	case 1: 
		{
			cv::transpose(read,read);
			cv::flip(read,read,0);
		}break;

	case 2:
		{
			cv::transpose(read,read);
			cv::flip(read,read,1);
		}break;

	case 3:
		{
			cv::flip(read,read,-1);
		}break;

	default:
		break;
	}

	dp->setOrgMat(read,Str,1);
	blParts = false;
	flags_Process.setProcess(DwSt_ProcFlags_读取,true);
	PageNum = num;
	_AllDrawingParts.push_back(dp);
	calSmallMap();
}





void DrawingSheet::write(cv::FileStorage &fs) const
{
	
	fs << "{";
	{
		fs << "Str_ImgName" << Str_ImgName;
		fs << "PageNum" << PageNum;

		//暂时不要这个 读的时候再说
		//fs << "Process_Flags" << flags_Process;
		
		fs << "Vlines" << "[";
		{
			int s = VLines.size();
			for (int i = 0;i < s;++i)
			{
				fs << VLines[i];
			}
		}
		fs << "]";

		fs << "Hlines" << "[";
		{
			int s = HLines.size();
			for (int i = 0;i < s;++i)
			{
				fs << HLines[i];
			}
		}
		fs << "]";

		fs << "DrawingParts" << "[";
		{
			int s = _AllDrawingParts.size();
			for (int i = 0;i < s; ++i)
			{
				fs << *( _AllDrawingParts[i]);  //所有的部分
			}
		}
		fs << "]";

		fs << "Rects" << "[";
		{
			int s = Rects.size();

			for (int i = 0;i < s; ++i)
			{
				fs << Rects[i];  //所有的部分
			}
		}
		fs << "]";

		fs << "Zones" << "[";
		{
			int s = m_ZoneBeSelected.size();

			for (int i = 0;i < s; ++i)
			{
				fs << *( m_ZoneBeSelected[i]);  //所有的部分
			}
		}
		fs << "]";
	}
	fs << "}";
}

void DrawingSheet::read(const cv::FileNode &node)
{
	 node["Str_ImgName"] >> Str_ImgName;

	 node["PageNum"] >> PageNum;	
	//暂时不要这个 读的时候再说
	//fs << "Process_Flags" << flags_Process;

	cv::FileNode fn2 = node["Vlines"];

	if (fn2.isSeq())
	{
		VLines.clear();
		cv::FileNodeIterator it = fn2.begin();
		cv::FileNodeIterator it_E = fn2.end();
		int idx = 0;
		for (;it != it_E;++it,++idx)
		{
			CvLine _cvline;
			(*it) >> _cvline;
			VLines.push_back(_cvline);
		}
	}

	fn2 = node["Hlines"];
	if (fn2.isSeq())
	{
		HLines.clear();

		cv::FileNodeIterator it = fn2.begin();
		cv::FileNodeIterator it_E = fn2.end();
		int idx = 0;
		for (;it != it_E;++it,++idx)
		{
			CvLine _cvline;
			(*it) >> _cvline;
			HLines.push_back(_cvline);
		}
	}

	fn2 = node["DrawingParts"];
	if (fn2.isSeq())
	{
		for (int i = 0;i < _AllDrawingParts.size();i++)
		{
			delete(_AllDrawingParts[i]);
		}
		_AllDrawingParts.clear();

		cv::FileNodeIterator it = fn2.begin();
		cv::FileNodeIterator it_E = fn2.end();

		int idx = 0;
		for (;it != it_E;++it,++idx)
		{
			DrawingPart dp(this);
			(*it) >> dp;
			dp.Parent_Ds = this;
			_AllDrawingParts.push_back(new DrawingPart(dp));
		}
	}

	if (_AllDrawingParts.size() == 1)
	{
		blParts = false;
	}else
	{
		blParts = true;
	}


	fn2 = node["Rects"];
	if (fn2.isSeq())
	{
		Rects.clear();

		cv::FileNodeIterator it = fn2.begin();
		cv::FileNodeIterator it_E = fn2.end();

		int idx = 0;
		for (;it != it_E;++it,++idx)
		{
			cv::Rect R;
			(*it) >> R;
			Rects.push_back(R);
		}
	}


	fn2 = node["Zones"];
	if (fn2.isSeq())
	{
		for (int i = 0;i < m_ZoneBeSelected.size();i++)
		{
			delete(m_ZoneBeSelected[i]);
		}
		m_ZoneBeSelected.clear();

		cv::FileNodeIterator it = fn2.begin();
		cv::FileNodeIterator it_E = fn2.end();

		int idx = 0;
		for (;it != it_E;++it,++idx)
		{
			DrawingZone dz =  DrawingZone(nullptr,nullptr);
			(*it) >> dz;

			dz.Parent_Ds = this;
			m_ZoneBeSelected.push_back(new DrawingZone(dz));
		}
	}


}


cv::Mat DrawingSheet::getZoneMat(CBITable::Headers head)
{
	return cv::noArray().getMat();
}


cv::Mat DrawingSheet::getMat(int step)
{
	return getWholeMap(step);
}

DrawingBook* DrawingSheet::getDrawingBook()
{
	return Parent_DB;
}

DrawingSheet* DrawingSheet::getDrawingSheet()
{
	return this;
}

DrawingType* DrawingSheet::getDrawingType()
{
	return getDrawingBook()->getDrawingType();
}

unsigned char DrawingSheet::getDrawingTableType()
{
	return getDrawingBook()->getDrawingTableType();
}

void DrawingSheet::calSmallMap()
{
	ImgProc::MatSuitableSize(getWholeMap(),smallMat,cv::Size(u_setQt.g_szDwStGui.width(),u_setQt.g_szDwStGui.height()));
}

void DrawingSheet::setImgProcCorrect(CvPoint Area[4],cv::InputArray _Crt)
{
	if (!blParts)
	{
		_AllDrawingParts[0]->setImgProcCorrect(Area,_Crt);
	}else
	{
		qFatal(U8c("FATAL_CODE:多分页sheet无法单独设置矫正参数"));
	}
}

void DrawingSheet::setImgProcCorrect(cv::Point Area[4],cv::InputArray _Crt)
{
	if (!blParts)
	{
		_AllDrawingParts[0]->setImgProcCorrect(Area,_Crt);
	}else
	{
		qFatal(U8c("FATAL_CODE:多分页sheet无法单独设置矫正参数"));
	}
}

void DrawingSheet::setImgProcFilter(std::vector<cv::Point> _fltPoints ,cv::InputArray _Crt)
{
	if (!blParts)
	{
		_AllDrawingParts[0]->setImgProcFilter(_fltPoints,_Crt);
	}else
	{
		qFatal(U8c("FATAL_CODE:多分页sheet无法单独设置滤波参数"));
	}
}

void DrawingSheet::setImgProcLiner(std::vector<CvLine> &_VLine,std::vector<CvLine> &_Hline)
{
	VLines = _VLine;
	HLines = _Hline;

	//给识别的横线排序
	std::sort(HLines.begin(),HLines.end(),CvLine::CompareY);

	//给识别的纵线排序
	std::sort(VLines.begin(),VLines.end(),CvLine::CompareX);

	flags_Process.setProcess(DwSt_ProcFlags_线处理,true);
	flags_Process.setProcess(DwSt_ProcFlags_人工选择线,true);
	//依据纵线计算Rects

	Rects.clear();

	int s = VLines.size();

	for(int i = 0;i < s - 1;i++)
	{
		if (VLines[i].Point1.y == VLines[i+1].Point2.y)  //防一下人工从后向前画线
		{
			Rects.push_back(cv::Rect(VLines[i].Point1,VLines[i+1].Point1));
		}else
		{
			Rects.push_back(cv::Rect(VLines[i].Point1,VLines[i+1].Point2));
		}
	}

	if (Rects.size() > 0)
	{
		flags_Process.setProcess(DwSt_ProcFlags_区域计算,true);
	}
	
}

void DrawingSheet::setImgProcZone(std::vector<GuiZone*> _GuiZone)
{
	int s = 0;

	if (m_ZoneBeSelected.size() > 0)   //支持以下重复写入
	{
		s = m_ZoneBeSelected.size();
		for (int i =0;i < s;++i)
		{
			delete(m_ZoneBeSelected[i]);
		}

		m_ZoneBeSelected.clear();
	}

	s = _GuiZone.size();
	for (int i =0;i < s;i++)
	{
		if (_GuiZone[i]->getZoneType() != CBITable::Headers::unKnow)
		{
			DrawingZone *dz = new DrawingZone(_GuiZone[i],this);
			m_ZoneBeSelected.push_back(dz);

			//没写Zone_Map 的写入
		}
	}

	if (m_ZoneBeSelected.size() > 0)  //有区域写入的话
	{
		flags_Process.setProcess(DwSt_ProcFlags_区域人选,true);
	}

}

void DrawingSheet::setParent(DrawingBook* parent)
{
	Parent_DB = parent;
}

void DrawingSheet::toiLockTable92()
{
	out_CbiTableResult.setData(this);


//	cv::FileStorage fs("testR.yaml",cv::FileStorage::WRITE);

//	fs << "TempR" << out_CbiTableResult;

//	fs.release();
}

bool DrawingSheet::isDwTypePublic()
{
	return true;
}

volatile bool DrawingSheet::isDictionaryReady()
{
	bool  bl_DicReady = false;

	if (m_selfDic)  //私有
	{
		// 查看私有字典是否生成

		qFatal(U8c("还没写"));

	}else //共有
	{
		bl_DicReady = Parent_DB->isDictionary4OCR();
		
		Ptr_dictionary = Parent_DB->getOcrDictionaryLock();
	}

	return bl_DicReady;
}

volatile bool DrawingSheet::generateDictionary()
{
	getCbiTableDictionary()->slot_GenerateFontPicture();

	return true;  
}

void DrawingSheet::TempSave() 
{
	cv::imwrite("tempSave.png",getMat());
}

cv::Mat DrawingSheet::drawResultPicture(int OrlUsed)
{
	cv::Mat outMat; //最后输出的图片


	if (OrlUsed == 2)
	{
		outMat = getMat(2).clone( ); //取矫正后的图片
	}else if (OrlUsed == 0)
	{
		outMat = getMat(3).clone();  //取滤波后的图片
	}else
	{
		outMat = getMat(2).clone( ); //取矫正后的图片
	}
	

	if (outMat.channels( ) == 1)
	{
		cv::cvtColor(outMat,outMat,cv::COLOR_GRAY2BGR);
	}

#ifdef _DEBUG
	ofstream file;
	file.open("data.txt");
#endif
	
	// 遍历所有的DrawingZone
	int sz = m_ZoneBeSelected.size();
	for (int i =0;i < sz;++i)
	{
		DrawingZone *dz = m_ZoneBeSelected[i];

		//遍历DrawZone下面的 AltPoint中的备选点 画在图上
		int szj = dz->AlternativeWords.size();
		for (int j = 0;j < szj ; j++)	
		{
			OCRAlternativePoint *ap = dz->AlternativeWords[j];
			std::string Str = ap->getBest( ).getWord( );
			cv::Rect _rect = cv::Rect(ap->getBest( ).getRect( ));
#ifdef _DEBUG
			file << ap->getBest().getWord().size() << "\t" << ap->getBest().possible << endl;
#endif
			cv::rectangle(outMat(dz->getRect()),_rect,ap->getColorCv( ),2);
			cv::putText(outMat(dz->getRect()),Str, _rect.tl( ) ,cv::FONT_HERSHEY_SIMPLEX,0.8,cvScalar(255,0,50));

		}
	}

#ifdef _DEBUG
	file.close();
#endif

	//增加打时间戳功能
		if (Str_OcrTime != "")
		{
			//给图像加一个上边
			cv::copyMakeBorder(outMat,outMat,50,0,0,0,cv::BORDER_ISOLATED ,cv::Scalar(0,0,0));

			//先打图像戳 再打警示语
			cv::putText(outMat,Str_OcrTime, cv::Point(0,45) ,cv::FONT_HERSHEY_SIMPLEX,1.5,cvScalar(255,255,255));
		}
		

	if (OrlUsed == 0)
	{
		_AllDrawingParts[0]->matFinal = outMat;
	}

	return outMat;
}

int DrawingSheet::getNextButtonStatus()
{
	return 0;
}

void DrawingSheet::setOCRTime(std::string str)
{
	Str_OcrTime = str;
}

std::string DrawingSheet::getOCRTime()
{
	return Str_OcrTime;
}


void read(const cv::FileNode& node, DrawingSheet& x,const DrawingSheet& default_value /*= DrawingSheet(nullptr)*/)
{
	if (node.empty())
	{
		qFatal(U8c("没给DrawingSheet写拷贝构造 Node读取空无法处理"));
	}else
	{
		x.read(node);
	}
}

void write(cv::FileStorage& fs, const std::string& name, const DrawingSheet& x)
{
	x.write(fs);
}



