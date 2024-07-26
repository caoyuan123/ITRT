#include "OCRDef.h"

#include "DrawingZone.h"
#include <QtGui/QtGui>
#include "FontPictureDef.hpp"
#include "ComApx.h"
#include "GlobalSetting.h"
#include "ImgProc.h"


using namespace std;

bool ptPsb::compare(const ptPsb& A,const ptPsb& B)
{
	double da = A.getStdPossible();
	double db = B.getStdPossible();
	if (da > db)
	{
		return true;
	}else
	{
		return false;
	}
}

double ptPsb::getStdPossible() const
{
	return possible;
}

void ptPsb::read(const cv::FileNode &node)
{
	node["point"] >> Zpoint;
	node["possible"] >> possible;
	node["isChecked"] >> checked;
}

void ptPsb::write(cv::FileStorage &fs) const
{
	fs << "{";
	{
		fs << "point" << Zpoint;
		fs << "possible" << possible;
		fs << "isChecked" << checked;
	}
	fs << "}";
}

ptPsb::ptPsb()
{
	checked = false; //有木有检查过
}

double ptPsb::getMultiPossible() const
{
	return MultiPossible;
}

double ptPsb::setMultiPossbile(const cv::Mat &MainMat,const FontPicture &fp)
{
	FontPicture Part;

	Part.ShallowCopy(fp);

	Part._Mat8u1c = MainMat(cv::Rect(Zpoint,fp._Mat8u1c.size()));

#if _DEBUG
	cv::Mat debugShow = Part._Mat8u1c;
#endif

	int wordSize = Part.words.size();

	double CompareReckon = 0;


	for (int num_word = 1;num_word <= wordSize;num_word++)
	{
		double temp_rs = ImgProc::MatCompare(Part.getSingleWordCv(num_word),fp.getSingleWordCv(num_word));

		CompareReckon += 1/temp_rs;
	}


	MultiPossible= wordSize / CompareReckon ; //最终结果


	return MultiPossible;
}



////////////////////////////////////////////////////////////////////////////////////////////////

std::string OCRPoint::getWord()const
{
	return word;
}

void OCRPoint::setPossible(double psb )
{
	possible = psb;
	double rt = Com::stdCompareRoit(possible,word.size());

	if (u_setCvOCR.g_blWordPosJudge && false)
	{
		if (MultiPossible > 0)
		{
			rt = Com::stdCompareRoit(MultiPossible,word.size());
		}
	}


	if (isInclude)
	{
		double wordSize = word.size();

		if (u_setCvOCR.g_blWordPosJudge && MultiPossible > 0 && false)
		{
			adjPossible = rt * wordSize / (wordSize + 2.5) ;  //调和均值降的太多了
		}else
		{
			adjPossible = rt * wordSize / (wordSize + 1.8) ;  //几何
		}

	}else
	{

		adjPossible = rt;
	}
}

void OCRPoint::setInclude(bool bl)
{
	isInclude = bl;

	setPossible(possible);
}

double OCRPoint::getStdPossible()const
{

	/*   //这个是老算法
	if (isInclude)
	{
		double wordSize = word.size();
		return possible * wordSize / (wordSize + 3) ;  //被包含的就是这个待遇

	}else
	{
		
		return possible;
	}*/
	
	return adjPossible;

}

double OCRPoint::getWordPossible() const
{
	return possible;
}

cv::Point OCRPoint::getZonePoint()const
{
	return Zpoint;
}

cv::Point OCRPoint::getSheetPoint() const
{
	if (_OCRZone != nullptr)
	{
		return cv::Point(Zpoint.x + _OCRZone->getRect().x , Zpoint.y + _OCRZone->getRect().y);

	}else
	{
		return cv::Point(-1,-1);
	}
}

OCRPoint::OCRPoint()
	: ptPsb()
{

	adjPossible = 0;

	isInclude = false;
	_DicFiled = IfDictionary::数字序列;

}

OCRPoint::OCRPoint(const ptPsb &_ptPsb,OCR_ResultOrlWord *orlWord) 
	: ptPsb(_ptPsb)
{

	isInclude = false;
	_DicFiled = orlWord->_FontPicture->dictionaryType;
	word = orlWord->getWord();
	_OCRWord = orlWord;
	_OCRZone = orlWord->m_Parent;

	setPossible(_ptPsb.possible);
}



FontPicture* OCRPoint::getFontPicture()
{
	return _OCRWord->_FontPicture;
}

cv::Rect OCRPoint::getRect() const
{
	return cv::Rect(Zpoint,_OCRWord->getSize());
}

void OCRPoint::read(const cv::FileNode &node)
{
	node["Word"] >> word;
	node["isInclude"] >> isInclude;

	int iDic;
	node["DicFiled"] >> iDic;
	_DicFiled = (IfDictionary::Field)iDic;

	node["point"] >> Zpoint;

	setPossible(node["possible"]);

	node["isChecked"] >> checked;
}

