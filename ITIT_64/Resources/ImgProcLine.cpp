#include "ImgProcLine.h"
#include <opencv.hpp>
#include <math.h>
#include <highgui.h>

#include "DrawingSheet.h"
#include <QDebug>

using namespace std;
using namespace cv;


///////////////////////////////////////////////////////////////////





inline std::vector<CvLine> ImgProcLine::NearLine(std::vector<CvLine> &Org)
{ 
	std::vector<CvLine> rt;

	int s = Org.size();
	for (int i = 0; i < s;i++)
	{
		if (Org[i].checked != true)
		{
			std::vector<CvLine> temp;

			temp.push_back(Org[i]);

			for (int j = i + 1;j < s;j++)
			{
				if(CvLine::isSimilarLine(Org[i],Org[j],NearLine_theta,NearLine_rho)) //�ҵ������ٽ���
				{
					if (!Org[j].checked)
					{				
						Org[j].checked = true;
						temp.push_back(Org[j]);
					} else ;
					
				}else ;
				
			}

			CvLine avg(0,0,0,0);  //��ƽ��
			
			//��ƽ����
			for (int j = 0;j < temp.size();j++)
			{
				avg.Point1.x += temp[j].Point1.x;    
				avg.Point1.y += temp[j].Point1.y;
				avg.Point2.x += temp[j].Point2.x;
				avg.Point2.y += temp[j].Point2.y;
			}

			avg.Point1.x /= temp.size();
			avg.Point1.y /= temp.size();
			avg.Point2.x /= temp.size();
			avg.Point2.y /= temp.size();

			rt.push_back(avg);

		}else;
		Org[i].checked = true;
	}

	return rt;
}

//����ֱ�� theta Ĭ����theta1/2 Ĭ���Ƕȣ�Dirction - true ������ false�Ǻ᷽�� 
static std::vector<CvLine> CalLine(IplImage* canny,int HoughMethod,int Threshold,float theta1,float theta2,bool Dirct)
{
	CV_Assert(canny->nChannels == 1,"�������");
	CV_Assert(HoughMethod == 0,"�������");
	CV_Assert(Threshold > 0,"�������");

	std::vector<CvLine> Rt;

	if (theta1 > theta2)
	{
		std::swap(theta1,theta2);
	}else ;

	CvMemStorage* storage = cvCreateMemStorage(0);
	CvSeq* results =  cvHoughLines2(canny,storage,CV_HOUGH_STANDARD,1,PI/90,Threshold);

	for (int i =0;i < results->total;i++)
	{
		float* p = (float*) cvGetSeqElem(results,i);

		double rho = p[0];
		double theta = p[1];

		if (Dirct)   
		{   //����
			if (theta < theta1 / 180 * PI || theta > theta2 / 180 * PI)  //ֻ�мн���10�Ƚ����ڵ��߲�Ҫ  +- 10
			{
				CvLine Push = CvLine::Polar2Descartes(cvGetSize(canny),rho,theta);
				Rt.push_back(Push);
			}else ;
		} 
		else //����
		{
			if (theta > theta1 / 180 * PI && theta < theta2 / 180 * PI )           //ֻ�мн���10�Ƚ����ڵ��߲�Ҫ (theta > PI/2.25f && theta < PI / 1.80f) 
			{
				CvLine Push = CvLine::Polar2Descartes(cvGetSize(canny),rho,theta);
				Rt.push_back(Push);

				/*  //��BUG�õĴ���
				if (Push.Point1.x == 0 && Push.Point2.x == 0 &&
					Push.Point1.y == 0 && Push.Point2.y == 0 )
				{
					qDebug() << "BUG!!!";
				}*/
			}else ;
		}

	}
	cvClearSeq(results);
	cvReleaseMemStorage(&storage);

	return Rt;
}

