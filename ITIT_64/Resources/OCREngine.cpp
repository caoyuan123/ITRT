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
	//1.将参数设置进去
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
	if (num <= 0)  //为了兼容之前的
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


//返回Rect的重叠区域大小

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


inline static void MergeNearPoint(const std::vector<ptPsb> &input,std::vector<ptPsb> &output,int Range) //拟合临近点
{
	int Halfrange = Range;  //四舍五入算范围

	output.clear();
	std::vector<bool> fastcheck;            //同步的一个检查已查过的表

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
				if (!fastcheck[j])  //只查没查过的
				{
					if (Rt.contains(input[j].Zpoint))
					{
						if (input[j].possible > pwp.possible)
						{
							Rt.x += pwp.Zpoint.x - input[j].Zpoint.x;  //换成新的中心点坐标 修复 拟合不完全bug
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
	//取 字的最小容忍距离
	int Range = getAdjPointY( );

	//开始按序处理
	int MaxSize = FontPictures.size( );


#ifdef PRINTDATA
	ofstream file;
	file.open("data.txt",ios::app);
#endif

	if (!CheckRegist( ))
	{
		omp_set_num_threads(1); //没有注册不能使用多线程
	}

	int dftCheck = _matBig.size().area() * 0.005;   

#pragma omp parallel for 
	for (int i = 0 ; i < MaxSize;++i)
	{
		//给迎面那俩短的用的,如果 算子图 比 被查宽就不查了
		if (FontPictures[i]->_Mat8u1c.size().width > _matBig.size().width)
		{
			qDebug( ) << FontPictures[i]->words.data() << " : " << U8s("跳过");
			continue;
		}

		double Max = 0,Min = 0;
		cv::Mat ResultRaw ;
		cv::Mat Result ;

		cv::Mat _mat8u = FontPictures[i]->_Mat8u1c;

		cv::matchTemplate(_matBig,_mat8u,ResultRaw,TM_CCOEFF_NORMED);
		cv::pow(ResultRaw,5,Result);
		cv::minMaxLoc(Result,&Min,&Max);
		
		/*  //调试取样函数
		if (FontPictures[i]->words == "(2002/2006)")
		{
			cv::imwrite("ts1.png",_matBig);
			cv::imwrite("ts2.png",_mat8u);
		}
		/**/


		//获取阈值
		double s_Threshold = 0;
		if(_Dz->getDrawingType()->saveType == 0)
		{
			s_Threshold = getMinThreshold(  );
		}
		else if(_Dz->getDrawingType()->saveType == 1)
		{
			s_Threshold = getMinThreshold( FontPictures[i]->words.size() );
		}

		double s_MuliThreshold = 0;  //逐位计算的阈值

		if (u_setCvOCR.g_blWordPosJudge)  //开启算逐位排除
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

			std::vector<ptPsb> pTemp;       //暂存结果
		
			//搜索所有点
			for (int H = 0; H < Result.rows; ++H)   
			{
				for (int W = 0; W < Result.cols; ++W)
				{
					double rs_temp = Result.at<float>(cv::Point(W,H));

					if (rs_temp > s_Threshold && rs_temp < 0.999)
					{
						//记录
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

			//CCOEFF用dft计算会出现问题的处理
		

			if (pTemp.size() > dftCheck)
			{
				//imwrite(Rt->getWord() + "cs.png",Result);
				//imwrite(Rt->getWord() + "csW.png",ResultRaw);
				//_mat8u.at<uchar>(_mat8u.rows /2, _mat8u.cols /2) = 255 - _mat8u.at<uchar>(_mat8u.rows /2, _mat8u.cols /2);
				
				//给_mat8u 加一个白色上边
				cv::copyMakeBorder(_mat8u,_mat8u,1,0,0,0,cv::BORDER_ISOLATED,cv::Scalar(255,255,255));

				cv::matchTemplate(_matBig,_mat8u,ResultRaw,TM_CCOEFF_NORMED);

				cv::pow(ResultRaw,5,Result);

				cv::minMaxLoc(Result,&Min,&Max);

				qDebug() << U8c(Rt->getWord().data()) << U8c("重算") << pTemp.size() ; 

				pTemp.clear();

				for (int H = 0; H < Result.rows; ++H)   
				{
					for (int W = 0; W < Result.cols; ++W)
					{
						double rs_temp = Result.at<float>(cv::Point(W,H));
						if (rs_temp > s_Threshold && rs_temp < 0.999)
						{
							//记录
							ptPsb p;
							p.Zpoint = cv::Point(W,H);
							p.possible = rs_temp; 
							pTemp.push_back(p);

						}
					}
				}
				qDebug() << U8c(Rt->getWord().data()) << U8c("重算后") << pTemp.size() ; 
			}
			
			int reNum = 0;
			while (pTemp.size() > dftCheck) 
			{
				s_Threshold += 0.02;
				reNum++;
				qDebug( ) << U8c(Rt->getWord( ).data( )) << U8c("超数") << pTemp.size( ) << U8c("重新计算") << reNum << U8c("次"); 
				pTemp.clear( );

				for (int H = 0; H < Result.rows; ++H)   
				{
					for (int W = 0; W < Result.cols; ++W)
					{
						double rs_temp = Result.at<float>(cv::Point(W,H));
						if (rs_temp > s_Threshold && rs_temp < 0.999)
						{
							//记录
							ptPsb p;
							p.Zpoint = cv::Point(W,H);
							p.possible = rs_temp; 
							pTemp.push_back(p);
							
						}
					}
				}
			}


			//利用单独比较算法排除一遍,
			if (u_setCvOCR.g_blWordPosJudge)
			{
				std::vector<ptPsb> pDelTemp;  //删除的中转

				//排除独立匹配度特别低的
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

			//合并临近的点
			if (pTemp.size() > 1)
			{
				MergeNearPoint(pTemp,Rt->PointList,Range); 
			}
			else
			{
				Rt->PointList = pTemp;
			}


			/*

			//增加处理内容计算 每个备选点的单字等效之和,放到其他地方处理了
			if (u_setCvOCR.g_blWordPosJudge && false)  
			{
				int SizeNum = Rt->PointList.size( );
				for (int pl = 0; pl < SizeNum; ++pl)
				{
					Rt->PointList[pl].setMultiPossbile(_matBig,*FontPictures[i]);
				}
			}*/


			//增加处理内容 消除掉临近边界的点






			//////////////////////////////////




#pragma omp critical  //临界区
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
//#pragma omp critical  //qt自带线程等待
			{
				ptr_thread->EmitProcessBar(-1);  //发送进度条自增1
			}
		}

	}

#ifdef PRINTDATA
	file.close();
#endif


}


void OCREngineCv::OCRDrawingSheet(DrawingSheet &Sheet)
{
	//遍历所有 OCRzone一个一个放到 OCREngineDrawingZone中
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


#undef _DEBUG  //不打印了

int OCREngineCv::OCRDrawingZone(DrawingZone &Zone,bool calNums)
{
	//1.通过不同的ZoneType加载不同的 待识别集合
	DrawingBook* ptr_DrawingBook = Zone.getDrawingBook();

	CBI_TableDictionary *ptr_Dictionary = ptr_DrawingBook->m_Dictionary;

	std::vector<OCR_ResultOrlWord*> OrlWordResult1;  //
	//std::vector<OCR_ResultOrlWord*> OrlWordResult2;  //预留的

	std::vector<OCRAlternativePoint*> AlterResult1;  //整合好的点阵

	std::vector<OCRLine*> LineResult1;  //无论是怎么样的都得按行整理

	int NumReturn = 0;

	switch (Zone.m_Header)
	{
	case CBITable::排列按钮 : 
		{
			if (calNums)  //计算个数 请保证和 正式MainEngine的计算流程一致
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::列车按钮名称].size();
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::调车按钮名称].size();

				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::列车按钮名称]
			,Zone.getMat(),OrlWordResult1,&Zone);

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::调车按钮名称]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//写入DrawingZone
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


	case CBITable::道岔_混合: 
		{
			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::道岔名称].size();
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::带防道岔].size();

				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::道岔名称]
				,Zone.getMat(),OrlWordResult1,&Zone);
			
			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::带防道岔]
				,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//写入DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rdch.png",TempShow);
#endif

		}break;
	
	case CBITable::轨道区段_混合 : 
		{
			// 扔进去区段 和 超限条件道岔
			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::区段名称].size();
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::条件道岔].size();

				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::区段名称]
			,Zone.getMat(),OrlWordResult1,&Zone);

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::条件道岔]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//写入DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rgdh.png",TempShow);
#endif

		}break;

	case CBITable::敌对信号 :
		{
			
			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::敌对信号].size();
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::敌对信号道岔].size();

				return NumReturn;
			}

			//生成 列车的调车信号灯进去才行
			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::敌对信号]
			,Zone.getMat(),OrlWordResult1,&Zone);

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::敌对信号道岔]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//写入DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rddxh.png",TempShow);
#endif

		}break;

	case CBITable::信号机_名称:
		{

			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::列车信号机名称].size();
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::调车信号机名称].size();

				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::列车信号机名称]
			,Zone.getMat(),OrlWordResult1,&Zone);

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::调车信号机名称]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//写入DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rxhjm.png",TempShow);
#endif

		}break;

	case CBITable::Headers::信号机_显示 :
		{

			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::灯显示样式].size();
				
				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::灯显示样式]
			,Zone.getMat(),OrlWordResult1,&Zone);


			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//写入DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rxxxs.png",TempShow);