void OCRPoint::write(cv::FileStorage &fs) const
{
	fs << "{";
	{
		fs << "Word" << word;
		fs << "isInclude" << isInclude;

		int iDic = _DicFiled;

		fs << "DicFiled" << iDic;

		fs  << "Zpoint" << Zpoint;
		fs  << "Possible" << possible;
		fs  << "isChecked"  << checked;
	}
	fs << "}";
}







/////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////////////////////
OCRAlternativePoint::OCRAlternativePoint(DrawingZone* Zone)
{
	isChecked = false;
	zoneLineNum = 0;
	m_Zone = Zone;
}

OCRAlternativePoint::OCRAlternativePoint(const OCRAlternativePoint& copy)
{
	isChecked = false;
	zoneLineNum = 0;
	this->Spoint = copy.Spoint;
	this->Alternative = vector<OCRPoint>(copy.Alternative);
	this->m_Zone = copy.m_Zone;
}

OCRAlternativePoint::OCRAlternativePoint(const OCRPoint& copy)
{
	isChecked = false;
	zoneLineNum = 0;
	this->Spoint = copy.Zpoint;
	this->addOcrPoint(copy);
	this->m_Zone = copy._OCRZone;
}

std::string OCRAlternativePoint::getWord()const
{
	return Alternative[0].getWord();
}

double OCRAlternativePoint::getPossible()const
{
	return Alternative[0].getStdPossible();
}

cv::Point OCRAlternativePoint::getZonePoint()const
{
	return Spoint;
}

void OCRAlternativePoint::sort()
{
	std::sort(Alternative.begin(),Alternative.end(),ptPsb::compare);
}

void OCRAlternativePoint::addOcrPoint(const OCRPoint& input)
{
	Alternative.push_back(input);

	sort();
}
/*
OCRPoint OCRAlternativePoint::operator[](int index) const
{
	return Alternative[index];
}*/

cv::Point OCRAlternativePoint::getSheetPoint() const
{
	return Alternative[0].getSheetPoint();
}

OCRPoint OCRAlternativePoint::getBest()const
{
	return Alternative[0];
}

std::string OCRAlternativePoint::toString() const
{
	return "";
}

void OCRAlternativePoint::setAllInclude(int size )
{
	int sz = Alternative.size();
	for (int i = 0;i < sz;++i)
	{
		if (size > Alternative[i].word.size())
		{
			Alternative[i].setInclude(true);
		}
	}
}

void OCRAlternativePoint::newWithPoint(const OCRPoint& input)
{
	Alternative.clear();
	this->Spoint = input.Zpoint;
	this->addOcrPoint(input);
	this->m_Zone = input._OCRZone;

}

void OCRAlternativePoint::addOutSideOcrPoint(const OCRPoint& input)
{
	Spoint = input.Zpoint;

	setAllInclude(input.getWord().size());

	addOcrPoint(input);
		
}

//
void OCRAlternativePoint::addOcrPointAuto(const OCRPoint &input)
{
	addOcrPoint(input);

	Spoint = getBest().getRect().tl();
	
}

OCRAlternativePoint::AlterType OCRAlternativePoint::getType()
{
	if (Alternative.size() == 1)
	{
		return AlterType::Singal;
	}else
	{

		if (Alternative[0].getStdPossible() - Alternative[1].getStdPossible() < 0.03)
		{
			return AlterType::MultipleMixed;
		}else
		{
			return AlterType::MultiplePeek;
		}

	}

}

#define cvGreen cvScalar(50,255,50)
#define qtGreen QColor(50,255,50)

#define cvYellow cvScalar(0,225,225)
#define qtYellow QColor(225,225,0)

#define cvRed cvScalar(0,0,225)
#define qtRed QColor(0,0,225)

cv::Scalar OCRAlternativePoint::getColorCv()
{
	switch(getType())
	{
	case AlterType::MultipleMixed : return cvRed; break;
	case AlterType::MultiplePeek : return cvYellow; break;
	case AlterType::Singal : return cvGreen; break;

	default: return cvRed; break;
	}
}

QColor OCRAlternativePoint::getColorQt()
{
	switch(getType())
	{
	case AlterType::MultipleMixed : return qtRed; break;
	case AlterType::MultiplePeek : return qtYellow; break;
	case AlterType::Singal : return qtGreen; break;

	default: return qtRed; break;
	}
}

bool OCRAlternativePoint::compareX(OCRAlternativePoint *P1, OCRAlternativePoint *P2)
{
	return P1->getZonePoint().x < P2->getZonePoint().x;
}


