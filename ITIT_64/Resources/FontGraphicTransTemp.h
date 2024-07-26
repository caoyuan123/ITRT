#pragma once
#include <cxcore.h>


class FontPicture;
//����ͼ�α任ģ��
//�������ɵ�ԭʼͼƬת��Ϊ��ʵɹ����ͼ��ͼ�δ���Ի����ݲ���
struct FontGraphicTransTemp
{



protected:
	//�任����,ԭͼ/���ͼ
	void TransForm(cv::InputArray Src,cv::Mat &dst) const;

public:
	double Roit; //�Ŵ���

	int GlussRoit;     //��˹ģ���ľ����С ���� 3,5,7,9
	int ErodeRoit;     //������ʴ����,Ĭ��3*3���ɸ���
	int PowRoit;       //�����

	double CompareRoit; //��ϵı���

	FontGraphicTransTemp();

	

	void TransForm(const FontPicture& Org,FontPicture &Dst) const;  //FontPicture��ʽ�ı任

	void SaveXML(char* Str = nullptr,char* FileFode = nullptr);//��xml,·��,����õĽṹ
	void ReadXML(char* Str = nullptr,char* FileFode = nullptr);//��xml,·��,����õĽṹ

	void read(const cv::FileNode &node);
	void write(cv::FileStorage &fs) const ;


	friend class FontApx;
	friend class FontSimilar;
};



extern void read(const cv::FileNode& node, FontGraphicTransTemp& x,const FontGraphicTransTemp& default_value = FontGraphicTransTemp());
extern void write(cv::FileStorage& fs, const std::string& name, const FontGraphicTransTemp& x);



