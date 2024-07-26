#include "FontGenerater.h"
#include <QtGui/QtGui>
#include "FontGeneraterCv.h"
#include "FontGeneraterQ.h"
#include "FontGraphicTransTemp.h"
#include "DrawingType.h"

FontPictureGenerater::FontPictureGenerater(void)
{
	ptr_Generater = nullptr;
	ptr_TransTemp = nullptr;

	s_selfTransTemp = false;
}

FontPictureGenerater::~FontPictureGenerater(void)
{
	if (ptr_Generater != nullptr)
	{
		delete(ptr_Generater);
	}

	if (s_selfTransTemp)
	{
		if (ptr_TransTemp != nullptr)
		{
			delete(ptr_TransTemp);
		}
	}

}

void FontPictureGenerater::setFontPropertyRef(FontWordProprety &FontProperty)
{
	if (ptr_Generater != nullptr)
	{
		delete(ptr_Generater);
	}

	if (FontProperty.FontName.contains("ZH"))
	{
		ptr_Generater = new FontGeneraterCv();
	}else
	{
		ptr_Generater = new FontGeneraterQ();
	}

	ptr_Generater->setFontPropertyRef(FontProperty);
}

void FontPictureGenerater::setFontProperty(const FontWordProprety &FontProperty)
{
	if (FontProperty.FontName.contains("ZH"))
	{
		ptr_Generater = new FontGeneraterCv();
	}else
	{
		ptr_Generater = new FontGeneraterQ();
	}

	ptr_Generater->setFontProperty(FontProperty);
}

void FontPictureGenerater::setDrawingType(const DrawingType & _dt)
 {
	 setGraphicTrans(_dt._GraphicTransTemp);
	 setFontProperty(_dt._FontProprety);
 }

void FontPictureGenerater::setGraphicTransRef(FontGraphicTransTemp * T)
{
	if (s_selfTransTemp)
	{
		if (ptr_TransTemp != nullptr)
		{
			delete(ptr_TransTemp);
		}
	}

	ptr_TransTemp = T;
	s_selfTransTemp = false;
}

void FontPictureGenerater::setGraphicTrans(const FontGraphicTransTemp &T)
{
	if (s_selfTransTemp)
	{
		if (ptr_TransTemp != nullptr)
		{
			delete(ptr_TransTemp);
		}
	}

	ptr_TransTemp = new FontGraphicTransTemp(T);
	s_selfTransTemp = true;
}

cv::Mat FontPictureGenerater::getFontImageCv(const QString &Str)
{

	/*
	cv::Mat M;
	cv::Mat M2;

	FontPicture fpOrl;
	FontPicture fpAft;

	M = getOrlFontImageCv(Str);

	fpOrl = getFontImageFp(Str);

	if (ptr_TransTemp != nullptr)
	{
		ptr_TransTemp->TransForm(M,M2);
		return M2;
	}

	qFatal("ERROR no TransForm Set");
	return cv::noArray().getMat();*/

	return getFontImageFp(Str)._Mat8u1c;
}

FontPicture FontPictureGenerater::getFontImageFp(const QString &Str,QLabel *Label /*= nullptr*/)
{
	FontPicture fp;
	FontPicture fp2;

	fp = getOrlFontImageFp(Str,Label);

	if (ptr_TransTemp != nullptr)
	{
		ptr_TransTemp->TransForm(fp,fp2);
		return fp2;
	}

	qFatal("ERROR no TransForm Set");
	return FontPicture();
}

FontWordProprety FontPictureGenerater::getFontWordProprety()
{
	if (ptr_Generater != nullptr)
	{
		return ptr_Generater->getFontWordProprety();
	}

	qFatal("ERROR no Generater Set");
	return FontWordProprety();
}

cv::Mat FontPictureGenerater::getOrlFontImageCv(const QString &Str)
{
	if (ptr_Generater != nullptr)
	{
		return ptr_Generater->getOrlFontImageCv(Str);
	}


	qFatal("ERROR no Generater Set");
	return cv::noArray().getMat();
}

QImage FontPictureGenerater::getOrlFontImageQt(const QString &Str,QLabel *Label /*= nullptr*/)
{
	if (ptr_Generater != nullptr)
	{
		return ptr_Generater->getOrlFontImageQt(Str,Label);
	}

	qFatal("ERROR no Generater Set");
	return QImage();
}

FontPicture FontPictureGenerater::getOrlFontImageFp(const QString &Str,QLabel *Label /*= nullptr*/)
{
	if (ptr_Generater != nullptr)
	{
		return ptr_Generater->getOrlFontImageFp(Str,Label);
	}

	qFatal("ERROR no Generater Set");
	return FontPicture();
}

void FontPictureGenerater::setFontName(QString fontName)
{
	ptr_Generater->setFontName(fontName);
}




