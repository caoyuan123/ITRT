#include <opencv.hpp>
#include <omp.h>
#include <QDebug>
#include <list>

#include "OCREngine.h"
#include "OCREngineThread.h"
#include "DrawingZone.h"
#include "DrawingSheet.h"
#include "DrawingBook.h"
#include "FontGenerater.h"

#include "ImgProc.h"
#include "GlobalSetting.h"

#include "CBI_TableDictionary.h"
#include "OCRDef.h"
#include "MessyCodeTerminator.h"

#include "Lisense.h"

#include "ComApx.h"

#include <ostream>

using namespace cv;
using namespace std;



#define abZore(R) R <= 0 ? 0 : R

///////////////////////////////////////////////////////////////
OCREngineBase::OCREngineBase()
{
	setStdWordSize(10,10);
	setStdThreshold(0.95);
}

OCREngineBase::OCREngineBase(int W,int H,float _Threshold)
{

	setStdWordSize(W,H);
	setStdThreshold(_Threshold);

}

void OCREngineBase::setSetting(const DrawingType& dt)
{
	//1.���������ý�ȥ
	setStdWordSize(dt.stdWordWidth,dt.stdWordHeight);
	setStdThreshold(dt._GraphicTransTemp.CompareRoit);
}


int OCREngineBase::getAdjPointX()
{
	return s_adjPointX;
}

int OCREngineBase::getAdjPointY()
{
	return s_adjPointY;
}

int OCREngineBase::getAdjWordX()
{
	return s_adjWordX;
}

int OCREngineBase::getAdjLineY()
{
	return s_adjLineY;
}

void OCREngineBase::setStdWordSize(int _width,int _hight)
{
	stdWidth = _width;
	stdHight = _hight;

	s_adjWordX = cvRound(stdWidth * u_setCvOCR.g_fWidthRoit) ;
	s_adjLineY = cvRound(stdHight * u_setCvOCR.g_fLineRoit) ;

	s_adjPointY = cvRound(stdHight * u_setCvOCR.g_fWordRoit);
	s_adjPointX = s_adjPointY;
}

void OCREngineBase::setStdThreshold(float _float32)
{
	stdThreshold = _float32;

	minThreshold = u_setCvOCR.g_fMinThresholdRoit * stdThreshold; 
}

float OCREngineBase::getStdThreshold()
{
	return stdThreshold;
}



float OCREngineBase::getMinThreshold(int num)
{
	if (num <= 0)  //Ϊ�˼���֮ǰ��
	{
		return minThreshold;
	}
	
	if (num > 0)
	{
		return Com::wordCompareRoit(stdThreshold,num) * u_setCvOCR.g_fMinThresholdRoit;
	}

	return 0.999999;
}



//////////////////////////////////////////////////////////////////////////////////
OCREngineCv::OCREngineCv(const OCREngineBase& _set)
	: OCREngineBase(_set)
{
	ptr_thread = nullptr;
}


OCREngineCv::OCREngineCv(const DrawingType& dt)
{
	setSetting(dt);
	ptr_thread = nullptr;
}


void OCREngineCv::setSetting(const OCREngineBase& _set)
{
	*this = _set;
}


void OCREngineCv::setSetting(const DrawingType& dt)
{
	OCREngineBase::setSetting(dt);
}


OCREngineCv::~OCREngineCv(void)
{


}


//����Rect���ص������С

inline static int RROverlap(const cv::Rect& box1,const cv::Rect& box2)
{
	if (box1.x > box2.x + box2.width) { return 0; }
	if (box1.y > box2.y + box2.height) { return 0; }
	if (box1.x + box1.width < box2.x) { return 0; }
	if (box1.y + box1.height < box2.y) { return 0; }

	int colInt =  std::min(box1.x + box1.width,box2.x + box2.width) - std::max(box1.x, box2.x);
	int rowInt =  std::min(box1.y + box1.height,box2.y + box2.height) - std::max(box1.y,box2.y);
	
	int intersection = colInt * rowInt;

	int area1 = box1.width * box1.height;
	int area2 = box2.width * box2.height;

	//return (float)intersection / (area1 + area2 - intersection);
	return intersection;
}


