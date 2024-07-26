#include <QtGui>
#include <opencv2/opencv.hpp>
#include <cmath>

#include "FontGeneraterQ.h"
#include "FontGenerater.h"
#include "FontSimilar.h"
#include "ComApx.h"
#include "ImgProc.h"
#include "DrawingType.h"
#include "MessyCodeTerminator.h"

using namespace cv;
using namespace std;

#include "FontSimilarDef.h"


//Apx用的找适合放大倍率的图像
double MatReSizeHandle(double input,void *obj)
{
	FontApx *fap = (FontApx*)obj;
	cv::Mat DstRz;

	fap->FontPicProprety->Roit = ImgProc::MatResizePix(fap->getFontImageCv(false),DstRz,Com::Round(input));
	
	return ImgProc::MatCompare(fap->_Mat8u1c,DstRz);
}


//确定字体压缩比
double MatFontStretchHandle(double input,void *obj)
{
	if (input <= MiniCompress)
	{
		return 0;
	}
	FontApx *fap = (FontApx*) obj;

	fap->FontPicProprety->FontCompressRatio = input;

	cv::Mat dst = fap->getFontImageCv( );

	return ImgProc::MatCompare(fap->_Mat8u1c,dst);
}

//图像三参数的整定器
//高斯的范围是 1,3,5 
//erode 的范围是 1,2,3
//加深的范围是 1/2,1/3,1,2,3,4
//一共60种情况都试一遍

FontGraphicTransTemp FontSimilar::ImageTreeble(FontApx &input,int num)
{
	double dbMax = 0;
	FontGraphicTransTemp MaxTrans; //记录当前最大的那一次

	FontGraphicTransTemp wgTemp;     //临时模板
	wgTemp = *(input.FontPicProprety);

	cv::Mat Src;
	cv::Mat Dst;

	if (num != 0)
	{
		Src = input.getSingleWordCv(num);
		Dst = input.getFontImageCv(num - 1,false);

	}else
	{
		Src = input._Mat8u1c;
		Dst = input.getFontImageCv(false); //拿原图
	}

	cv::Mat MtTemp;

	for (int G = -1;G <= 5; G+=2) //高斯层
	{
		wgTemp.GlussRoit = G;
		for (int E = 0; E <= 3 ;E++) //加粗
		{
			wgTemp.ErodeRoit = E;
			for (int P = -3;P <= 5;P++) //变浅/加深
			{
				if (P == 0) //没有0的情况
				{
					continue;
				}
				wgTemp.PowRoit = P;
				wgTemp.TransForm(Dst,MtTemp);
		
				double dbTemp = ImgProc::MatCompare(Src,MtTemp);
				
				if (dbTemp > dbMax)
				{
					dbMax = dbTemp;
					MaxTrans = wgTemp;
				}

			}
		}
	}

#ifdef _DEBUG
/*	qDebug("Three Max %f,G:%d E:%d P:%d",
		dbMax,MaxTrans.GlussRoit,MaxTrans.ErodeRoit,MaxTrans.PowRoit);
	    MaxTrans.TransForm(Dst,MtTemp);		*/
#endif

	input.MaxCompareRoit = dbMax;
	input.FontPicProprety->ErodeRoit = MaxTrans.ErodeRoit;
	input.FontPicProprety->GlussRoit = MaxTrans.GlussRoit;
	input.FontPicProprety->PowRoit   = MaxTrans.PowRoit;

	return MaxTrans;
}

#ifdef _DEBUG


