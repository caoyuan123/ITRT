#pragma once
#include <QImage>
#include <cxcore.h>



//这是用于 Qt与Cv格式的图形格式转换函数
class QtCv
{
public:
	//1.Cv2Qt 深拷贝
	static QImage cvMat2Qimg(const cv::Mat &input);

	//2.Qt2Cv 深拷贝
	static cv::Mat Qimg2cvMat(const QImage &input);


	//3.Cv2Qt 灰色单通道-增添图像头仅供显示

	//4.Qt2Cv 灰色单通道-增加图像头的显示


	
};