inline static void MergeNearPoint(const std::vector<ptPsb> &input,std::vector<ptPsb> &output,int Range) //����ٽ���
{
	int Halfrange = Range;  //���������㷶Χ

	output.clear();
	std::vector<bool> fastcheck;            //ͬ����һ������Ѳ���ı�

	int sz = input.size();
	for(int i = 0;i< sz;i++)
	{
		fastcheck.push_back(false);
	}

	for (int i = 0; i < sz;i++)
	{
		if (!fastcheck[i])
		{
			ptPsb pwp = input[i];

			fastcheck[i] = true;

			int x1 = input[i].Zpoint.x - Halfrange;
			x1 < 0 ? 0 : x1;
			int y1 = input[i].Zpoint.y - Halfrange;
			y1 < 0 ? 0 : y1;

			int x2 = input[i].Zpoint.x + Halfrange;
			int y2 = input[i].Zpoint.y + Halfrange;

			cv::Rect Rt = cv::Rect(cv::Point(x1,y1),cv::Point(x2,y2));

			for (int j = i;j < sz;j++)
			{
				if (!fastcheck[j])  //ֻ��û�����
				{
					if (Rt.contains(input[j].Zpoint))
					{
						if (input[j].possible > pwp.possible)
						{
							Rt.x += pwp.Zpoint.x - input[j].Zpoint.x;  //�����µ����ĵ����� �޸� ��ϲ���ȫbug
							Rt.y += pwp.Zpoint.y - input[j].Zpoint.y;

							pwp = input[j];
						}
						fastcheck[j] = true;
					}
				}
			}

			output.push_back(pwp);
		}
	}


}

//#define PRINTDATA 1