void testFuc()
{

//	cv::Mat S = imread("E:\\Cpp\\TTTTT.png",CV_8UC1);
	//int sss = calWordHight(S);

	FontApx fan;
	fan._Mat8u1c = imread("D507A.png",CV_8UC1);
	fan.words = "D501A";
	int pos[6] = {2,37,72,103,128,164};
	fan.wordPos.insert(fan.wordPos.begin(),pos,pos+6);
	fan.FontGenerater = new FontGeneraterQ;

	FontSimilar fontSimilar;

	fontSimilar.List_FontSelect.push_back(fan);

	fontSimilar.FontPropertyFinal.FontNum = 36;

	fontSimilar.FontPropertyFinal.FontBold = true;

	/*
	double d1,d2;
	double cp = fontSimilar.singleWord(fan,1,d1,d2);
	qDebug("7cp: %f sizeRoit: %f FontSretch:%f",cp,d1,d2);

	cp = fontSimilar.singleWord(fan,2,d1,d2);
	qDebug("-cp: %f sizeRoit: %f FontSretch:%f",cp,d1,d2);

	cp = fontSimilar.singleWord(fan,3,d1,d2);
	qDebug("1cp: %f sizeRoit: %f FontSretch:%f",cp,d1,d2);

	cp = fontSimilar.singleWord(fan,4,d1,d2);
	qDebug("3cp: %f sizeRoit: %f FontSretch:%f",cp,d1,d2);

	cp = fontSimilar.singleWord(fan,5,d1,d2);
	qDebug("Dcp: %f sizeRoit: %f FontSretch:%f",cp,d1,d2);

	cp = fontSimilar.singleWord(fan,6,d1,d2);
	qDebug("Gcp: %f sizeRoit: %f FontSretch:%f",cp,d1,d2);*/

	fontSimilar.init();
	fontSimilar.TotalSearch(fontSimilar.List_FontSelect[0]);


	for(;1;)
	{
		cvWaitKey(0);
	}
}



#endif



FontSimilar::FontSimilar(void)
{
	miniThreshold = 85;              //最低阈值
	manualFontName = "";
	manualFontNum = 0;
	manualFontSpacing = -255.0f;
	manualFontCompressRatio = -255.0f;

	List_FontSelect = std::vector<FontApx>();

	FontGenerater = nullptr;
}


FontSimilar::~FontSimilar(void)
{
	if (FontGenerater != nullptr)
	{
		delete(FontGenerater);
	}
	
}



const char FastList[11] = {'3','G','8','D','6','W','5','2','S','9','0'};//效果比较好的单字图 优先选择系列

//首次拟合
void FontSimilar::fittingInit(int FontApxNum)
{
	init(); //初始化所有的参数
	
	//fontApxNew 通过FontApxNew 切出单字图
	FontApx fontApx = List_FontSelect[FontApxNum];

	//通过FastList找有没有List当中的字符,如果有优先选择里面的字符
		//1.有的话 把所有 优List 中的字符提取出来 做单字逼近
		//2.没有的话 多切几个普通字符来回做逼近
	
	//算基础三参

	//算

}


//初始化函数
//确定使用引擎
void FontSimilar::init()
{
	if (manualFontName.contains("zh")) //使用自己写的那个 字库生成方式的生成引擎,暂时还没有移植过来
	{
		qFatal(" DO NOT CODING HERE!!!");
	}else
	{
		FontGenerater = new FontGeneraterQ();  

		int size = List_FontSelect.size();

		for (int i = 0; i < size;++i)  //设定每一个的生成引擎
		{
			List_FontSelect[i].FontPicProprety = &FontPropertyFinal; 
			List_FontSelect[i].FontGenerater = FontGenerater; //把其中每一个引擎都换成Qt字体生成引擎
		}
	}
}


