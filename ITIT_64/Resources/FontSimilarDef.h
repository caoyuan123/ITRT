#pragma once
#include <vector>

class FontSimilarPara
{
protected:
	int idx;      //����
	bool over;    //����
public:
	bool isSearch;  //�Ƿ�Ҫ����
	double range[2];   //��Χ- ǰ��
	double step;    //����

	FontSimilarPara();
	FontSimilarPara(bool,double R1,double R2,double S);

	std::vector<double> dTable;  //ʹ�ñ�ķ�ʽ

	void setStart();  //
	double getNext();
	int getIdx();
	bool isEnd();     //�Ƿ񵽽���
	
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

