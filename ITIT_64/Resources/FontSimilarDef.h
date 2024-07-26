#pragma once
#include <vector>

class FontSimilarPara
{
protected:
	int idx;      //步进
	bool over;    //结束
public:
	bool isSearch;  //是否要搜索
	double range[2];   //范围- 前后
	double step;    //步长

	FontSimilarPara();
	FontSimilarPara(bool,double R1,double R2,double S);

	std::vector<double> dTable;  //使用表的方式

	void setStart();  //
	double getNext();
	int getIdx();
	bool isEnd();     //是否到结束
	
};

class FontSimilarDef
{
public:
	FontSimilarPara FontNum;
	FontSimilarPara FontSpacing;
	FontSimilarPara FontCompress;

	FontSimilarPara RoitGluss;
	FontSimilarPara RoitErode;
	FontSimilarPara RoitPow;
	FontSimilarPara RoitSize;

	FontSimilarDef( );
};