void OCREngineCv::OCRMainEngine(std::vector<FontPicture*> FontPictures,
	const cv::Mat &_matBig,std::vector<OCR_ResultOrlWord*> &Orl_result,DrawingZone* _Dz)
{
	//ȡ �ֵ���С���̾���
	int Range = getAdjPointY( );

	//��ʼ������
	int MaxSize = FontPictures.size( );


#ifdef PRINTDATA
	ofstream file;
	file.open("data.txt",ios::app);
#endif

	if (!CheckRegist( ))
	{
		omp_set_num_threads(1); //û��ע�᲻��ʹ�ö��߳�
	}

	int dftCheck = _matBig.size().area() * 0.005;   

#pragma omp parallel for 
	for (int i = 0 ; i < MaxSize;++i)
	{
		//��ӭ�������̵��õ�,��� ����ͼ �� �����Ͳ�����
		if (FontPictures[i]->_Mat8u1c.size().width > _matBig.size().width)
		{
			qDebug( ) << FontPictures[i]->words.data() << " : " << U8s("����");
			continue;
		}

		double Max = 0,Min = 0;
		cv::Mat ResultRaw ;
		cv::Mat Result ;

		cv::Mat _mat8u = FontPictures[i]->_Mat8u1c;

		cv::matchTemplate(_matBig,_mat8u,ResultRaw,TM_CCOEFF_NORMED);
		cv::pow(ResultRaw,5,Result);
		cv::minMaxLoc(Result,&Min,&Max);
		
		/*  //����ȡ������
		if (FontPictures[i]->words == "(2002/2006)")
		{
			cv::imwrite("ts1.png",_matBig);
			cv::imwrite("ts2.png",_mat8u);
		}
		/**/


		//��ȡ��ֵ
		double s_Threshold = 0;
		if(_Dz->getDrawingType()->saveType == 0)
		{
			s_Threshold = getMinThreshold(  );
		}
		else if(_Dz->getDrawingType()->saveType == 1)
		{
			s_Threshold = getMinThreshold( FontPictures[i]->words.size() );
		}

		double s_MuliThreshold = 0;  //��λ�������ֵ

		if (u_setCvOCR.g_blWordPosJudge)  //��������λ�ų�
		{
			//s_Threshold *= 0.8;
			s_MuliThreshold = s_Threshold * 0.8;
		}

		if (Max > s_Threshold)
		{
			OCR_ResultOrlWord* Rt = new OCR_ResultOrlWord(FontPictures[i],_Dz); 
			if (u_setCvOCR.g_blSaveTempResult)
			{
				Rt->Mat_32f1c = Result;
			}

			std::vector<ptPsb> pTemp;       //�ݴ���
		
			//�������е�
			for (int H = 0; H < Result.rows; ++H)   
			{
				for (int W = 0; W < Result.cols; ++W)
				{
					double rs_temp = Result.at<float>(cv::Point(W,H));

					if (rs_temp > s_Threshold && rs_temp < 0.999)
					{
						//��¼
						ptPsb p;
						p.Zpoint = cv::Point(W,H);
						p.possible = rs_temp;  

						pTemp.push_back(p);
					}
				}
			}

		
			/*	{
				std::vector<int> m_hist;
				std::vector<float> m_range;
				m_hist.push_back(256);
				m_range.push_back(0);
				m_range.push_back(1);

				cv::Mat histMat;

				cv::calcHist(Result,m_hist,noArray(),histMat,m_hist,m_range);
			}*/

			//CCOEFF��dft������������Ĵ���
		

			if (pTemp.size() > dftCheck)
			{
				//imwrite(Rt->getWord() + "cs.png",Result);
				//imwrite(Rt->getWord() + "csW.png",ResultRaw);
				//_mat8u.at<uchar>(_mat8u.rows /2, _mat8u.cols /2) = 255 - _mat8u.at<uchar>(_mat8u.rows /2, _mat8u.cols /2);
				
				//��_mat8u ��һ����ɫ�ϱ�
				cv::copyMakeBorder(_mat8u,_mat8u,1,0,0,0,cv::BORDER_ISOLATED,cv::Scalar(255,255,255));

				cv::matchTemplate(_matBig,_mat8u,ResultRaw,TM_CCOEFF_NORMED);

				cv::pow(ResultRaw,5,Result);

				cv::minMaxLoc(Result,&Min,&Max);

				qDebug() << U8c(Rt->getWord().data()) << U8c("����") << pTemp.size() ; 

				pTemp.clear();

				for (int H = 0; H < Result.rows; ++H)   
				{
					for (int W = 0; W < Result.cols; ++W)
					{
						double rs_temp = Result.at<float>(cv::Point(W,H));
						if (rs_temp > s_Threshold && rs_temp < 0.999)
						{
							//��¼
							ptPsb p;
							p.Zpoint = cv::Point(W,H);
							p.possible = rs_temp; 
							pTemp.push_back(p);

						}
					}
				}
				qDebug() << U8c(Rt->getWord().data()) << U8c("�����") << pTemp.size() ; 
			}
			
			int reNum = 0;
			while (pTemp.size() > dftCheck) 
			{
				s_Threshold += 0.02;
				reNum++;
				qDebug( ) << U8c(Rt->getWord( ).data( )) << U8c("����") << pTemp.size( ) << U8c("���¼���") << reNum << U8c("��"); 
				pTemp.clear( );

				for (int H = 0; H < Result.rows; ++H)   
				{
					for (int W = 0; W < Result.cols; ++W)
					{
						double rs_temp = Result.at<float>(cv::Point(W,H));
						if (rs_temp > s_Threshold && rs_temp < 0.999)
						{
							//��¼
							ptPsb p;
							p.Zpoint = cv::Point(W,H);
							p.possible = rs_temp; 
							pTemp.push_back(p);
							
						}
					}
				}
			}


			//���õ����Ƚ��㷨�ų�һ��,
			if (u_setCvOCR.g_blWordPosJudge)
			{
				std::vector<ptPsb> pDelTemp;  //ɾ������ת

				//�ų�����ƥ����ر�͵�
				for (int pTemp_i = 0; pTemp_i < pTemp.size();pTemp_i++)
				{

					double dbg_d = pTemp[pTemp_i].setMultiPossbile(_matBig,*FontPictures[i]);

					if (dbg_d > s_MuliThreshold)
					{
						pDelTemp.push_back(pTemp[pTemp_i]);
					}

				}

				pTemp = pDelTemp;
			}

			//�ϲ��ٽ��ĵ�
			if (pTemp.size() > 1)
			{
				MergeNearPoint(pTemp,Rt->PointList,Range); 
			}
			else
			{
				Rt->PointList = pTemp;
			}


			/*

			//���Ӵ������ݼ��� ÿ����ѡ��ĵ��ֵ�Ч֮��,�ŵ������ط�������
			if (u_setCvOCR.g_blWordPosJudge && false)  
			{
				int SizeNum = Rt->PointList.size( );
				for (int pl = 0; pl < SizeNum; ++pl)
				{
					Rt->PointList[pl].setMultiPossbile(_matBig,*FontPictures[i]);
				}
			}*/


			//���Ӵ������� �������ٽ��߽�ĵ�






			//////////////////////////////////




#pragma omp critical  //�ٽ���
			{


				qDebug( ) << U8c(Rt->_FontPicture->words.data( )) << " : " << Rt->PointList.size( ) << s_Threshold;

#if PRINTDATA
				for (int i = 0;i < Rt->PointList.size();i++)
				{
					file << Rt->getWord().size() << "\t" << Rt->PointList[i].possible << "\r\n";
				}
#endif


				Orl_result.push_back(Rt);
			}

		}

		if (ptr_thread != nullptr)
		{
//#pragma omp critical  //qt�Դ��̵߳ȴ�
			{
				ptr_thread->EmitProcessBar(-1);  //���ͽ���������1
			}
		}

	}

#ifdef PRINTDATA
	file.close();
#endif


}


