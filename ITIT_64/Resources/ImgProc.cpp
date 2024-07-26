#include "ImgProc.h"
#include "ComApx.h"
#include "StdAfx.h"
#include <QtGui/QtGui>
#include "MessyCodeTerminator.h"

using namespace cv;

int ImgProc::getPPi(cv::InputArray Src,cv::Size size /*= cv::Size(1169,1653)*/)
{
	double width = Src.getMat().size().width;

	double ppi = width * 100 / size.width;

	return Com::Round(ppi);

}

#define  TimeCul 0
#define  Debug_Print 0

double ImgProc::MatCompare(cv::InputArray Src,cv::InputArray Dst,cv::Rect *Roi)
{
	using namespace cv;

#if TimeCul

	double T1 = (double)getTickCount();

#endif

	//1.确定输入的格式是 灰色,并统一输入格式为灰色
	CV_Assert(Src.getMat( ).channels() == 3 || Src.getMat( ).channels() == 1);


	cv::Mat TargetImgT;
	if (Roi == nullptr)
	{
		TargetImgT = Src.getMat( );

	}
	else
	{
		TargetImgT = Src.getMat( )(*Roi);
	}


	cv::Mat TargetImgG; //灰色大图
	cv::Mat TemplatG;   //灰色小图

	if (TargetImgT.channels() == 3)
	{
		cv::cvtColor(TargetImgT,TargetImgG,CV_BGR2GRAY);
	}
	else if(TargetImgT.channels() == 1)
	{
		TargetImgG = TargetImgT;
	}
	else
	{
		CV_Assert(0);
	}

	if (Dst.getMat().channels() == 3)
	{
		cv::cvtColor(Dst.getMat(),TemplatG,CV_BGR2GRAY);
	}
	else if(Dst.getMat().channels() == 1)
	{
		TemplatG = Dst.getMat();
	}
	else
	{
		CV_Assert(0);
	}

	//加个白边确保 被识别的图比模板图大
	int i = 0;
	do 
	{
		i++;
		cv::copyMakeBorder(TargetImgG,TargetImgG,10,10,10,10,cv::BORDER_ISOLATED ,cv::Scalar(255));
	} while (!(TargetImgG.size().height > TemplatG.size().height + 10 && TargetImgG.size().width > TemplatG.size().width + 10 || i > 20));

	if (i > 20)
	{
		return 0; //模板图不能小于的太多
	}

	cv::Mat Result;   //结果

	double Max = 0,Min = 0;

	cv::Point pMin(0,0),pMax(0,0);

	cv::matchTemplate(TargetImgG,TemplatG,Result,cv::TM_CCOEFF_NORMED);
	cv::pow(Result,5,Result);

#if Debug_Print
	imshow("Result",Result);
	imshow("TargetImg",TargetImgT);
	imshow("Dst",Dst);
#endif

	cv::minMaxLoc(Result,&Min,&Max,&pMin,&pMax);

#if TimeCul
	double T2 = (double)getTickCount();
	cout<<"time_SearchImg:"<<(T2-T1)*1000/(getTickFrequency())<<endl;
#endif

	return Max;
}

double ImgProc::MatResizePix(const cv::Mat &src,cv::Mat &dst,int plus)
{
	double Rt = 1; 
	if (plus == 0)
	{
		Rt = 1;
	}
	else if(plus > 0)
	{
		Rt += ((double)plus + 0.5) / src.size().height;
	}
	else
	{
		Rt += ((double)plus - 0.5) / src.size().height;
	}

	cv::resize(src,dst,cv::Size(src.size().width * Rt ,src.size().height * Rt)); //确保成比例放大
	return Rt;
}

extern void ImgProc::MatSuitableSize(cv::InputArray src,cv::OutputArray dst,cv::Size Newsize)
{
	cv::Mat Msrc = src.getMat();

	double dY  = Msrc.rows / Newsize.height;
	double dX = Msrc.cols / Newsize.width;
	cv::Size FinalSize;

	if (dY > dX)
	{
		FinalSize.height = Msrc.rows / dY;
		FinalSize.width = Msrc.cols / dY;

	}else
	{
		FinalSize.height = Msrc.rows / dX;
		FinalSize.width = Msrc.cols / dX;
	}

	cv::resize(src,dst,FinalSize);

}

