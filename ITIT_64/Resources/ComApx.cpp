#include "ComApx.h"
#include <math.h>
#include <stdio.h>
#include <iostream>
#include <opencv.hpp>
#include <QDebug>
#include  <io.h>

//#define _DEBUG 1

double Com::ApxMid(double iLeft,double Range,calculateFunction func,void *object,int outRange)
{
	double Left = iLeft;
	double Right = iLeft + Range;
	double Mid;

	double dL = func(Left,object);
	double dR = func(Right,object);
	double dM; 

	double lastBest = 0;

	for(int i = 0,j = 0;i < 50;i++)
	{
		Mid = (Left + Right) / 2;
		dM = func(Mid,object);
		
		if(abs(Left - Right) < abs(Right) * 0.01) //左右极限大致相等了
		{
#ifdef _DEBUG
			//printf("%f-%f\r\n",Mid,dM);
#endif
			
			if(j < 4) 
			{
				if(outRange >= 0 )
				{
					if(j != 0 && abs(lastBest - Mid) < abs(lastBest) * 0.01)
					{
						break; 
					}
					lastBest = Mid;
					if(outRange == 0)
					{
						Left = Mid - Range / 10;
						Right = Mid + Range /10;
					}
					else
					{
						Left =  Mid - Range / (double)outRange;
						Right = Mid + Range / (double)outRange;
					}

					dL = func(Left,object);
					dR = func(Right,object);

					j++;
				}else
				{
					break;
				}

			}else
			{
				break;
			}
		}
		else if(dL > dR)
		{
			dR = dM;
			Right = Mid;
		}
		else
		{
			dL = dM;
			Left = Mid;
		}
	}

	return Mid;
}

int Com::ApxMid_int(int iLeft,int Range,calculateFunction func,void *object,int outRange )
{

	int Left = iLeft;
	int Right = iLeft + Range;
	int Mid;

	double dL = func(Left,object);
	double dR = func(Right,object);
	double dM; 

	double lastBest = 0;

	for(int i = 0,j = 0;i < 30;i++)
	{
		Mid = Com::Round((Left + (double)Right) / 2);
		dM = func(Mid,object);

		if(Left == Right || Mid == Right) //左右极限大致相等了
		{

			if (dL > dM) //看一下是否不一样
			{
				dM = dL;
				Mid = Left;
			}
#ifdef _DEBUG
			//printf("%d-%f\r\n",Mid,dM);
#endif

			if(j < 2) //扩充寻找两次
			{
				if(outRange >= 0 )
				{
					if(j != 0 && lastBest == Mid)
					{
						break; //扩充了之后找的还和上次一样就不找了
					}

					lastBest = Mid;
					if(outRange == 0)
					{
						Left = Mid - Range / 5;
						Right = Mid + Range /5;
					}
					else
					{
						Left = Mid - outRange;
						Right = Mid + outRange;
					}

					dL = func(Left,object);
					dR = func(Right,object);

					j++;
				}else
				{
					break;
				}

			}else
			{
				break;
			}
		}
		else if(dL > dR)
		{
			dR = dM;
			Right = Mid;
		}
		else
		{
			dL = dM;
			Left = Mid;
		}
	}

	return Mid;
}



double Com::Round(double r)
{
	return (r > 0.0) ? floor(r + 0.5) : ceil(r - 0.5);
}

double Com::SearchMax(double iLeft,double iRight,double iScale,calculateFunction func,void *object)
{
	double Left = iLeft;
	double Right = iRight;
	if (Left > Right)
	{
		std::swap(Left,Right);
	}

	double Temp = Left;
	double TempResult;
	double Max = Left;
	double MaxResult = 0;
	
	for (int i = 0 ; i < 1000 && Temp <= Right;i++)
	{
		Temp += iScale; //每步长

		TempResult = func(Temp,object);
		if (TempResult > MaxResult)
		{
			Max = Temp;
			MaxResult = TempResult;
		}

	}

#ifdef _DEBUG

	printf("MaxFind Max:%f - MaxResult:%f\r\n",Max,MaxResult);

#endif
	return Max;
}

extern double Com::stdCompareRoit(double wordRoit,int szNum)
{

	if (szNum <= 0)
	{
		qFatal("Error szNum must upper than zero!");
	}else
	{
		double rt = wordRoit * cv::cubeRoot(szNum);

		return  rt ; 
	}
	
	return 1;  //F2S
}

extern double Com::wordCompareRoit(double stdRoit,int szNum)
{
	if (szNum <= 0)
	{
		qFatal("Error szNum must upper than zero!");
	}else
	{
		return stdRoit / cv::cubeRoot(szNum);
	}
	
	return 0;
}

extern bool Com::isFileInUse(std::string fileName)
{
	if (_access(fileName.data(),2) != -1)
	{
		return true;
	}else
	{
		return false;
	}
}




char Com::ZHFontSavePlace[] = ".\\Font\\newZHfontFF2.png";


const std::string FileTypes[ImageTypesMax] = {"jpg","dng","tif"};