void OCREngineCv::OCRDrawingSheet(DrawingSheet &Sheet)
{
	//�������� OCRzoneһ��һ���ŵ� OCREngineDrawingZone��
	for (int i = 0;i < Sheet.m_ZoneBeSelected.size( );i++)
	{
		OCRDrawingZone(*(Sheet.m_ZoneBeSelected[i]) );
	}

	qDebug() << "ocrOVER!";
}


int OCREngineCv::calTotalNums(DrawingSheet &Sheet)
{
	int rt = 0;

	for (int i = 0;i < Sheet.m_ZoneBeSelected.size();i++)
	{
		rt += OCRDrawingZone(*(Sheet.m_ZoneBeSelected[i]),true);
	}

	return rt;
}


#undef _DEBUG  //����ӡ��

int OCREngineCv::OCRDrawingZone(DrawingZone &Zone,bool calNums)
{
	//1.ͨ����ͬ��ZoneType���ز�ͬ�� ��ʶ�𼯺�
	DrawingBook* ptr_DrawingBook = Zone.getDrawingBook();

	CBI_TableDictionary *ptr_Dictionary = ptr_DrawingBook->m_Dictionary;

	std::vector<OCR_ResultOrlWord*> OrlWordResult1;  //
	//std::vector<OCR_ResultOrlWord*> OrlWordResult2;  //Ԥ����

	std::vector<OCRAlternativePoint*> AlterResult1;  //���Ϻõĵ���

	std::vector<OCRLine*> LineResult1;  //��������ô���Ķ��ð�������

	int NumReturn = 0;

	switch (Zone.m_Header)
	{
	case CBITable::���а�ť : 
		{
			if (calNums)  //������� �뱣֤�� ��ʽMainEngine�ļ�������һ��
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::�г���ť����].size();
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::������ť����].size();

				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::�г���ť����]
			,Zone.getMat(),OrlWordResult1,&Zone);

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::������ť����]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//д��DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("RpLA.png",TempShow);
#endif
		}
	    break;


	case CBITable::����_���: 
		{
			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::��������].size();
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::��������].size();

				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::��������]
				,Zone.getMat(),OrlWordResult1,&Zone);
			
			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::��������]
				,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//д��DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rdch.png",TempShow);
#endif

		}break;
	
	case CBITable::�������_��� : 
		{
			// �ӽ�ȥ���� �� ������������
			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::��������].size();
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::��������].size();

				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::��������]
			,Zone.getMat(),OrlWordResult1,&Zone);

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::��������]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//д��DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rgdh.png",TempShow);
#endif

		}break;

	case CBITable::�ж��ź� :
		{
			
			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::�ж��ź�].size();
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::�ж��źŵ���].size();

				return NumReturn;
			}

			//���� �г��ĵ����źŵƽ�ȥ����
			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::�ж��ź�]
			,Zone.getMat(),OrlWordResult1,&Zone);

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::�ж��źŵ���]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//д��DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rddxh.png",TempShow);
#endif

		}break;

	case CBITable::�źŻ�_����:
		{

			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::�г��źŻ�����].size();
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::�����źŻ�����].size();

				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::�г��źŻ�����]
			,Zone.getMat(),OrlWordResult1,&Zone);

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::�����źŻ�����]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//д��DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rxhjm.png",TempShow);
#endif

		}break;

	case CBITable::Headers::�źŻ�_��ʾ :
		{

			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::����ʾ��ʽ].size();
				
				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::����ʾ��ʽ]
			,Zone.getMat(),OrlWordResult1,&Zone);


			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//д��DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rxxxs.png",TempShow);
#endif

		}break;

	case CBITable::Headers::�������� :
		{

			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::��������].size();
				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::��������]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//д��DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rqtls.png",TempShow);
#endif

		}break;	

	case CBITable::Headers::�ж�_ӭ���г� :
		{

			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::��������].size();
				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::��������]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//д��DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rymlc.png",TempShow);
#endif
		

		}break;	

	case CBITable::Headers::�ж�_ӭ����� :
		{

			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::��������].size();
				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::��������]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//д��DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rymdd.png",TempShow);