//************************************
// Method:    singleWord
// FullName:  FontSimilar::singleWord - 找放大倍率和压缩比
// Access:    public 
// Returns:   double 匹配率
// Qualifier:
// Parameter: FontApxNew ApxTarget
// Parameter: int wordNum  第几个
//************************************
double FontSimilar::singleWord(FontApx ApxTarget,int wordNum,double &SizeRoit,double &FontScretch)
{
	cv::Mat Src = ApxTarget.getSingleWordCv(wordNum);
	FontPictureProprety FontPropretyTemp = FontPropertyFinal;

	FontApx SinglePart = ApxTarget;//只有单字的部分

	SinglePart.words = ApxTarget.words.at(wordNum - 1);
	SinglePart._Mat8u1c = ApxTarget.getSingleWordCv(wordNum);
	SinglePart.FontPicProprety = &FontPropretyTemp; 

	//1.字号用apxTarget里自带的字号 - 先找大致放大比率
	int imax = Com::ApxMid_int(-10,60,MatReSizeHandle,&SinglePart,10);
	SizeRoit = SinglePart.FontPicProprety->Roit;

	//2.再找字体压缩比率
	FontScretch = Com::ApxMid(MiniCompress,1,MatFontStretchHandle,&SinglePart,5);

	return MatFontStretchHandle(FontScretch,&SinglePart);
}

//静态的真正的全暴力运算
static double TotalSearchStatic(FontApx &input,FontSimilarDef &def)
{
	double dbMax = 0;
	cv::Mat Src = input._Mat8u1c;
	cv::Mat Dst = input.getFontImageCv(false);
	int dstHeight = Dst.size( ).height;

	FontPictureProprety MaxTrans; 

	for (def.FontNum.setStart();!def.FontNum.isEnd();) 
	{
		if (def.FontNum.isSearch)
		{
			input.FontPicProprety->FontNum = cvRound(def.FontNum.getNext());
		}else 
		{
			def.FontNum.getNext();
		}

		for (def.FontCompress.setStart();!def.FontCompress.isEnd();) 
		{
			if (def.FontCompress.isSearch)
			{
				input.FontPicProprety->FontCompressRatio = def.FontCompress.getNext();   //把值赋上

			} else
			{
				def.FontCompress.getNext();
			}

			for (def.FontSpacing.setStart();!def.FontSpacing.isEnd();)  
			{
				if (def.FontSpacing.isSearch)
				{
					input.FontPicProprety->FontSpacing = def.FontSpacing.getNext();   //把值赋上

				} else
				{
					def.FontSpacing.getNext();
				}

				for (def.RoitGluss.setStart();!def.RoitGluss.isEnd();) 
				{
					if (def.RoitGluss.isSearch)
					{
						input.FontPicProprety->GlussRoit = cvRound(def.RoitGluss.getNext()) ;   //把值赋上

					} else
					{
						def.RoitGluss.getNext();
					}

					for (def.RoitErode.setStart();!def.RoitErode.isEnd();)  
					{
						if (def.RoitErode.isSearch)
						{
							input.FontPicProprety->ErodeRoit = cvRound(def.RoitErode.getNext());   //把值赋上

						} else
						{
							def.RoitErode.getNext();
						}

						for (def.RoitPow.setStart();!def.RoitPow.isEnd();)  
						{
							if (def.RoitPow.isSearch)
							{
								input.FontPicProprety->PowRoit = def.RoitPow.getNext();   //把值赋上

							} else
							{
								def.RoitPow.getNext();
							}

							
							if (def.RoitSize.isSearch)
							{
								int R1 = cvRound(def.RoitSize.range[0]);
								int R2 = cvRound(def.RoitSize.range[1]);

								int Rm = Com::ApxMid_int(cvRound(R1),::abs(cvRound(R2 - R1)),MatReSizeHandle,&input);
								//int Rm = Com::ApxMid_int(6,10,MatReSizeHandle,&input);
								
								
								input.FontPicProprety->Roit = ((double)Rm + dstHeight) / dstHeight;

							}else;
							

								Dst = input.getFontImageCv();
								double dbTemp = ImgProc::MatCompare(Src,Dst);
							

						/*
								if (abs(input.FontPicProprety->FontCompressRatio - 0.73) < 0.001 &&
									abs(input.FontPicProprety->FontSpacing - 0.90) < 0.001)
								{
									printf("11");
								}*/

							if (dbTemp > dbMax)
							{
								dbMax = dbTemp;
								MaxTrans = *(input.FontPicProprety);

								qDebug()<<"MaxCompare：" << dbTemp
									<<"FontNum:" << input.FontPicProprety->FontNum << "FontName:" << input.FontPicProprety->FontName 
									<<"FontCompress:" << input.FontPicProprety->FontCompressRatio << "FontSpacing:" << input.FontPicProprety->FontSpacing
									<< "Sr:"<<input.FontPicProprety->Roit << "G" << input.FontPicProprety->GlussRoit <<"E"<< input.FontPicProprety->ErodeRoit <<"P" << input.FontPicProprety->PowRoit;		
							}

							


						}
					}
				}
			}
		}
	}

	*(input.FontPicProprety) = MaxTrans;
	input.MaxCompareRoit = dbMax;

	return dbMax;
}


