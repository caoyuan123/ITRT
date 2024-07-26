#pragma once
#include <cxcore.h>


class FontPicture;
//文字图形变换模板
//引擎生成的原始图片转换为真实晒制蓝图的图形处理对换数据参数
struct FontGraphicTransTemp
{



protected:
	//变换方法,原图/输出图
	void TransForm(cv::InputArray Src,cv::Mat &dst) const;

public:
	double Roit; //放大倍率

	int GlussRoit;     //高斯模糊的矩阵大小 奇数 3,5,7,9
	int ErodeRoit;     //扩增腐蚀倍率,默认3*3不可更改
	int PowRoit;       //加深倍率

	double CompareRoit; //拟合的倍率

	FontGraphicTransTemp();

	

	void TransForm(const FontPicture& Org,FontPicture &Dst) const;  //FontPicture格式的变换

	void SaveXML(char* Str = nullptr,char* FileFode = nullptr);//存xml,路径,或读好的结构
	void ReadXML(char* Str = nullptr,char* FileFode = nullptr);//读xml,路径,或读好的结构

	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const ;


	friend class FontApx;
	friend class FontSimilar;
};



extern void read(const cv::FileNode& node, FontGraphicTransTemp& x,const FontGraphicTransTemp& default_value = FontGraphicTransTemp());
extern void write(cv::FileStorage& fs, const std::string& name, const FontGraphicTransTemp& x);



