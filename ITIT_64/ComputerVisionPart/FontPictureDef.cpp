#include "FontPictureDef.hpp"
#include <opencv.hpp>
#include <qlabel.h>
#include <qimage.h>
#include <QtCv.h>
#include <QFont>
#include "StdAfx.h"
#include "MessyCodeTerminator.h"

void FontWordProprety::setQfont(QFont &font) const
{
	font = QFont(FontName);
	font.setBold(FontBold);
	font.setPointSize(FontNum);
	font.setStretch( FontCompressRatio* 100);
	font.setLetterSpacing(QFont::PercentageSpacing,FontSpacing * 100);
}



FontWordProprety::FontWordProprety()
{
	FontName = U8s("txt");

	FontNum = 28;
	FontSpacing = 1;
	FontCompressRatio  = 1;

	FontBold = true;

	MarginX = 0;
	MarginY = 0;
}

void FontWordProprety::setFormQfont(const QFont &font)
{
	FontName = font.family();
	FontNum = font.pointSize();
	FontCompressRatio = (double)font.stretch() / 100;
	FontSpacing = (double)font.letterSpacing() / 100;

	FontBold = font.bold();

	//加粗忽略
}

void FontWordProprety::read(const cv::FileNode &node)
{
	std::string str = node["FontName"];

	FontName = str.data();

	FontNum = node["FontNum"];
	FontSpacing = node["FontSpacing"];
	FontCompressRatio = node["FontCompressRatio"];
	
	
	int b = node["Bold"];
	FontBold = (int)b;


	MarginX = node["MarginX"];
	MarginY = node["MarginY"];
}

void FontWordProprety::write(cv::FileStorage &fs) const
{
	fs << "{";
	{
		fs << "FontName" << FontName.toStdString();
		fs << "FontNum" << FontNum;
		fs << "Bold" << (int)FontBold;

		fs << "FontSpacing" << FontSpacing;
		fs << "FontCompressRatio" << FontCompressRatio;

		fs << "MarginX" << MarginX;
		fs << "MarginY" << MarginY;
	}
	fs << "}";
}

///////////////////////////////////////////////////////////////


//归一化
double Normalized_pow(double BeNormal,double MAX,int Order)
{
	return pow(BeNormal,Order)/pow(MAX,Order - 1);
}

void read(const cv::FileNode& node, FontWordProprety& x,const FontWordProprety& default_value /*= FontWordProprety()*/)
{
	if (node.empty())
	{
		x = default_value;
	}else
	{
		x.read(node);
	}
}

void write(cv::FileStorage& fs, const std::string& name, const FontWordProprety& x)
{
	x.write(fs);
}

cv::Mat FontPicture::getSingleWordCv(int num,int longth /*= 1*/) const
{
	using namespace  cv;

	cv::Rect RectUsed;

	int endPos;

	RectUsed.height = _Mat8u1c.size().height; //高都是统一的原来高度

	if (num <= 1) //只取第一个
	{
		if (words.size() == 1)
		{

			return _Mat8u1c;  //只有一个字的图 只取一个直接返回
			
		}else
		{
			RectUsed.x = wordPos[0];
			RectUsed.y = 0;

			RectUsed.width = wordPos[num + longth - 1] - wordPos[0] + 3;
		}

		
	}else
	{
		RectUsed.x = wordPos[num - 2]; //起始点

		if (num + longth - 1 >= wordPos.size())
		{
			endPos = _Mat8u1c.size().width;
		}
		else
		{
			endPos = wordPos[num + longth - 1];
		}
		RectUsed.width = endPos - wordPos[num - 2];

		if (RectUsed.tl().x + RectUsed.width > _Mat8u1c.size().width)
		{
			RectUsed.width  = _Mat8u1c.size().width - RectUsed.tl().x;
		}
	}

	return _Mat8u1c(RectUsed).clone();
}

//输入是文字,返回被截取的原图
cv::Mat FontPicture::getSingleWordCv(std::string PartStr) const
{
	int pos = words.find(PartStr);

	if ( pos >= 0 )
	{
		return getSingleWordCv(pos + 1,PartStr.size());
	}
	else
	{
		return cv::noArray().getMat();
	}
}

QImage FontPicture::getQtImg()
{
	return QtCv::cvMat2Qimg(_Mat8u1c);
}

FontPicture::FontPicture(const FontPicture& copy)
{
	this->words = copy.words;
	this->_Mat8u1c = copy._Mat8u1c.clone();
	this->wordPos = copy.wordPos;
	this->dictionaryType = copy.dictionaryType;
}

FontPicture::FontPicture()
{
	dictionaryType = IfDictionary::Field::None;
}

bool FontPicture::isHasWordPos() const
{
	if (wordPos.size() == words.size() + 1)
	{
		return true;
	}else
	{
		return false;
	}
}

void FontPicture::ShallowCopy(const FontPicture &input_new)
{
	words = input_new.words;
	wordPos = input_new.wordPos;
	_Mat8u1c = input_new._Mat8u1c;
	dictionaryType = input_new.dictionaryType;
}




void IfFontPictureGenerater::setDrawingType(const DrawingType &)
{

}