#endif
	
		}break;	


	case CBITable::Headers::��·����_�� :
		{

			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::��������].size();
				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::��������]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//д��DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

		}break;	

	default:

		QString str = U8s( CBITable::HeadersChinese[Zone.m_Header].data() ) +  U8s(" ����");
		
		qDebug() << str;

		break;
	}


	return NumReturn;

}

void OCREngineCv::DrawShowPicture(std::vector<OCR_ResultOrlWord*> &Orl_result,const cv::Mat &src,cv::Mat &_Mat)
{
	_Mat = src;

	if (_Mat.channels() == 1)
	{
		cv::cvtColor(_Mat,_Mat,cv::COLOR_GRAY2BGR);
	}

	int S = Orl_result.size();
	for (int i = 0;i < S; ++i)
	{

		std::string Str = Orl_result[i]->getWord();
		cv::Rect _rect = cv::Rect(cv::Point(0,0),Orl_result[i]->getSize());

		for (int j = 0;j < Orl_result[i]->PointList.size(); ++j)
		{
			_rect.x = Orl_result[i]->PointList[j].Zpoint.x;
			_rect.y = Orl_result[i]->PointList[j].Zpoint.y;
			cv::rectangle(_Mat,_rect,cvScalar(50,255,50),2);
			cv::putText(_Mat,Str,Orl_result[i]->PointList[j].Zpoint,cv::FONT_HERSHEY_SIMPLEX,1.0,cvScalar(255,0,50));
		}
	
	}


}

void OCREngineCv::DrawShowPicture(std::vector<OCR_ResultOrlWord*> &Orl_result,cv::Mat &_Mat)
{

	if (_Mat.channels() == 1)
	{
		cv::cvtColor(_Mat,_Mat,cv::COLOR_GRAY2BGR);
	}

	int S = Orl_result.size();
	for (int i = 0;i < S; ++i)
	{

		std::string Str = Orl_result[i]->getWord();
		cv::Rect _rect = cv::Rect(cv::Point(0,0),Orl_result[i]->getSize());

		for (int j = 0;j < Orl_result[i]->PointList.size(); ++j)
		{
			_rect.x = Orl_result[i]->PointList[j].Zpoint.x;
			_rect.y = Orl_result[i]->PointList[j].Zpoint.y;

			cv::rectangle(_Mat,_rect,cvScalar(50,255,50),2);
			cv::putText(_Mat,Str,Orl_result[i]->PointList[j].Zpoint,cv::FONT_HERSHEY_SIMPLEX,1.0,cvScalar(255,0,50));

		}

	}

}

void OCREngineCv::DrawShowPicture(std::vector<OCRAlternativePoint*> &Orl_result,cv::Mat &src,cv::Mat &_Mat)
{
	_Mat = src;

	if (_Mat.channels() == 1)
	{
		cv::cvtColor(_Mat,_Mat,cv::COLOR_GRAY2BGR);
	}

	int S = Orl_result.size();
	for (int i = 0;i < S; ++i)
	{
		std::string Str = Orl_result[i]->getWord();
		cv::Rect _rect = Orl_result[i]->getBest().getRect();

		cv::rectangle(_Mat,_rect,Orl_result[i]->getColorCv(),2);

		cv::putText(_Mat,Str,_rect.tl( ),cv::FONT_HERSHEY_SIMPLEX,1.0,cvScalar(255,0,50));
	}
}

