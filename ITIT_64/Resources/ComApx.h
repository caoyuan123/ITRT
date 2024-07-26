#pragma once
#include <string>

namespace Com
{
	inline double Round(double r);

	//rt:算出值,input给定值,object 是用于传参数的
	typedef double (*calculateFunction)(double input,void *object);
	//二分法找最优,返回最优解,左初始值,初始范围,计算函数,是否允许超范围寻找,超范围时每部长寻找多少，-1 表示不允许，0默认为range / 10 ,其余正数为每步长 默认截止为8步 或 两步返回差值小于当前最优解的 0.01 且已经寻找了超范围解
	extern  double ApxMid(double iLeft,double Range,calculateFunction func,void *object,int outRange = 0);
	extern  int ApxMid_int(int iLeft,int Range,calculateFunction func,void *object,int outRange = 0);

	//暴力搜索最佳,左值-右值-分划度
	extern  double SearchMax(double iLeft,double iRight,double iScale,calculateFunction func,void *object);
	
	extern char ZHFontSavePlace[];  //zH字体存放地点

	

#define  ImageTypesMax 6    //可以读入的图形种类
	extern const std::string FileTypes[ImageTypesMax];



	extern double stdCompareRoit(double wordRoit,int szNum);  //词本身的比较率转标准率
	extern double wordCompareRoit(double stdRoit,int szNum);  //标准率转词的比较率
	

	extern bool isFileInUse(std::string fileName);
}