extern bool ImgProc::isNearPoint(CvPoint p1,CvPoint p2,int distance /*= 20*/)
{
	if(abs(p1.x - p2.x) < distance && abs(p1.y - p2.y) < distance) 
	{
		return true;
	}
	else
	{
		return false;
	}
}


extern int ImgProc::calWordHight(const cv::Mat &_Mat)
{
	//从第一行向下扫 扫到的第一个出现 <= 50 的字
	int first = 0,end = 0;

	calWordHight(_Mat,first,end);

	return end - first + 1;

	//从最后一行向上扫 扫到的第一个出现的 <= 50 的行
}

extern void ImgProc::calWordHight(const cv::Mat &_Mat,int &first,int &end)
{

	int R = _Mat.rows;  //行

	int C = _Mat.cols;  //列
	first = 0;
	end = 0;
	bool blBk = false;  //提前跳出去

	for (int i = 0; i < R; ++i)
	{
		for(int j = 0;j < C;++j)
		{
			if (_Mat.at<uchar>(i,j) < 50)
			{
				first = i;
				blBk = true;
				break;
			}

		}

		if (blBk)
		{
			break;
		}
	}

	blBk = false;
	for (int i = R - 1; i > 0; --i)
	{
		for(int j = 0;j < C;++j)
		{
			if (_Mat.at<uchar>(i,j) < 50)
			{
				end = i;
				blBk = true;
				break;
			}

		}

		if (blBk)
		{
			break;
		}
	}
}

extern bool ImgProc::isNearRect(cv::Rect &r1,cv::Rect &r2,int distance /*= 10*/)
{

	if (isNearPoint(r1.tl(),r2.tl(),distance) && isNearPoint(r1.br(),r2.br(),distance))
	{
		return true;
	}else
	{
		return false;
	}

}

//计算原始的字间距的函数
std::vector<int> ImgProc::calWordDistance(const cv::Mat &input_mat,int factor_threshold/* = 250*/)
{
	std::vector<int> rt_Result;


	//只能做8uc1格式的图片
	if (input_mat.type() != CV_8UC1)
	{
		qFatal("Error Input unsupport Mat type");
		return rt_Result;
	}

	//第一个分隔线就在最左端
	//	rt_Result.push_back(0);

	//从上往下 从左往右 扫竖线
	int width = input_mat.size().width;
	int height = input_mat.size().height;

	std::vector<int> temp_vecInt;  //暂存的玩意

	bool allWhite = true; //纯白列
	bool continueWhite = true; //连续白色列

	for (int w = 0;w < width; ++w)
	{
		allWhite = true;

		for (int h = 0; h < height; ++h)
		{
			//检测是否大于250 也就是白色, 白色则算为分隔
			if (input_mat.at<unsigned char>( h , w ) < factor_threshold)
			{
				allWhite = false;
				break;
			}

		}

		if (allWhite == true)
		{
			temp_vecInt.push_back(w);
		}

		if (continueWhite == true && allWhite == false)
		{

			//取中间那个
			int sum = 0;

			for (int i = 0;i < temp_vecInt.size();i++)
			{
				sum += temp_vecInt[i];
			}

			if (temp_vecInt.size() == 0 && w == 0)
			{
				sum = 0;  //起始就是黑的
			}else
			{
				sum /= temp_vecInt.size();
			}


			rt_Result.push_back(sum);

			temp_vecInt.clear();
			continueWhite = false;
		}

		if (w == width - 1) //结尾的处理
		{
			if (continueWhite == true)
			{
				//取中间那个
				int sum = 0;

				for (int i = 0;i < temp_vecInt.size();i++)
				{
					sum += temp_vecInt[i];
				}

				sum /= temp_vecInt.size();

				rt_Result.push_back(sum);

				temp_vecInt.clear();
				continueWhite = false;
			}else
			{
				rt_Result.push_back(width);
			}

		}

		if (continueWhite == false && allWhite == true)
		{
			continueWhite = true;  //开始第一个白色的数列
		}
	}

	//如果最后没有 在最后补一个

	if (abs(rt_Result[rt_Result.size() - 1] - width) > 3)
	{

		rt_Result.push_back(width);

	}



	return rt_Result;
}