void OCREngineCv::MergeSameRect(const std::vector<OCR_ResultOrlWord*> &Orl_result,std::vector<OCRAlternativePoint*> &AlternativeWords)
{
	//std::vector <OCRAlternativePoint*> AlternativeWords;

	int i = 0,j = 0,n = 0,m = 0;

	int _rectNear = getAdjPointX() > 6 ? getAdjPointX() : 6 ;  //���ƾ��ο������ֵ,�����6
	int _rectInclude = cvRound(getAdjWordX() / (double)1.5) ;

	int OrlSize = Orl_result.size();
	for (i = 0;i < OrlSize;++i)
	{
		int plsz = Orl_result[i]->PointList.size();
		for (j = 0;j < plsz;++j)  //����ԭʼ����Ϣ
		{
			ptPsb* pt = &(Orl_result[i]->PointList[j]);
			
			if (!pt->checked)  //������Ѿ����ձ��˾Ͳ�����,��ʡʱ��
			{
				OCRPoint ocrP = OCRPoint(*pt,Orl_result[i]);
				//cv::Rect Rpt = ocrP.getRect();
				
				OCRAlternativePoint *ocrAp = new OCRAlternativePoint(ocrP);
				pt->checked = true;


				bool firstCheck = true;
				bool needCheckSecond = false;

				if (ocrAp->m_Zone->m_Header ==  CBITable::�źŻ�_��ʾ)
				{
					needCheckSecond = true;
				}

				//����ʣ�����嵥���Ƿ���Ȼ����
				for (n = i;n < OrlSize;++n) //�����i+1
				{
					//��������ְ����Ļ� ����������ʽ
					int szplo = Orl_result[n]->PointList.size();
					for (m = 0 ; m < szplo ; ++m)
					{
						ptPsb *mPt = &(Orl_result[n]->PointList[m]);

						if (!mPt->checked)  //ֻ��û��check����
						{
							OCRPoint ocrMP = OCRPoint(*mPt,Orl_result[n]);  //�������Ķ���
						
							cv::Rect _Rect1 = ocrAp->getBest().getRect();
							cv::Rect _Rect2 = ocrMP.getRect();
							int adjust = _rectInclude;

							//�����ཻ���

							// ���������ο� ��һ��adjust�ı߿򳤶�
							cv::Rect AdRect1 = cv::Rect(abZore(_Rect1.x - adjust),abZore(_Rect1.y - adjust),_Rect1.width + adjust * 2,_Rect1.height + adjust *2);
							cv::Rect AdRect2 = cv::Rect(abZore(_Rect2.x - adjust),abZore(_Rect2.y - adjust),_Rect2.width + adjust * 2,_Rect2.height + adjust *2);

							int crossArea = RROverlap(AdRect1,AdRect2);  //���ཻ�����С

							float intersection = (float)crossArea / (AdRect1.area() + AdRect2.area() - crossArea);  //�ཻ��

							float Inter1 = (float)crossArea / AdRect1.area();
							float Inter2 = (float)crossArea / AdRect2.area();


							if (crossArea <= 0)
							{
								//���ཻ�Ļ�ɶ��������


							}else if (Inter1 > 0.90f || Inter2 > 0.90f)  // ���˱���ȫ�ཻ��
							{
								
								//˭��˭����ߵ�
								if (_Rect1.area() > _Rect2.area())  
								{
									std::string Str = ocrAp->getBest().getWord();

									if (Str.size() > ocrMP.getWord().size())
									{
										ocrMP.setInclude(true);
										ocrAp->addOcrPoint(ocrMP);
										mPt->checked = true;

									}else
									{
										//Ӧ���ǲ��������
										ocrAp->addOcrPoint(ocrMP);
										mPt->checked = true;
									}
								}
								else //����������������
								{
									std::string Str = ocrAp->getBest( ).getWord( );

									if (Str.size( ) < ocrMP.getWord( ).size( ))
									{
										//ocrAp->Alternative[0].setInclude(true); //����Ҫ��������
										ocrAp->addOutSideOcrPoint(ocrMP);
										mPt->checked = true;

										needCheckSecond = true;

									}else
									{
										ocrAp->addOcrPoint(ocrMP);
										mPt->checked = true;
									}
								}
							}else
							{
								

								if (Inter1 > 0.40f || Inter2 > 0.40f)  // ��һ����Ȳ���
								{
									if (ocrAp->m_Zone->m_Header == CBITable::��·����_�� || ocrAp->m_Zone->m_Header == CBITable::��·����_β )
									{

										//���������ֵĿ�������


									}else
									{
										//�Ϳ�˭��������
										ocrAp->addOcrPointAuto(ocrMP);  
										mPt->checked = true;

										needCheckSecond = true;
									}

								}else
								{
									//����С������������⴦��
							
									switch (ocrAp->m_Zone->m_Header)
									{
									case CBITable::�źŻ�_��ʾ:
										{
											//��Ҫ�Ǵ���U��L�������
											if (Inter1 > 0.005f || Inter2 > 0.005f)  // ��һ�������ཻ
											{
												ocrAp->addOcrPointAuto(ocrMP);
												mPt->checked = true;

											}else
											{
												qDebug() << U8c("��bug - �źŻ�_��ʾ С���ߴ���ʧ��") << Inter1 << "-" << Inter2;
											}
										}break;
									
									default:
										{
											
										}
										break;
									}

								}

							}
						}
						
					
					}

					if (needCheckSecond)
					{
						//һ��Ҫ����Ȧ���ⶪ
						if (firstCheck && n == OrlSize - 1 )  
						{
							n = i - 1;
							firstCheck = false;
						}
					}
				

						//ÿ����
				}

				AlternativeWords.push_back(ocrAp);
			}

				
				
		}
			//ÿ����
	}
	
	
	//��תһȦ���� UU��U ����LU�� U��L ������


}