void OCRAlternativePoint::read(const cv::FileNode &node)
{
	 node["Spoint"] >> Spoint;
	 node["zoneLineNum"] >> zoneLineNum;
	 node["isChecked"] >> isChecked;

	 cv::FileNode node2 = node["AlternativePoint"];
	 
	 if (node2.isSeq())
	 {
		Alternative.clear();


		cv::FileNodeIterator it = node2.begin();
		cv::FileNodeIterator it_E = node2.end();

		int idx = 0;

		for (;it != it_E; ++it,++idx)
		{
			

		}
	 }
}


void OCRAlternativePoint::write(cv::FileStorage &fs) const
{
	fs << "{";

	fs << "Spoint" << Spoint;
	fs << "zoneLineNum" << zoneLineNum;
	fs << "isChecked" << isChecked;

	fs << "AlternativePoint" << "[";
	int sz = Alternative.size();
	for (int i = 0;i < sz ;++i)
	{
		fs << Alternative[i];
	}
	fs << "]";


	fs << "}";
}

bool OCRAlternativePoint::isSpliter() const
{
	if (getBest()._DicFiled == IfDictionary::条件道岔)
	{
		return false;
	}

	if (getBest()._DicFiled == IfDictionary::数字序列)
	{
		return false;
	}

	if (getBest()._DicFiled == IfDictionary::敌对信号道岔)
	{
		return false;
	}

	return true;
}









//输出的字,需要转化超限符号为"<"和分隔符号
std::string OCRLine::toString(std::string spliter /*= ','*/)
{
	std::string rt = "";

	int sz = Line.size();

	for (int i = 0;i < sz;i++)
	{
		rt += Line[i]->getWord();
		
		
		if (i < sz -1 && Line[i]->isSpliter())  //最后一个不加"," 条件超限默认不加
		{
			rt += spliter;
		}
	}

	return rt;	
}

OCRLine::OCRLine(OCRAlternativePoint *_altPoint)
{
	_OCRZone = _altPoint->m_Zone;
	LineNum = 0;
	y = _altPoint->getZonePoint().y;

	addAltPoint(_altPoint);
}



void OCRLine::addAltPoint(OCRAlternativePoint *_altPoint)
{
	//按左右排序
	Line.push_back(_altPoint);

	if (Line.size() >= 2)
	{
		std::sort(Line.begin(),Line.end(),OCRAlternativePoint::compareX);
	}


}

OCRLine::~OCRLine()
{
	//没有东西要析构
}


bool OCRLine::compare( OCRLine *L1, OCRLine *L2)
{
	return L1->y < L2->y;
}

int OCRLine::getSheetPosY()
{
	return y + _OCRZone->getRect().y;
}

OCR_ResultOrlWord::OCR_ResultOrlWord(FontPicture* word,DrawingZone* _parent)
{
	_FontPicture = word;
	m_Parent = _parent;
}

cv::Size OCR_ResultOrlWord::getSize()
{
	return _FontPicture->_Mat8u1c.size();
}

std::string OCR_ResultOrlWord::getWord()
{
	return _FontPicture->words;
}



////////////////////////////////////////////////////////
OCROrlResults::~OCROrlResults()
{
	int sz = m_OrlResults.size();
	for (int i =0;i< sz;i++)
	{
		delete(m_OrlResults[i]);
	}
}

void OCROrlResults::add(OCR_ResultOrlWord* Add_word)
{
	m_OrlResults.push_back(Add_word);
}


/////////////////////////////////////
void read(const cv::FileNode& node, ptPsb& x,const ptPsb& default_value /*= ptPsb()*/)
{
	if (node.empty())
	{
		x = default_value;
	}else
	{
		x.read(node);
	}
}

void read(const cv::FileNode& node, OCRAlternativePoint& x,const OCRAlternativePoint& default_value /*= OCRAlternativePoint(nullptr)*/)
{
	if (node.empty())
	{
		x = default_value;
	}else
	{
		x.read(node);
	}
}

void read(const cv::FileNode& node, OCRPoint& x,const OCRPoint& default_value /*= OCRPoint()*/)
{
	if (node.empty())
	{
		x = default_value;
	}else
	{
		x.read(node);
	}
}

void write(cv::FileStorage& fs, const std::string& name, const ptPsb& x)
{
	x.write(fs);
}

void write(cv::FileStorage& fs, const std::string& name, const OCRAlternativePoint& x)
{
	x.write(fs);
}

void write(cv::FileStorage& fs, const std::string& name, const OCRPoint& x)
{
	x.write(fs);
}

//////////////////////////////////////////////////////////////////////////////

cv::Rect IfOCRPoint::getSheetRect() const
{
	return cv::Rect(0,0,0,0);
}

cv::Rect IfOCRPoint::getZoneRect() const
{
	return cv::Rect(0,0,0,0);
}



