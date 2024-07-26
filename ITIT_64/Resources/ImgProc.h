#pragma once
#include <opencv.hpp>
//#include <vector>

namespace ImgProc //ImageProcessing 图像处理
{
	int getPPi(cv::InputArray Src,cv::Size size = cv::Size(1169,1653)); //算PPi值,size为英寸

	// 返回单次匹配的结果, 8uc1
	extern double MatCompare(cv::InputArray Src,cv::InputArray Dst,cv::Rect *Roi = nullptr);

	//plus是放大的像素数量,输出放大倍率,dst是放大后的图形.成比例放大到plus要求大小
	extern double MatResizePix(const cv::Mat &src,cv::Mat &dst,int plus);

	//将图像正比例调整到不超过Size的大小
	extern void MatSuitableSize(cv::InputArray src,cv::OutputArray dst,cv::Size Size);

	//p1和p2是临近点么？
	extern bool isNearPoint(CvPoint p1,CvPoint p2,int distance = 20);

	//是差不多一样的Rect么？
	extern bool isNearRect(cv::Rect &r1,cv::Rect &r2,int distance = 10);

	extern int calWordHight(const cv::Mat &_Mat);  //计算8uc1图片最上下

	extern void calWordHight(const cv::Mat &_Mat,int &first,int &end);      //计算上下
	extern std::vector<int> calWordDistance(const cv::Mat &input_mat,int factor_threshold = 250); //计算原始的字间距的函数

}