//************************************
// Method:    TotalSearch 暴力运算得出最终结果
// FullName:  FontSimilar::TotalSearch
// Access:    public 
// Returns:   void
// Qualifier:
//************************************
void FontSimilar::TotalSearch(FontApx input)
{
	double SizeRoit = 0;
	double FontScretch = 0;

	double dbMax = singleWord(input,1,SizeRoit,FontScretch); //多字不能比单字低

	//修改增加新方案

//	int SizePlus = Com::Round ((SizeRoit - 1) * input.getSingleWordCv(4).size().height);

	cv::Mat mat_Font = input.getFontImageCv(false);

	int orlHight = ImgProc::calWordHight(input.getSingleWordCv(1));  //原始图片的字高
	int handcraft = 0;
	if (input.DistanceY.size() > 1) //如果手动指定了字的上下高度的话
	{
		handcraft = input.DistanceY[1] - input.DistanceY[0];  //手工指定的字高

		if (orlHight > handcraft)
		{
			orlHight = handcraft;
			printf("%d 手工字高\r\n",orlHight);
		}
	}

	int SizePlus = orlHight - ImgProc::calWordHight(mat_Font);
	int FontImgHight = mat_Font.size( ).height;
	
	SizeRoit = (double)(FontImgHight + SizePlus) / FontImgHight;

	input.FontPicProprety->Roit = SizeRoit;

	FontPictureProprety wgTemp = *(input.FontPicProprety);     //临时模板
	FontPictureProprety MaxTrans = wgTemp; //记录当前最大的那一次

	FontApx fanTemp = input;


	fanTemp.FontPicProprety = &wgTemp;

	/* 单字拟合 只找字宽
	*/
	FontApx SinglePart = input;//只有单字的部分

	SinglePart.words = input.words.at(1 - 1);
	SinglePart._Mat8u1c = input.getSingleWordCv(1);
	SinglePart.FontPicProprety = &wgTemp; 

	FontScretch = Com::ApxMid(MiniCompress, 1, MatFontStretchHandle, &SinglePart, 5); //摸底结果

	ImageTreeble(fanTemp,1);

	//fanTemp.FontPicProprety->GlussRoit = 3;
	//fanTemp.FontPicProprety->ErodeRoit = 0;
	//fanTemp.FontPicProprety->PowRoit = 4;


	cv::Mat Src = fanTemp._Mat8u1c;
	cv::Mat Dst = fanTemp.getFontImageCv(false); //拿原图
	int dstHeight = Dst.size().height;
	cv::Mat MtTemp;

	//字体压缩比裁剪
	//

	std::vector<double> effectiveSretch; //有效表,字体压缩不是每个参数都有效的
	MtTemp = fanTemp.getFontImageCv(false); //减少放大干扰

	double ScretchMin = FontScretch - 0.05;
	double ScretchMax = FontScretch + 0.05;

	if (ScretchMin > 0.65) ScretchMin = 0.65;
	if (ScretchMin < 0.2) ScretchMin = 0.2;
	
	if (ScretchMax < 1.10) ScretchMax = 1.10;
	if (ScretchMax > 2 ) ScretchMax = 2;

	for (double C = ScretchMin; C <= ScretchMax ; C+= 0.01)//生成字压缩的缩放表
	{
		wgTemp.FontCompressRatio = C;
		Dst = fanTemp.getFontImageCv(false);

		if (ImgProc::MatCompare(MtTemp, Dst) < 0.999)
		{
			effectiveSretch.push_back(C);
			MtTemp = Dst.clone();
		}
	}
	//


/*	for (int R = SizePlus -15; R <= SizePlus + 15;R++) //放大倍率,单字的倍率附近
	{
		wgTemp.Roit = ((double)SizePlus + R + dstHeight) / dstHeight;*/

	/*
	
#if 0
	#pragma omp parallel for firstprivate(fanTemp),firstprivate(wgTemp)
#endif
		for (int C = 0;C < effectiveSretch.size();C++)//字压缩
		{
			cv::Mat Dst;

			wgTemp.FontCompressRatio = effectiveSretch[C];

			for (double S = 0.70;S < 1.0;S+= 0.01) //字间距
			{
				//三参数
				wgTemp.FontSpacing = S;

				int Rm = Com::ApxMid_int(SizePlus - 5,10,MatReSizeHandle,&fanTemp);
				wgTemp.Roit = ((double)Rm + dstHeight) / dstHeight;

				//ImageTreeble(fanTemp);

				Dst = fanTemp.getFontImageCv();

				fanTemp.MaxCompareRoit = ImgProc::MatCompare(fanTemp._Mat8u1c,Dst);

		
				if (fanTemp.MaxCompareRoit > dbMax)
				{
					dbMax = fanTemp.MaxCompareRoit;
					MaxTrans = wgTemp;

					qDebug()<<"MaxCompare：" << fanTemp.MaxCompareRoit
						<<"FontNum:" << wgTemp.FontNum << "FontName:" << wgTemp.FontName 
						<<"FontCompress:" << wgTemp.FontCompressRatio << "FontSpacing:" << wgTemp.FontSpacing
						<< "Sr:"<<wgTemp.Roit << "G" << wgTemp.GlussRoit <<"E"<< wgTemp.ErodeRoit <<"P" << wgTemp.PowRoit;		

				}
			}
		 }/**/
	
	FontSimilarDef def;

	def.RoitSize.isSearch = false;
	def.RoitSize.range[0] = SizePlus - 5;
	def.RoitSize.range[1] = SizePlus + 5;

    def.FontCompress.isSearch = true;
	def.FontCompress.dTable = effectiveSretch;
	
	def.FontSpacing.isSearch = true;

	dbMax = TotalSearchStatic(fanTemp,def);

	MaxTrans = *(fanTemp.FontPicProprety);

	/**/

	qDebug()<<"MaxCompare：" << dbMax;
	qDebug()<<"FontNum：" << MaxTrans.FontNum << "FontName：" << MaxTrans.FontName ;
	qDebug() <<"FontCompress:" << MaxTrans.FontCompressRatio << "FontSpacing:" << MaxTrans.FontSpacing;
	qDebug() << "Sr:"<< MaxTrans.Roit << "G" << MaxTrans.GlussRoit <<"E"<< MaxTrans.ErodeRoit <<"P" << MaxTrans.PowRoit;

	ImageTreeble(fanTemp);

	fanTemp.FontPicProprety = &MaxTrans;

#ifdef _DEBUG
	imshow("DSt",fanTemp.getFontImageCv());
	imshow("Src",fanTemp._Mat8u1c);
#endif
	

	qDebug() <<"MaxCompare：" << dbMax;
	qDebug() <<"FontNum：" << MaxTrans.FontNum << "FontName：" << MaxTrans.FontName ;
	qDebug() <<"FontCompress:" << MaxTrans.FontCompressRatio << "FontSpacing:" << MaxTrans.FontSpacing;
	qDebug() << "Sr:"<< MaxTrans.Roit << "G" << MaxTrans.GlussRoit <<"E"<< MaxTrans.ErodeRoit <<"P" << MaxTrans.PowRoit;


	/*2019-11-12增加等效Roit的计算*/
	double stdRoit = 0;
	stdRoit = Com::stdCompareRoit(dbMax,input.words.size());
	FontPropertyFinal = MaxTrans;

	/*2019-11-12增加等效Roit的计算*/

	//增加合理性检查

	//最后的结果的那个图不会比目标图差太多 +/- 10%,差的越多减stdRoit

	cv::Size inputMatSize = input._Mat8u1c.size();
	cv::Size TempSize = fanTemp.getFontImageCv().size();

	float sizeRoit = (float)TempSize.area() / (float)inputMatSize.area();

	if (sizeRoit > 1.2 || sizeRoit < 0.25)
	{
		stdRoit = - stdRoit;
		qDebug( ) << "Error Cal!";
	}

	if (stdRoit > 1.001)  //封顶100分
	{
		stdRoit = 1.001;
	}

	qDebug( ) << "stdRoit:" << stdRoit;
	FontPropertyFinal.CompareRoit = stdRoit; 
	//FontPropertyFinal.CompareRoit = dbMax;   //换成标准比例
	
	//给字体信息
	//fanTemp.words = "j";
	//cv::Mat Comma = fanTemp.getFontImageCv( );

	//j的字高字宽
	FontPropertyFinal.stdWordHeight = orlHight;  //字高
	FontPropertyFinal.stdWordWidth = cvRound ((double)orlHight / 3);
	

}




