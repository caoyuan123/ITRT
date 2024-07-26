#pragma once
#include <vector>
#include "FontGraphicTransTemp.h"
#include "FontPictureDef.hpp"


#ifdef _DEBUG
extern void testFuc();
#endif

using namespace std;

class DrawingType;

//字体到图像的完整属性
struct FontPictureProprety : public FontWordProprety ,public FontGraphicTransTemp
{
//	double CompareRoit;   //匹配率

	int stdWordHeight;  //标准字符高度 //给一下","的字高字宽即可 
	int stdWordWidth;	//标准字符宽度

	FontPictureProprety()
	{
		CompareRoit = 0;
		stdWordHeight = 0;
		stdWordWidth = 0;
	}

	//	cv::Mat getFontMat(); //没写完
	//	virtual void read(const cv::FileNode &node)=0;
	//	virtual void write(cv::FileStorage &fs) const=0;

	void setDrawingType(DrawingType& _dt ); //将数据拷贝到DrawingType中
};



//字体逼近用的结构体,可以根据字体数据生成配用的FontPicturePos类型的自己生成的文字小图片
struct FontApx :public FontPicture
{
public:
	double MaxCompareRoit;  //最佳适配率

	FontPictureProprety    *FontPicProprety;//文字属性_里面有俩属性Font的还有图像变换的4个参数
	IfFontOrlPictureGenerater *FontGenerater; //字体生成引擎


	//生成字模的函数,有Wordchoice的可以选择是那个字
	cv::Mat getFontImageCv(bool TransUsed = true/*是否自动使用模板变换*/) const;
	cv::Mat getFontImageCv(int Wordchoice,bool TransUsed = true) const;

	//手工指定的字高
	std::vector<int> DistanceY;  

	FontApx( );
	~FontApx( );

	//read
	//write
};

#define MiniCompress 0.2


//手工或自动整定参数
//1.寻找字体参数
//2.拟合生成图片到真实图片
class FontSimilar
{
private:
	FontGraphicTransTemp ImageTreeble(FontApx &input, int num = 0);

public:
	QString manualFontName;                  //用户给的初始数据—字体名称 这个名称现在分为_ZH,TXT,宋体
	int     manualFontNum;                   //用户给的初始数据—字号  不考虑这个参数
	double  manualFontSpacing;               //用户给的初始数据—字间距 给的初始值
	double  manualFontCompressRatio;         //用户给的初始数据—字压缩比 给个初始值
	
	double miniThreshold;					 //最低通过阈值
	FontPictureProprety FontPropertyFinal;   //最后输出的参数,指定参数的话会定参数

	//FontApxNew _FontApxNew;                    //只取一个词的保存

	std::vector <FontApx> List_FontSelect;   //如果取好多个词的话

	IfFontOrlPictureGenerater *FontGenerater;  //字体生成引擎,使用默认引擎即可
	 
	double singleWord(FontApx ApxTarget,int wordNum,double &SizeRoit,double &FontScretch);
	 
	void init( );  //初始化
	void fittingInit(int FontApxNum = 0);  //初次拟合
	void TotalSearch(FontApx input);    //暴力运算
	void TotalSearch2(FontApx input);

//	void fitting( );      //之后继续拟合

	FontSimilar(void);
	~FontSimilar(void);
};