void OCREngineCv::MergeIntoLine(const std::vector<OCRAlternativePoint*> &AlternativeWords,std::vector<OCRLine*> &_OCRLine)//�ϲ�ͬһ��
{
	int sz = _OCRLine.size();
	for(int i = 0;i < sz ;++i)
	{
		delete(_OCRLine[i]);
	}
	_OCRLine.clear();

	//�ϲ�
	for (int i = 0;i < AlternativeWords.size();i++)
	{
		if (!AlternativeWords[i]->isChecked)
		{
			//����һ��
			OCRLine *ocrL = new OCRLine(AlternativeWords[i]);
			AlternativeWords[i]->isChecked = true;

			for (int j = i + 1; j < AlternativeWords.size(); ++j)
			{
				if (!AlternativeWords[j]->isChecked)
				{
					if (std::abs(ocrL->y - AlternativeWords[j]->getZonePoint().y) <= getAdjLineY())
					{
						AlternativeWords[j]->isChecked = true;

						ocrL->addAltPoint(AlternativeWords[j]);
					}
				}
			}

			_OCRLine.push_back(ocrL);
		}
	}

	//����
	std::sort(_OCRLine.begin(),_OCRLine.end(),OCRLine::compare);
}

void OCREngineCv::setStdWordSize(int _width,int _hight)
{
	OCREngineBase::setStdWordSize( _width, _hight);
}

void OCREngineCv::setStdThreshold(float _float32)
{
	OCREngineBase::setStdThreshold( _float32);
}

void OCREngineCv::setOCREnginThread(OCREngineThread * _ptr /*= nullptr*/)
{
	ptr_thread = _ptr;
}


//************************************
// Method:    isRectInclude
// FullName:  isRectInclude
// Access:    public 
// Returns:   int ����0Ϊû�й�ϵ������ΪR1��R2������֮,1Ϊȫ����2Ϊ����,3Ϊ���У���R1��
// Qualifier:
// Parameter: cv::Rect & _Rect1
// Parameter: cv::Rect & _Rect2
// Parameter: int adjust ������Χ
// Parameter: bool jdL �Ƿ񷵻�ͬһ�в���3
//************************************

int OCREngineCv::isRectInclude(cv::Rect &_Rect1,cv::Rect &_Rect2,int adjust ,bool jdL )
{
	return 0;
}

