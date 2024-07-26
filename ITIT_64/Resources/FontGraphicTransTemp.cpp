#include "FontGraphicTransTemp.h"
#include <opencv.hpp>
#include "FontPictureDef.hpp"
#include <math.h>

FontGraphicTransTemp::FontGraphicTransTemp()
{
	Roit = 1.0f;
	GlussRoit = 1;
	ErodeRoit = 1;
	PowRoit = 1;

	CompareRoit = 0.0f;
}

//变换字符图用的函数
void FontGraphicTransTemp::TransForm(cv::InputArray Input,cv::Mat &dst) const
{
	cv::Mat Src = Input.getMat();
	cv::Mat temp ;

    //检查输入是8uc1的灰色图
	assert(Src.channels() != 1 || Src.depth() !=8
		/*,"TransForm函数输入错误"*/);

	if (ErodeRoit == 0)
	{
		temp = Src.clone();
	}else if (ErodeRoit > 0)
	{
		cv::erode(Src,temp,cv::Mat(),cv::Point(-1,-1),ErodeRoit);
	//	cv::erode(Src,temp,0,cv::Point(-1,-1),ErodeRoit);
	}
	
	
	//IplImage *tempIpl = new IplImage(Src);

 //BLUR的去除边界的高斯模糊,高斯蒙版控制蒙版大小/平均为0,自动切掉卷积边界

	if (GlussRoit > 0)
	{
		cv::boxFilter( temp, temp, temp.depth(), cv::Size(GlussRoit, GlussRoit), cv::Point(-1,-1),
			CV_BLUR, cv::BORDER_REPLICATE );
	}
	
//	cvSmooth(tempIpl,tempIpl,CV_BLUR,GlussRoit);  //和上面的一样
	
/*
	char* ptr = tempIpl->imageData;
	for (int i =0;i<tempIpl->height;i++) //这个已经决定输入必须是8u+channel1。加深算法
	{
		for (int j =0;j < tempIpl->width;j++)
		{
			double temp2 = ptr[j * tempIpl->nChannels] & 0xff; 
			ptr[j * tempIpl->nChannels]  = Normalized_pow(temp2,255,PowRoit);
		}

		ptr+= tempIpl->widthStep;
	}*/

	temp.convertTo(temp,CV_32FC1,1/255.0f);
	
	if (PowRoit >= 0) 
	{
		pow(temp, PowRoit,temp);     //加深
	}else
	{
		pow(temp,1 / -(double)PowRoit,temp);  //变浅
	}
	
	temp.convertTo(temp,CV_8UC1,255.0f);
	
	cv::resize(temp,dst,cv::Size(temp.size().width * Roit,temp.size().height * Roit));
	temp.release();
	
}

void FontGraphicTransTemp::TransForm(const FontPicture& Org,FontPicture &Dst) const
{
	Dst.ShallowCopy(Org);

	TransForm(Org._Mat8u1c,Dst._Mat8u1c);  //变换

	//字间距随放大倍率而放大

	if (Org.isHasWordPos())
	{
		Dst.wordPos.clear();

		int S = Org.wordPos.size();
		for (int i = 0 ;i < S; ++i)
		{
			Dst.wordPos.push_back( cvRound((double) Org.wordPos[i] * Roit ));
		}
	}

}

void FontGraphicTransTemp::SaveXML(char* Str /*= nullptr*/,char* FileFode /*= nullptr*/)
{
	using namespace cv;

	CvFileStorage*fs;
	if (FileFode == nullptr)
	{
		fs = cvOpenFileStorage("LsbFont.xml",0,CV_STORAGE_READ);
	}
	else
	{
		//没做指路经注意！！！
		fs = cvOpenFileStorage(FileFode,0,CV_STORAGE_READ);
	}
	cvStartWriteStruct(fs,"TransformTemp",CV_NODE_MAP);

	cvWriteReal(fs,"Roit",Roit);

	cvWriteInt(fs,"GlussRoit",GlussRoit);
	cvWriteInt(fs,"ErodeRoit",ErodeRoit);
	cvWriteInt(fs,"PowRoit",PowRoit);

	cvEndWriteStruct(fs);


	

#ifdef _DEBUG
	//	MessageBox(0,TEXT("保存成功！！"),TEXT("保存成功"),MB_OK);
		printf("Trans保存成功！！\r\n");

#endif // _DEBUG


	cvReleaseFileStorage(&fs);
}

void FontGraphicTransTemp::ReadXML(char* Str /*= nullptr*/,char* FileFode /*= nullptr*/)
{
	CvFileStorage*fs;
	CvFileNode *node;

	if (FileFode == nullptr)
	{
		fs = cvOpenFileStorage("LsbFont.xml",0,CV_STORAGE_READ);
	}
	else
	{
		//没做指路经注意！！！
		fs = cvOpenFileStorage(FileFode,0,CV_STORAGE_READ);
	}

	node = cvGetFileNodeByName(fs,0,"TransformTemp");

	Roit = cvReadRealByName(fs,node,"Roit",1.0f);
	ErodeRoit = cvReadIntByName(fs,node,"ErodeRoit",1);
	GlussRoit = cvReadIntByName(fs,node,"GlussRoit",1);
	PowRoit = cvReadIntByName(fs,node,"PowRoit",1);


	cvReleaseFileStorage(&fs);

#ifdef _DEBUG
	printf("%sTransformTemp读取成功！！\r\n",Str);
#endif // _DEBUG

}

void FontGraphicTransTemp::read(const cv::FileNode &node)
{

	node["Roit"] >> Roit;
	node["GlussRoit"] >> GlussRoit;
	node["ErodeRoit"] >> ErodeRoit;
	node["PowRoit"] >> PowRoit;
	node["CompareRoit"] >> CompareRoit;
}

void FontGraphicTransTemp::write(cv::FileStorage &fs) const
{
	fs << "{";
	{
		fs << "Roit" << Roit; 
		fs << "GlussRoit" << GlussRoit;
		fs << "ErodeRoit" << ErodeRoit;
		fs << "PowRoit" << PowRoit;
		fs << "CompareRoit" << CompareRoit;
	}
	fs << "}";
}






void read(const cv::FileNode& node, FontGraphicTransTemp& x,const FontGraphicTransTemp& default_value /*= FontGraphicTransTemp()*/)
{
	if (node.empty())
	{
		x = default_value;
	}else
	{
		x.read(node);
	}
}

void write(cv::FileStorage& fs, const std::string& name, const FontGraphicTransTemp& x)
{
	x.write(fs);
}