#endif

		}break;

	case CBITable::Headers::其他联锁 :
		{

			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::其他联锁].size();
				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::其他联锁]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//写入DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rqtls.png",TempShow);
#endif

		}break;	

	case CBITable::Headers::敌对_迎面列车 :
		{

			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::区段名称].size();
				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::区段名称]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//写入DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rymlc.png",TempShow);
#endif
		

		}break;	

	case CBITable::Headers::敌对_迎面调车 :
		{

			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::区段名称].size();
				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::区段名称]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//写入DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

#if _DEBUG
			cv::Mat TempShow;
			DrawShowPicture(AlterResult1,Zone.getMat(),TempShow);
			cv::imwrite("Rymdd.png",TempShow);
#endif
	
		}break;	


	case CBITable::Headers::进路号码_首 :
		{

			if (calNums)
			{
				NumReturn += ptr_Dictionary->_FontPicture[IfDictionary::数字序列].size();
				return NumReturn;
			}

			OCRMainEngine(ptr_Dictionary->_FontPicture[IfDictionary::数字序列]
			,Zone.getMat(),OrlWordResult1,&Zone);

			MergeSameRect(OrlWordResult1,AlterResult1);
			MergeIntoLine(AlterResult1,LineResult1);

			//写入DrawingZone
			Zone.m_OrlResults.m_OrlResults = OrlWordResult1;
			Zone.AlternativeWords = AlterResult1;
			Zone.All_OCRLine = LineResult1;

		}break;	

	default:

		QString str = U8s( CBITable::HeadersChinese[Zone.m_Header].data() ) +  U8s(" 跳过");
		
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

	int _rectNear = getAdjPointX() > 6 ? getAdjPointX() : 6 ;  //近似矩形框的容忍值,最低是6
	int _rectInclude = cvRound(getAdjWordX() / (double)1.5) ;

	int OrlSize = Orl_result.size();
	for (i = 0;i < OrlSize;++i)
	{
		int plsz = Orl_result[i]->PointList.size();
		for (j = 0;j < plsz;++j)  //各个原始点信息
		{
			ptPsb* pt = &(Orl_result[i]->PointList[j]);
			
			if (!pt->checked)  //这个点已经被收编了就不查了,节省时间
			{
				OCRPoint ocrP = OCRPoint(*pt,Orl_result[i]);
				//cv::Rect Rpt = ocrP.getRect();
				
				OCRAlternativePoint *ocrAp = new OCRAlternativePoint(ocrP);
				pt->checked = true;


				bool firstCheck = true;
				bool needCheckSecond = false;

				if (ocrAp->m_Zone->m_Header ==  CBITable::信号机_显示)
				{
					needCheckSecond = true;
				}

				//搜索剩余结果清单中是否依然还有
				for (n = i;n < OrlSize;++n) //变更了i+1
				{
					//如果是文字包含的话 有其他处理方式
					int szplo = Orl_result[n]->PointList.size();
					for (m = 0 ; m < szplo ; ++m)
					{
						ptPsb *mPt = &(Orl_result[n]->PointList[m]);

						if (!mPt->checked)  //只找没有check过的
						{
							OCRPoint ocrMP = OCRPoint(*mPt,Orl_result[n]);  //被检索的对象
						
							cv::Rect _Rect1 = ocrAp->getBest().getRect();
							cv::Rect _Rect2 = ocrMP.getRect();
							int adjust = _rectInclude;

							//计算相交情况

							// 把两个矩形框 扩一个adjust的边框长度
							cv::Rect AdRect1 = cv::Rect(abZore(_Rect1.x - adjust),abZore(_Rect1.y - adjust),_Rect1.width + adjust * 2,_Rect1.height + adjust *2);
							cv::Rect AdRect2 = cv::Rect(abZore(_Rect2.x - adjust),abZore(_Rect2.y - adjust),_Rect2.width + adjust * 2,_Rect2.height + adjust *2);

							int crossArea = RROverlap(AdRect1,AdRect2);  //算相交区域大小

							float intersection = (float)crossArea / (AdRect1.area() + AdRect2.area() - crossArea);  //相交率

							float Inter1 = (float)crossArea / AdRect1.area();
							float Inter2 = (float)crossArea / AdRect2.area();


							if (crossArea <= 0)
							{
								//不相交的话啥都不用做


							}else if (Inter1 > 0.90f || Inter2 > 0.90f)  // 有人被完全相交了
							{
								
								//谁大谁是外边的
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
										//应该是差不多字数的
										ocrAp->addOcrPoint(ocrMP);
										mPt->checked = true;
									}
								}
								else //被搜索比主搜索大
								{
									std::string Str = ocrAp->getBest( ).getWord( );

									if (Str.size( ) < ocrMP.getWord( ).size( ))
									{
										//ocrAp->Alternative[0].setInclude(true); //不需要这样做了
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
								

								if (Inter1 > 0.40f || Inter2 > 0.40f)  // 有一方深度擦边
								{
									if (ocrAp->m_Zone->m_Header == CBITable::进路号码_首 || ocrAp->m_Zone->m_Header == CBITable::进路号码_尾 )
									{

										//这两个数字的可以容忍


									}else
									{
										//就看谁更符合了
										ocrAp->addOcrPointAuto(ocrMP);  
										mPt->checked = true;

										needCheckSecond = true;
									}

								}else
								{
									//其他小擦边情况的特殊处理
							
									switch (ocrAp->m_Zone->m_Header)
									{
									case CBITable::信号机_显示:
										{
											//主要是处理U和L经常混合
											if (Inter1 > 0.005f || Inter2 > 0.005f)  // 有一方擦边相交
											{
												ocrAp->addOcrPointAuto(ocrMP);
												mPt->checked = true;

											}else
											{
												qDebug() << U8c("有bug - 信号机_显示 小擦边处理失败") << Inter1 << "-" << Inter2;
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
						//一共要算两圈避免丢
						if (firstCheck && n == OrlSize - 1 )  
						{
							n = i - 1;
							firstCheck = false;
						}
					}
				

						//每周内
				}

				AlternativeWords.push_back(ocrAp);
			}

				
				
		}
			//每周外
	}
	
	
	//再转一圈处理 UU和U 还有LU和 U或L 的事情


}

void OCREngineCv::MergeIntoLine(const std::vector<OCRAlternativePoint*> &AlternativeWords,std::vector<OCRLine*> &_OCRLine)//合并同一行
{
	int sz = _OCRLine.size();
	for(int i = 0;i < sz ;++i)
	{
		delete(_OCRLine[i]);
	}
	_OCRLine.clear();

	//合并
	for (int i = 0;i < AlternativeWords.size();i++)
	{
		if (!AlternativeWords[i]->isChecked)
		{
			//新启一行
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

	//排序
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
// Returns:   int 返回0为没有关系，正数为R1包R2负数反之,1为全包，2为交集,3为并列（正R1左）
// Qualifier:
// Parameter: cv::Rect & _Rect1
// Parameter: cv::Rect & _Rect2
// Parameter: int adjust 调整范围
// Parameter: bool jdL 是否返回同一行参数3
//************************************

int OCREngineCv::isRectInclude(cv::Rect &_Rect1,cv::Rect &_Rect2,int adjust ,bool jdL )
{
	return 0;
}

/*  //保留的一个版本的矩形包含计算函数
int OCREngineCv::isRectInclude(cv::Rect &_Rect1,cv::Rect &_Rect2,int adjust ,bool jdL )
{

	
	int absY = abs(_Rect1.y - _Rect2.y);
	int absX = abs(_Rect1.x - _Rect2.x);

	//特例 不是同一行的 
	if (absY > getAdjLineY()  )
	{
		return 0;
	}

	//若果点的距离过大直接 滚出
	if (absX > _Rect1.width + 2 * adjust &&  absX > _Rect2.width + 2 * adjust)
	{
		return 0;
	}

	// 把两个矩形框 扩一个adjust的边框长度
	cv::Rect AdRect1 = cv::Rect(abZore(_Rect1.x - adjust),abZore(_Rect1.y - adjust),_Rect1.width + adjust * 2,_Rect1.height + adjust *2);
	cv::Rect AdRect2 = cv::Rect(abZore(_Rect2.x - adjust),abZore(_Rect2.y - adjust),_Rect2.width + adjust * 2,_Rect2.height + adjust *2);

	uchar ic;  //包含判断因子

	//首先判断严格全包关系 
	

	if (absX < adjust * 0.5 )  //差不多同起点
	{

		if (_Rect1.width  >= _Rect2.width)
		{
			return 1;
		}else
		{
			return -1;
		}


	}else if (_Rect2.x > _Rect1.x)  //R1在左
	{

		int Er = _Rect1.width - absX; //差

		if (Er > 0)
		{

			if (Er + adjust > _Rect2.width)
			{
				return 1;   //R1包含R2 且R1在左
			}else
			{
				return 2;  //相交
			}

		}else
		{
			if (jdL)
			{
				return 3;  //同行未相交 R1在左
			}

			return 0;  //没有相交
		}

	}else //R2在右
	{
		int Er = _Rect2.width -absX;

		if (Er > 0)
		{
			if (Er + adjust > _Rect1.width)
			{
				return -1;   //R1包含R2 且R1在左
			}else
			{
				return -2;  //相交
			}


		}else
		{
			if (jdL)
			{
				return -3;  //同行未相交 R1在左
			}

			return 0;  //没有相交
		}


	}
	




	return 0;
	
}*/

//////////////////////////////////////////////////////////////////////////////////////////////////////




/////////////////////////////////////////////////////////////////////

/*  //保存的老算法
void OCREngineCv::MergeSameRect(const std::vector<OCR_ResultOrlWord*> &Orl_result,std::vector<OCRAlternativePoint*> &AlternativeWords)
{
	int i = 0,j = 0,n = 0,m = 0;

	int _rectNear = getAdjWordX() > 6 ? getAdjWordX() : 6 ;  //近似矩形框的容忍值,最低是6
	int _rectInclude = cvRound(_rectNear * 1.5) ;

	int OrlSize = Orl_result.size();
	for (i = 0;i < OrlSize;++i)
	{
		int plsz = Orl_result[i]->PointList.size();
		for (j = 0;j < plsz;++j)  //各个原始点信息
		{
			ptPsb* pt = &(Orl_result[i]->PointList[j]);

			if (!pt->checked)  //这个点已经被收编了就不查了,节省时间
			{
				OCRPoint ocrP = OCRPoint(*pt,Orl_result[i]);
				//cv::Rect Rpt = ocrP.getRect();

				OCRAlternativePoint *ocrAp = new OCRAlternativePoint(ocrP);
				pt->checked = true;

				bool pushEd = true;

				//搜索剩余结果清单中是否依然还有
				for (n = i+1;n < OrlSize;++n)
				{
					//如果是文字包含的话 有其他处理方式
					int szplo = Orl_result[n]->PointList.size();
					for (m = 0 ; m < szplo ; ++m)
					{
						ptPsb *mPt = &(Orl_result[n]->PointList[m]);

						if (!mPt->checked)  //只找没有check过的
						{
							OCRPoint ocrMP = OCRPoint(*mPt,Orl_result[n]);  //被检索的对象


							if (ImgProc::isNearRect(ocrAp->getBest().getRect(),ocrMP.getRect(),_rectNear))  //存在完全一致的矩形
							{
								ocrAp->addOcrPoint(ocrMP);
								mPt->checked = true;
								break;  //一个字下的点是唯一的,而且默认已经拟合过了
							}else
							{
								int alpha = ImgProc::isRectInclude(ocrAp->getBest().getRect(),ocrMP.getRect(),_rectInclude);  //包含判定


								switch(alpha)
								{
								case 1:  //主检索大于被检索
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
								case -1:  //被检索的大于主检索 
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


								case 2 :   //交集
								case -2 :
									{
										if (ocrAp->getBest().getPossible() - ocrMP.getPossible() > 0.2)
										{
											mPt->checked = true;  //把被交点删除
										}else if (ocrMP.getPossible() -ocrAp->getBest().getPossible() > 0.2)
										{
											pushEd = false; //将主删掉
										}else
										{
											ocrAp->addOcrPoint(ocrMP);
											mPt->checked = true;
										}

									}break;  

								case 3:
								case -3: break; //并列同行

								case 0 :  //什么都不会发生
								default: break;
								}



							}
						}
						//每周内
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
			//每周外
		}
	}

}

*/



