#include "QtCv.h"
#include <opencv.hpp>
#include <QtGui/QtGui>


QImage QtCv::cvMat2Qimg(const cv::Mat &mat)
{
	switch(mat.type())
	{
	case CV_8UC1: //Gray 8 ,有可能那个表不精确以后改成 先转成RGB888 给QIMAGE
		{
			QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);   
			image.setColorCount(256);  
			for(int i = 0; i < 256; i++)  
			{  
				image.setColor(i, qRgb(i, i, i));  
			}  
			uchar *pSrc = mat.data;  
			for(int row = 0; row < mat.rows; row ++)  
			{  
				uchar *pDest = image.scanLine(row);  
				memcpy(pDest, pSrc, mat.cols);  
				pSrc += mat.step;  
			}  
			return image;  
		}break;
	case CV_8UC3:
		{
			// Copy input Mat  
			const uchar *pSrc = (const uchar*)mat.data;  
			// Create QImage with same dimensions as input Mat  
			QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);  
			return image.rgbSwapped();  

		}break;
	case CV_8UC4:
		{
			qDebug() << "转换模式:CV_8UC4";  
			// Copy input Mat  
			const uchar *pSrc = (const uchar*)mat.data;  
			// Create QImage with same dimensions as input Mat  
			QImage image(pSrc, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);  
			return image.copy(); 
		}break;

	default:
		{
			qDebug() << "ERROR: Mat could not be converted to QImage.";  
			return QImage();  
		}break;
	}
}
cv::Mat QtCv::Qimg2cvMat(const QImage &image)
{
	cv::Mat mat;  
	//	qDebug() << image.format();  
	switch(image.format())  
	{  
	case QImage::Format_ARGB32:  
	case QImage::Format_RGB32:  
	case QImage::Format_ARGB32_Premultiplied:  
		mat = cv::Mat(image.height(), image.width(), CV_8UC4, (void*)image.constBits(), image.bytesPerLine());  
		break;  
	case QImage::Format_RGB888:  
		mat = cv::Mat(image.height(), image.width(), CV_8UC3, (void*)image.constBits(), image.bytesPerLine());  
		cv::cvtColor(mat, mat, CV_BGR2RGB);  
		break;  
	case QImage::Format_Indexed8:  
		mat = cv::Mat(image.height(), image.width(), CV_8UC1, (void*)image.constBits(), image.bytesPerLine());  
		break;  
	default:
		{
			qDebug()<<"ERROR: QImage could not be converted to Mat.";
		}break;
	}  
	return mat; 
}