//************************************
// Method:    TotalSearch2 可以给定预定参数的版本
// FullName:  FontSimilar::TotalSearch2
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: FontApx input
//************************************
void FontSimilar::TotalSearch2(FontApx input)
{
	//1. 通过人肉给出的真实字高 给出大致Qfont字高 和 缩放比例


	//2. 调整 抽单字的匹配 出 字宽（压缩比参数）。 -> 搜索其+-1挡的数据


	//3. 计算大致的三参 参数 每次搜索其+-1挡的数据


	//4. 主搜索引擎 给出 压缩比 和 字间距 最佳适配参数


}



FontApx::~FontApx()
{
	//delete(FontGenerater);
}

FontApx::FontApx()
{
	MaxCompareRoit = -1;
	FontPicProprety = nullptr;
	FontGenerater = nullptr;
	DistanceY.clear();
	//FontGenerater = new FontPictureGenerater();
}

cv::Mat FontApx::getFontImageCv(bool TransUsed /*= true/*是否自动使用模板变换*/) const
{
	FontGenerater->setFontPropertyRef(*FontPicProprety); //重新设置一遍
	cv::Mat temp =	FontGenerater->getOrlFontImageCv(QString(words.data()));
	cv::Mat Rt;
	if (TransUsed)
	{
		FontPicProprety->TransForm(temp,Rt);
	}
	else
	{
		Rt = temp;
	}
	return Rt;
}

cv::Mat FontApx::getFontImageCv(int Wordchoice,bool TransUsed /*= true*/) const
{
	FontGenerater->setFontPropertyRef(*FontPicProprety); //重新设置一遍
	QString s = words.at(Wordchoice);
	cv::Mat temp =	FontGenerater->getOrlFontImageCv(s);
	cv::Mat Rt;
	if (TransUsed)
	{
		FontPicProprety->TransForm(temp,Rt);
	}
	else
	{
		Rt = temp;
	}
	return Rt;
}


//////////////////////////////////////////////////////////////////////////////////////


void FontPictureProprety::setDrawingType(DrawingType& _dt)
{
	_dt._FontProprety = *this;
	_dt._GraphicTransTemp = *this;

	_dt.stdWordHeight = stdWordHeight;
	_dt.stdWordWidth = stdWordWidth;

	
}



