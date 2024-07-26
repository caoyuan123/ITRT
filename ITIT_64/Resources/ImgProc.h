#pragma once
#include <opencv.hpp>
//#include <vector>

namespace ImgProc //ImageProcessing ͼ����
{
	int getPPi(cv::InputArray Src,cv::Size size = cv::Size(1169,1653)); //��PPiֵ,sizeΪӢ��

	// ���ص���ƥ��Ľ��, 8uc1
	extern double MatCompare(cv::InputArray Src,cv::InputArray Dst,cv::Rect *Roi = nullptr);

	//plus�ǷŴ����������,����Ŵ���,dst�ǷŴ���ͼ��.�ɱ����Ŵ�plusҪ���С
	extern double MatResizePix(const cv::Mat &src,cv::Mat &dst,int plus);

	//��ͼ��������������������Size�Ĵ�С
	extern void MatSuitableSize(cv::InputArray src,cv::OutputArray dst,cv::Size Size);

	//p1��p2���ٽ���ô��
	extern bool isNearPoint(CvPoint p1,CvPoint p2,int distance = 20);

	//�ǲ��һ����Rectô��
	extern bool isNearRect(cv::Rect &r1,cv::Rect &r2,int distance = 10);

	extern int calWordHight(const cv::Mat &_Mat);  //����8uc1ͼƬ������

	extern void calWordHight(const cv::Mat &_Mat,int &first,int &end);      //��������
	extern std::vector<int> calWordDistance(const cv::Mat &input_mat,int factor_threshold = 250); //����ԭʼ���ּ��ĺ���

}


