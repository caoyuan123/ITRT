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

//�任�ַ�ͼ�õĺ���
void FontGraphicTransTemp::TransForm(cv::InputArray Input,cv::Mat &dst) const
{
	cv::Mat Src = Input.getMat();
	cv::Mat temp ;

    //���������8uc1�Ļ�ɫͼ
	assert(Src.channels() != 1 || Src.depth() !=8
		/*,"TransForm�����������"*/);

	if (ErodeRoit == 0)
	{
		temp = Src.clone();
	}else if (ErodeRoit > 0)
	{
		cv::erode(Src,temp,cv::Mat(),cv::Point(-1,-1),ErodeRoit);
	//	cv::erode(Src,temp,0,cv::Point(-1,-1),ErodeRoit);
	}
	
	
	//IplImage *tempIpl = new IplImage(Src);

 //BLUR��ȥ���߽�ĸ�˹ģ��,��˹�ɰ�����ɰ��С/ƽ��Ϊ0,�Զ��е�����߽�

	if (GlussRoit > 0)
	{
		cv::boxFilter( temp, temp, temp.depth(), cv::Size(GlussRoit, GlussRoit), cv::Point(-1,-1),
			CV_BLUR, cv::BORDER_REPLICATE );
	}
	
//	cvSmooth(tempIpl,tempIpl,CV_BLUR,GlussRoit);  //�������һ��
	
/*
	char* ptr = tempIpl->imageData;
	for (int i =0;i<tempIpl->height;i++) //����Ѿ��������������8u+channel1�������㷨
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
		pow(temp, PowRoit,temp);     //����
	}else
	{
		pow(temp,1 / -(double)PowRoit,temp);  //��ǳ
	}
	
	temp.convertTo(temp,CV_8UC1,255.0f);
	
	cv::resize(temp,dst,cv::Size(temp.size().width * Roit,temp.size().height * Roit));
	temp.release();
	
}

void FontGraphicTransTemp::TransForm(const FontPicture& Org,FontPicture &Dst) const
{
	Dst.ShallowCopy(Org);

	TransForm(Org._Mat8u1c,Dst._Mat8u1c);  //�任

	//�ּ����Ŵ��ʶ��Ŵ�

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
		//û��ָ·��ע�⣡����
		fs = cvOpenFileStorage(FileFode,0,CV_STORAGE_READ);
	}
	cvStartWriteStruct(fs,"TransformTemp",CV_NODE_MAP);

	cvWriteReal(fs,"Roit",Roit);

	cvWriteInt(fs,"GlussRoit",GlussRoit);
	cvWriteInt(fs,"ErodeRoit",ErodeRoit);
	cvWriteInt(fs,"PowRoit",PowRoit);

	cvEndWriteStruct(fs);


	

#ifdef _DEBUG
	//	MessageBox(0,TEXT("����ɹ�����"),TEXT("����ɹ�"),MB_OK);
		printf("Trans����ɹ�����\r\n");

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
		//û��ָ·��ע�⣡����
		fs = cvOpenFileStorage(FileFode,0,CV_STORAGE_READ);
	}

	node = cvGetFileNodeByName(fs,0,"TransformTemp");

	Roit = cvReadRealByName(fs,node,"Roit",1.0f);
	ErodeRoit = cvReadIntByName(fs,node,"ErodeRoit",1);
	GlussRoit = cvReadIntByName(fs,node,"GlussRoit",1);
	PowRoit = cvReadIntByName(fs,node,"PowRoit",1);


	cvReleaseFileStorage(&fs);

#ifdef _DEBUG
	printf("%sTransformTemp��ȡ�ɹ�����\r\n",Str);
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
