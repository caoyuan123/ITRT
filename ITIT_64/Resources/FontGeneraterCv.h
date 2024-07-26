#pragma once
#include "fontpicturedef.hpp"
#include <QFont>
#include <list>

class FontGeneraterCv :
	public IfFontOrlPictureGenerater
{

	cv::Mat ZhFontDic; //zh 字体的字典 

	cv::Mat generateModthd(QString str,QFont font,QLabel *sampleLabel = nullptr,std::list<cv::Rect> *wordRect = nullptr); //生成方法
	bool calulateWordPlace;
	bool selfProperty; //字体属性是自己存的还是用别人的
	QFont _QFont;
	bool FontChanged;  //字体属性有变

protected:
	FontWordProprety* __FontWordProprety; //属性存贮地点,Ref模式不会析构,非地址模式会析构

public:
	FontGeneraterCv(void);
	~FontGeneraterCv(void);

	 FontWordProprety getFontWordProprety();

	 void setFontPropertyRef(FontWordProprety &FontProperty);

	 void setFontProperty(const FontWordProprety &FontProperty);

	 cv::Mat getOrlFontImageCv(const QString &Str);

	 QImage getOrlFontImageQt(const QString &Str,QLabel *Label = nullptr);

	 FontPicture getOrlFontImageFp(const QString &Str,QLabel *Label = nullptr) ;

	 virtual void setFontName(QString fontName);

};

