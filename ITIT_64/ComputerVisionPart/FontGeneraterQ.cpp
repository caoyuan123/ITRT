#include "FontGeneraterQ.h"
//#include <QtGui/QtGui>
#include <QPainter>
#include <assert.h>
#include <opencv2/opencv.hpp>
#include "QtCv.h"
#include "ImgProc.h"
#include <QLabel>
#include "StdAfx.h"
#include "GlobalSetting.h"


QImage FontGeneraterQ ::generateModthd(QString str,QFont font,QLabel *sampleLabel,std::list<cv::Rect> *wordRect)
{

	bool outsideLabelUsed = true;
	if (sampleLabel == nullptr)
	{
		sampleLabel = new QLabel(); //利用label中的fontMetrics函数弄出字的长宽
		outsideLabelUsed = false;
	}
	sampleLabel->setFont(font);//边沿大小/8-/3

	//边沿需要修改成字体百分比形式

	int OffsetX = __FontWordProprety->MarginX;
	int OffsetY = __FontWordProprety->MarginY;
	int round = 0;

	int FontX = sampleLabel->fontMetrics().width(str);
	int FontY = sampleLabel->fontMetrics().height();
	
	//需要计算每个字的位置的话
	if (wordRect != nullptr && calulateWordPlace)
	{
		qFatal(U8c("generateModthd 这块还没写"));
	}

	int totalX = FontX + OffsetX;
	int totalY = FontY + OffsetY;


	/*
	if(totalX % 2 == 0)
	{
		totalX += 1;
	}

	if(totalY % 2 == 0)
	{
		totalY += 1;
	}*/

	QImage Image(totalX , totalY , QImage::Format::Format_RGB888);
//	QImage Image(FontX *(1 + OffsetX) , FontY  + OffsetY , QImage::Format::Format_RGB888);

	Image.fill(Qt::white);
	QPainter painter(&Image);
	painter.setPen(Qt::GlobalColor::black);
	painter.setFont(font);

	//	painter.drawText(OffsetX /2 ,- OffsetY  / 5 ,FontX,FontY,Qt::AlignmentFlag::AlignBottom|Qt::AlignmentFlag::AlignHCenter,str);
	painter.drawText(FontX * OffsetX /2 ,- OffsetY  / 5 ,FontX,FontY,Qt::AlignmentFlag::AlignBottom|Qt::AlignmentFlag::AlignHCenter,str);
	

	if (outsideLabelUsed == false)
	{
		delete(sampleLabel);
	}

	return Image;
}

FontGeneraterQ::FontGeneraterQ(void)
{
	__FontWordProprety = new FontWordProprety();   
	__FontWordProprety->setQfont(_QFont);
	selfProperty = true;
	FontChanged = false;
}

FontGeneraterQ::~FontGeneraterQ(void)
{
	if (selfProperty)
	{
		delete(__FontWordProprety);
	}
}

void FontGeneraterQ::setFontProperty(const FontWordProprety &FontProperty)
{
	if (selfProperty)
	{
		*__FontWordProprety = FontProperty;
	}else
	{
		__FontWordProprety = new FontWordProprety(FontProperty);
		selfProperty = true;
	}
	
}

void FontGeneraterQ::setFontPropertyRef(FontWordProprety &FontProperty)
{
	if (selfProperty)
	{
		delete(__FontWordProprety);
		selfProperty = false;
	}
	__FontWordProprety = &FontProperty;
}

cv::Mat FontGeneraterQ::getOrlFontImageCv(const QString &Str)
{
	calulateWordPlace = false;

	return getOrlFontImageFp(Str)._Mat8u1c;
}

QImage FontGeneraterQ::getOrlFontImageQt(const QString &Str,QLabel *Label /*= nullptr*/)
{
	calulateWordPlace = false;
	__FontWordProprety->setQfont(_QFont);
	return generateModthd(Str,_QFont,Label);
}


FontPicture FontGeneraterQ::getOrlFontImageFp(const QString &Str,QLabel *Label /*= nullptr*/) 
{
	__FontWordProprety->setQfont(_QFont);
	calulateWordPlace = true;
	QImage RtColorQ = generateModthd(Str,_QFont,Label).convertToFormat(QImage::Format_ARGB32);
	cv::Mat RtColorC = QtCv::Qimg2cvMat(RtColorQ);

	FontPicture RtFp;

	RtFp.words = Str.toStdString();

	cv::cvtColor(RtColorC,RtFp._Mat8u1c,CV_RGBA2GRAY);

	//这里删除上下多余的部分
	int Top = 0,Down = 0;

	ImgProc::calWordHight(RtFp._Mat8u1c,Top,Down);

	if (Top > 5)
	{
		Top-=4;  //留个边
	}

	if (RtFp._Mat8u1c.rows - Down >5)
	{
		Down += 4; //留个边
	}

	cv::Rect R = cv::Rect(0,Top,RtFp._Mat8u1c.size().width,Down - Top + 1);

	RtFp._Mat8u1c = RtFp._Mat8u1c(R);
    //这里删除上下多余的部分 /**/

	//计算每个字的边界
	if (u_setCvOCR.g_blWordPosJudge)  
	{
		//1.情况可以区分每个字
		std::vector<int> wordDistance;
		wordDistance = ImgProc::calWordDistance(RtFp._Mat8u1c);

		RtFp.wordPos = wordDistance;

		//2.情况就是粘粘的比较严重无法区分字的间距。等效为wordPos的size为0
	}
	
	return RtFp;
}

FontPicture FontGeneraterQ::getOrlFontImageFp(const QString &Str,const QFont &Fontq,const FontWordProprety &fontProperty)
{
	qFatal("ERROR");
	return FontPicture();
}

FontWordProprety FontGeneraterQ::getFontWordProprety()
{
	return *__FontWordProprety;
}

void FontGeneraterQ::setFontName(QString fontName)
{
	_QFont.setFamily(fontName);
	__FontWordProprety->setFormQfont(_QFont);
}



