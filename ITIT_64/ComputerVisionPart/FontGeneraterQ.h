#pragma once
#include "FontPictureDef.hpp"
#include <QFont>



//使用Qt的画字引擎做出来的图像接口
 class FontGeneraterQ :public IfFontOrlPictureGenerater
{
	//生成方法
	QImage generateModthd(QString str,QFont font,QLabel *sampleLabel = nullptr,std::list<cv::Rect> *wordRect = nullptr); 
	bool calulateWordPlace;   //是否计算字的详细位置
	bool selfProperty; //字体属性是自己存的还是用别人的
	QFont _QFont;
	bool FontChanged;  //字体属性有变

protected:
	FontWordProprety* __FontWordProprety; //属性存贮地点,Ref模式不会析构,非地址模式会析构

public:
	FontGeneraterQ(void);
	~FontGeneraterQ(void);

	 FontWordProprety getFontWordProprety();
	
	  //设置字体的属性-Ref是用别人的属性,非Ref用的是自己的属性
	 void setFontPropertyRef(FontWordProprety &FontProperty);
	 void setFontProperty(const FontWordProprety &FontProperty);

	  //获得cv::Mat格式的图像文件
	 cv::Mat getOrlFontImageCv(const QString &Str);
	 //获得Qt图像格式的文字图像,也可以获得Qlabel格式的画板
	 QImage getOrlFontImageQt(const QString &Str,QLabel *Label = nullptr);

	 //获得自定义的文字图像格式的函数
	 FontPicture getOrlFontImageFp(const QString &Str,QLabel *Label = nullptr) ;
	 static FontPicture getOrlFontImageFp(const QString &Str,const QFont &Fontq,const FontWordProprety &fontProperty);

	 //设定字体名称
	 virtual void setFontName(QString fontName);


 };