void ImgProcLine::ImgProc(cv::InputArray _inputArray)
{
	IplImage srcMat = _inputArray.getMat();
	IplImage* Src = new IplImage(srcMat);

	IplImage* canny = cvCreateImage(cvSize(Src->width,Src->height),Src->depth,1);

	std::vector<CvLine> verticalLine;      //��ʵ��hough�������Ķ���
	std::vector<CvLine> horizontalLine;

	int VThreshold = 0;    //��ֱ��ֵ   
	int HThreshold = 0;    //ˮƽ��ֵ

	IplImage * gray = cvCreateImage(cvSize(Src->width,Src->height),Src->depth,1);
	
	//1.����ͼƬ�����ʽ - ֻ����ڰ׵�ͼƬ
	if (Src->nChannels == 1) //ֻ�ܸ���ɫ
	{
		cvCopy(Src,gray);
	}else return;

	//2.�̶���Canny

	switch(ThresholdType)
	{
	case 0:
		{
			cvCanny(gray,canny,130,80);   //Ĭ���Ѿ��˲�����
		}break;
	case 1:
		{
			cvThreshold(gray,canny,128,255,THRESH_BINARY_INV);
		}break;
	default:
		{
			cvThreshold(gray,canny,128,255,THRESH_BINARY_INV); 
		}break;
	}


	//�趨��ʼֵΪ ���ߵ� 80% �Ĺᴩ��Ϊֱ��
	HThreshold = cvRound((double)Src->width * HRoit);
	VThreshold = cvRound((double)Src->height * VRoit);

	//3.Ȼ�����ֱ����ֱ��
	verticalLine = CalLine(canny,CV_HOUGH_STANDARD,VThreshold,VAngle1,VAngle2,true);
	horizontalLine = CalLine(canny,CV_HOUGH_STANDARD,HThreshold,HAngle1,HAngle2,false);

//	VLine = verticalLine;
//	HLine = horizontalLine;



	/*  //��BUG�õĴ���
	for (int i = 0;i < verticalLine.size();++i)
	{
		if (verticalLine[i].Point1.x == 0 && verticalLine[i].Point2.x == 0 &&
			verticalLine[i].Point1.y == 0 && verticalLine[i].Point2.y == 0 )
		{
			qDebug() << "V" << i;
		}
	}

	for (int i = 0;i < horizontalLine.size();++i)
	{
		if (horizontalLine[i].Point1.x == 0 && horizontalLine[i].Point2.x == 0 &&
			horizontalLine[i].Point1.y == 0 && horizontalLine[i].Point2.y == 0 )
		{
			qDebug() << "H" << i;
		}
	}*/

	VLine.clear();
	HLine.clear();

	//�ٽ��ߺ�ƴ
	VLine = NearLine(verticalLine);
	HLine = NearLine(horizontalLine);



	//�����㷨��ʼ
	verticalLine.clear();
	horizontalLine.clear();

	if (broadPlus)  //��ӱ��ߵĻ�
	{
#define BorderLine 2   //��ӱ��߾�����صľ���
		//��ӱ���
		//�����
		CvLine Top = CvLine(0, BorderLine, canny->width, BorderLine);
		CvLine Buttom = CvLine(0, canny->height - BorderLine, canny->width, canny->height - BorderLine);
		
		int s = HLine.size();

		for (int i = 0;i < s; ++i)
		{
			if (Top.checked == false && CvLine::isSimilarLine(HLine[i], Top, 0.01, canny->height * 0.05))
			{
				Top.checked = true;
			}

			if (Buttom.checked == false && CvLine::isSimilarLine(HLine[i], Buttom, 0.01, canny->height * 0.05))
			{
				Buttom.checked = true;
			}

		}
		if (Top.checked == false)
		{
			HLine.push_back(Top);
		}

		if (Buttom.checked == false)
		{
			HLine.push_back(Buttom);
		}



		//������
		CvLine Left = CvLine(BorderLine,0,BorderLine,canny->height);
		CvLine Right = CvLine(canny->width - BorderLine,0,canny->width - BorderLine,canny->height);

		s = VLine.size();
		for (int i = 0;i < s;++i)
		{
			if (Left.checked == false && CvLine::isSimilarLine(VLine[i],Left,0.01,canny->width * 0.05))
			{
				Left.checked = true;
			}

			if (Right.checked == false && CvLine::isSimilarLine(VLine[i],Right,0.01,canny->width * 0.05))
			{
				Right.checked = true;
			}
		}

		if (Left.checked == false)
		{
			VLine.push_back(Left);
		}

		if (Right.checked == false)
		{
			VLine.push_back(Right);
		}
	}
	

	cvReleaseImage(&canny);
}

//��û���Թ�
void ImgProcLine::ImgProc(DrawingSheet* Ptr_DwSt)
{

//ȡPtr_Dwst�е�ͼ��
	ImgProc(Ptr_DwSt->getMat( ));
//ʹ��ImgProc����

//д��Ptr_Dwst��,дһ�½׶�
	Ptr_DwSt->VLines = this->VLine;
	Ptr_DwSt->HLines = this->HLine;


}


ImgProcLine::ImgProcLine(void)
{
	ThresholdType = 1;

	broadPlus = true;

	Ptr_VLine = new std::vector<CvLine>();

	switch(ThresholdType)
	{
	case 0:
		{
			HRoit = 0.35;
			VRoit = 0.35;

			VAngle1 = 10;
			VAngle2 = 170;

			HAngle1 = 80;
			HAngle2 = 100;
		}break;
	case 1:
		{
			HRoit = 0.55;
			VRoit = 0.55;

			VAngle1 = 5;
			VAngle2 = 175;

			HAngle1 = 85;
			HAngle2 = 95;
		}break;
	default: 
		{
			HRoit = 0.8;
			VRoit = 0.8;

			VAngle1 = 10;
			VAngle2 = 170;

			HAngle1 = 80;
			HAngle2 = 100;
		}break;
	}


	NearLine_theta = 0.035f;
	NearLine_rho = 20.0f;
}

ImgProcLine::~ImgProcLine(void)
{
	
}


//���Ժ���



int testZonemian(int argc, char *argv[])
{
	/*
	ImgProcLine ipl;

	cv::Mat mat = imread("E:\\Cpp\\t123.png");
	cv::Mat g;

	cv::cvtColor(mat,g,CV_BGR2GRAY);

	ipl.ImgProc(g);

	for (int i = 0;i < ipl.HLine.size();i++)
	{
		cv::line(mat,ipl.HLine[i].Point1,ipl.HLine[i].Point2,cv::Scalar(0,128,255),3);
	}

	for (int i = 0;i < ipl.VLine.size();i++)
	{
		cv::line(mat,ipl.VLine[i].Point1,ipl.VLine[i].Point2,cv::Scalar(255,128,0),3);
	}



	cvNamedWindow("222",CV_WINDOW_FREERATIO);
	imshow("222",mat);
	cv::imwrite("E:\\Cpp\\RR.bmp",mat);

	cvWaitKey(0);

	*/
	return 0;
}






