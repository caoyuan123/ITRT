#include "FontGeneraterCv.h"
#include <opencv.hpp>
#include "ComApx.h"
#include <QImage>

FontGeneraterCv::FontGeneraterCv(void)
{
	ZhFontDic = cv::imread(Com::ZHFontSavePlace);

}


FontGeneraterCv::~FontGeneraterCv(void)
{
}

FontWordProprety FontGeneraterCv::getFontWordProprety()
{
	throw std::logic_error("The method or operation is not implemented.");
}

void FontGeneraterCv::setFontPropertyRef(FontWordProprety &FontProperty)
{
	if (selfProperty)
	{
		delete(__FontWordProprety);
		selfProperty = false;
	}
	__FontWordProprety = &FontProperty;
}

void FontGeneraterCv::setFontProperty(const FontWordProprety &FontProperty)
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

cv::Mat FontGeneraterCv::getOrlFontImageCv(const QString &Str)
{
	throw std::logic_error("The method or operation is not implemented.");
}

QImage FontGeneraterCv::getOrlFontImageQt(const QString &Str,QLabel *Label /*= nullptr*/)
{
	throw std::logic_error("The method or operation is not implemented.");
}

FontPicture FontGeneraterCv::getOrlFontImageFp(const QString &Str,QLabel *Label /*= nullptr*/) 
{
	throw std::logic_error("The method or operation is not implemented.");
}

//主要的生成方法
cv::Mat FontGeneraterCv::generateModthd(QString str,QFont font,QLabel *sampleLabel /*= nullptr*/,std::list<cv::Rect> *wordRect /*= nullptr*/)
{
	return cv::noArray().getMat();
}

void FontGeneraterCv::setFontName(QString fontName)
{
	qFatal("cv Generater can not set font name");
}
