#pragma once
#include <QImage>
#include <cxcore.h>



//�������� Qt��Cv��ʽ��ͼ�θ�ʽת������
class QtCv
{
public:
	//1.Cv2Qt ���
	static QImage cvMat2Qimg(const cv::Mat &input);

	//2.Qt2Cv ���
	static cv::Mat Qimg2cvMat(const QImage &input);


	//3.Cv2Qt ��ɫ��ͨ��-����ͼ��ͷ������ʾ

	//4.Qt2Cv ��ɫ��ͨ��-����ͼ��ͷ����ʾ


	
};




