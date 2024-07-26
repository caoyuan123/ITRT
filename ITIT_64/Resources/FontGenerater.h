#pragma once
#include "FontPictureDef.hpp"


class FontGraphicTransTemp;

//通用的带自动变换的引擎 根据字体自动选择 内部引擎
class FontPictureGenerater : public IfFontPictureGenerater
{
protected:
	IfFontOrlPictureGenerater * ptr_Generater;

	FontGraphicTransTemp * ptr_TransTemp;

	bool s_selfTransTemp;

public:
	virtual void setGraphicTransRef(FontGraphicTransTemp *);
	virtual void setGraphicTrans(const FontGraphicTransTemp &);

	virtual void setFontPropertyRef(FontWordProprety &FontProperty);
	virtual void setFontProperty(const FontWordProprety &FontProperty);

	virtual FontWordProprety getFontWordProprety();

	virtual void setDrawingType(const DrawingType &);

public:
	FontPictureGenerater(void);
	~FontPictureGenerater(void);


	virtual cv::Mat getFontImageCv(const QString &Str);
	virtual FontPicture getFontImageFp(const QString &Str,QLabel *Label = nullptr);
	
	virtual cv::Mat getOrlFontImageCv(const QString &Str);
	virtual QImage getOrlFontImageQt(const QString &Str,QLabel *Label = nullptr);
	virtual FontPicture getOrlFontImageFp(const QString &Str,QLabel *Label = nullptr);

	virtual void setFontName(QString fontName);

	

};

