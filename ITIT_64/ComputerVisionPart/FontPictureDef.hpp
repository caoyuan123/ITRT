#pragma once

#include <QString>
#include <core\core.hpp>
#include <string>
#include "CBi_TableInterface.h"

class QFont;
class QLabel;
class QImage;

//字体文字属性集合
struct FontWordProprety 
{
public:
	QString FontName;  //字体名称
	int FontNum;      //字号
	double FontSpacing;  //字间距
	double FontCompressRatio; //字压缩比

	bool FontBold;

	int MarginX; //x轴的边界
	int MarginY; //y轴的边界

	//	void* objectOtherUsed;  //保留特殊的用途传递
public:
	FontWordProprety();

	void setFormQfont(const QFont &font);
	void setQfont(QFont &font) const;  //通过字体属性设置Qfont类型

	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const;
};


extern void read(const cv::FileNode& node, FontWordProprety& x,const FontWordProprety& default_value = FontWordProprety());
extern void write(cv::FileStorage& fs, const std::string& name, const FontWordProprety& x);


//带字内详细坐标位置的字图
struct FontPicture 
{
public:
	std::string words;          //字名字

	std::vector<int> wordPos; //字的坐标和矩形框,按字的顺序走的,有可能没有不能调用以下两种方法,第一个值是首字左边
	cv::Mat _Mat8u1c;   //源图cv格式8uc1

	IfDictionary::Field dictionaryType;  //字典中的属性 -1代表未指定

	FontPicture(const FontPicture& copy);
	FontPicture();
public:
	QImage getQtImg();
	cv::Mat getSingleWordCv(int num,int longth = 1) const; //切出只有单字的图片,从谁开始切几个
	cv::Mat getSingleWordCv(std::string PartStr) const;

	bool isHasWordPos() const;  //是否带有字位置信息

	void ShallowCopy(const FontPicture &input_new);
};


//图片生成原始图片的接口
 class IfFontOrlPictureGenerater
{
public:

	//获得字体的属性
	virtual FontWordProprety getFontWordProprety() =0;	
	//设置字体的属性Ref模式
	virtual void setFontPropertyRef(FontWordProprety &FontProperty) =0;
	//设置字体的属性
	virtual void setFontProperty(const FontWordProprety &FontProperty) =0;

	//设置字体名称
	virtual void setFontName(QString fontName)=0;


	//获得cv::Mat 原始格式的图像文件
	virtual cv::Mat getOrlFontImageCv(const QString &Str)=0;
	//获得Qt图像格式的文字图像,也可以获得Qlabel格式的画板
	virtual QImage getOrlFontImageQt(const QString &Str,QLabel *Label = nullptr)=0;
	//获得自定义的文字图像格式的函数
	virtual FontPicture getOrlFontImageFp(const QString &Str,QLabel *Label = nullptr) =0;

};

 class FontGraphicTransTemp;

 class DrawingType;

 //生成图片接口
 class IfFontPictureGenerater : public IfFontOrlPictureGenerater
 {
 public:
	 virtual void setGraphicTransRef(FontGraphicTransTemp *) = 0;
	 virtual void setGraphicTrans(const FontGraphicTransTemp &) = 0;

	 virtual void setDrawingType(const DrawingType & ) = 0;

	 //获得cv::Mat 变换后格式的图像文件
	 virtual cv::Mat getFontImageCv(const QString &Str) = 0;
	 virtual FontPicture getFontImageFp(const QString &Str,QLabel *Label = nullptr) =0;
 };