/*  //������һ���汾�ľ��ΰ������㺯��
int OCREngineCv::isRectInclude(cv::Rect &_Rect1,cv::Rect &_Rect2,int adjust ,bool jdL )
{

	
	int absY = abs(_Rect1.y - _Rect2.y);
	int absX = abs(_Rect1.x - _Rect2.x);

	//���� ����ͬһ�е� 
	if (absY > getAdjLineY()  )
	{
		return 0;
	}

	//������ľ������ֱ�� ����
	if (absX > _Rect1.width + 2 * adjust &&  absX > _Rect2.width + 2 * adjust)
	{
		return 0;
	}

	// ���������ο� ��һ��adjust�ı߿򳤶�
	cv::Rect AdRect1 = cv::Rect(abZore(_Rect1.x - adjust),abZore(_Rect1.y - adjust),_Rect1.width + adjust * 2,_Rect1.height + adjust *2);
	cv::Rect AdRect2 = cv::Rect(abZore(_Rect2.x - adjust),abZore(_Rect2.y - adjust),_Rect2.width + adjust * 2,_Rect2.height + adjust *2);

	uchar ic;  //�����ж�����

	//�����ж��ϸ�ȫ����ϵ 
	

	if (absX < adjust * 0.5 )  //���ͬ���
	{

		if (_Rect1.width  >= _Rect2.width)
		{
			return 1;
		}else
		{
			return -1;
		}


	}else if (_Rect2.x > _Rect1.x)  //R1����
	{

		int Er = _Rect1.width - absX; //��

		if (Er > 0)
		{

			if (Er + adjust > _Rect2.width)
			{
				return 1;   //R1����R2 ��R1����
			}else
			{
				return 2;  //�ཻ
			}

		}else
		{
			if (jdL)
			{
				return 3;  //ͬ��δ�ཻ R1����
			}

			return 0;  //û���ཻ
		}

	}else //R2����
	{
		int Er = _Rect2.width -absX;

		if (Er > 0)
		{
			if (Er + adjust > _Rect1.width)
			{
				return -1;   //R1����R2 ��R1����
			}else
			{
				return -2;  //�ཻ
			}


		}else
		{
			if (jdL)
			{
				return -3;  //ͬ��δ�ཻ R1����
			}

			return 0;  //û���ཻ
		}


	}
	




	return 0;
	
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////

/*  //��������㷨
void OCREngineCv::MergeSameRect(const std::vector<OCR_ResultOrlWord*> &Orl_result,std::vector<OCRAlternativePoint*> &AlternativeWords)
{
	int i = 0,j = 0,n = 0,m = 0;

	int _rectNear = getAdjWordX() > 6 ? getAdjWordX() : 6 ;  //���ƾ��ο������ֵ,�����6
	int _rectInclude = cvRound(_rectNear * 1.5) ;

	int OrlSize = Orl_result.size();
	for (i = 0;i < OrlSize;++i)
	{
		int plsz = Orl_result[i]->PointList.size();
		for (j = 0;j < plsz;++j)  //����ԭʼ����Ϣ
		{
			ptPsb* pt = &(Orl_result[i]->PointList[j]);

			if (!pt->checked)  //������Ѿ����ձ��˾Ͳ�����,��ʡʱ��
			{
				OCRPoint ocrP = OCRPoint(*pt,Orl_result[i]);
				//cv::Rect Rpt = ocrP.getRect();

				OCRAlternativePoint *ocrAp = new OCRAlternativePoint(ocrP);
				pt->checked = true;

				bool pushEd = true;

				//����ʣ�����嵥���Ƿ���Ȼ����
				for (n = i+1;n < OrlSize;++n)
				{
					//��������ְ����Ļ� ����������ʽ
					int szplo = Orl_result[n]->PointList.size();
					for (m = 0 ; m < szplo ; ++m)
					{
						ptPsb *mPt = &(Orl_result[n]->PointList[m]);

						if (!mPt->checked)  //ֻ��û��check����
						{
							OCRPoint ocrMP = OCRPoint(*mPt,Orl_result[n]);  //�������Ķ���


							if (ImgProc::isNearRect(ocrAp->getBest().getRect(),ocrMP.getRect(),_rectNear))  //������ȫһ�µľ���
							{
								ocrAp->addOcrPoint(ocrMP);
								mPt->checked = true;
								break;  //һ�����µĵ���Ψһ��,����Ĭ���Ѿ���Ϲ���
							}else
							{
								int alpha = ImgProc::isRectInclude(ocrAp->getBest().getRect(),ocrMP.getRect(),_rectInclude);  //�����ж�


								switch(alpha)
								{
								case 1:  //���������ڱ�����
									{

										//if (QString(ocrP.getWord().data()).contains(ocrMP.getWord().data()))
										{
											std::string Str = ocrAp->getBest().getWord();

											if (Str.size() > ocrMP.getWord().size())
											{
												ocrMP.setInclude(true);

												ocrAp->addOcrPoint(ocrMP);
												mPt->checked = true;

											}else
											{
												ocrAp->addOcrPoint(ocrMP);
												mPt->checked = true;
											}

										}

									}break;
								case -1:  //�������Ĵ��������� 
									{
										std::string Str = ocrAp->getBest().getWord();

										if (Str.size() < ocrMP.getWord().size())
										{
											ocrAp->addOutSideOcrPoint(ocrMP);
											mPt->checked = true;

										}else
										{
											ocrAp->addOcrPoint(ocrMP);
											mPt->checked = true;
										}


									}break;


								case 2 :   //����
								case -2 :
									{
										if (ocrAp->getBest().getPossible() - ocrMP.getPossible() > 0.2)
										{
											mPt->checked = true;  //�ѱ�����ɾ��
										}else if (ocrMP.getPossible() -ocrAp->getBest().getPossible() > 0.2)
										{
											pushEd = false; //����ɾ��
										}else
										{
											ocrAp->addOcrPoint(ocrMP);
											mPt->checked = true;
										}

									}break;  

								case 3:
								case -3: break; //����ͬ��

								case 0 :  //ʲô�����ᷢ��
								default: break;
								}



							}
						}
						//ÿ����
					}


				}
				if (pushEd)
				{
					AlternativeWords.push_back(ocrAp);
				}else
				{
					delete(ocrAp);
				}

			}
			//ÿ����
		}
	}

}

*/



