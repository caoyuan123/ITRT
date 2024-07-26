#include "FontSimilarDef.h"



FontSimilarPara::FontSimilarPara()
{

	//默认给非法数据
	isSearch = false;
	range[0] = -23767;
	range[1] = -32767;
	step = 65535;

	idx = 0;
	over = false;
}

FontSimilarPara::FontSimilarPara(bool bl,double R1,double R2,double S)
{
	isSearch = bl;
	range[0] = R1;
	range[1] = R2;
	step = S;

	idx = 0;
	over = false;
}

void FontSimilarPara::setStart()
{
	idx = 0;
	over = false;
}

#define HugNum 0xffff

double FontSimilarPara::getNext()
{
	if (!isSearch)  //不启用
	{
		over = true;
		return HugNum;
	}else
	{
		if (over)
		{
			return HugNum;
		}else
		{
			if (dTable.empty())  //没有用表的方式
			{
				double Rt = range[0] + step * idx;
				if (Rt <= range[1])
				{
					idx ++;

					if (::abs(Rt - range[1]) < 0.00001)
					{
						over = true;
					}

					return Rt;
				}else
				{
					over = true;
					return HugNum;
				}

			}else  //使用表的方式
			{
				if (idx < dTable.size())
				{
					double rt = dTable[idx];
					++idx;
					if (idx == dTable.size())
					{
						over = true;
					}
					return rt;
				}
				else 
				{
					over = true;
					return HugNum;
				}

			}
		}
	}
	

}

bool FontSimilarPara::isEnd()
{
	return over;
}

int FontSimilarPara::getIdx()
{
	return idx;
}

FontSimilarDef::FontSimilarDef()
{
	FontNum = FontSimilarPara(false,26,57,1);
	FontSpacing = FontSimilarPara(false,0.60,1.1,0.01);
	FontCompress = FontSimilarPara(false,0.60,1.1,0.01);

	RoitGluss = FontSimilarPara(false,1,5,2);
	RoitErode = FontSimilarPara(false,1,3,1);
	RoitSize = FontSimilarPara(false,0,0,0);

	RoitPow = FontSimilarPara(false,0,0,0);
	double Pow[6] = {1/2,1/3,1,2,3,4}; 
	RoitPow.dTable = std::vector<double>(Pow,Pow+6);
}




